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

#ifndef _DICTIONARYH
#define _DICTIONARYH

#pragma once

#include "config.h"
#include "flann/flann.h"
#include "flann/kdtree.h"

class Dictionary
{
public:
	// create a dictionary
	static bool Create(const char *imagedir, int imagedim, int clusters, int knn, int iterations, int points,
		float *&idf, float *&visualwords, KDTree *&kdtree, Dataset<float> *&kddata, FLANNParameters &kdparam);
private:
	// read images from image directory and its subdirectories
	static void ReadImages(const char *imagedir, vector<string> &filenames);
	// determine subset of interest points
	static bool DetermineSubset(const vector<string> &filenames, int imagedim, int points, float *&subsetf, int &subsetp);
	// extract random points
	static void ExtractRandomPoints(int points, const float *src, float *dst, int &ip);
	// perform the clustering
	static bool PerformClustering(int clusters, int knn, int iterations, const float *subsetf, int subsetp, float *visualwords, KDTree &kdtree, FLANNParameters &p);
	// determine new cluster centers
	static int DetermineClusterCenters(int clusters, int knn, const float *subsetf, const int *indices, const float *dists, float *centers);
	// calculate idf weights
	static bool CalculateIDF(const vector<string> &filenames, int imagedim, int clusters, const float *visualwords, KDTree &kdtree, FLANNParameters &p, float *idf);
	// get flann parameters
	static void GetFLANNParameters(FLANNParameters &kdparam);

public:
	// load size of dictionary
	static bool LoadSize(const char *fname, int &clusters);
	// save size of dictionary
	static bool SaveSize(const char *fname, int clusters);
	// load kdtree
	static bool LoadKDTree(const char *fname, int clusters, float *visualwords, KDTree *&kdtree, Dataset<float> *&kddata, FLANNParameters &kdparam);
	// save kdtree
	static bool SaveKDTree(const char *fname, const KDTree &kdtree);
	// load visual words
	static bool LoadVisualWords(const char *fname, int clusters, float *&visualwords);
	// save visual words
	static bool SaveVisualWords(const char *fname, int clusters, const float *visualwords);
	// load idf
	static bool LoadIDF(const char *fname, int clusters, float *&idf);
	// save idf
	static bool SaveIDF(const char *fname, int clusters, const float *idf);
};

#endif