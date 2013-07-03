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

#ifndef _TOPSURFH
#define _TOPSURFH

#pragma once

#include "config.h"
#include "opencv/cv.h"
#include "flann/flann.h"
#include "flann/kdtree.h"
#include "opensurf.h"
#include "descriptor.h"

// internal structure to keep track of the visual words
struct TOPSURF_ELEMENT
{
	// visual word index
	int vw;
	// term frequency
	float tf;
	// inverse document frequency
	float idf;
	// location of a visual word
	vector<TOPSURF_LOCATION> loc;
	// compare two descriptor elements by their visual word indices
	// Note: calling this during sorting will result in the elements
	//       being sorted from low indices to high indices
	static bool VW_COMPARE(const TOPSURF_ELEMENT &e1, const TOPSURF_ELEMENT &e2)
	{
		if (e1.vw < e2.vw)
			return true;
		return false;
	}
	// compare two descriptor elements by their tf-idf scores
	// Note: calling this during sorting will result in the elements
	//       being sorted from high scores to low scores
	static bool TFIDF_COMPARE(const TOPSURF_ELEMENT &e1, const TOPSURF_ELEMENT &e2)
	{
		if (e1.tf*e1.idf > e2.tf*e2.idf)
			return true;
		return false;
	}
};


class TopSurf
{
public:
	TopSurf(int imagedim, int top);
	~TopSurf();

public:
	// load dictionary
	bool LoadDictionary(const char *dictionarydir);
	// save dictionary
	bool SaveDictionary(const char *dictionarydir);
	// create dictionary
	bool CreateDictionary(const char *imagedir, int clusters, int knn, int iterations, int points);

public:
	// extract descriptor
	bool ExtractDescriptor(IplImage &image, TOPSURF_DESCRIPTOR &descriptor);
	// return distance between two descriptors
	static float CompareDescriptorsCosine(const TOPSURF_DESCRIPTOR &descriptor1, const TOPSURF_DESCRIPTOR &descriptor2);
	static float CompareDescriptorsAbsolute(const TOPSURF_DESCRIPTOR &descriptor1, const TOPSURF_DESCRIPTOR &descriptor2);

public:
	// load descriptor from disk
	static bool LoadDescriptor(const char *fname, TOPSURF_DESCRIPTOR &descriptor);
	static bool LoadDescriptor(FILE *file, TOPSURF_DESCRIPTOR &descriptor);
	// save descriptor to disk
	static bool SaveDescriptor(const char *fname, const TOPSURF_DESCRIPTOR &descriptor);
	static bool SaveDescriptor(FILE *file, const TOPSURF_DESCRIPTOR &descriptor);
	// visualize descriptor
	static void VisualizeDescriptor(IplImage &image, const TOPSURF_DESCRIPTOR &descriptor);
	// release memory of descriptor
	static void ReleaseDescriptor(TOPSURF_DESCRIPTOR &descriptor);
	
private:
	bool m_initialized;
	int m_imagedim;
	int m_top;
	OpenSurf *m_opensurf;
	int m_clusters;
	float *m_idf;
	float *m_visualwords;
	FLANNParameters m_kdparam;
	KDTree *m_kdtree;
	Dataset<float> *m_kddata;
	TOPSURF_ELEMENT *m_weights;
};

#endif
