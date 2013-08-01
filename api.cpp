/*	TOP-SURF: a visual words toolkit
	Copyright (C) 2010 LIACS Media Lab, Leiden University,
	                   Bart Thomee (bthomee@liacs.nl),
					   Erwin M. Bakker (erwin@liacs.nl)	and
					   Michael S. Lew (mlew@liacs.nl).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    See http://www.gnu.org/licenses/gpl.html for the full license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

	In addition, this work is covered under the Creative Commons
	Attribution license version 3.
    See http://creativecommons.org/licenses/by/3.0/ for the full license.
*/

#include "api.h"

#include "config.h"
#include "topsurf.h"

// function prototype for converting a descriptor to a byte array and back
void Descriptor2Array(const TOPSURF_DESCRIPTOR &td, unsigned char *&data, int &length);
void Array2Descriptor(const unsigned char *data, TOPSURF_DESCRIPTOR &td);

// the topsurf object
TopSurf *topsurf = NULL;

bool TopSurf_Initialize(int imagedim, int top)
{
	if (topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has already been initialized\n");
		return false;
	}
	topsurf = NEW TopSurf(imagedim, top);
	return true;
}

void TopSurf_Terminate()
{
	SAFE_DELETE(topsurf);
}

bool TopSurf_LoadDictionary(const char *dictionarydir)
{
	if (!topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has not yet been initialized\n");
		return false;
	}
	return topsurf->LoadDictionary(dictionarydir);
}

bool TopSurf_SaveDictionary(const char *dictionarydir)
{
	if (!topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has not yet been initialized\n");
		return false;
	}
	return topsurf->SaveDictionary(dictionarydir);
}

bool TopSurf_CreateDictionary(const char *imagedir, int clusters, int knn, int iterations, int points)
{
	if (!topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has not yet been initialized\n");
		return false;
	}
	return topsurf->CreateDictionary(imagedir, clusters, knn, iterations, points);
}

bool TopSurf_ExtractDescriptor(const char *fname, TOPSURF_DESCRIPTOR &td)
{
	if (!topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has not yet been initialized\n");
		return false;
	}
	// load image from disk
	IplImage *image = cvLoadImage(fname, CV_LOAD_IMAGE_COLOR);
	if (image == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not load image %s\n", fname);
		return false;
	}
	// extract the descriptor
	if (!topsurf->ExtractDescriptor(*image, td))
	{
		cvReleaseImage(&image);
		return false;
	}
	// release resources
	cvReleaseImage(&image);
	return true;
}

bool TopSurf_ExtractDescriptor(const char *fname, unsigned char *&data, int &length)
{
	if (&data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// extract the descriptor
	TOPSURF_DESCRIPTOR td;
	if (!TopSurf_ExtractDescriptor(fname, td))
		return false;
	// convert the descriptor to an array
	Descriptor2Array(td, data, length);
	// release resources
	TopSurf::ReleaseDescriptor(td);
	return true;
}

bool TopSurf_ExtractDescriptor(const unsigned char *pixels, int dimx, int dimy, TOPSURF_DESCRIPTOR &td)
{
	if (!topsurf)
	{
		SAFE_FLUSHPRINT(stderr, "TOP-SURF has not yet been initialized\n");
		return false;
	}
	if (pixels == NULL || dimx <= 0 || dimy <= 0)
	{
		SAFE_FLUSHPRINT(stderr, "invalid image data provided\n");
		return false;
	}
	// convert pixels to opencv image type
	IplImage *image = cvCreateImage(cvSize(dimx, dimy), IPL_DEPTH_8U, 3);
	if (image == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not convert image data\n");
		return false;
	}
	const unsigned char *temp = pixels;
	for (int y = 0; y < dimy; y++)
	{
		for (int x = 0; x < dimx; x++, temp+=3)
		{
			// the pixels are stored as RGB, whereas opencv stores them as BGR
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 0] = temp[2];
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 1] = temp[1];
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 2] = temp[0];
		}
	}
	// use the newly created image to extract the descriptor
	if (!topsurf->ExtractDescriptor(*image, td))
	{
		cvReleaseImage(&image);
		return false;
	}
	// release resources
	cvReleaseImage(&image);
	return true;
}

bool TopSurf_ExtractDescriptor(const unsigned char *pixels, int dimx, int dimy, unsigned char *&data, int &length)
{
	if (&data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// extract the descriptor
	TOPSURF_DESCRIPTOR td;
	if (!TopSurf_ExtractDescriptor(pixels, dimx, dimy, td))
		return false;
	// convert the descriptor to an array
	Descriptor2Array(td, data, length);
	// release resources
	TopSurf::ReleaseDescriptor(td);
	return true;
}

float TopSurf_CompareDescriptors(const TOPSURF_DESCRIPTOR &td1, const TOPSURF_DESCRIPTOR &td2, TOPSURF_SIMILARITY similarity)
{
	switch (similarity)
	{
	case TOPSURF_COSINE: return TopSurf::CompareDescriptorsCosine(td1, td2);
	case TOPSURF_ABSOLUTE: return TopSurf::CompareDescriptorsAbsolute(td1, td2);
	default: return -1.0f;
	}
}

float TopSurf_CompareDescriptors(const unsigned char *data1, const unsigned char *data2, TOPSURF_SIMILARITY similarity)
{
	if (data1 == NULL || data2 == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// convert the arrays of bytes to a descriptor
	TOPSURF_DESCRIPTOR td1, td2;
	Array2Descriptor(data1, td1);
	Array2Descriptor(data2, td2);
	// compare the descriptors
	return TopSurf_CompareDescriptors(td1, td2, similarity);
}

bool TopSurf_LoadDescriptor(const char *fname, TOPSURF_DESCRIPTOR &td)
{
	return TopSurf::LoadDescriptor(fname, td);
}

bool TopSurf_LoadDescriptor(const char *fname, unsigned char *&data, int &length)
{
	if (&data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// load the descriptor
	TOPSURF_DESCRIPTOR td;
	if (!TopSurf_LoadDescriptor(fname, td))
		return false;
	// convert the descriptor to an array
	Descriptor2Array(td, data, length);
	// release resources
	TopSurf::ReleaseDescriptor(td);
	return true;
}

bool TopSurf_LoadDescriptor(FILE *file, TOPSURF_DESCRIPTOR &td)
{
	return TopSurf::LoadDescriptor(file, td);
}

bool TopSurf_LoadDescriptor(FILE *file, unsigned char *&data, int &length)
{
	// load the descriptor
	TOPSURF_DESCRIPTOR td;
	if (!TopSurf_LoadDescriptor(file, td))
		return false;
	// convert the descriptor to an array
	Descriptor2Array(td, data, length);
	// release resources
	TopSurf::ReleaseDescriptor(td);
	return true;
}

bool TopSurf_SaveDescriptor(const char *fname, const TOPSURF_DESCRIPTOR &td)
{
	return TopSurf::SaveDescriptor(fname, td);
}

bool TopSurf_SaveDescriptor(const char *fname, const unsigned char *data)
{
	if (data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// convert the array of bytes to a descriptor
	TOPSURF_DESCRIPTOR td;
	Array2Descriptor(data, td);
	// save the descriptor
	return TopSurf_SaveDescriptor(fname, td);
}

bool TopSurf_SaveDescriptor(FILE *file, const TOPSURF_DESCRIPTOR &td)
{
	return TopSurf::SaveDescriptor(file, td);
}

bool TopSurf_SaveDescriptor(FILE *file, const unsigned char *data)
{
	if (data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// convert the array of bytes to a descriptor
	TOPSURF_DESCRIPTOR td;
	Array2Descriptor(data, td);
	// save the descriptor
	return TopSurf_SaveDescriptor(file, td);
}

bool TopSurf_VisualizeDescriptor(unsigned char *pixels, int dimx, int dimy, TOPSURF_DESCRIPTOR &td)
{
	if (pixels == NULL || dimx <= 0 || dimy <= 0)
	{
		SAFE_FLUSHPRINT(stderr, "invalid image data provided\n");
		return false;
	}
	// convert pixels to opencv image type
	IplImage *image = cvCreateImage(cvSize(dimx, dimy), IPL_DEPTH_8U, 3);
	if (image == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not convert image data\n");
		return false;
	}
	unsigned char *temp = pixels;
	for (int y = 0; y < dimy; y++)
	{
		for (int x = 0; x < dimx; x++, temp+=3)
		{
			// the pixels are stored as RGB, whereas opencv stores them as BGR
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 0] = temp[2];
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 1] = temp[1];
			((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 2] = temp[0];
		}
	}
	// visualize the descriptor
	TopSurf::VisualizeDescriptor(*image, td);
	// convert our image representation back to the RGB array
	temp = pixels;
	for (int y = 0; y < dimy; y++)
	{
		for (int x = 0; x < dimx; x++, temp+=3)
		{
			temp[2] = ((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 0];
			temp[1] = ((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 1];
			temp[0] = ((uchar *)(image->imageData + y*image->widthStep))[x*image->nChannels + 2];
		}
	}
	// release resources
	cvReleaseImage(&image);
	return true;
}

bool TopSurf_VisualizeDescriptor(unsigned char *pixels, int dimx, int dimy, const unsigned char *data)
{
	if (data == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid descriptor data provided\n");
		return false;
	}
	// convert the array of bytes to a descriptor
	TOPSURF_DESCRIPTOR td;
	Array2Descriptor(data, td);
	// visualize the descriptor
	return TopSurf_VisualizeDescriptor(pixels, dimx, dimy, td);
}

bool TopSurf_VisualizeDescriptorFromImage(const char *fname, const char *xname, TOPSURF_DESCRIPTOR &td)
{
	if (fname == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid image data provided\n");
		return false;
	}

	IplImage *image = cvLoadImage(fname);
	if (image == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not convert image data\n");
		return false;
	}

	// visualize the descriptor
	TopSurf::VisualizeDescriptor(*image, td);

  // save image to file
	int p[3];
	p[0] = CV_IMWRITE_JPEG_QUALITY;
  p[1] = 100;
	p[2] = 0;
	cvSaveImage(xname, image, p);

	// release resources
  cvReleaseImage(&image);

	return true;
}

void TopSurf_ReleaseDescriptor(TOPSURF_DESCRIPTOR &td)
{
	TopSurf::ReleaseDescriptor(td);
}

void TopSurf_ReleaseDescriptor(unsigned char *data)
{
	// Note: we actually don't need the passed in descriptor
	//       length, although we could use it to verify the
	//       descriptor
	SAFE_DELETE_ARRAY(data);
}

void Descriptor2Array(const TOPSURF_DESCRIPTOR &td, unsigned char *&data, int &length)
{
	// count the number of bytes to write
	length = sizeof(int);
	if (td.count > 0)
	{
		length += sizeof(float) + td.count * sizeof(TOPSURF_VISUALWORD);
		for (int i = 0; i < td.count; i++)
			length += sizeof(int) + sizeof(float) + sizeof(float) + sizeof(int) + td.visualword[i].count * sizeof(TOPSURF_LOCATION);
	}
	// write the data
	// Note: we save the descriptor as 'count', 'length', 'visualwords'
	data = NEW unsigned char[length];
	unsigned char *temp = data;
	memcpy(temp, &td.count, sizeof(int));
	if (td.count > 0)
	{
		temp += sizeof(int);
		memcpy(temp, &td.length, sizeof(float));
		temp += sizeof(float);
		TOPSURF_VISUALWORD *vw = td.visualword;
		for (int i = 0; i < td.count; i++)
		{
			memcpy(temp, &vw[i].identifier, sizeof(int));
			temp += sizeof(int);
			memcpy(temp, &vw[i].tf, sizeof(float));
			temp += sizeof(float);
			memcpy(temp, &vw[i].idf, sizeof(float));
			temp += sizeof(float);
			memcpy(temp, &vw[i].count, sizeof(int));
			temp += sizeof(int);
			if (vw[i].count > 0) // this should always be the case
			{
				memcpy(temp, vw[i].location, vw[i].count * sizeof(TOPSURF_LOCATION));
				temp += vw[i].count * sizeof(TOPSURF_LOCATION);
			}
		}
	}
}

void Array2Descriptor(const unsigned char *data, TOPSURF_DESCRIPTOR &td)
{
	memcpy(&td.count, data, sizeof(int));
	if (td.count > 0)
	{
		data += sizeof(int);
		memcpy(&td.length, data, sizeof(float));
		data += sizeof(float);
		td.visualword = NEW TOPSURF_VISUALWORD[td.count];
		TOPSURF_VISUALWORD *vw = td.visualword;
		for (int i = 0; i < td.count; i++)
		{
			memcpy(&vw[i].identifier, data, sizeof(int));
			data += sizeof(int);
			memcpy(&vw[i].tf, data, sizeof(float));
			data += sizeof(float);
			memcpy(&vw[i].idf, data, sizeof(float));
			data += sizeof(float);
			memcpy(&vw[i].count, data, sizeof(int));
			data += sizeof(int);
			if (vw[i].count > 0) // this should always be the case
			{
				vw[i].location = NEW TOPSURF_LOCATION[vw[i].count];
				memcpy(vw[i].location, data, vw[i].count * sizeof(TOPSURF_LOCATION));
				data += vw[i].count * sizeof(TOPSURF_LOCATION);
			}
		}
	}
}
