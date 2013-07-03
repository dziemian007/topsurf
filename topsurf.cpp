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

#include "topsurf.h"

#include "dictionary.h"

#include "unistd.h"

TopSurf::TopSurf(int imagedim, int top)
{
	m_initialized = false;
	m_imagedim = imagedim;
	m_top = top;
	m_opensurf = NEW OpenSurf(m_imagedim);
	m_clusters = 0;
	m_idf = NULL;
	m_visualwords = NULL;
	memset(&m_kdparam, 0, sizeof(FLANNParameters));
	m_kdtree = NULL;
	m_kddata = NULL;
	m_weights = NULL;
}

TopSurf::~TopSurf()
{
	if (m_initialized)
	{
		delete[] m_visualwords;
		delete[] m_idf;
		delete m_kdtree;
		delete m_kddata;
		delete[] m_weights;
	}
	delete m_opensurf;
}

bool TopSurf::LoadDictionary(const char *dictionarydir)
{
	// release any old resources
	SAFE_DELETE_ARRAY(m_idf);
	SAFE_DELETE_ARRAY(m_visualwords);
	SAFE_DELETE(m_kdtree);
	SAFE_DELETE(m_kddata);
	SAFE_DELETE_ARRAY(m_weights);
	m_initialized = false;
	// check parameter
	if (dictionarydir == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid dictionary directory\n");
		return false;
	}
	// append directory separator character if necessary
	string dictdir = dictionarydir;
	if (!dictdir.empty() && dictdir[dictdir.size()-1] != PATH_SEPARATOR_CHAR)
		dictdir += PATH_SEPARATOR_STRING;
	// load the size of the dictionary
	char fname[MAX_PATH];
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "dictionary.txt");
	if (!Dictionary::LoadSize(fname, m_clusters))
		return false;
	// load idf weights
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "idf.dat");
	if (!Dictionary::LoadIDF(fname, m_clusters, m_idf))
		return false;
	// load visual words
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "visualwords.dat");
	if (!Dictionary::LoadVisualWords(fname, m_clusters, m_visualwords))
	{
		delete[] m_idf;
		return false;
	}
	// load the kdtree
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "kdtree.dat");
	if (!Dictionary::LoadKDTree(fname, m_clusters, m_visualwords, m_kdtree, m_kddata, m_kdparam))
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		delete[] m_visualwords;
		delete[] m_idf;
		return false;
	}
	// setup visual word weights to use during extraction
	m_weights = NEW TOPSURF_ELEMENT[m_clusters];
	m_initialized = true;
	return true;
}

bool TopSurf::SaveDictionary(const char *dictionarydir)
{
	if (!m_initialized)
		return false;
	// check parameter
	if (dictionarydir == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid dictionary directory\n");
		return false;
	}
	// create the directory if necessary
	string dictdir = dictionarydir;
	if (!dictdir.empty())
	{
		if (_access(dictdir.c_str(), 0) != 0 && _mkdir(dictdir.c_str()) != 0)
		{
			SAFE_FLUSHPRINT(stderr, "could not create dictionary directory %s\n", dictdir.c_str());
			return false;
		}
		// append directory separator character if necessary
		if (dictdir[dictdir.size()-1] != PATH_SEPARATOR_CHAR)
			dictdir += PATH_SEPARATOR_STRING;
	}
	// save the size of the dictionary
	char fname[MAX_PATH];
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "dictionary.txt");
	if (!Dictionary::SaveSize(fname, m_clusters))
		return false;
	// save idf weights
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "idf.dat");
	if (!Dictionary::SaveIDF(fname, m_clusters, m_idf))
		return false;
	// save visual words
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "visualwords.dat");
	if (!Dictionary::SaveVisualWords(fname, m_clusters, m_visualwords))
		return false;
	// save the kdtree
	SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dictdir.c_str(), "kdtree.dat");
	if (!Dictionary::SaveKDTree(fname, *m_kdtree))
		return false;
	return true;
}

bool TopSurf::CreateDictionary(const char *imagedir, int clusters, int knn, int iterations, int points)
{
	// release any old resources
	SAFE_DELETE_ARRAY(m_idf);
	SAFE_DELETE_ARRAY(m_visualwords);
	SAFE_DELETE(m_kdtree);
	SAFE_DELETE(m_kddata);
	SAFE_DELETE_ARRAY(m_weights);
	m_clusters = 0;
	m_initialized = false;
	// create a new dictionary
	if (!Dictionary::Create(imagedir, m_imagedim, clusters, knn, iterations, points, m_idf, m_visualwords, m_kdtree, m_kddata, m_kdparam))
		return false;
	// set the number of clusters
	m_clusters = clusters;
	// setup visual word weights to use during extraction
	m_weights = NEW TOPSURF_ELEMENT[clusters];
	m_initialized = true;
	return true;
}

bool TopSurf::ExtractDescriptor(IplImage &image, TOPSURF_DESCRIPTOR &descriptor)
{
	if (!m_initialized)
		return false;
	// prepare the descriptor
	descriptor.count = 0;
	descriptor.visualword = NULL;
	// extract the interest points
	OpenSurfInterestPoint *points;
	int ip;
	if (!m_opensurf->ExtractDescriptor(image, points, ip))
	{
		SAFE_FLUSHPRINT(stderr, "could not extract SURF interest points\n");
		return false;
	}
	// check if any points were detected
	if (ip == 0)
		return true;
	// initialize the weights associated with the visual words
	for (int i = 0; i < m_clusters; i++)
	{
		m_weights[i].vw = i;
		m_weights[i].tf = 0.0f;
		m_weights[i].idf = m_idf[i];
		m_weights[i].loc.clear();
		m_weights[i].loc.reserve(ip);
	}
	// find the best matching visual word for each interest point
	OpenSurfInterestPoint *p = points;
	int index;
	float dist;
	for (int i = 0; i < ip; i++, p++)
	{
		if (flann_find_nearest_neighbors_index(m_kdtree, p->descriptor, 1, &index, &dist, 1, &m_kdparam) != 0)
		{
			SAFE_FLUSHPRINT(stderr, "could not find best matching visual word in the kd-tree\n");
			SAFE_DELETE_ARRAY(points);
			return false;
		}
		// add the locations of the point
		// Note: change the locations and scale of the interest points so they range
		//       between 0 and 1 and become independent of the current image size
		// Note: the location origin is the bottom-left hand corner of the image
		TOPSURF_LOCATION l;
		l.x = p->x / m_imagedim;
		l.y = p->y / m_imagedim;
		l.scale = p->scale / m_imagedim;
		l.orientation = p->orientation;
		m_weights[index].loc.push_back(l);
	}
	SAFE_DELETE_ARRAY(points);
	// calculate the tf of each visual word
	int top = 0;
	for (int i = 0; i < m_clusters; i++)
	{
		if (m_idf[i] > 0)
		{
			m_weights[i].tf = (float)m_weights[i].loc.size() / (float)ip;
			top++;
		}
	}
	if (top == 0)
		return true;
	// sort them by td-idf score
	sort(m_weights, m_weights + m_clusters, TOPSURF_ELEMENT::TFIDF_COMPARE);
	top = 0;
	for (int i = 0; i < m_clusters && top < m_top; i++, top++)
	{
		if (m_weights[i].tf == 0 || m_weights[i].idf == 0)
			break;
	}
	sort(m_weights, m_weights + top, TOPSURF_ELEMENT::VW_COMPARE);
	// save only the best top that got detected
	descriptor.count = top;
	descriptor.length = 0.0f;
	descriptor.visualword = NEW TOPSURF_VISUALWORD[top];
	for (int i = 0; i < top; i++)
	{
		descriptor.visualword[i].identifier = m_weights[i].vw;
		descriptor.visualword[i].tf = m_weights[i].tf;
		descriptor.visualword[i].idf = m_weights[i].idf;
		descriptor.visualword[i].count = (int)m_weights[i].loc.size();
		descriptor.visualword[i].location = NEW TOPSURF_LOCATION[m_weights[i].loc.size()];
		int j = 0;
		for (vector<TOPSURF_LOCATION>::const_iterator it = m_weights[i].loc.begin(); it != m_weights[i].loc.end(); ++it, j++)
			descriptor.visualword[i].location[j] = *it;
		descriptor.length += (m_weights[i].tf * m_weights[i].idf) * (m_weights[i].tf * m_weights[i].idf);
	}
	descriptor.length = sqrt(descriptor.length);
	return true;
}

float TopSurf::CompareDescriptorsCosine(const TOPSURF_DESCRIPTOR &descriptor1, const TOPSURF_DESCRIPTOR &descriptor2)
{
	if (descriptor1.count == 0 || descriptor2.count == 0)
		return 1.0f;
	// calculate distance by using the cosine normalized score of the descriptor differences
	float distance = 0.0f;
	const TOPSURF_VISUALWORD *vw1 = descriptor1.visualword;
	const TOPSURF_VISUALWORD *vw2 = descriptor2.visualword;
	int count1 = 0, count2 = 0;
	while (count1 != descriptor1.count)
	{
		if (count2 == descriptor2.count)
			break;
		// compare visual word index
		if (vw1->identifier < vw2->identifier)
		{
			vw1++;
			count1++;
		}
		else if (vw1->identifier > vw2->identifier)
		{
			do
			{
				vw2++;
				count2++;
				if (count2 == descriptor2.count)
					break;
			} while (vw1->identifier > vw2->identifier);
		}
		else
		{
			distance += vw1->tf * vw1->idf * vw2->tf * vw2->idf;
			vw1++;
			count1++;
			vw2++;
			count2++;
		}
	}
	distance /= descriptor1.length * descriptor2.length;
	distance = 1.0f - distance;
	return distance;
}


float TopSurf::CompareDescriptorsAbsolute(const TOPSURF_DESCRIPTOR &descriptor1, const TOPSURF_DESCRIPTOR &descriptor2)
{
	float distance = 0.0f;
	const TOPSURF_VISUALWORD *vw1 = descriptor1.visualword;
	const TOPSURF_VISUALWORD *vw2 = descriptor2.visualword;
	int count1 = 0, count2 = 0;
	// calculate distance by using the absolute differences between the tf-idf scores
	while (count1 != descriptor1.count)
	{
		if (count2 == descriptor2.count)
		{
			distance += vw1->tf * vw1->idf;
			vw1++;
			count1++;
		}
		else
		{
			// compare visual word identifier
			if (vw1->identifier < vw2->identifier)
			{
				distance += vw1->tf * vw1->idf;
				vw1++;
				count1++;
			}
			else if (vw1->identifier > vw2->identifier)
			{
				do
				{
					distance += vw2->tf * vw2->idf;
					vw2++;
					count2++;
					if (count2 == descriptor2.count)
						break;
				} while (vw1->identifier > vw2->identifier);
			}
			else
			{
				distance += abs((vw1->tf * vw1->idf) - (vw2->tf * vw2->idf));
				vw1++;
				count1++;
				vw2++;
				count2++;
			}
		}
	}
	while (count2 != descriptor2.count)
	{
		distance += vw2->tf * vw2->idf;
		vw2++;
		count2++;
	}
	return distance;
}

bool TopSurf::LoadDescriptor(const char *fname, TOPSURF_DESCRIPTOR &descriptor)
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
	bool success = LoadDescriptor(file, descriptor);
	// close the file
	fclose(file);
	return success;
}

bool TopSurf::LoadDescriptor(FILE *file, TOPSURF_DESCRIPTOR &descriptor)
{
	if (file == NULL)
		goto failure;
	// clear the descriptor
	memset(&descriptor, 0, sizeof(TOPSURF_DESCRIPTOR));
	// read the number of visual words
	if (fread(&descriptor.count, sizeof(int), 1, file) != 1)
		goto failure;
	if (descriptor.count > 0)
	{
		// read the length of the descriptor
		if (fread(&descriptor.length, sizeof(float), 1, file) != 1)
			goto failure;
		// read the details of each of the visual words
		descriptor.visualword = NEW TOPSURF_VISUALWORD[descriptor.count];
		TOPSURF_VISUALWORD *vw = descriptor.visualword;
		for (int i = 0; i < descriptor.count; i++)
		{
			if (fread(&vw[i].identifier, sizeof(int), 1, file) != 1)
				goto failure;
			if (fread(&vw[i].tf, sizeof(float), 1, file) != 1)
				goto failure;
			if (fread(&vw[i].idf, sizeof(float), 1, file) != 1)
				goto failure;
			if (fread(&vw[i].count, sizeof(int), 1, file) != 1)
				goto failure;
			if (vw[i].count > 0) // this should always be the case
			{
				vw[i].location = NEW TOPSURF_LOCATION[vw[i].count];
				if (fread(vw[i].location, sizeof(TOPSURF_LOCATION), vw[i].count, file) != vw[i].count)
					goto failure;
			}
		}
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not read from file\n");
	SAFE_DELETE_ARRAY(descriptor.visualword);
	return false;
}

bool TopSurf::SaveDescriptor(const char *fname, const TOPSURF_DESCRIPTOR &descriptor)
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
	bool success = SaveDescriptor(file, descriptor);
	// close the file
	fclose(file);
	return success;
}

bool TopSurf::SaveDescriptor(FILE *file, const TOPSURF_DESCRIPTOR &descriptor)
{
	if (file == NULL)
		goto failure;
	// save the number of visual words
	if (fwrite(&descriptor.count, sizeof(int), 1, file) != 1)
		goto failure;
	if (descriptor.count > 0)
	{
		// save the length of the descriptor
		if (fwrite(&descriptor.length, sizeof(float), 1, file) != 1)
			goto failure;
		// save the details of each of the visual words
		TOPSURF_VISUALWORD *vw = descriptor.visualword;
		for (int i = 0; i < descriptor.count; i++)
		{
			if (fwrite(&vw[i].identifier, sizeof(int), 1, file) != 1)
				goto failure;
			if (fwrite(&vw[i].tf, sizeof(float), 1, file) != 1)
				goto failure;
			if (fwrite(&vw[i].idf, sizeof(float), 1, file) != 1)
				goto failure;
			if (fwrite(&vw[i].count, sizeof(int), 1, file) != 1)
				goto failure;
			if (vw[i].count > 0) // this should always be the case
			{
				if (fwrite(vw[i].location, sizeof(TOPSURF_LOCATION), vw[i].count, file) != vw[i].count)
					goto failure;
			}
		}
	}
	return true;

failure:
	SAFE_FLUSHPRINT(stderr, "could not write to file\n");
	return false;
}

void TopSurf::VisualizeDescriptor(IplImage &image, const TOPSURF_DESCRIPTOR &descriptor)
{
	// determine the multiplication factors we need to use to properly
	// display the locations and scale of the interest points, since they
	// were modified to become independent of the input image
	float factorx = (float)image.width;
	float factory = (float)image.height;
	float factors = (factorx + factory) / 2;
	// show the locations of the points
	for (int i = 0; i < descriptor.count; i++)
	{
		// choose random color
		// Note: always make the coloring the same for a particular visual word
		//       by using its identifier as the seed
		srand(descriptor.visualword[i].identifier);
		CvScalar rgb = cvScalar(rand() % UCHAR_MAX, rand() % UCHAR_MAX, rand() % UCHAR_MAX);
		// paint all points
		for (int j = 0; j < descriptor.visualword[i].count; j++)
		{
			TOPSURF_LOCATION l = descriptor.visualword[i].location[j];
			// start with a rectangle around the origin with size of the scale
			float x1 = -l.scale * factors;
			float y1 = -l.scale * factors;
			float x2 = -l.scale * factors;
			float y2 = l.scale * factors;
			float x3 = l.scale * factors;
			float y3 = l.scale * factors;
			float x4 = l.scale * factors;
			float y4 = -l.scale * factors;
			// rotate the rectangle around the origin using the orientation
			float x11 = x1 * cos(l.orientation) - y1 * sin(l.orientation);
			float y11 = x1 * sin(l.orientation) + y1 * cos(l.orientation);
			float x22 = x2 * cos(l.orientation) - y2 * sin(l.orientation);
			float y22 = x2 * sin(l.orientation) + y2 * cos(l.orientation);
			float x33 = x3 * cos(l.orientation) - y3 * sin(l.orientation);
			float y33 = x3 * sin(l.orientation) + y3 * cos(l.orientation);
			float x44 = x4 * cos(l.orientation) - y4 * sin(l.orientation);
			float y44 = x4 * sin(l.orientation) + y4 * cos(l.orientation);
			// move the point to the location it was detected
			int x111 = (int)(x11 + l.x * factorx);
			int y111 = (int)(y11 + l.y * factory);
			int x222 = (int)(x22 + l.x * factorx);
			int y222 = (int)(y22 + l.y * factory);
			int x333 = (int)(x33 + l.x * factorx);
			int y333 = (int)(y33 + l.y * factory);
			int x444 = (int)(x44 + l.x * factorx);
			int y444 = (int)(y44 + l.y * factory);
			// draw the rectangle
			cvLine(&image, cvPoint(x111, y111), cvPoint(x222, y222), rgb, 1);
			cvLine(&image, cvPoint(x222, y222), cvPoint(x333, y333), rgb, 1);
			cvLine(&image, cvPoint(x333, y333), cvPoint(x444, y444), rgb, 1);
			cvLine(&image, cvPoint(x444, y444), cvPoint(x111, y111), rgb, 1);
			// draw an orientation line
			int xo1 = (x111 + x222) / 2;
			int yo1 = (y111 + y222) / 2;
			int xo2 = (int)(l.x * factorx);
			int yo2 = (int)(l.y * factory);
			cvLine(&image, cvPoint(xo1, yo1), cvPoint(xo2, yo2), rgb, 1);
		}
	}
}

void TopSurf::ReleaseDescriptor(TOPSURF_DESCRIPTOR &descriptor)
{
	for (int i = 0; i < descriptor.count; i++)
		SAFE_DELETE_ARRAY(descriptor.visualword[i].location);
	SAFE_DELETE_ARRAY(descriptor.visualword);
	memset(&descriptor, 0, sizeof(TOPSURF_DESCRIPTOR));
}
