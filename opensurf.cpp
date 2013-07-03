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

#include "opensurf.h"

#include "integral.h"
#include "fasthessian.h"
#include "surf.h"

OpenSurf::OpenSurf(int imagedim)
{
	m_imagedim = imagedim;
}

OpenSurf::~OpenSurf()
{
}

bool OpenSurf::ExtractDescriptor(IplImage &image, float *&descriptor, int &ipoints)
{
	// resize image
	IplImage *small = cvCreateImage(cvSize(m_imagedim, m_imagedim), IPL_DEPTH_8U, 3);
	cvResize(&image, small, CV_INTER_CUBIC);
	// create the integral image
	IplImage *integral = Integral(small);
	cvReleaseImage(&small);
	// start finding the SURF points
	vector<OpenSurfInterestPoint> ivector;
	ivector.reserve(2000);	
	// extract interest points
	FastHessian fh(integral, ivector, OCTAVES, INTERVALS, INIT_SAMPLE, THRES);
	fh.getIpoints();
	if (ivector.empty())
	{
		ipoints = 0;
		descriptor = NULL;
		return true;
	}
	// extract descriptors
	Surf des(integral, ivector);
	des.getDescriptors(false);
	cvReleaseImage(&integral);
	// save the points
	ipoints = (int)ivector.size();
	descriptor = NEW float[ipoints * OPENSURF_FEATURECOUNT];
	float *f = descriptor;
	for (vector<OpenSurfInterestPoint>::const_iterator it = ivector.begin(); it != ivector.end(); ++it, f += OPENSURF_FEATURECOUNT)
		memcpy(f, (*it).descriptor, OPENSURF_FEATURECOUNT * sizeof(float));
	return true;
}

bool OpenSurf::ExtractDescriptor(IplImage &image, OpenSurfInterestPoint *&points, int &ipoints)
{
	// resize image
	IplImage *small = cvCreateImage(cvSize(m_imagedim, m_imagedim), IPL_DEPTH_8U, 3);
	cvResize(&image, small, CV_INTER_CUBIC);
	// create the integral image
	IplImage *integral = Integral(small);
	cvReleaseImage(&small);
	// start finding the SURF points
	vector<OpenSurfInterestPoint> ivector;
	ivector.reserve(2000);	
	// extract interest points
	FastHessian fh(integral, ivector, OCTAVES, INTERVALS, INIT_SAMPLE, THRES);
	fh.getIpoints();
	if (ivector.empty())
	{
		ipoints = 0;
		points = NULL;
		return true;
	}
	// extract descriptors
	Surf des(integral, ivector);
	des.getDescriptors(false);
	cvReleaseImage(&integral);
	// save the points
	ipoints = (int)ivector.size();
	points = NEW OpenSurfInterestPoint[ipoints];
	OpenSurfInterestPoint *p = points;
	for (vector<OpenSurfInterestPoint>::const_iterator it = ivector.begin(); it != ivector.end(); ++it, p++)
		memcpy(p, &(*it), sizeof(OpenSurfInterestPoint));
	return true;
}

float OpenSurf::CompareDescriptors(const float *descriptor1, int ipoints1, const float *descriptor2, int ipoints2)
{
	// modified copy from getMatches function in ipoint.cpp
	int matches = 0;
	float d1, d2;
	const float *temp1 = descriptor1;
	const float *temp2;
	for (int i = 0; i < ipoints1; i++, temp1 += OPENSURF_FEATURECOUNT) 
	{
		d1 = d2 = FLT_MAX;
		temp2 = descriptor2;
		for (int j = 0; j < ipoints2; j++, temp2 += OPENSURF_FEATURECOUNT) 
		{
			// calculate the distance between the descriptors of two interest points
			float dist = 0.0f;
			for (int k = 0; k < OPENSURF_FEATURECOUNT; k++)
				dist += (temp1[k] - temp2[k]) * (temp1[k] - temp2[k]);
			// check if this feature matches better than current best or second best
			if (dist < d1)
			{
				d2 = d1;
				d1 = dist;
			}
			else if (dist < d2)
				d2 = dist;
		}
		// if match has a d1:d2 ratio < 0.65 ipoints are a match
		// Note: it should be impossible for d2 to be zero. even when the best interest
		//       point match is a perfect fit, another interest point will have a different
		//       descriptor and consequently have a distance larger than zero
		if (d1/d2 < 0.65f) 
			matches++;
	}
	// return 1 minus percentage of interest points that were matched
	return 1.0f - ((float)matches / ipoints1);
}

float OpenSurf::CompareDescriptors(const OpenSurfInterestPoint *points1, int ipoints1, const OpenSurfInterestPoint *points2, int ipoints2)
{
	// modified copy from getMatches function in ipoint.cpp
	int matches = 0;
	float d1, d2;
	const OpenSurfInterestPoint *temp1 = points1;
	const OpenSurfInterestPoint *temp2;
	for (int i = 0; i < ipoints1; i++, temp1++) 
	{
		d1 = d2 = FLT_MAX;
		temp2 = points2;
		for (int j = 0; j < ipoints2; j++, temp2++) 
		{
			// only match if the laplacian corresponds
			if (temp1->laplacian != temp2->laplacian)
				continue;
			// calculate the distance between the descriptors of two interest points
			float dist = 0.0f;
			for (unsigned int k = 0; k < OPENSURF_FEATURECOUNT; k++)
				dist += (temp1->descriptor[k] - temp2->descriptor[k]) * (temp1->descriptor[k] - temp2->descriptor[k]);
			// check if this feature matches better than current best or second best
			if (dist < d1)
			{
				d2 = d1;
				d1 = dist;
			}
			else if (dist < d2)
				d2 = dist;
		}
		// if match has a d1:d2 ratio < 0.65 ipoints are a match
		// Note: it should be impossible for d2 to be zero. even when the best interest
		//       point match is a perfect fit, another interest point will have a different
		//       descriptor and consequently have a distance larger than zero
		if (d1/d2 < 0.65f) 
			matches++;
	}
	// return 1 minus percentage of interest points that were matched
	return 1.0f - ((float)matches / ipoints1);
}

bool OpenSurf::LoadDescriptor(const char *fname, float *&descriptor, int &ipoints)
{
	if (fname == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "an empty filename was provided\n");
		return false;
	}
	// open the file
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s\n", fname);
		return false;
	}
	// load the descriptor
	bool success = LoadDescriptor(file, descriptor, ipoints);
	// close the file
	fclose(file);
	return success;
}

bool OpenSurf::LoadDescriptor(FILE *file, float *&descriptor, int &ipoints)
{
	if (file == NULL)
		goto failure;
	// read the number of points
	if (fread(&ipoints, sizeof(int), 1, file) != 1)
		goto failure;
	// read the point data
	descriptor = NULL;
	if (ipoints > 0)
	{
		descriptor = NEW float[ipoints * OPENSURF_FEATURECOUNT];
		float *temp = descriptor;
		for (int i = 0; i < ipoints; i++, temp+=OPENSURF_FEATURECOUNT)
		{
			if (fread(temp, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT)
				goto failure;
		}
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not read from file\n");
	SAFE_DELETE_ARRAY(descriptor);
	return false;
}

bool OpenSurf::LoadDescriptor(const char *fname, OpenSurfInterestPoint *&points, int &ipoints)
{
	if (fname == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "an empty filename was provided\n");
		return false;
	}
	// open the file
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s\n", fname);
		return false;
	}
	// load the descriptor
	bool success = LoadDescriptor(file, points, ipoints);
	// close the file
	fclose(file);
	return success;
}

bool OpenSurf::LoadDescriptor(FILE *file, OpenSurfInterestPoint *&points, int &ipoints)
{
	if (file == NULL)
		goto failure;
	// read the number of points
	if (fread(&ipoints, sizeof(int), 1, file) != 1)
		goto failure;
	// read the point data
	points = NULL;
	if (ipoints > 0)
	{
		points = NEW OpenSurfInterestPoint[ipoints];
		for (int i = 0; i < ipoints; i++)
		{
			if (fread(&points[i].x, sizeof(float), 1, file) != 1 ||
				fread(&points[i].y, sizeof(float), 1, file) != 1 ||
				fread(&points[i].scale, sizeof(float), 1, file) != 1 ||
				fread(&points[i].orientation, sizeof(float), 1, file) != 1 ||
				fread(&points[i].laplacian, sizeof(int), 1, file) != 1 ||
				fread(points[i].descriptor, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT)
				goto failure;
		}
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not read from file\n");
	SAFE_DELETE_ARRAY(points);
	return false;
}

bool OpenSurf::SaveDescriptor(const char *fname, const float *descriptor, int ipoints)
{
	if (fname == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "an empty filename was provided\n");
		return false;
	}
	// open the file
	FILE *file = fopen(fname, "wb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s\n", fname);
		return false;
	}
	// save the descriptor
	bool success = SaveDescriptor(file, descriptor, ipoints);
	// close the file
	fclose(file);
	return success;
}

bool OpenSurf::SaveDescriptor(FILE *file, const float *descriptor, int ipoints)
{
	const float *temp;

	if (file == NULL)
		goto failure;
	// save the number of points
	if (fwrite(&ipoints, sizeof(int), 1, file) != 1)
		goto failure;
	// save the point data
	temp = descriptor;
	for (int i = 0; i < ipoints; i++, temp+=OPENSURF_FEATURECOUNT)
	{
		if (fwrite(temp, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT)
			goto failure;
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not write to file\n");
	return false;
}

bool OpenSurf::SaveDescriptor(const char *fname, const OpenSurfInterestPoint *points, int ipoints)
{
	if (fname == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "an empty filename was provided\n");
		return false;
	}
	// open the file
	FILE *file = fopen(fname, "wb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s\n", fname);
		return false;
	}
	// save the descriptor
	bool success = SaveDescriptor(file, points, ipoints);
	// close the file
	fclose(file);
	return success;
}

bool OpenSurf::SaveDescriptor(FILE *file, const OpenSurfInterestPoint *points, int ipoints)
{
	if (file == NULL)
		goto failure;
	// save the number of points
	if (fwrite(&ipoints, sizeof(int), 1, file) != 1)
		goto failure;
	// save the point data
	for (int i = 0; i < ipoints; i++)
	{
		if (fwrite(&points[i].x, sizeof(float), 1, file) != 1 ||
			fwrite(&points[i].y, sizeof(float), 1, file) != 1 ||
			fwrite(&points[i].scale, sizeof(float), 1, file) != 1 ||
			fwrite(&points[i].orientation, sizeof(float), 1, file) != 1 ||
			fwrite(&points[i].laplacian, sizeof(int), 1, file) != 1 ||
			fwrite(points[i].descriptor, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT)
			goto failure;
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not write to file\n");
	return false;
}
