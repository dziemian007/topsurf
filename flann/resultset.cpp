/*
Copyright 2008-2009  Marius Muja (mariusm@cs.ubc.ca). All rights reserved.
Copyright 2008-2009  David G. Lowe (lowe@cs.ubc.ca). All rights reserved.

THE BSD LICENSE

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "resultset.h"

KNNResultSet::KNNResultSet(int capacity_, float* target_ /*= NULL*/, int veclen_ /*= 0*/) :
ResultSet(target_, veclen_), capacity(capacity_), count(0)
{
	indices = new int[capacity_];
	dists = new float[capacity_];
}

KNNResultSet::~KNNResultSet()
{
	delete[] indices;
	delete[] dists;
}

void KNNResultSet::init(float* target_, int veclen_)
{
	target = target_;
	veclen = veclen_;
	target_end = target + veclen;
	count = 0;
}


int* KNNResultSet::getNeighbors()
{
	return indices;
}

float* KNNResultSet::getDistances()
{
	return dists;
}

int KNNResultSet::size() const
{
	return count;
}

bool KNNResultSet::full() const
{
	return count == capacity;
}


bool KNNResultSet::addPoint(float* point, int index)
{
	for (int i=0;i<count;++i) {
		if (indices[i]==index) return false;
	}
	float dist = flann_dist(target, target_end, point);

	if (count<capacity) {
		indices[count] = index;
		dists[count] = dist;
		++count;
	}
	else if (dist < dists[count-1] || (dist == dists[count-1] && index < indices[count-1])) {
		//         else if (dist < dists[count-1]) {
		indices[count-1] = index;
		dists[count-1] = dist;
	}
	else {
		return false;
	}

	int i = count-1;
	// bubble up
	while (i>=1 && (dists[i]<dists[i-1] || (dists[i]==dists[i-1] && indices[i]<indices[i-1]) ) ) {
		//         while (i>=1 && (dists[i]<dists[i-1]) ) {
		swap(indices[i],indices[i-1]);
		swap(dists[i],dists[i-1]);
		i--;
	}

	return true;
}

float KNNResultSet::worstDist() const
{
	// BT: collision of 'max' between the numeric_limits version and the one
	//     defined in windows.h
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
#pragma push_macro("max")
#undef max
	float result;
	if (count < capacity)
		result = numeric_limits<float>::max();
	else
		result = dists[count-1];
#pragma pop_macro("max")
	return result;
#else
	return (count<capacity) ? numeric_limits<float>::max() : dists[count-1];
#endif
}

/**
* A result-set class used when performing a radius based search.
*/
RadiusResultSet::RadiusResultSet(float radius_) :
radius(radius_), indices(NULL), dists(NULL)
{
	sorted = false;
	items.reserve(16);
	count = 0;
}

RadiusResultSet::~RadiusResultSet()
{
	if (indices!=NULL) delete[] indices;
	if (dists!=NULL) delete[] dists;
}

void RadiusResultSet::init(float* target_, int veclen_)
{
	target = target_;
	veclen = veclen_;
	target_end = target + veclen;
	items.clear();
	sorted = false;
}

int* RadiusResultSet::getNeighbors()
{
	if (!sorted) {
		sorted = true;
		sort_heap(items.begin(), items.end());
	}
	resize_vecs();
	for (int i=0;i<items.size();++i) {
		indices[i] = items[i].index;
	}
	return indices;
}

float* RadiusResultSet::getDistances()
{
	if (!sorted) {
		sorted = true;
		sort_heap(items.begin(), items.end());
	}
	resize_vecs();
	for (int i=0;i<items.size();++i) {
		dists[i] = items[i].dist;
	}
	return dists;
}

int RadiusResultSet::size() const
{
	return items.size();
}

bool RadiusResultSet::full() const
{
	return true;
}

bool RadiusResultSet::addPoint(float* point, int index)
{
	Item it;
	it.index = index;
	it.dist = flann_dist(target, target_end, point);
	if (it.dist<=radius) {
		items.push_back(it);
		push_heap(items.begin(), items.end());
		return true;
	}
	return false;
}

float RadiusResultSet::worstDist() const
{
	return radius;
}

void RadiusResultSet::resize_vecs()
{
	if (items.size()>count) {
		if (indices!=NULL) delete[] indices;
		if (dists!=NULL) delete[] dists;
		count = items.size();
		indices = new int[count];
		dists = new float[count];
	}
}
