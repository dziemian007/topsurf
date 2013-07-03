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

#ifndef RESULTSET_H
#define RESULTSET_H

#include <algorithm>
#include <limits>
#include <vector>
using namespace std;

#include "dist.h"

/* This record represents a branch point when finding neighbors in
	the tree.  It contains a record of the minimum distance to the query
	point, as well as the node at which the search resumes.
*/

template <typename T>
struct BranchStruct {
	T node;           /* Tree node at which search resumes */
	float mindistsq;     /* Minimum distance to query for all nodes below. */

	bool operator<(const BranchStruct<T>& rhs)
	{
        return mindistsq<rhs.mindistsq;
	}

    static BranchStruct<T> make_branch(T aNode, float dist)
    {
        BranchStruct<T> branch;
        branch.node = aNode;
        branch.mindistsq = dist;
        return branch;
    }
};


class ResultSet
{
protected:
	float* target;
	float* target_end;
    int veclen;

public:

	ResultSet(float* target_ = NULL, int veclen_ = 0) :
	  target(target_), veclen(veclen_) { target_end = target + veclen; }
	virtual ~ResultSet() {}
	virtual void init(float* target_, int veclen_) = 0;
	virtual int* getNeighbors() = 0;
	virtual float* getDistances() = 0;
	virtual int size() const = 0;
	virtual bool full() const = 0;
	virtual bool addPoint(float* point, int index) = 0;
	virtual float worstDist() const = 0;
};

class KNNResultSet : public ResultSet
{
	int* indices;
	float* dists;
    int capacity;

	int count;

public:
	KNNResultSet(int capacity_, float* target_ = NULL, int veclen_ = 0 );
	virtual ~KNNResultSet();
	void init(float* target_, int veclen_);
	int* getNeighbors();
    float* getDistances();
    int size() const;
	bool full() const;
	bool addPoint(float* point, int index);
	float worstDist() const;
};

/**
 * A result-set class used when performing a radius based search.
 */
class RadiusResultSet : public ResultSet
{
	struct Item {
		int index;
		int dist;

		bool operator<(Item rhs) {
			return dist<rhs.dist;
		}
	};

	vector<Item> items;
	float radius;

	bool sorted;
	int* indices;
	float* dists;
	int count;

public:
	RadiusResultSet(float radius_);
	~RadiusResultSet();
	void init(float* target_, int veclen_);
	int* getNeighbors();
    float* getDistances();
    int size() const;
	bool full() const;
	bool addPoint(float* point, int index);
	float worstDist() const;

private:
	void resize_vecs();
};


#endif //RESULTSET_H
