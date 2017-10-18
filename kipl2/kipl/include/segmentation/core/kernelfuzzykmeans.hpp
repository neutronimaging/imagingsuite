//<LICENCE>
 
#ifndef SEGMENTATIONKERNELFUZZYKMEANS_HPP
#define SEGMENTATIONKERNELFUZZYKMEANS_HPP

#include <stdlib.h>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <sstream>

#include "../../base/timage.h"
#include "../../math/image_statistics.h"
#include "../../logging/logger.h"
#include "../../math/LUTCollection.h"


#ifdef _OPENMP
#include <omp.h>
#endif

namespace kipl { namespace segmentation {

template<typename ImgType, typename SegType ,size_t NDim>
KernelFuzzyKMeans<ImgType,SegType,NDim>::KernelFuzzyKMeans() :
    kipl::segmentation::SegmentationBase<ImgType,SegType,NDim>("KernelFuzzyKMeans"),
	maxIterations(250),
	haveCenters(false),
	centers(NULL),
	fuzziness(1.5f),
	m_fSigma(2.0f)
{
	this->nClasses=2;
	mpower=1.0f/(fuzziness-1.0f);
	centers=new float[this->nClasses];
}

template<typename ImgType, typename SegType ,size_t NDim>
KernelFuzzyKMeans<ImgType,SegType,NDim>::~KernelFuzzyKMeans()
{
	if (centers!=NULL)
		delete [] centers;
}

template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::set(int NClasses, float fuz, int maxIt)
{ 
	this->nClasses=NClasses; 
	if (centers!=NULL)
		delete [] centers;
	centers=new float[this->nClasses];
	
	fuzziness=fuz; 
	mpower=1.0f/(fuzziness-1.0f);
	maxIterations=maxIt;
	return 0;
}

template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::initCenters(const kipl::base::TImage<ImgType,NDim> &img)
{ 
	std::cout<<"Init centers"<<endl;
	ImgType const * const pImg=img.GetDataPtr();
	for (int i=0; i<this->nClasses; i++) 
		centers[i]=pImg[abs((rand()*rand())) % img.Size()];
	
	std::sort(centers, centers+this->nClasses);
	
	return 0;
}

template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::initU(kipl::base::TImage<float,2> &U)
{
	float *pU=U.GetDataPtr();
	
	float norm=1.0f/RAND_MAX;

	ptrdiff_t i=0;
	ptrdiff_t N=U.Size();

	unsigned char mask=255;
	#pragma omp parallel for
	for (i=0; i<N; i++) {
		pU[i]=static_cast<float>(rand() * norm); 
	}
	
    return 0;
}

template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> & seg)
{
    std::stringstream msgstr;
	size_t const * const dims=img.Dims();
    int NC=this->nClasses;
    ptrdiff_t Nimg=img.Size();
	
    size_t dimsU[]={static_cast<size_t>(Nimg),static_cast<size_t>(NC)};
	// Todo: Make the likelihood images into 8bit to save memory
	kipl::base::TImage<float,2> U(dimsU), oldU(dimsU);
	
	initU(U);
	
	// Initialization 
	ptrdiff_t i;
	int c;

	float * pU=U.GetDataPtr();
	float * pUold=NULL;
	
	
	ImgType const * const pX=img.GetDataPtr();
	int ci,cj;
	float djk, dik;
	double change=0.0;
	double oldChange=0.0;
	int cnt=0;
	int r;
	double *sum   = new double[NC];
	double *uxSum = new double[NC];
    long long int indexU=0;
    float x=0.0f;	
	ImgType imgmin=static_cast<ImgType>(0);
	ImgType imgmax=static_cast<ImgType>(0);
	kipl::math::minmax(img.GetDataPtr(),img.Size(),&imgmin,&imgmax);
	kipl::math::PowLUT fuzzyLUT(65535,fuzziness,0.0,1.0);


	for (r=0; r<maxIterations; r++) {
		memcpy(oldU.GetDataPtr(),U.GetDataPtr(),sizeof(float)*U.Size());
    
		// Step II, compute centers
        memset(sum,0,sizeof(double)*NC);
        memset(uxSum,0,sizeof(double)*NC);
        pU=U.GetDataPtr();
		// Loong
        for (i=0; i<Nimg; i++) {
            indexU=NC*i;
            x=pX[i];
        	for (c=0; c<NC; c++, indexU++) {
				pU[indexU]=fuzzyLUT(static_cast<float>(pU[indexU]));
				
                sum[c]+=pU[indexU];
                uxSum[c]+=x*pU[indexU];	
            }

		}

		msgstr.str("");
		msgstr<<"Iteration "<<r<<": centers= ";
		
		
		// Short
        for (c=0; c<NC; c++) {
            if (sum[c]!=0.0)
                centers[c]=uxSum[c]/sum[c];
            msgstr<<centers[c]<<" ";
        }
        //logger(kipl::logging::Logger::LogMessage,msgstr.str());

        std::sort(centers,centers+NC); // Assure that the typename index
									   // is ordered with the center value

		pU=U.GetDataPtr();
        // Loong
		
		kipl::math::PowLUT fuzzyLUT2(65535,-mpower,0.0f,10.0f);
		for (i=0; i<Nimg; i++) {
            indexU=i*NC;
            x=pX[i];
			
            for (ci=0; ci<NC; ci++,indexU++) {
				dik=x-centers[ci];
				if (dik!=0.0f) {
                    dik=1.0f/dik;
					for (cj=0; cj<NC; cj++) {
						djk=(x-centers[cj]);
						pU[indexU]+=fuzzyLUT2(fabs(dik*djk));  // sum((|djk|/|dik|)^mpower)
					}
				}
			}

		}
		
		/// Step IV, compute change
		pU=U.GetDataPtr();
		pUold=oldU.GetDataPtr();

		double errorSum=kipl::math::diffsum2(pU,pU+U.Size(),pUold);

		change=sqrt(errorSum);
		
		msgstr<<" change= "<<change;
		//logger(kipl::logging::Logger::LogVerbose,msgstr.str());
			
		if ((change<1e-5) || (10<cnt))
			break;
		else {
			if (abs(change-oldChange)<1e-3)
				cnt++;
			else {
				cnt=0;
				oldChange=change;
			}
		}					
	}
	msgstr.str("");
	msgstr<<r<<"("<<maxIterations<<") iterations were used, cnt="<<cnt;
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
		
	oldU.FreeImage();
	seg.Resize(dims);
	SegType *pSeg=seg.GetDataPtr();
	
	pU=U.GetDataPtr();
    float maxclass=0.0f;
	// medium
	for (i=0; i<Nimg; i++) {
        indexU=i*NC;
        pSeg[i]=0;
        float maxClass=pU[indexU];
        indexU++;
        for (c=1; c< this->nClasses; c++, indexU++) {
            if (maxClass<pU[indexU]) {
                maxClass=pU[indexU];
                pSeg[i]=c;
            }
		}
	}
    delete [] sum;
    delete [] uxSum;
    
	return 0;
}

template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::parallel(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> & seg)
{
	//logger(kipl::logging::Logger::LogWarning,"The parallel version is not ready");
    std::stringstream msgstr;
	size_t const * const dims=img.Dims();
    int NC=this->nClasses;
    ptrdiff_t Nimg=img.Size();
	
    size_t dimsU[]={static_cast<size_t>(Nimg),static_cast<size_t>(NC)};
	kipl::base::TImage<float,2> U(dimsU), oldU(dimsU);
	
	initU(U);
	
	// Initialization
	ptrdiff_t i;
	int c;

	float * pU    = U.GetDataPtr();
	float * pUold = NULL;
	
	
	ImgType const * const pX=img.GetDataPtr();
	int ci,cj;
	float djk, dik;
	double change=0.0;
	double oldChange=0.0;
	int cnt=0;
	int r;
	double *sum   = new double[NC];
	double *uxSum = new double[NC];
    long long int indexU=0;
    long long int indexU0=0;
    float x=0.0f;	
    
	for (r=0; r<maxIterations; r++) {
		memcpy(oldU.GetDataPtr(),U.GetDataPtr(),sizeof(unsigned char)*U.Size());

        
		// Step II, compute centers
        memset(sum,0,sizeof(double)*NC);
        memset(uxSum,0,sizeof(double)*NC);
        pU=U.GetDataPtr();
		// Loong
        for (i=0; i<Nimg; i++) {
            indexU=NC*i;
            x=pX[i];
        	for (c=0; c<NC; c++, indexU++) {
                pU[indexU]=pow(pU[indexU],fuzziness);
                sum[c]+=pU[indexU];
                uxSum[c]+=x*pU[indexU];	
            }
		}

		msgstr.str("");
		msgstr<<"Iteration "<<r<<": centers= ";    
		// Short
        for (c=0; c<NC; c++) {
            if (sum[c]!=0.0)
                centers[c]=uxSum[c]/sum[c];
            msgstr<<centers[c]<<" ";
        }
            
		std::sort(centers,centers+NC); // Assure that the class index 
									   // is ordered with the center value

		pU=U.GetDataPtr();
        // Loong
		#pragma omp parallel for 
		for (i=0; i<Nimg; i++) {
            indexU=i*NC;
            x=pX[i];
            for (ci=0; ci<NC; ci++,indexU++) {
				dik=x-centers[ci];
				if (dik!=0.0f) {
                    dik=1/dik;
					for (cj=0; cj<NC; cj++) {
						djk=(x-centers[cj]);
						pU[indexU]+=pow(fabs(dik*djk),-mpower);  // sum((|djk|/|dik|)^mpower)
					}
				}
			}
		}
		pU=U.GetDataPtr();
		// Medium
		for (i=0; i<U.Size(); i++) { 
			pU[i]=1.0f/pU[i];
		}
		
		/// Step IV, compute change
		pU=U.GetDataPtr();
		pUold=oldU.GetDataPtr();

		double errorSum=kipl::math::diffsum2(pU,pU+U.Size(),pUold);

		change=sqrt(errorSum);
		
		msgstr<<" change= "<<change;
		this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
			
		if ((change<1e-5) || (10<cnt))
			break;
		else {
			if (abs(change-oldChange)<1e-3)
				cnt++;
			else {
				cnt=0;
				oldChange=change;
			}
		}					
	}
	msgstr.str("");
	msgstr<<r<<"("<<maxIterations<<") iterations were used, cnt="<<cnt;
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
		
	
	seg.Resize(dims);
	SegType *pSeg=seg.GetDataPtr();
	
	pU=U.GetDataPtr();
    float maxclass=0.0f;
	// medium
	for (i=0; i<Nimg; i++) {
        indexU=i*NC;
        pSeg[i]=0;
        float maxClass=pU[indexU];
        indexU++;
        for (c=1; c< this->nClasses; c++, indexU++) {
            if (maxClass<pU[indexU]) {
                maxClass=pU[indexU];
                pSeg[i]=c;
            }
		}
	}
    delete [] sum;
    delete [] uxSum;
    
	return 0;

}


/// \brief Segments an image
///	\param img Image to be segmented
///	\param seg Segmented image
/// \param mask mask image for stayaway regions
template<typename ImgType, typename SegType ,size_t NDim>
int KernelFuzzyKMeans<ImgType,SegType,NDim>::operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg, const kipl::base::TImage<bool,NDim> & mask)
{
    std::stringstream msgstr;
	size_t const * const dims=img.Dims();
    int NC=this->nClasses;
    long long int Nimg=img.Size();
	
	size_t dimsU[]={img.Size(),NC};
	kipl::base::TImage<float,2> U(dimsU), oldU(dimsU);
	
	initU(U);
	
	// Initialization
	size_t i;
	int c;

	float * pU=U.GetDataPtr();
	float * pUold=NULL;
	
	
	ImgType const * const pX=img.GetDataPtr();
	bool const * const pMask=mask.GetDataPtr();
	
	int ci,cj;
	float djk, dik;
	double change=0.0;
	double oldChange=0.0;
	int cnt=0;
	int r;
	double *sum   = new double[NC];
	double *uxSum = new double[NC];
    long long int indexU=0;
    long long int indexU0=0;
    float x=0.0f;	
    
	for (r=0; r<maxIterations; r++) {
		memcpy(oldU.GetDataPtr(),U.GetDataPtr(),sizeof(float)*U.Size());

        
		// Step II, compute centers
        memset(sum,0,sizeof(double)*NC);
        memset(uxSum,0,sizeof(double)*NC);
        pU=U.GetDataPtr();
		
        for (i=0; i<Nimg; i++) {
            indexU=NC*i;
            x=pX[i];
            if (pMask[i]==true) {
	        	for (c=0; c<NC; c++, indexU++) {
	                pU[indexU]=pow(pU[indexU],fuzziness);
	                sum[c]+=pU[indexU];
	                uxSum[c]+=x*pU[indexU];	
	            }
            }
            else {
     	       	for (c=0; c<NC; c++, indexU++) {
	               pU[indexU]=0;
	           }
            }
		}
		msgstr.str("");
		msgstr<<"Iteration "<<r<<": centers= ";        
        for (c=0; c<NC; c++) {
            if (sum[c]!=0.0)
                centers[c]=uxSum[c]/sum[c];
            msgstr<<centers[c]<<" ";
        }
            
		std::sort(centers,centers+NC); // Assure that the class index 
									   // is ordered with the center value

		pU=U.GetDataPtr();
     
		for (i=0; i<Nimg; i++) {
            indexU=i*NC;
            x=pX[i];
            if (pMask[i]==true) {
	            for (ci=0; ci<NC; ci++,indexU++) {
					dik=x-centers[ci];
	                if (dik!=0)
	                    dik=1/dik;
					for (cj=0; cj<NC; cj++) {
						djk=(x-centers[cj]);
						pU[indexU]+=pow(fabs(dik*djk),-mpower);  // sum((|djk|/|dik|)^mpower)
					}
				}
            }
		}
		pU=U.GetDataPtr();
		for (i=0; i<U.Size(); i++) { 
            if (pU[i]!=0)
    			pU[i]=1.0f/pU[i];
		}
		
		/// Step IV, compute change
		pU=U.GetDataPtr();
		pUold=oldU.GetDataPtr();

		double errorSum=kipl::math::diffsum2(pU,pU+U.Size(),pUold);

		change=sqrt(errorSum);
		
		msgstr<<" change= "<<change;
		this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
			
		if ((change<1e-5) || (10<cnt))
			break;
		else {
			if (abs(change-oldChange)<1e-3)
				cnt++;
			else {
				cnt=0;
				oldChange=change;
			}
		}					
	}
	msgstr.str("");
	msgstr<<r<<"("<<maxIterations<<") iterations were used, cnt="<<cnt;
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
		
	
	seg.Resize(dims);
	SegType *pSeg=seg.GetDataPtr();
	
	pU=U.GetDataPtr();
    float maxclass=0;

	for (i=0; i<Nimg; i++) {
        pSeg[i]=pMask[i];
        
        if (pMask[i]==true) {
        	indexU=i*NC;
	        pSeg[i]=0;
	        float maxClass=pU[indexU];
	        indexU++;
	        for (c=1; c< this->nClasses; c++, indexU++) {
	            if (maxClass<pU[indexU]) {
	                maxClass=pU[indexU];
	                pSeg[i]=c;
	            }
			}
        }
        else {
        	pSeg[i]=this->nClasses;
        }
	}
    delete [] sum;
    delete [] uxSum;
    
	return 0;	
}

}}

#endif
