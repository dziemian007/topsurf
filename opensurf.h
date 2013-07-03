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

#ifndef _OPENSURF_OPENSURFH
#define _OPENSURF_OPENSURFH

#pragma once

#include "config.h"

#include "ipoint.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

class OpenSurf
{
public:
	OpenSurf(int imagedim);
	~OpenSurf();

public:
	// extract descriptor
	bool ExtractDescriptor(IplImage &image, float *&descriptor, int &ipoints);
	bool ExtractDescriptor(IplImage &image, OpenSurfInterestPoint *&points, int &ipoints);
	// return 1 minus percentage of interest points that were matched
	static float CompareDescriptors(const float *descriptor1, int ipoints1, const float *descriptor2, int ipoints2);
	static float CompareDescriptors(const OpenSurfInterestPoint *points1, int ipoints1, const OpenSurfInterestPoint *points2, int ipoints2);

public:
	// load descriptor from disk
	static bool LoadDescriptor(const char *fname, float *&descriptor, int &ipoints);
	static bool LoadDescriptor(FILE *file, float *&, int &ipoints);
	static bool LoadDescriptor(const char *fname, OpenSurfInterestPoint *&points, int &ipoints);
	static bool LoadDescriptor(FILE *file, OpenSurfInterestPoint *&points, int &ipoints);
	// save descriptor to disk
	static bool SaveDescriptor(const char *fname, const float *descriptor, int ipoints);
	static bool SaveDescriptor(FILE *file, const float *descriptor, int ipoints);
	static bool SaveDescriptor(const char *fname, const OpenSurfInterestPoint *points, int ipoints);
	static bool SaveDescriptor(FILE *file, const OpenSurfInterestPoint *points, int ipoints);

private:
	int m_imagedim;
};

#endif
