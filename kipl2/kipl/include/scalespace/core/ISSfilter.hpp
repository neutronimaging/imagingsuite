//<LICENCE>

#ifndef ISSFILTER_HPP_
#define ISSFILTER_HPP_
#include <iomanip>
#include <omp.h>

#include "../../base/timage.h"
#include "../../base/imageoperators.h"
#include "../../morphology/morphfilters.h"
#include "../../math/mathfunctions.h"


namespace akipl { namespace scalespace {

template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::Process(kipl::base::TImage<ImgType,NDims> &img, double dTau, double dLambda, double dAlpha, int nN)
{
    if (error!=NULL)
        delete [] error;
    
    error=new ImgType[nN];
	m_f=img;
	m_f.Clone();

	m_dTau=dTau; 
	m_dLambda=dLambda;
	m_dAlpha=dAlpha;
	m_v.Resize(img.Dims());
	m_v=static_cast<ImgType>(0);

	for (int i=0; i<nN	; i++) {
		if (i%10==9)
			logstream<<std::setw(4)<<i<<endl<<flush;
		else
			logstream<<std::setw(4)<<i<<", "<<flush;
	
		error[i]=_SolveIteration(img);
		
//		if (isnan(error[i]))
//			throw kipl::base::KiplException("A NaN occured in the process.",__FILE__,__LINE__);
//		if (isinf(error[i]))
//			throw kipl::base::KiplException("An inf occured in the process.",__FILE__,__LINE__);		
	}
	logstream<<endl;
	
	return nN;
}

template <typename ImgType, size_t NDims>
ImgType ISSfilter<ImgType, NDims>::_SolveIteration(kipl::base::TImage<ImgType,NDims> &img)
{
	kipl::base::TImage<ImgType,NDims> p;

	_P(img,p);

    long long int i;
    double sum2=0.0;
    #pragma omp parallel private(i) reduction(+:sum2)
    {
        long long int N=img.Size();
        ImgType tempFU;
        ImgType fTau=m_dTau;
        ImgType dTauAlpha=m_dTau*m_dAlpha;
        ImgType *pU=img.GetDataPtr();
        ImgType *pV=m_v.GetDataPtr();
        ImgType *pP=p.GetDataPtr();
    	ImgType *pF=m_f.GetDataPtr();
        double diff=0.0;
        #pragma omp for schedule(guided,1000)
        for (i=0; i<N ; i++) {
            tempFU=pF[i]-pU[i];
            ImgType prev=pU[i];
            pU[i]+=(ImgType)(fTau*(pP[i]+m_dLambda*(tempFU+pV[i])));
            pV[i]+=(ImgType)(dTauAlpha*tempFU);
            diff=static_cast<double>(pU[i]-prev);
            sum2+=diff*diff;
        }
    }

	return sum2;
}

template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::_LeadDiff(kipl::base::TImage<ImgType,NDims> &img, 
                                  	kipl::base::TImage<ImgType,NDims> &diff, 
									Direction dir)
{
//	ImgType se[3]={1,1,0};
//	size_t sedims[3][3]={{3,1,1},{1,3,1},{1,1,3}};
//	kipl::morphology::TErode<ImgType,NDims> erodeX(se,sedims[0]);
//	kipl::morphology::TErode<ImgType,NDims> erodeY(se,sedims[1]);
//	kipl::morphology::TErode<ImgType,NDims> erodeZ(se,sedims[2]);
	
	diff.Resize(img.Dims());
	diff=static_cast<ImgType>(0);
    ImgType *pImg0=img.GetDataPtr();
    ImgType *pDiff0=diff.GetDataPtr();
    size_t const * const dims=img.Dims();
    long long int sx=dims[0]-1;	
    long long int sy=dims[1]-1;	
    long long int sz=dims[2]-1;
    long long int sxy=dims[0]*dims[1];
        
    long long int dx=dims[0];
    long long int dy=dims[1];
    long long int dz=dims[2];
        
    #pragma omp parallel firstprivate(pImg0,pDiff0,sx,sy,sz,sxy, dx,dy,dz)
    {
        ImgType *pDiff;
        
        ImgType *pA, *pB;
        long long int x,y,z;
        {
            long long int dx=dims[0];
            long long int dy=dims[1];
            long long int dz=dims[2];
            
            switch (dir) {
                case dirX : 
                  #pragma omp for 
                  for (z=0; z<dz; z++) {
                          for (y=0; y<dy; y++) {
                              size_t pos=y*dx+z*sxy;
                              pA=pImg0+pos;
                              pB=pA+1;
                              pDiff=pDiff0+pos;
                             
                              for (x=0; x<sx; x++) {
                                  pDiff[x]=pB[x]-pA[x];
                              }
                              pDiff[sx]=0;	
                    }
                  }
               
                break;
                case dirY :
                    #pragma omp for
                    for (z=0; z<dz; z++) {
                      for (y=0; y<sy; y++) {
                          size_t pos=y*dx+z*sxy;
                          pA=pImg0+pos;
                          pB=pA+dx;
                          pDiff=pDiff0+pos;
                          for (x=0; x<dx; x++) 
                              pDiff[x]=pB[x]-pA[x];				
                      }
                      memset(pDiff0+sy*dx+z*sxy,0,sizeof(ImgType)*dims[0]);
                    }

                    break;
              case dirZ :
                  memset(pDiff0+sz*sxy,0,sizeof(ImgType)*sxy);
                  #pragma omp for
                  for (z=0; z<sz; z++) {
                      size_t pos=z*sxy;
                      pA=pImg0+pos;
                      pB=pA+sxy;
                      pDiff=pDiff0+pos;
                    
                    for (x=0; x<sxy; x++) {
                      pDiff[x]=pB[x]-pA[x];
                    }
                  }

                  break;
            }
        }
    }
	return 0;
}

template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::_LagDiff( kipl::base::TImage<ImgType,NDims> &img, 
                                  kipl::base::TImage<ImgType,NDims> &diff,
								  typename ISSfilter<ImgType,NDims>::Direction dir)
{
	diff.Resize(img.Dims());
	diff=static_cast<ImgType>(0);
    ImgType * pImg0=img.GetDataPtr();
    ImgType * pDiff0=diff.GetDataPtr();
    #pragma omp parallel firstprivate(pImg0,pDiff0)
    {
        ImgType *pDiff;        
        ImgType *pA, *pB; 
                
        long long int x,y,z;

        size_t const * const  dims=img.Dims();
        
        long long int dx=dims[0];
        long long int dy=dims[1];
        long long int dz=dims[2];
        long long int sxy=dims[0]*dims[1];

        switch (dir) {
            case dirX :
              memset(pDiff0,0,diff.Size()*sizeof(ImgType));
              #pragma omp for
              for (z=0; z<dz; z++) {
                for (y=0; y<dy; y++) {
                  size_t pos=y*dx+z*sxy;
                  pB=pImg0+pos;
                  pA=pB-1;
                  pDiff=pDiff0+pos;
                  
                  pDiff[0]=(ImgType)0;
                  for (x=1; x<dx; x++) {
                      pDiff[x]=pB[x]-pA[x];
                  }
                }
              }
              break;

            case dirY :
                #pragma omp for
                for (z=0; z<dz; z++) {
                  memset(pDiff0+z*sxy,0,sizeof(ImgType)*dx);
                  
                  for (y=1; y<dy; y++) {
                      size_t pos=y*dx+z*sxy;
                      
                      pB=pImg0+pos;
                      pA=pB-dx;
                      pDiff=pDiff0+pos;
                      for (x=0; x<dx; x++) {
                          pDiff[x]=pB[x]-pA[x];
                      }
                  }
          }
                break;
        case dirZ :
          memset(pDiff0,0,sizeof(ImgType)*sxy);
          #pragma omp for          
          for (z=1; z<dz; z++) {
              size_t pos=z*sxy;
              pB=pImg0+pos;
              pA=pB-sxy;
              pDiff=pDiff0+pos;

            for (x=0; x<sxy; x++) {
                *pDiff=*pB-*pA;
                pDiff++; pB++; pA++;
            }
          }
          break;
        }
    }
	return 0;
}
	
template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::_P(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<ImgType,NDims> &res)
{
	logstream<<m_dEpsilon<<endl;
    kipl::base::TImage<ImgType,NDims> dx,dy,dz, d2x, d2y,d2z;
	const long long int N=img.Size();
	
	_LeadDiff(img,dx,dirX);
//	_LagRegularize(&dx,dirX);
	_LeadDiff(img,dy,dirY);
//	_LagRegularize(&dy,dirY);
	
	if (NDims==3) {
		_LeadDiff(img,dz,dirZ);
//		_LagRegularize(&dz,dirZ);
	}
	
	
	ImgType denom;
	if (NDims==3) {
        ImgType *pX=dx.GetDataPtr();
        ImgType *pY=dy.GetDataPtr();
        ImgType *pZ=dz.GetDataPtr();
        ImgType epsilon=m_dEpsilon;
        
        long long int i;
        #pragma omp parallel private(i,denom) firstprivate(pX,pY,pZ, epsilon)
        {
            long long int nn=N;
            
            #pragma omp for schedule(guided,1000)
            for (i=0; i< nn; i++) {
            	denom=(ImgType)(1/sqrt(pX[i]*pX[i]+pY[i]*pY[i]+pZ[i]*pZ[i]+epsilon));

            	pX[i]*=denom;
                pY[i]*=denom;
                pZ[i]*=denom;
            }
        }
	}
	else {
        ImgType *pX=dx.GetDataPtr();
        ImgType *pY=dy.GetDataPtr();

		for (size_t i=0; i< N; i++) {
			denom=(ImgType)(1/sqrt(pX[i]*pX[i]+pY[i]*pY[i]+m_dEpsilon));
			pX[i]*=denom;
			pY[i]*=denom;
		}
	}

	d2x.Resize(dx.Dims());
	_LagDiff(dx,d2x,dirX);
//	_LeadRegularize(&d2x,dirX);
	
	
	d2y.Resize(dy.Dims());
	_LagDiff(dy,d2y,dirY);
//	_LeadRegularize(&d2y,dirX);

	if (NDims==3) {
		d2z.Resize(dz.Dims());
		_LagDiff(dz,d2z,dirZ);
//		_LeadRegularize(&d2y,dirX);
	}
	res.Resize(img.Dims());
	res=static_cast<ImgType>(0);
	
	if (NDims==3) {
        long long int i;
        ImgType *pX=d2x.GetDataPtr();
        ImgType *pY=d2y.GetDataPtr();
        ImgType *pZ=d2z.GetDataPtr();
        long long int nn=N;
        ImgType *pRes=res.GetDataPtr();
        #pragma omp parallel private(i) firstprivate(pX,pY,pZ, pRes,nn)
        {
            #pragma omp for 
            for (i=0; i< nn; i++) {
                pRes[i]=0.5*(pX[i]+pY[i]+pZ[i]);
            }
        }
	}
	else {
        ImgType *pX=d2x.GetDataPtr();
        ImgType *pY=d2y.GetDataPtr();

        ImgType *pRes=NULL;
		for (size_t i=0; i< N; i++) 
			pRes[i]=pX[i]+pY[i];
	}
  
	return 0;
}

template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::_LeadRegularize(kipl::base::TImage<ImgType,NDims> *img, Direction dir)
{
	ImgType *pA=NULL;
	ImgType *pB=NULL;
	
	size_t const * const dims=img->Dims();
	size_t sxy=dims[0]*dims[1];
	
	switch (dir) {
	case dirX :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<dims[1]; y++) {
				pA=img->GetLinePtr(y,z);
				
				for (size_t x=0; x<(dims[0]-1); x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pA[x+1]));
				}
			}
		}
		break;
	case dirY :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<(dims[1]-1); y++) {
				pA=img->GetLinePtr(y,z);
				pB=img->GetLinePtr(y+1,z);
				for (size_t x=0; x<dims[0]; x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pB[x]));
				}
			}
		}
		break;
	case dirZ :
		
		for (size_t z=0; z<(dims[2]-1); z++) {
			pA=img->GetLinePtr(0,z);
			pB=img->GetLinePtr(0,z+1);
			
			for (size_t i=0; i<sxy; i++) {
					pA[i]=kipl::math::sign(pA[i])*min(std::abs(pA[i]),std::abs(pB[i]));
				}
			}
		
		break;
	}
	
	return 0;
}


template <typename ImgType, size_t NDims>
int ISSfilter<ImgType, NDims>::_LagRegularize(kipl::base::TImage<ImgType,NDims> *img, Direction dir)
{
	ImgType *pA=NULL;
	ImgType *pB=NULL;
	
	size_t const * const dims=img->Dims();
	size_t sxy=dims[0]*dims[1];
	
	switch (dir) {
	case dirX :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<dims[1]; y++) {
				pA=img->GetLinePtr(y,z)+1;
				
				for (size_t x=0; x<(dims[0]-1); x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pA[x-1]));
				}
			}
		}
		break;
	case dirY :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<(dims[1]-1); y++) {
				pA=img->GetLinePtr(y+1,z);
				pB=img->GetLinePtr(y,z);
				for (size_t x=0; x<dims[0]; x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pB[x]));
				}
			}
		}
		break;
	case dirZ :
		
		for (size_t z=0; z<(dims[2]-1); z++) {
			pA=img->GetLinePtr(0,z+1);
			pB=img->GetLinePtr(0,z);
			
			for (size_t i=0; i<sxy; i++) {
				pA[i]=kipl::math::sign(pA[i])*min(std::abs(pA[i]),std::abs(pB[i]));
			}
		}
		break;
	}
	
	return 0;
}


}}

#endif /*ISSFILTER_HPP_*/
