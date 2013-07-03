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

#include "dictionary.h"

#include "imagedimensions.h"
#include "opensurf.h"

// maximum number of points to extract for the subset
// Note: there is some kind of internal limit to the flann library
//       that building the tree with more than this number of points
//       causes the application to crash
#define FLANN_POINTSMAX			33550000

bool Dictionary::Create(const char *imagedir, int imagedim, int clusters, int knn, int iterations, int points,
		float *&idf, float *&visualwords, KDTree *&kdtree, Dataset<float> *&kddata, FLANNParameters &kdparam)
{
	// check parameters
	if (imagedir == NULL || imagedim <= 0 || clusters <= 0 || knn <= 0 || iterations <= 0 || points <= 0 ||
		&idf == NULL || &visualwords == NULL || &kdtree == NULL || &kddata == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "invalid parameter(s)\n");
		return false;
	}
	// analyze the images in the provided directory to see if they are valid
	SAFE_FLUSHPRINT(stdout, "analyzing images from the image directory...\n");
	vector<string> filenames;
	ReadImages(imagedir, filenames);
	if (filenames.empty())
	{
		SAFE_FLUSHPRINT(stdout, "no images found\n");
		return false;
	}
	SAFE_FLUSHPRINT(stdout, "%u images found\n", filenames.size());
	// randomize the vector of images
	// Note: we do this for two reasons: first, we might end up with more extracted
	//       points than FLANN can handle (see define above) and if we would process
	//       the images in order they might not be representative enough, and second,
	//       when calculating the idf weights, we need the images to be representative
	//       for the entire set. images that are located in the same directory and
	//       thus placed after each other often share similarities (e.g when the images
	//       were downloaded from the web they might have been found on the same website,
	//       and thus they can be closely related, or when downloading from flickr they
	//       might be from the same user that took photos from the same scene). thus,
	//       we should randomize the set to avoid both issues.
	SAFE_FLUSHPRINT(stdout, "randomizing image list...\n");
	random_shuffle(filenames.begin(), filenames.end());
	// extract a subset of points from the images
	SAFE_FLUSHPRINT(stdout, "creating subset of points...\n");
	t_date begin = GetCurrentDate();
	float *subsetf;
	int subsetp;
	if (!DetermineSubset(filenames, imagedim, points, subsetf, subsetp))
		return false;
	t_date end = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "%i points extracted\n", subsetp);
	SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	// check for minimum number of points
	if (subsetp < clusters)
	{
		SAFE_FLUSHPRINT(stdout, "number of points extracted is smaller than the number of clusters\n");
		delete[] subsetf;
		return false;
	}
	// check for maximum number of points
	if (subsetp > FLANN_POINTSMAX)
	{
		SAFE_FLUSHPRINT(stdout, "number of points is larger than the maximum supported, reducing number to %i...\n", FLANN_POINTSMAX);
		subsetp = FLANN_POINTSMAX;
	}
	// prepare the flann parameters
	GetFLANNParameters(kdparam);
	// build the flann kdtree from the subset
	SAFE_FLUSHPRINT(stdout, "creating subset kd-tree...\n");
	begin = GetCurrentDate();
	Dataset<float> subsetds = Dataset<float>(subsetp, OPENSURF_FEATURECOUNT, subsetf);
	KDTree *subsetkdt = NEW KDTree(subsetds, kdparam);
	subsetkdt->buildIndex();
	end = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	// start clustering
	SAFE_FLUSHPRINT(stdout, "creating visual words...\n");
	begin = GetCurrentDate();
	visualwords = NEW float[clusters * OPENSURF_FEATURECOUNT];
	if (!PerformClustering(clusters, knn, iterations, subsetf, subsetp, visualwords, *subsetkdt, kdparam))
	{
		delete[] visualwords;
		delete subsetkdt;
		delete[] subsetf;
		return false;
	}
	end = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	// release resources
	delete subsetkdt;
	delete[] subsetf;
	// create the dictionary kdtree
	SAFE_FLUSHPRINT(stdout, "creating dictionary kdtree...");
	begin = GetCurrentDate();
	kddata = NEW Dataset<float>(clusters, OPENSURF_FEATURECOUNT, visualwords);
	kdtree = NEW KDTree(*kddata, kdparam);
	kdtree->buildIndex();
	end = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	// create idf weights
	begin = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "creating idf weights...\n");
	idf = NEW float[clusters];
	if (!CalculateIDF(filenames, imagedim, clusters, visualwords, *kdtree, kdparam, idf))
	{
		delete[] idf;
		delete kdtree;
		delete[] visualwords;
		return false;
	}
	end = GetCurrentDate();
	SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	return true;
}

void Dictionary::ReadImages(const char *imagedir, vector<string> &filenames)
{
	// append a slash to the image directory if necessary
	string dir = imagedir;
	if (!dir.empty() && dir[dir.size()-1] != PATH_SEPARATOR_CHAR)
		dir += PATH_SEPARATOR_STRING;

	char fname[MAX_PATH];
	char *fnamep;
	FILE *file;
	int width, height;
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
	// build the initial search path, with the wildcards to return all
	// files and directories and then perform the query
	SAFE_SPRINTF(fname, sizeof(fname), "%s*.*", dir.c_str());
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(fname, &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	// proceed to iterate over all files
	BOOL bContinue = TRUE;
	while (hFind && bContinue)
	{
		fnamep = data.cFileName;
		SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dir.c_str(), fnamep);
		// check if this entry is a directory
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
#else
	// access the directory
	DIR *d = opendir(dir.c_str());
	if (d == NULL)
		return;
	// walk through all files in the directory
	dirent *dp;
	while ((dp = readdir(d)) != NULL)
	{
		fnamep = dp->d_name;
		SAFE_SPRINTF(fname, sizeof(fname), "%s%s", dir.c_str(), fnamep);
		// check if this entry is a directory
		struct stat s;
		stat(fname, &s);
		if (S_ISDIR(s.st_mode))
		{
#endif
			// skip over . and ..
			if (strcmp(fnamep, ".") != 0 && strcmp(fnamep, "..") != 0)
			{
				// recursively read the images from the encountered directory
				ReadImages(fname, filenames);
			}
		}
		else
		{
			// check if the extension indicates we found an image
			string ext = GetFileExtension(fnamep);
			if (_stricmp(ext.c_str(), "jpg") == 0 || _stricmp(ext.c_str(), "jpeg") == 0)
			{
				// verify that this is an image simply by opening it up and checking its
				// signature and image size
				if ((file = fopen(fname, "rb")) != NULL)
				{
					if (!ReadJpgDimensions(file, width, height))
						SAFE_FLUSHPRINT(stdout, "could not read %s\n", fname);
					else
					{
						filenames.push_back(fname);
						// print out progress
						if (filenames.size() % 1000 == 0)
							SAFE_FLUSHPRINT(stdout, "%u\n", filenames.size());
					}
					fclose(file);
				}
			}
			else if (_stricmp(ext.c_str(), "png") == 0)
			{
				if ((file = fopen(fname, "rb")) != NULL)
				{
					if (!ReadPngDimensions(file, width, height))
						SAFE_FLUSHPRINT(stdout, "could not read %s\n", fname);
					else
					{
						filenames.push_back(fname);
						// print out progress
						if (filenames.size() % 1000 == 0)
							SAFE_FLUSHPRINT(stdout, "%u\n", filenames.size());
					}
					fclose(file);
				}
			}
		}
		// continue to the next image
		// Note: this happens automatically on linux in the while loop above
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
		bContinue = FindNextFile(hFind, &data);
#endif
	}
	// close the directory we are in
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
	FindClose(hFind);
#else
	 closedir(d);
#endif
}

bool Dictionary::DetermineSubset(const vector<string> &filenames, int imagedim, int points, float *&subsetf, int &subsetp)
{
	// initialize opensurf
	OpenSurf opensurf(imagedim);
	// process all images in the directory
	float *f;
	int ip;
	float *subf = NEW float[filenames.size() * points * OPENSURF_FEATURECOUNT];
	int subp = 0;
	int i = 0;
	for (vector<string>::const_iterator it = filenames.begin(); it != filenames.end(); ++it, i++)
	{
		SAFE_FLUSHPRINT(stdout, "processing image %i\n", i);
		// load the image
		const char *fname = (*it).c_str();
		IplImage *image = cvLoadImage(fname, CV_LOAD_IMAGE_COLOR);
		if (!image)
			continue;
		// extract the features
		if (!opensurf.ExtractDescriptor(*image, f, ip))
		{
			SAFE_FLUSHPRINT(stderr, "could not extract SURF descriptor from %s\n", fname);
			SAFE_DELETE_ARRAY(subf);
			cvReleaseImage(&image);
			return false;
		}
		cvReleaseImage(&image);
		if (ip == 0)
			continue;
		// extract random points that we will use for clustering
		// Note: it is possible no points were found in the image,
		//       so then nothing will happen
		float *temps = subf + subp * OPENSURF_FEATURECOUNT;
		ExtractRandomPoints(points, f, temps, ip);
		subp += ip;
		SAFE_DELETE_ARRAY(f);
	}
	subsetf = subf;
	subsetp = subp;
	return true;
}

void Dictionary::ExtractRandomPoints(int points, const float *src, float *dst, int &ip)
{
	// Note: we assume the caller has ensured that ip is at least 1
	if (ip > points)
	{
		// we don't use all ip as that will be too computationally intensive
		// for the clustering algorithm. thus we shuffle a list and randomly
		// select the requested number of ip
		unsigned int *shuffle = NEW unsigned int[ip];
		for (int i = 0; i < ip; i++)
			shuffle[i] = i;
		random_shuffle(shuffle, shuffle + ip);
		// copy the features of the randomly selected points
		float *temp = dst;
		for (int i = 0; i < points; i++, temp += OPENSURF_FEATURECOUNT)
			memcpy(temp, src + shuffle[i] * OPENSURF_FEATURECOUNT, OPENSURF_FEATURECOUNT * sizeof(float));
		SAFE_DELETE_ARRAY(shuffle);
		ip = points;
	}
	else
	{
		// if there are less points than we would like, which will occasionally happen,
		// use all points
		memcpy(dst, src, ip * OPENSURF_FEATURECOUNT * sizeof(float));
	}
}

bool Dictionary::PerformClustering(int clusters, int knn, int iterations, const float *subsetf, int subsetp, float *visualwords, KDTree &kdtree, FLANNParameters &p)
{
	// our strategy is to perform approximate k-nearest neighbors to determine
	// the cluster centers. we do this by for each cluster center finding the
	// k nearest neighbors. each nearest neighbor that is not uniquely assigned
	// to only one cluster (i.e. is near enough to two or more centers) is
	// then assigned to the closest one. next, the new cluster centers are
	// created by averaging its associated nearest neighbors. the algorithm
	// is run until it converges, or for a maximum number of iterations,
	// whichever occurs sooner
	int *indices = NEW int[clusters * knn];
	float *dists = NEW float[clusters * knn];
	float **uniqueness = NEW float*[subsetp];
	memset(uniqueness, 0, subsetp * sizeof(float *));
	// randomly shuffle a list of points so we can randomly select
	// those that will act as the initial cluster centers
	int *shuffle = NEW int[subsetp];
	for (int i = 0; i < subsetp; i++)
		shuffle[i] = i;
	random_shuffle(shuffle, shuffle + subsetp);
	// use the first random locations as cluster centers
	float *vwtemp = visualwords;
	for (int i = 0; i < clusters; i++, vwtemp += OPENSURF_FEATURECOUNT)
		memcpy(vwtemp, subsetf + shuffle[i] * OPENSURF_FEATURECOUNT, OPENSURF_FEATURECOUNT * sizeof(float));
	SAFE_DELETE_ARRAY(shuffle);
	// start clustering
	for (int i = 0; i < iterations; i++)
	{
		t_date begin = GetCurrentDate();
		// find nearest neighbors of each cluster center
		SAFE_FLUSHPRINT(stdout, "%4i: finding nearest neighbors...\n", i);
		if (flann_find_nearest_neighbors_index(&kdtree, visualwords, clusters, indices, dists, knn, &p) != 0)
		{
			SAFE_FLUSHPRINT(stderr, "could not find best matching visual word in the kd-tree\n");
			delete[] indices;
			delete[] dists;
			delete[] uniqueness;
			return false;
		}
		// filter out only the unique neighbors, as we don't want a
		// neighbor to be used by multiple clusters
		SAFE_FLUSHPRINT(stdout, "%4i: filtering unique nearest neighbors...\n", i);
		int *tempi = indices;
		float *tempd = dists;
		for (int j = 0; j < clusters; j++)
		{
			for (int k = 0; k < knn; k++, tempi++, tempd++)
			{
				// check if this is the first time this neighbor is used
				if (uniqueness[*tempi] == NULL)
					uniqueness[*tempi] = tempd;
				else
				{
					// check if the distance of the neighbor to the previous
					// closest cluster is less than to the current cluster
					if (*uniqueness[*tempi] <= *tempd)
						*tempd = FLT_MAX;
					else
					{
						*uniqueness[*tempi] = FLT_MAX;
						uniqueness[*tempi] = tempd;
					}
				}
			}
		}
		// determine the new cluster centers
		SAFE_FLUSHPRINT(stdout, "%4i: determining new cluster centers...\n", i);
		int centersame = DetermineClusterCenters(clusters, knn, subsetf, indices, dists, visualwords);
		if (centersame == clusters)
			break;
		// save current clusters and the time it took
		t_date end = GetCurrentDate();
		SAFE_FLUSHPRINT(stdout, "elapsed: %u seconds\n", (unsigned int)(end - begin));
	}
	// release resources
	delete[] indices;
	delete[] dists;
	delete[] uniqueness;
	return true;
}

int Dictionary::DetermineClusterCenters(int clusters, int knn, const float *subsetf, const int *indices, const float *dists, float *centers)
{
	// determine the new cluster centers
	int centersame = 0;
	float cc[OPENSURF_FEATURECOUNT];
	float *ctemp = centers;
	for (int i = 0; i < clusters; i++, ctemp += OPENSURF_FEATURECOUNT)
	{
		// the current cluster center is also used in determining
		// the new cluster center, unless it actually is one of
		// the points that initiated the search as otherwise it
		// will be included twice; this point has a distance of
		// zero to the cluster center and is always listed first
		memcpy(cc, ctemp, OPENSURF_FEATURECOUNT * sizeof(float));
		int average = 1;
		// walk through all its valid nearest neighbors
		const int *itemp = indices + (i * knn);
		const float *dtemp = dists + (i * knn);
		for (int j = 0; j < knn; j++, itemp++, dtemp++)
		{
			if (*dtemp == FLT_MAX || (j == 0 && *dtemp == 0.0f))
				continue;
			// find the feature vector belonging to this nearest neighbor
			const float *ftemp = subsetf + (*itemp * OPENSURF_FEATURECOUNT);
			// add its values to the current cluster center
			for (int k = 0; k < OPENSURF_FEATURECOUNT; k++)
				cc[k] += ftemp[k];
			average++;
		}
		if (average == 1)
		{
			centersame++;
			continue;
		}
		// average the values to obtain the new cluster center
		// Note: it is certainly possible to introduce a threshold value
		//       that can be used to check the distance between the old
		//       cluster center and the new one to see if the center has
		//       stabilized
		float d = 0.0f;
		for (int j = 0; j < OPENSURF_FEATURECOUNT; j++)
		{
			cc[j] /= average;
			d += fabs(cc[j] - ctemp[j]);
		}
		// copy the new cluster center
		memcpy(ctemp, cc, OPENSURF_FEATURECOUNT * sizeof(float));
	}
	return centersame;
}

bool Dictionary::CalculateIDF(const vector<string> &filenames, int imagedim, int clusters, const float *visualwords, KDTree &kdtree, FLANNParameters &p, float *idf)
{
	// initialize opensurf
	OpenSurf opensurf(imagedim);
	// recalculate the interest points for a fraction of the training images,
	// as now we want to know which visual words occur in which images and to
	// be representative we must thus use all interest points of an image
	// Note: statistically we would need to sample around 2500 images to get
	//       good idf estimates
	float *f;
	int ip;
	int index;
	float dist;
	int *vwhist = NEW int[clusters];
	memset(idf, 0, clusters * sizeof(float));
	int count = 0;
	int i = 0;
	for (vector<string>::const_iterator it = filenames.begin(); it != filenames.end(); ++it, i++)
	{
		SAFE_FLUSHPRINT(stdout, "processing image %i\n", i);
		// load the image
		const char *fname = (*it).c_str();
		IplImage *image = cvLoadImage(fname, CV_LOAD_IMAGE_COLOR);
		if (!image)
			continue;
		// extract the features
		if (!opensurf.ExtractDescriptor(*image, f, ip))
		{
			SAFE_FLUSHPRINT(stderr, "could not extract SURF descriptor from %s\n", fname);
			SAFE_DELETE_ARRAY(vwhist);
			cvReleaseImage(&image);
			return false;
		}
		cvReleaseImage(&image);
		if (ip == 0)
			continue;
		// find the best matching visual word for each interest point
		// Note: for the idf, even if a visual word is detected more than
		//       once in an image we only count it a single time
		float *temp = f;
		memset(vwhist, 0, clusters * sizeof(int));
		for (int z = 0; z < ip; z++, temp+=OPENSURF_FEATURECOUNT)
		{
			if (flann_find_nearest_neighbors_index(&kdtree, temp, 1, &index, &dist, 1, &p) != 0)
			{
				SAFE_FLUSHPRINT(stderr, "could not find best matching visual word in the kd-tree\n");
				SAFE_DELETE_ARRAY(f);
				SAFE_DELETE_ARRAY(vwhist);
				return false;
			}
			if (++vwhist[index] == 1)
				idf[index]++;
		}
		SAFE_DELETE_ARRAY(f);
		// check if we have processed enough images
		if (++count == 2500)
			break;
	}
	// take the 2-log using the visual word frequency and the total number of images used to
	// obtain the visual words
	// Note: make sure that visual words that are not found in the sample
	//       set are completely ignored rather than giving them a very high
	//       weight, due to their supposed rarity of occuring
	for (int i = 0; i < clusters; i++)
	{
		if (idf[i] != 0)
			idf[i] = log(count / idf[i]) / log(2.0f);
	}
	// release resources
	SAFE_DELETE_ARRAY(vwhist);
	return true;
}

void Dictionary::GetFLANNParameters(FLANNParameters &kdparam)
{
	memset(&kdparam, 0, sizeof(FLANNParameters));
	kdparam.algorithm = KDTREE;
	kdparam.trees = 8;              // as specified by the authors
	kdparam.checks = 128;           // seems to be a reasonable value
	kdparam.target_precision = -1;  // we don't want any auto-tuning to take place
	kdparam.log_level = LOG_WARN;   // only print out warnings, errors, etc
	kdparam.log_destination = NULL; // print to console
}

bool Dictionary::LoadSize(const char *fname, int &clusters)
{
	FILE *file = fopen(fname, "r");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		return false;
	}
	char line[100];
	unsigned int length;
	if (!SAFE_GETLINE(line, sizeof(line), length, file))
	{
		SAFE_FLUSHPRINT(stderr, "could not read from %s\n", fname);
		fclose(file);
		return false;
	}
	fclose(file);
	clusters = atoi(line);
	if (clusters <= 0)
	{
		SAFE_FLUSHPRINT(stderr, "invalid data in %s\n", fname);
		return false;
	}
	return true;
}

bool Dictionary::SaveSize(const char *fname, int clusters)
{
	FILE *file = fopen(fname, "w");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		return false;
	}
	SAFE_FLUSHPRINT(file, "%i", clusters);
	fclose(file);
	return true;
}

bool Dictionary::LoadKDTree(const char *fname, int clusters, float *visualwords, KDTree *&kdtree, Dataset<float> *&kddata, FLANNParameters &kdparam)
{
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		return false;
	}
	// get the flann parameters
	GetFLANNParameters(kdparam);
	// load the tree
	kddata = NEW Dataset<float>(clusters, OPENSURF_FEATURECOUNT, visualwords);
	kdtree = NEW KDTree(*kddata, kdparam);
	if (!kdtree->loadIndex(file))
	{
		SAFE_FLUSHPRINT(stderr, "could not read from %s\n", fname);
		delete kddata;
		delete kdtree;
		fclose(file);
		return false;
	}
	fclose(file);
	return true; 
}

bool Dictionary::SaveKDTree(const char *fname, const KDTree &kdtree)
{
	FILE *file = fopen(fname, "wb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for write\n", fname);
		return false;
	}
	if (!kdtree.saveIndex(file))
	{
		SAFE_FLUSHPRINT(stderr, "could not write to %s\n", fname);
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

bool Dictionary::LoadVisualWords(const char *fname, int clusters, float *&visualwords)
{
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		return false;
	}
	// to prevent some weird errors with reading one huge chunk of data
	// to disk, break it up in smaller chunks
	visualwords = NEW float[clusters * OPENSURF_FEATURECOUNT];
	float *temp = visualwords;
	for (int i = 0; i < clusters; i++, temp+=OPENSURF_FEATURECOUNT)
	{
		if (fread(temp, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT) 
		{
			SAFE_FLUSHPRINT(stderr, "could not read from %s\n", fname);
			delete[] visualwords;
			fclose(file);
			return false;
		}
	}
	fclose(file);
	return true;
}

bool Dictionary::SaveVisualWords(const char *fname, int clusters, const float *visualwords)
{
	FILE *file = fopen(fname, "wb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for write\n", fname);
		return false;
	}
	// to prevent some weird errors with writing one huge chunk of data
	// to disk, break it up in smaller chunks
	for (int i = 0; i < clusters; i++, visualwords+=OPENSURF_FEATURECOUNT)
	{
		if (fwrite(visualwords, sizeof(float), OPENSURF_FEATURECOUNT, file) != OPENSURF_FEATURECOUNT) 
		{
			SAFE_FLUSHPRINT(stderr, "could not write to %s\n", fname);
			fclose(file);
			return false;
		}
	}
	fclose(file);
	return true;
}

bool Dictionary::LoadIDF(const char *fname, int clusters, float *&idf)
{
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for read\n", fname);
		return false;
	}
	idf = NEW float[clusters];
	if (fread(idf, sizeof(float), clusters, file) != clusters)
	{
		SAFE_FLUSHPRINT(stderr, "could not read from %s\n", fname);
		delete[] idf;
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

bool Dictionary::SaveIDF(const char *fname, int clusters, const float *idf)
{
	FILE *file = fopen(fname, "wb");
	if (file == NULL)
	{
		SAFE_FLUSHPRINT(stderr, "could not open %s for write\n", fname);
		return false;
	}
	if (fwrite(idf, sizeof(float), clusters, file) != clusters)
	{
		SAFE_FLUSHPRINT(stderr, "could not write to %s\n", fname);
		fclose(file);
	}
	fclose(file);
	return true;
}
