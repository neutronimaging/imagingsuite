//<LICENCE>

#ifndef MORPHOLOGYBASE3DSKELETON_H
#define MORPHOLOGYBASE3DSKELETON_H
#include <map>
#include <string>

#include "../base/timage.h"
#include "morphology.h"
#include "morphdist.h"

namespace kipl { namespace morphology {

/**
Base class for three dimensional skeletonization algorithms 

@author Anders Kaestner
*/
template<class ImgType>
class Base3Dskeleton{
public:
	/// Constructor doing general initializaion
    Base3Dskeleton();

    virtual ~Base3Dskeleton();
	/** \brief Interface function to use apply a skeleton algorithm on an image
		\param img Image to skeletonize, the result will be stored in the same image
		\param Quiet Switch to turn the processing output on or off
	*/
    virtual int process(base::TImage<ImgType,3> &img, bool Quiet=true)=0;
	/// Returns the name of the current skeletonization algorithm
	const string & getName() {return algorithm_name;}
protected:
	bool quiet;
	/** \brief Copies the neigborhood of a voxel into a 3x3x3 array
		\param img Source image
		\param p voxel reference in the image
		\param nb array containing the 3x3x3 neighborhood
	*/
	int CopyNeighborhoodInBuffer(base::TImage<ImgType,3> &img,
													  long p, 
													  ImgType nb[3][3][3]);

	const ImgType background;
	const ImgType nondeletable;
	const ImgType queued;
	const ImgType unqueued;
	const ImgType object;
	const ImgType border;
	const ImgType simple;
	
	CNeighborhood *N26;
	std::string algorithm_name;
};


template<class ImgType>
Base3Dskeleton<ImgType>::Base3Dskeleton() : background(0), nondeletable(1), queued(2), unqueued(3), object(1), border(4),simple(5)
{
	N26=NULL;
	algorithm_name="Base3Dskeleton";
}

template<class ImgType>
Base3Dskeleton<ImgType>::~Base3Dskeleton()
{
	if (N26)
		delete N26;
}

template<class ImgType>
int Base3Dskeleton<ImgType>::CopyNeighborhoodInBuffer(base::TImage<ImgType,3> &img,
													  long p, 
													  ImgType nb[3][3][3])
{
  	ImgType sum=0;
    ImgType nn[27]={0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0};

	int i;
	long pos;
	for (i=0; i<26; i++) 
		if ((pos=N26->neighbor(p,i))!=-1)
			if (img[pos]!=0) {
				nn[this->N26->index3x3x3(i)]=object;
				sum++;
			}
			else
				nn[this->N26->index3x3x3(i)]=background;
			
				
	nn[13]=img[p]!=0 ? object : background;
	
	short x,y,z;
	//ImgType n[3][3][3];
	for (z=0, i=0; z<3; z++) {
		for (y=0; y<3; y++) {
			for (x=0; x<3; x++,i++) {
				nb[z][y][x]=nn[i];
			}
		}
	}	
	return static_cast<int>(sum-nb[1][1][1]);
}

}}

#endif
