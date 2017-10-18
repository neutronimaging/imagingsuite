//<LICENCE>

#ifndef MAPUPDATER_H
#define MAPUPDATER_H

#include <segmentation/multiresseg.h>
#include <image/image.h>
#include <map>

namespace Segmentation {
/** \brief Updates the pyramid levels by selecting teh class with the highest likelihood

@author Anders Kaestner
*/
template <class ImgType, class SegType, int NDim>
class LikelihoodUpdater : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	/** \brief Initializes the MAPUpdater
	
	\param NC Number of classes to find
	\param os stream for processing output
	*/
	LikelihoodUpdater(int NC=2,std::ostream &os=cout) :
	 SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		nClasses=NC;
		name="MAPUpdater";
	}
	
	/** \brief Process function that computes the classes of the child level based on statistics from the parent
	
	\param parent Graylevel image of the parent 
	\param parentSeg Segments of the parent
	\param child Graylevel image of the child
	\param childSeg output segments of the child level
	*/
	int update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg);
				
protected:

	
	/** \brief Support function the determines the most likely class membership
	\param val pixel value to classify
	\param apriori Apriori information of the parent class
	*/
	SegType GetClass(ImgType &val);
	

};

/** \brief Updates the pyramid levels by selecting the class with the highest likelihood. The likelihood is weighted by the apriori distribution of the parent voxel.

@author Anders Kaestner
*/
template<class ImgType, class SegType, int NDim>
class MAPUpdaterApriori : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterApriori(int NC=2, std::ostream & os=std::cout) : SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		nClasses=NC;
		name="MAPUpdaterApriori";
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
	/** \brief Support function the determines the most likely class membership
	\param val pixel value to classify
	\param apriori Apriori information of the parent class
	*/
	SegType GetClass(ImgType &val,SegType apriori);
};


/** \brief Updates the pyramid levels by selecting the class with the highest likelihood. The likelihood is weighted by the apriori distribution of the parent voxel.

@author Anders Kaestner
*/
template<class ImgType, class SegType, int NDim>
class MAPUpdaterNeighborhood : public MAPUpdaterApriori<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterNeighborhood(int NC=2, std::ostream & os=std::cout) :
		 MAPUpdaterApriori<ImgType,SegType,NDim>(NC,os)
	{
		name="MAPUpdaterNeighborhood";
	}
	
	/** \brief Process function that computes the classes of the child 
				level based on statistics from the parent
	\param parent Graylevel image of the parent 
	\param parentSeg Segments of the parent
	\param child Graylevel image of the child
	\param childSeg output segments of the child level
	*/
	int update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg);
	
	
};

/**	\brief Updates the pyramid levels by selecting the class with the highest likelihood. 
	*
	* The likelihood is weighted by the class probability of the available voters. 
	* The number of voters can be set to avoid multiclass ambiguities.
	* The class probability is determined from the parent level using a 8- or 26-neighborhood
	* for 2D and 3D images respectively.
	*
	* @author Anders Kaestner
	*/
template<class ImgType, class SegType, int NDim>
class MAPUpdaterNeighborhood2 : public SegmentUpdateBase<ImgType,SegType,NDim>
{
public:
	
	MAPUpdaterNeighborhood2(int NC=2, std::ostream & os=std::cout) :
		 SegmentUpdateBase<ImgType,SegType,NDim>(os)
	{
		nClasses=NC;
		nVotingClasses=2;
		name="MAPUpdaterNeighborhood2";
	}
	
	/** \brief Process function that computes the classes of the child 
				level based on statistics from the parent
	\param parent Graylevel image of the parent 
	\param parentSeg Segments of the parent
	\param child Graylevel image of the child
	\param childSeg output segments of the child level
	*/
	int update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg);
				
	/** \brief Set the number of voters to consider 
		\param n number of alowed voting classes
	*/
	int setVotingClasses(int n) {nVotingClasses=n; return 0;}
	
protected:
	/** \brief Gets the class of a voxel, given local class probabilities
	\param val intensity value of the pixel to classify
	\param cntMap list of present classes and their probabilities
	*/
	SegType GetClass(ImgType &val, std::map<SegType,unsigned short> &cntMap);
	/// Number of voting classes to consider
	int nVotingClasses;
};



	
template<class ImgType, class SegType, int NDim>
SegType LikelihoodUpdater<ImgType,SegType,NDim>::GetClass(ImgType &val)
{
	double maxVal=-1, tmp;
	SegType maxClass=-1, c;
	
	vector<double>::iterator meanIt, varIt, probIt;
	
	for (meanIt=mean.begin(), varIt=var.begin(), probIt=cnt.begin(),c=0; 
		meanIt!=mean.end(); 
		meanIt++, varIt++, probIt++, c++) {
		tmp=val-*meanIt;
		
		tmp=exp(-tmp*tmp*0.5/ *varIt)/(sqrt(2*pi* *varIt));
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}

template<class ImgType, class SegType, int NDim>
int LikelihoodUpdater<ImgType,SegType,NDim>::update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	for (p=0, z=0; z<dims[2]; z++) {
		for (y=0; y<dims[1]; y++) {
			for (x=0; x<dims[0]; x++,p++) {
				cl=GetClass(child[p]);
				childSeg[p]=cl;
			}
		}
	}
	
	return 0;
}


template<class ImgType, class SegType, int NDim>
SegType MAPUpdaterApriori<ImgType,SegType,NDim>::GetClass(ImgType &val,SegType apriori)
{
	double maxVal=-1, tmp;
	SegType maxClass=-1, c;
	
	vector<double>::iterator meanIt, varIt, probIt;
	
	
	tmp=val-mean[apriori];
	double w=exp(-tmp*tmp*0.5/ var[apriori])/(sqrt(2*pi* var[apriori]));
	
	for (meanIt=mean.begin(), varIt=var.begin(), probIt=cnt.begin(),c=0; 
		 meanIt!=mean.end(); 
		 meanIt++, varIt++, probIt++, c++) {
		 tmp=val-*meanIt;
		 
		 tmp=w*exp(-tmp*tmp*0.5/ *varIt)/(sqrt(2*pi* *varIt));
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}

template<class ImgType, class SegType, int NDim>
int MAPUpdaterApriori<ImgType,SegType,NDim>::update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	
	int cnt=0;
	const int *parentDims=parent.getDimsptr();
	
	for (p=0, z=0; z<dims[2]; z++) {
		z2=z/2; z2=z2<parentDims[2] ? z2 : parentDims[2]-1;
		for (y=0; y<dims[1]; y++) {
			y2=y/2; y2=y2<parentDims[1] ? y2 : parentDims[1]-1;
			for (x=0; x<dims[0]; x++,p++) {
				x2=x/2; x2=x2<parentDims[0] ? x2 : parentDims[0]-1;
				cl=GetClass(child[p],parentSeg(x2,y2,z2));
				childSeg[p]=cl;
			}
		}
	}

	return 0;
}

template<class ImgType, class SegType, int NDim>
int MAPUpdaterNeighborhood<ImgType,SegType,NDim>::update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	
	int cnt=0;
	const int *parentDims=parent.getDimsptr();
	int idxX[6]={-1,1,0,0,0,0};
	int idxY[6]={0,0,-1,1,0,0};
	int idxZ[6]={0,0,0,0,-1,1};
	int nx2,ny2,nz2;
	int i;
	bool edge;
	float sum;
	for (p=0, z=0; z<dims[2]; z++) {
		z2=z/2; z2=z2<parentDims[2] ? z2 : parentDims[2]-1;
		for (y=0; y<dims[1]; y++) {
			y2=y/2; y2=y2<parentDims[1] ? y2 : parentDims[1]-1;
			for (x=0; x<dims[0]; x++,p++) {
				x2=x/2; x2=x2<parentDims[0] ? x2 : parentDims[0]-1;
				edge=false;
				cl=parentSeg(x2,y2,z2);
				cnt=0;
				sum= 0.0f;
				for (i=0; i<2*NDim; i++) {
					nx2=x2+idxX[i]; 
					if (nx2<0)
						continue;
					else if (nx2>=parentDims[0])
						continue;
					
					ny2=y2+idxY[i]; 
					if (ny2<0)
						continue;
					else if (ny2>=parentDims[1])
						continue;
						
					nz2=z2+idxZ[i]; 
					if (nz2<0)
						continue;
					else if (nz2>=parentDims[2])
						continue;
					
					if (parentSeg(nx2,ny2,nz2)!=cl) 
						edge=true;

				}
				
				if (edge)
					cl=GetClass(child[p],cl);
					
				childSeg[p]=cl;
			}
		}
	}

	return 0;
}

template<class ImgType, class SegType, int NDim>
int MAPUpdaterNeighborhood2<ImgType,SegType,NDim>::update(const Image::CImage<ImgType, NDim> &parent, 
				const Image::CImage<SegType, NDim> & parentSeg, 
				const Image::CImage<ImgType, NDim> &child, 
				Image::CImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	
	int cnt=0;
	const int *parentDims=parent.getDimsptr();
	int nx2,ny2,nz2;
	
	int i,j,k;
	int startZ,stopZ;
	if (NDim==3) {
		startZ=-1;
		stopZ=1;
	}
	else {
		startZ=0;
		stopZ=0;
	}
	bool edge;
	float sum;
	map<SegType,unsigned short> cntMap;
	for (p=0, z=0; z<dims[2]; z++) {
		z2=z/2; z2=z2<parentDims[2] ? z2 : parentDims[2]-1;
		for (y=0; y<dims[1]; y++) {
			y2=y/2; y2=y2<parentDims[1] ? y2 : parentDims[1]-1;
			for (x=0; x<dims[0]; x++,p++) {
				x2=x/2; x2=x2<parentDims[0] ? x2 : parentDims[0]-1;
				cl=parentSeg(x2,y2,z2);
				cnt=0;
				sum= 0.0f;
				cntMap.clear();
				
				for (i=startZ; i<=stopZ; i++) {
					nz2=z2+i; 
					if (nz2<0)
						continue;
					else if (nz2>=parentDims[2])
						continue;
						
					for (j=-1; j<=1; j++) {
						ny2=y2+j; 
						if (ny2<0)
							continue;
						else if (ny2>=parentDims[1])
							continue;
						
						for (k=-1; k<=1; k++) {
							nx2=x2+k; 
							if (nx2<0)
								continue;
							else if (nx2>=parentDims[0])
								continue;
								
							cntMap[parentSeg(nx2,ny2,nz2)]++;
						}
					}
				}
				
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

bool pairCMPless(std::pair<const short,unsigned short> a,std::pair<const short ,unsigned short> b) {
	return a.second < b.second;
}

template<class ImgType, class SegType, int NDim>
SegType MAPUpdaterNeighborhood2<ImgType,SegType,NDim>::GetClass(ImgType &val, std::map<SegType,unsigned short> &cntMap)
{
	double maxVal=-1, tmp, vartmp;
	SegType maxClass=-1, c;
	
	typename std::map<SegType,unsigned short>::iterator cntIt;
	
	while (cntMap.size()>nVotingClasses) {
		cntIt=min_element(cntMap.begin(), cntMap.end(),  pairCMPless);
		cntMap.erase(cntIt);
	}
	
	for (cntIt=cntMap.begin(); cntIt!=cntMap.end(); cntIt++) {
		 c=cntIt->first;
		 tmp=val-mean[c];
		 vartmp=var[c];
		 
		 tmp=cntIt->second*exp(-tmp*tmp*0.5/ vartmp)/sqrt(vartmp); // Reduced form, scaling by a constant does not affect the decision
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}

}

#endif

// min_element( std::_Rb_tree_iterator<<>,<>,<>>, <<>, <>,<>> , <unknown type>)

