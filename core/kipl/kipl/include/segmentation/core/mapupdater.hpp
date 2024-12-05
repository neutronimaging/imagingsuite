

#ifndef MAPUPDATER_HPP
#define MAPUPDATER_HPP

#include <map>

#include "../../base/timage.h"
#include "../../morphology/morphology.h"
#include "../../math/mathconstants.h"
//#include "../multiresseg.h"

namespace akipl { namespace segmentation {

template<class ImgType, class SegType, size_t NDim>
SegType LikelihoodUpdater<ImgType,SegType,NDim>::GetClass(ImgType &val)
{
	double maxVal=-1, tmp;
	
	SegType maxClass=-1, c;
	
	vector<double>::iterator meanIt, varIt, probIt;
	
	for (meanIt=this->mean.begin(), varIt=this->var.begin(), probIt=this->cnt.begin(),c=0; 
		meanIt!=this->mean.end(); 
		meanIt++, varIt++, probIt++, c++) {
		tmp=val-*meanIt;
		
		tmp=exp(-tmp*tmp*0.5/ *varIt)/(sqrt(2*fPi* *varIt));
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}

template<class ImgType, class SegType, size_t NDim>
int LikelihoodUpdater<ImgType,SegType,NDim>::update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const unsigned int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	unsigned int x,y,z;
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


template<class ImgType, class SegType, size_t NDim>
SegType MAPUpdaterApriori<ImgType,SegType,NDim>::GetClass(const ImgType val,const SegType apriori)
{
	double maxVal=-1;
	double tmp;
	SegType maxClass=-1, c;
	
	vector<double>::iterator meanIt, varIt, probIt;
	
	size_t aIndex=static_cast<size_t>(apriori);
	tmp=val-this->mean[aIndex];
	double w=exp(-tmp*tmp*0.5/ this->var[aIndex])/(sqrt(2*dPi* this->var[aIndex]));
	
	for (meanIt=this->mean.begin(), varIt=this->var.begin(), probIt=this->cnt.begin(),c=0; 
		 meanIt!=this->mean.end(); 
		 meanIt++, varIt++, probIt++, c++) {
		 tmp=val-*meanIt;
		 
		 tmp=w*exp(-tmp*tmp*0.5/ *varIt)/(sqrt(2*dPi* *varIt));
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}

template<class ImgType, class SegType, size_t NDim>
int MAPUpdaterApriori<ImgType,SegType,NDim>::update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	size_t const * const dims=child.Dims();
	
	childSeg.Resize(dims);
	size_t x2,y2,z2;
	SegType cl=0;
	size_t p=0;
	size_t x,y,z;
	
	size_t const * const parentDims=parent.Dims();
	size_t sxy=parentDims[0]*parentDims[1];
	
	for (p=0, z=0; z<dims[2]; z++) {
		z2=z/2; z2=z2<parentDims[2] ? z2 : parentDims[2]-1;
		for (y=0; y<dims[1]; y++) {
			y2=y/2; y2=y2<parentDims[1] ? y2 : parentDims[1]-1;
			for (x=0; x<dims[0]; x++,p++) {
				x2=x/2; x2=x2<parentDims[0] ? x2 : parentDims[0]-1;
				//cl=GetClass(child[p],parentSeg(x2,y2,z2));
				//todo fix GetClass
				//cl=GetClass(child[p],parentSeg[x2+y2*parentDims[0]+z2*sxy]);
				//childSeg[p]=cl;
			}
		}
	}

	return 0;
}

bool pairCMPless(std::pair<const short,unsigned short> a,std::pair<const short ,unsigned short> b) {
	return a.second < b.second;
}

template<class ImgType, class SegType, size_t NDim>
int MAPUpdaterNeighborhood<ImgType,SegType,NDim>::update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	size_t const * const dims=child.Dims();
	
	childSeg.Resize(dims);
	int x2,y2,z2;
	
	unsigned long p=0;
	size_t pos=0;
	size_t x,y,z;
	SegType cl=0;
	
	size_t const * const parentDims=parent.Dims();
	kipl::morphology::CNeighborhood ng(parentDims,3 ,kipl::morphology::conn26);

	map<SegType,unsigned short> cntMap;
	ImgType const * const pChild=child.GetDataPtr();
	for(p=0,z=0; z<dims[2] ; z++){
		z2=(z/2*parentDims[0]*parentDims[1]);

		for (y=0; y<dims[1] ; y++) {
			y2=z2+y/2*parentDims[0];
			
			for (x=0; x<dims[0]; x++, p++) {
				x2=y2+x/2;
				cntMap.clear();
				for (int i=0 ; i<ng.N(); i++) {
					cl=parentSeg[x2];
					cntMap[cl]++;
					if ((pos=ng.neighbor(x2,i))>-1) 
						cntMap[parentSeg[pos]]++;
				}
// todo fix GetClass				
				if (cntMap.size()>1)
					//cl=GetClass(child[p],cntMap);
					cl=GetClass(pChild[p],cntMap);

				childSeg[p]=cl;
			}
		}
	}

	return 0;
}

template<class ImgType, class SegType, size_t NDim>
SegType MAPUpdaterNeighborhood<ImgType,SegType,NDim>::GetClass(const ImgType val, std::map<SegType,unsigned short> &cntMap)
{
	//double maxVal=-1, tmp, vartmp;
	SegType maxClass=-1, c;
	
	typename std::map<SegType,unsigned short>::iterator cntIt;
	
	cntIt=std::max_element(cntMap.begin(), cntMap.end(),  pairCMPless);
	c=cntIt->first;
	maxClass=this->MAPUpdaterApriori<ImgType,SegType,NDim>::GetClass(val,c);
	
	return maxClass;
}

template<class ImgType, class SegType, size_t NDim>
int MAPUpdaterNeighborhood2<ImgType,SegType,NDim>::update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);
	
	const unsigned int *dims=child.getDimsptr();
	
	childSeg.resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	
	int cnt=0;
	const unsigned int *parentDims=parent.getDimsptr();
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



template<class ImgType, class SegType, size_t NDim>
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
		 tmp=val-this->mean[c];
		 vartmp=this->var[c];
		 // Reduced form, scaling by a constant does not affect the decision
		 tmp=cntIt->second*exp(-tmp*tmp*0.5/ vartmp)/sqrt(vartmp); 
		
		if (tmp>maxVal) {
			maxVal=tmp;
			maxClass=c;
		}
	}
	
	return maxClass;
}


template<class ImgType, class SegType, size_t NDim>
int LaplacianUpdater<ImgType,SegType,NDim>::update(const kipl::base::TImage<ImgType, NDim> &parent, 
				const kipl::base::TImage<SegType, NDim> & parentSeg, 
				const kipl::base::TImage<ImgType, NDim> &child, 
				kipl::base::TImage<SegType, NDim> &childSeg)
{
	ComputeClassStatistics(parent,parentSeg);

  	size_t const * const dims=child.Dims();
	
	childSeg.Resize(dims);
	int x2,y2,z2;
	SegType cl;
	long p=0;
	int x,y,z;
	
	int cnt=0;
	size_t const * const parentDims=parent.Dims();
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
				
        if (cntMap.size()>1) {
          cl=GetClass(cntMap,Laplacian(child,x,y,z,dims));
        }
				else
					cl=cntMap.begin()->first;
					
				childSeg[p]=cl;
			}
		}
	}

	return 0;
}

template<class ImgType, class SegType, size_t NDim>
ImgType LaplacianUpdater<ImgType,SegType,NDim>::Laplacian(const kipl::base::TImage<ImgType, NDim> &img,int X, int Y, int Z, size_t const * const dims)
{
  if ((X==0) || (Y==0) || (Z==0))
    return (ImgType)0;

  if ((X>=dims[0]-1) || (Y>=dims[1]-1) || (Z>=dims[2]-1))
    return (ImgType)0;

  const ImgType k[3][3][3]={{{1,2,1},{2,4,2},{1,2,1}},
                    {{2,4,2},{4,-56,4},{2,4,2}},
                    {{1,2,1},{2,4,2},{1,2,1}}};

  
  ImgType sum=(ImgType)0;
  for (int z=0; z<3; z++)
    for (int y=0; y<3; y++)
      for (int x=0; x<3; x++) {
        sum+=k[z][x][y]*img(X+x-1,Y+y-1,Z+z-1);
      }

  return sum;
}
template<class ImgType, class SegType, size_t NDim>
SegType LaplacianUpdater<ImgType,SegType,NDim>::GetClass(std::map<SegType,unsigned short> &cntMap,ImgType laplacian)
{
  typename std::map<SegType,unsigned short>::iterator cntIt;
	while (cntMap.size()>2) {
		cntIt=min_element(cntMap.begin(), cntMap.end(),  pairCMPless);
		cntMap.erase(cntIt);
	}

  if (laplacian>0) {
    if (this->mean[cntMap.begin()->first]<this->mean[cntMap.rbegin()->first])
      return cntMap.begin()->first;
    else
      return cntMap.rbegin()->first;
  }
  else {
    if (this->mean[cntMap.begin()->first]<this->mean[cntMap.rbegin()->first])
      return cntMap.rbegin()->first;
    else
      return cntMap.begin()->first;
  }

}
}}

#endif
