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

#ifndef KDTREE_H
#define KDTREE_H

#include <algorithm>
#include <map>
#include <stack>
#include <cassert>
using namespace std;

#include "heap.h"
#include "common.h"
#include "constants.h"
#include "allocator.h"
#include "dataset.h"
#include "resultset.h"
#include "random.h"
#include "nnindex.h"
#include "flann.h"

/**
 * Randomized kd-tree index
 *
 * Contains the k-d trees and other information for indexing a set of points
 * for nearest-neighbor matching.
 */
class KDTree : public NNIndex
{
	enum {
		/**
		 * To improve efficiency, only SAMPLE_MEAN random values are used to
		 * compute the mean and variance at each level when building a tree.
		 * A value of 100 seems to perform as well as using all values.
		 */
		SAMPLE_MEAN = 100,
		/**
		 * Top random dimensions to consider
		 *
		 * When creating random trees, the dimension on which to subdivide is
		 * selected at random from among the top RAND_DIM dimensions with the
		 * highest variance.  A value of 5 works well.
		 */
		RAND_DIM=5
	};
	/**
	 * Number of randomized trees that are used
	 */
	int numTrees;
	/**
	 *  Array of indices to vectors in the dataset.  When doing lookup,
	 *  this is used instead to mark checkID.
	 */
	int* vind;
	/**
	 * An unique ID for each lookup.
	 */
	int checkID;
	/**
	 * The dataset used by this index
	 */
	Dataset<float>& dataset;
    int size_;
    int veclen_;
    float* mean;
    float* var;
	/*--------------------- Internal Data Structures --------------------------*/
	/**
	 * A node of the binary k-d tree.
	 *
	 *  This is   All nodes that have vec[divfeat] < divval are placed in the
	 *   child1 subtree, else child2., A leaf node is indicated if both children are NULL.
	 */
	struct TreeSt {
		/**
		 * Index of the vector feature used for subdivision.
		 * If this is a leaf node (both children are NULL) then
		 * this holds vector index for this leaf.
		 */
		int divfeat;
		/**
		 * The value used for subdivision.
		 */
		float divval;
		/**
		 * The child nodes.
		 */
		TreeSt *child1, *child2;
	};
	typedef TreeSt* Tree;
    /**
     * Array of k-d trees used to find neighbors.
     */
    Tree* trees;
    typedef BranchStruct<Tree> BranchSt;
    typedef BranchSt* Branch;
    /**
     * Priority queue storing intermediate branches in the best-bin-first search
     */
    Heap<BranchSt>* heap;
	/**
	 * Pooled memory allocator.
	 *
	 * Using a pooled memory allocator is more efficient
	 * than allocating memory directly when there is a large
	 * number small of memory allocations.
	 */
	PooledAllocator pool;

public:
    flann_algorithm_t getType() const;

	/**
	 * KDTree constructor
	 *
	 * Params:
	 * 		inputData = dataset with the input features
	 * 		params = parameters passed to the kdtree algorithm
	 */
	KDTree(Dataset<float>& inputData, Params params);
	// BT: alternative constructor
	KDTree(Dataset<float>& inputData, const FLANNParameters &params);
	/**
	 * Standard destructor
	 */
	~KDTree();
	/**
	 * Builds the index
	 */
	void buildIndex();
	// BT: save the index
	bool saveIndex(FILE *file) const;
	// BT: load the index
	bool loadIndex(FILE *file);
    /**
    *  Returns size of index.
    */
    int size() const;
    /**
    * Returns the length of an index feature.
    */
    int veclen() const;
	/**
	 * Computes the inde memory usage
	 * Returns: memory used by the index
	 */
	int usedMemory() const;
    /**
     * Find set of nearest neighbors to vec. Their indices are stored inside
     * the result object.
     *
     * Params:
     *     result = the result object in which the indices of the nearest-neighbors are stored
     *     vec = the vector for which to search the nearest neighbors
     *     maxCheck = the maximum number of restarts (in a best-bin-first manner)
     */
    void findNeighbors(ResultSet& result, float* vec, Params searchParams);
	void continueSearch(ResultSet& result, float* vec, int maxCheck);
    Params estimateSearchParams(float precision, Dataset<float>* testset = NULL);
private:
	/**
	 * Create a tree node that subdivides the list of vecs from vind[first]
	 * to vind[last].  The routine is called recursively on each sublist.
	 * Place a pointer to this new tree node in the location pTree.
	 *
	 * Params: pTree = the new node to create
	 * 			first = index of the first vector
	 * 			last = index of the last vector
	 */
	void divideTree(Tree* pTree, int first, int last);
	/**
	 * Choose which feature to use in order to subdivide this set of vectors.
	 * Make a random choice among those with the highest variance, and use
	 * its variance as the threshold value.
	 */
	void chooseDivision(Tree node, int first, int last);
	/**
	 * Select the top RAND_DIM largest values from v and return the index of
	 * one of these selected at random.
	 */
	int selectDivision(float* v);
	/**
	 *  Subdivide the list of exemplars using the feature and division
	 *  value given in this node.  Call divideTree recursively on each list.
	*/
	void subdivide(Tree node, int first, int last);
	/**
	 * Performs an exact nearest neighbor search. The exact search performs a full
	 * traversal of the tree.
	 */
	void getExactNeighbors(ResultSet& result, float* vec);
	/**
	 * Performs the approximate nearest-neighbor search. The search is approximate
	 * because the tree traversal is abandoned after a given number of descends in
	 * the tree.
	 */
	void getNeighbors(ResultSet& result, float* vec, int maxCheck);
	/**
	 *  Search starting from a given node of the tree.  Based on any mismatches at
	 *  higher levels, all exemplars below this level must have a distance of
	 *  at least "mindistsq".
	*/
	void searchLevel(ResultSet& result, float* vec, Tree node, float mindistsq, int& checkCount, int maxCheck);
	/**
	 * Performs an exact search in the tree starting from a node.
	 */
	void searchLevelExact(ResultSet& result, float* vec, Tree node, float mindistsq);
};   // class KDTree

register_index(KDTREE,KDTree)

#endif //KDTREE_H
