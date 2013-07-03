/*********************************************************** 
*  --- OpenSURF ---                                        *
*  This library is distributed under the GNU GPL. Please   *
*  contact chris.evans@irisys.co.uk for more information.  *
*                                                          *
*  C. Evans, Research Into Robust Visual Features,         *
*  MSc University of Bristol, 2008.                        *
*                                                          *
************************************************************/

#ifndef _OPENSURF_IPOINTH
#define _OPENSURF_IPOINTH

#include "opencv/cv.h"

#include <vector>
using namespace std;

#define OPENSURF_FEATURECOUNT	64

//-------------------------------------------------------

class OpenSurfInterestPoint; // Pre-declaration
typedef std::vector<OpenSurfInterestPoint> IpVec;
typedef std::vector<std::pair<OpenSurfInterestPoint, OpenSurfInterestPoint> > IpPairVec;
typedef std::vector<std::pair<const OpenSurfInterestPoint *, const OpenSurfInterestPoint *> > IpPairVecConst;

//-------------------------------------------------------

//! OpenSurfInterestPoint operations
void getMatches(IpVec &ipts1, IpVec &ipts2, IpPairVec &matches);
void getMatches(IpVec &ipts1, IpVec &ipts2, IpPairVecConst &matches);
int translateCorners(IpPairVec &matches, const CvPoint src_corners[4], CvPoint dst_corners[4]);

//-------------------------------------------------------

class OpenSurfInterestPoint {

public:

	//! Destructor
	~OpenSurfInterestPoint() {};

	//! Constructor
	OpenSurfInterestPoint() : orientation(0) {};

	//! Gets the distance in descriptor space between Ipoints
	float operator-(const OpenSurfInterestPoint &rhs)
	{
		float sum=0.f;
		for(int i=0; i < OPENSURF_FEATURECOUNT; ++i)
			sum += (this->descriptor[i] - rhs.descriptor[i])*(this->descriptor[i] - rhs.descriptor[i]);
		return sqrt(sum);
	};

	//! Coordinates of the detected interest point
	float x, y;

	//! Detected scale
	float scale;

	//! Orientation measured anti-clockwise from +ve x-axis
	float orientation;

	//! Sign of laplacian for fast matching purposes
	int laplacian;

	//! Vector of descriptor components
	float descriptor[OPENSURF_FEATURECOUNT];

	//! Placeholds for point motion (can be used for frame to frame motion analysis)
	float dx, dy;
};

//-------------------------------------------------------


#endif
