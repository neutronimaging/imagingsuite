//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//
 
#ifndef SEGMENTATIONFUZZYKMEANS_HPP
#define SEGMENTATIONFUZZYKMEANS_HPP

#include <base/timage.h>
#include <math/image_statistics.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <sstream>
#include <logging/logger.h>
#include <io/io_tiff.h>
#include <math/LUTCollection.h>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace kipl { namespace segmentation {

template<class ImgType, class SegType ,size_t NDim>
FuzzyKMeans<ImgType,SegType,NDim>::FuzzyKMeans() :
	SegmentationBase<ImgType,SegType,NDim>("FuzzyKMeans"),
	maxIterations(250),
	haveCenters(false),
    centers(nullptr),
	fuzziness(1.5f)
	
{
	this->nClasses=2;
	mpower=2.0f/(fuzziness-1.0f);
	centers=new float[this->nClasses];
}

template<class ImgType, class SegType ,size_t NDim>
FuzzyKMeans<ImgType,SegType,NDim>::~FuzzyKMeans()
{
    if (centers!=nullptr)
		delete [] centers;
}

template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::set(int NClasses, float fuz, int maxIt ,bool bSaveIterations, std::string sFname)
{ 
	this->nClasses=NClasses; 
    if (centers!=nullptr)
		delete [] centers;
	centers=new float[this->nClasses];
	
	fuzziness=fuz; 
	mpower=2.0f/(fuzziness-1.0f);
	maxIterations=maxIt;

	m_bSaveIterations=bSaveIterations;
	m_sIterationFileName=sFname;
	return 0;
}

template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::initCenters(const kipl::base::TImage<ImgType,NDim> &img)
{ 
	std::cout<<"Init centers"<<endl;
	ImgType const * const pImg=img.GetDataPtr();
	for (int i=0; i<this->nClasses; i++) 
		centers[i]=pImg[abs((rand()*rand())) % img.Size()];
	
	std::sort(centers, centers+this->nClasses);
	
	return 0;
}

template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::initU(kipl::base::TImage<float,2> &U)
{
	float *pU=U.GetDataPtr();
	
	float norm=1.0f/RAND_MAX;

	ptrdiff_t i=0;
	ptrdiff_t N=U.Size();

	#pragma omp parallel for
	for (i=0; i<N; i++) {
		pU[i]=static_cast<float>(rand()*norm); 
	}
	
    return 0;
}

template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> & seg)
{
    std::stringstream msgstr;
	size_t const * const dims=img.Dims();
    int NC=this->nClasses;
    ptrdiff_t Nimg=img.Size();
	
    size_t dimsU[]={static_cast<size_t>(img.Size()),static_cast<size_t>(NC)};
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
    long long int indexU0=0;
    float x=0.0f;	
	ImgType imgmin=static_cast<ImgType>(0);
	ImgType imgmax=static_cast<ImgType>(0);
	kipl::math::minmax(img.GetDataPtr(),img.Size(),&imgmin,&imgmax);
	kipl::math::PowLUT fuzzyLUT(65535,fuzziness,0.0,2.0);
	if (m_bSaveIterations) {
		kipl::base::TImage<float,2> slice(img.Dims());

		memcpy(slice.GetDataPtr(),img.GetLinePtr(0,img.Size(2)/2), sizeof(float)*slice.Size());
		std::string fname;
		std::string ext;
		kipl::strings::filenames::MakeFileName(m_sIterationFileName,0,fname,ext,'#','i');
		kipl::io::WriteTIFF(slice,fname.c_str());
	}

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
         //     pU[indexU]=pow(pU[indexU],fuzziness);
				pU[indexU]=fuzzyLUT(pU[indexU]);
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
						//pU[indexU]+=pow(fabs(dik*djk),-mpower);  // sum((|djk|/|dik|)^mpower)
						pU[indexU]+=fuzzyLUT2(fabs(dik*djk));  // sum((|djk|/|dik|)^mpower)
					}
				}
			}
		}
		pU=U.GetDataPtr();
		// Medium
		for (ptrdiff_t i=0; i<Nimg*NC; i++) {
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

		if (m_bSaveIterations) {
			kipl::base::TImage<float,2> slice(img.Dims());

			pU=U.GetDataPtr()+this->nClasses*slice.Size()*(img.Size(2)/2);
			float *pSlice=slice.GetDataPtr();
			for (int i=0; i<slice.Size(); i++) {
		        indexU=i*NC;
		        pSlice[i]=0;
		        float maxClass=pU[indexU];
		        indexU++;
		        for (c=1; c< this->nClasses; c++, indexU++) {
		            if (maxClass<pU[indexU]) {
		                maxClass=pU[indexU];
		                pSlice[i]=c;
		            }
				}
			}
			std::string fname;
			std::string ext;
			kipl::strings::filenames::MakeFileName(m_sIterationFileName,r,fname,ext,'#','0');
			kipl::io::WriteTIFF(slice,fname.c_str());

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
	for (ptrdiff_t i=0; i<Nimg; i++) {
        indexU=i*NC;
        pSeg[i]=0;
        float maxClass=pU[indexU];
        indexU++;
        for (int c=1; c< this->nClasses; c++, indexU++) {
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

template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::parallel(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> & seg)
{
    std::stringstream msgstr;
	size_t const * const dims=img.Dims();
    int NC=this->nClasses;
    ptrdiff_t Nimg=img.Size();
	
    size_t dimsU[]={img.Size(),static_cast<size_t>(NC)};
	kipl::base::TImage<float,2> U(dimsU), oldU(dimsU);
	
	initU(U);
	
	// Initialization
	float * pUold=NULL;
	float * pU=NULL;
	
	ImgType const * const pX=img.GetDataPtr();


	double change=0.0;
	double oldChange=0.0;
	int cnt=0;
	int r;
	double *sum   = new double[NC];
	double *uxSum = new double[NC];

    long long int indexU0=0;

    
	for (r=0; r<maxIterations; r++) {
		memcpy(oldU.GetDataPtr(),U.GetDataPtr(),sizeof(float)*U.Size());
        
		// Step II, compute centers
        memset(sum,0,sizeof(double)*NC);
        memset(uxSum,0,sizeof(double)*NC);
        pU=U.GetDataPtr();
		// Loong
		#pragma omp parallel
        {
        	float * pLocalU=U.GetDataPtr();
        	double *localsum=new double[NC];
        	double *localuxsum=new double[NC];
        	ptrdiff_t indexU=0;

            float x=0.0f;

			#pragma omp for
			for (ptrdiff_t i=0; i<Nimg; i++)
			{
				indexU=NC*i;
				x=pX[i];
				for (int c=0; c<NC; c++, indexU++)
				{
					pU[indexU]=pow(pLocalU[indexU],fuzziness);
					localsum[c]+=pLocalU[indexU];
					localuxsum[c]+=x*pLocalU[indexU];
				}
			}

			#pragma omp critical
			{
				for (int i=0; i<NC; i++)
				{
					sum[i]+=localsum[i];
					uxSum[i]+=uxSum[i];
				}
			}

			delete [] localsum;
			delete [] localuxsum;
        }

		msgstr.str("");
		msgstr<<"Iteration "<<r<<": centers= ";    
		// Short
        for (int c=0; c<NC; c++) {
            if (sum[c]!=0.0)
                centers[c]=uxSum[c]/sum[c];
            msgstr<<centers[c]<<" ";
        }
            
		std::sort(centers,centers+NC); // Assure that the class index 
									   // is ordered with the center value

        // Loong
		#pragma omp parallel
		{
			float * pLocalU=U.GetDataPtr();
			ptrdiff_t indexU=0;
			float djk, dik;
			float x;

			#pragma omp for
			for (ptrdiff_t i=0; i<Nimg; i++) {
				indexU=i*NC;
				x=static_cast<float>(pX[i]);
				for (int ci=0; ci<NC; ci++,indexU++) {
					dik=x-centers[ci];
					if (dik!=0.0f) {
						dik=1/dik;
						for (int cj=0; cj<NC; cj++) {
							djk=(x-centers[cj]);
							pLocalU[indexU]+=pow(fabs(dik*djk),-mpower);  // sum((|djk|/|dik|)^mpower)
						}
					}
				}
			}

			#pragma omp for
			for (ptrdiff_t i=0; i<Nimg; i++) {
				pLocalU[i]=1.0f/pLocalU[i];
			}

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
	

    float maxclass=0.0f;
	// medium
	#pragma omp parallel
    {
    	ptrdiff_t indexU=0;
    	float *pLocalU=U.GetDataPtr();
    	SegType *pSeg=seg.GetDataPtr();
		#pragma omp for
		for (ptrdiff_t i=0; i<Nimg; i++) {
			indexU=i*NC;
			pSeg[i]=0;
			float maxClass=pU[indexU];
			indexU++;
			for (int c=1; c< this->nClasses; c++, indexU++) {
				if (maxClass<pLocalU[indexU]) {
					maxClass=pLocalU[indexU];
					pSeg[i]=c;
				}
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
template<class ImgType, class SegType ,size_t NDim>
int FuzzyKMeans<ImgType,SegType,NDim>::operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg, const kipl::base::TImage<bool,NDim> & mask)
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
