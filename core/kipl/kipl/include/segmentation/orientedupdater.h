

#ifndef __ORIENTEDUPDATER_H
#define __ORIENTEDUPDATER_H

#include "../segmentation/multiresseg.h"
#include "../segmentation/mapupdater.h"

namespace Segmentation {

template<class ImgType, class SegType, int NDim>
class OrientationUpdater : public MAPUpdaterNeighborhood<ImgType,SegType,NDim> {
public:
	OrientationUpdater(int NC=2, std::ostream & os=std::cout) : MAPUpdaterNeighborhood<ImgType,SegType,NDim>(NC,os)
	{
		// do something
		if (NDim!=3) {
			os<<"Warning, OrientationUpdater is not initialized as 3D"<<endl;
		}
	}
	/** \brief Process function that computes the classes of the child 
				level based on statistics from the parent
	
	\param parent Graylevel image of the parent 
	\param parentSeg Segments of the parent
	\param child Graylevel image of the child
	\param childSeg output segments of the child level
	*/
	virtual int update(const Image::CImage<ImgType, NDim> &parent, 
			const Image::CImage<SegType, NDim> & parentSeg, 
			const Image::CImage<ImgType, NDim> &child, 
			Image::CImage<SegType, NDim> &childSeg);

protected:
	int BuildFilterBank();
	int GetEdgeStatistics(int x2, int y2, int z2, const Image::CImage<SegType,NDim> & img, map<SegType,unsigned short> & cntMap);
	char butterflyX[5][5][5];
	char butterflyY[5][5][5];
	char butterflyZ[5][5][5];

	float grad[3][2];
};

template<class ImgType, class SegType, int NDim>
int OrientationUpdater<ImgType,SegType,NDim>::BuildFilterBank() 
{
	int x,y,z;


	for (y=0; y<5; y++) 
		for (x=0; x<5; x++) {
				// Planes -2,2
				butterflyX[x][y][0]= (abs(x-2)==2) && (abs(x-2)==2) ? 0 : 1;
				butterflyX[x][y][4]=butterflyX[x][y][0];
		
				// Planes -1,1
				butterflyX[x][y][1]= (abs(x-2)==2) || (abs(x-2)==2) ? 0 : 1;
				butterflyX[x][y][3]=butterflyX[x][y][0];

				// Plane 0
				butterflyX[x][y]= (x!=2) && (y!=2) ? 0 : 1;
		}

	// Copy X-butterfly to Y and Z
	for (z=0; z<5; z++) {
		for (y=0; y<5; y++) {
			for (x=0; x<5; x++) {
					butterflyY[x][y][z]=butterflyX[y][x][z];
					butterflyZ[x][y][z]=butterflyX[z][y][x];
			}
		}
	}

	// Initializing simple Jaehne gradient
	grad[0][0]=-3.0f/32.0f;
	grad[2][0]=grad[0][0];
	grad[0][1]=-grad[0][0];
	grad[2][1]=-grad[0][0];
	grad[1][0]=-10.0f/32.0f;
	grad[1][1]=grad[0][0];

	return 0;
}

template<class ImgType, class SegType, int NDim>
int OrientationUpdater<ImgType,SegType,NDim>::update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const unsigned int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	unsigned long p=0;
	long pos=0;
	int x,y,z;
	
	int cnt=0;
	const unsigned int *parentDims=parent.getDimsptr();
	Morphology::CNeighborhood ng(parentDims,3 ,Morphology::conn26);
	bool edge;
	map<SegType,unsigned short> cntMap;

	for(p=0,z=0; z<dims[2] ; z++){
		z2=(z/2*parentDims[0]*parentDims[1]);

		for (y=0; y<dims[1] ; y++) {
			y2=z2+y/2*parentDims[0];
			
			for (x=0; x<dims[0]; x++, p++) {
				x2=y2+x/2;
				
				GetEdgeStatistics(x2, y2, z2, parentSeg, cntMap);
				
				if (cntMap.size()>1)
					cl=GetClass(child[p],cntMap);
				else
					cl=cntMap.begin()->first;
				
				childSeg[p]=cl;
			}
		}
	}

	return 0;
}

template<class ImgType, class SegType, int NDim>
int OrientationUpdater<ImgType,SegType,NDim>::GetEdgeStatistics(int x2, int y2, int z2, const Image::CImage<SegType,NDim> & img, map<SegType,unsigned short> & cntMap)
{
	map<SegType,unsigned short> xMap,yMap,zMap;

	int x,y,z;
	SegType val;
	for (z=0; z<5; z++) {
		for (y=0; y<5; y++) {
			for (x=0; x<5; x++) {
					val=img(x2+x-2,y2+y-2,z2+z-2);
					if (butterflyX[x][y][z])
						xMap[val]++;
					if (butterflyY[x][y][z])
						yMap[val]++;
					if (butterflyZ[x][y][z])
						zMap[val]++;
			}
		}
	}
	
	typename map<SegType,unsigned short>::iterator maxposX,maxposY,maxposZ,maxP;

	maxposX=max_element(xMap.begin(),xMap.end(), pairCMPless);	
	maxposY=max_element(yMap.begin(), yMap.end(), pairCMPless);
	maxposZ=max_element(zMap.begin(), zMap.end(), pairCMPless);

	if (maxposX->second>=maxposY->second) {
		cntMap=xMap;
		maxP=maxposX;
	}
	else {
		cntMap=yMap;
		maxP=maxposY;
	}

	if (maxP->second<maxposZ->second)
		cntMap=zMap;

	return 0;
}

}
#endif

