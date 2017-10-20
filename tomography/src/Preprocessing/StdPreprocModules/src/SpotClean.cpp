//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//
//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#undef min
#undef max
#ifdef _OPENMP
#include <omp.h>
#endif
#include <base/timage.h>
#include <io/io_matlab.h>
#include <filters/filter.h>
#include <filters/medianfilter.h>
#include <math/mathfunctions.h>
#include <profile/Timer.h>
#include <morphology/morphology.h>
#include <morphology/morphdist.h>
#include <morphology/morphfilters.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <iostream>
#include <set>
#include <strings/miscstring.h>

#include "../include/SpotClean.h"
#include <ReconException.h>
#include <ReconConfig.h>

SpotClean::SpotClean(void) : PreprocModuleBase("SpotClean"),
	pKernel(NULL), 
	nIterations(1), 
	fThreshold(0.1f), 
	fWidth(0.0075f), 
	mAlgorithm(SpotClean_Iterations),
	mLUT(1<<15,0.1f,0.0075f),
	eEdgeProcessingStyle(kipl::filters::FilterBase::EdgeMirror)
{
	nFilterDims[0]=3;
	nFilterDims[1]=3;
	nKernelSize=nFilterDims[0]*nFilterDims[1];
	pKernel=new float[nKernelSize];
	for (size_t i=0; i<nKernelSize; i++)
		pKernel[i]=1.0f;
	
}

SpotClean::~SpotClean(void)
{
	if (pKernel!=NULL)
		delete [] pKernel;
}

int SpotClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	// todo: enter the config entries
//	fThreshold  = config->Reconstructor.Clean.fGamma;
//	fWidth      = config->Reconstructor.Clean.fSigma;
//	nIterations = config->Reconstructor.Clean.nIterations;
//	mAlgorithm  = config->Reconstructor.Clean.mAlgorithm;
//	mDetection  = config->Reconstructor.Clean.mDetection;
//	nWindowSize = config->Reconstructor.Clean.nWindowSize;

	mLUT.Setup(1<<15,fThreshold,fWidth);
	
	return 0;
}

std::map<std::string, std::string> SpotClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	return parameters;
}
//int SpotClean::Setup(size_t iterations, float threshold, float width,  bool bUseLUT, eSpotCleanAlgorithm alg)
//{
//	fThreshold=threshold;
//	fWidth=width;
//	nIterations=iterations;
//	mAlgorithm=alg;
//	mLUT.Setup(1<<15,fThreshold,fWidth);
//
//	return 0;
//}

int SpotClean::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> &coeff)
{
	kipl::base::TImage<float,2> res;
	processList.clear();
	
	switch (mAlgorithm) {
		case SpotClean_Iterations: 
			for (nCurrentIteration=0; nCurrentIteration<nIterations; nCurrentIteration++) {
				res=CleanIteration(img);
				img=res;
			}
			break;
		case SpotClean_PixelList: 
			img=ProcessPixelList(img);
			
			break;
		case SpotClean_LevelSets:
			img=ProcessLevelSets(img);
			break;
        case SpotClean_AbsUnbiased:
            throw ReconException("AbsUnbiased is not implemented",__FILE__,__LINE__);
	}
	
	return 0;
}

kipl::base::TImage<float,2> SpotClean::CleanIteration(kipl::base::TImage<float,2> img)
{
	const size_t nFilterSize=3;
	size_t nMedFiltDims[]={nFilterSize, nFilterSize};
	kipl::filters::TMedianFilter<float,2> medfilter(nMedFiltDims);
	
	kipl::base::TImage<float,2> mimg=medfilter(img);
	kipl::base::TImage<float,2> unbiased=img-mimg;

	kipl::base::TImage<float,2> s=StdDev(unbiased, nFilterSize);

	mLUT.InPlace(s.GetDataPtr(),s.Size());

	// Correction 
	float *pMedian=mimg.GetDataPtr();
	float *pWeight=s.GetDataPtr();
	float *pImg=img.GetDataPtr();
	float *pRes=unbiased.GetDataPtr();

	memcpy(pRes,pImg,sizeof(float)*img.Size());

	for (size_t i=0; i<img.Size(); i++) {
		if (pWeight[i]!=0) {
			pRes[i]=(1-pWeight[i])*pRes[i]+pWeight[i]*pMedian[i];
			processList.push_back(i);
		}
	}
	return unbiased;
}

double SpotClean::ChangeStatistics(kipl::base::TImage<float,2> img)
{
	const size_t N=img.Size();
	size_t cnt=0;
	float *pData=img.GetDataPtr();
	for (size_t i=0; i<N; i++) {
		cnt+=(pData[i]!=0.0f);
	}	
	return static_cast<double>(cnt)/static_cast<double>(N);
}

kipl::base::TImage<float,2> SpotClean::BoxFilter(kipl::base::TImage<float,2> img, size_t dim)
{
	size_t dimsU[]={dim,1};
	size_t dimsV[]={1,dim};
	size_t N=dim*dim;
	float *fKernel=new float[N];
	for (size_t i=0; i<N; i++)
		fKernel[i]=1.0f;

	kipl::filters::TFilter<float,2> filterU(fKernel,dimsU);
	kipl::filters::TFilter<float,2> filterV(fKernel,dimsV);

	kipl::base::TImage<float,2> imgU=filterU(img, eEdgeProcessingStyle);
	delete [] fKernel;

	return filterV(imgU, eEdgeProcessingStyle);
}

kipl::base::TImage<float,2> SpotClean::StdDev(kipl::base::TImage<float,2> img, size_t dim) 
{
	kipl::base::TImage<float,2> img2=kipl::math::sqr(img);

	kipl::base::TImage<float,2> s2=BoxFilter(img2,dim);
	kipl::base::TImage<float,2> m=BoxFilter(img,dim);

	img2=0.0f;
	float *pRes=img2.GetDataPtr();
	float *pS2=s2.GetDataPtr();
	float *pM=m.GetDataPtr();
	
	const float M=1/float(dim*dim);
	const float M1=1/float(dim*dim-1);

	for (size_t i=0; i<img2.Size(); i++) {
		pRes[i]=sqrt(M1*(pS2[i]-M*pM[i]*pM[i]));
	}

	return img2;
}

kipl::base::TImage<float,2> SpotClean::ProcessLevelSets(kipl::base::TImage<float,2> img)
{
	kipl::base::TImage<float,2> s=DetectionImage(img,mDetection,nWindowSize);
	int i;
	#pragma omp parallel 
	{
		float *pS=s.GetDataPtr();
		float *pImg=img.GetDataPtr();
	
		#pragma omp for
        for (i=0; i<static_cast<int>(s.Size()); i++)
				if (this->fThreshold < pS[i])
					pImg[i]=0;
		
	}
	kipl::base::TImage<float,2> result;
	result=FillHoles(img);
	return result;
}

kipl::base::TImage<float,2> SpotClean::ProcessPixelList(kipl::base::TImage<float,2> img)
{
	kipl::base::TImage<float,2> result=img;
	first_line=img.Size(0)-1;
	last_line=img.Size()-img.Size(0);
	sx=img.Size(0);

	kipl::base::TImage<float,2> s=DetectionImage(img,mDetection,nWindowSize);
	
	// Make first threshold image
	kipl::base::TImage<char,2> mask(s.Dims());
	for (size_t i=0; i< s.Size(); i++)
		mask[i]=mLUT.Low()<s[i];

	kipl::base::TImage<char,2> dist;
	kipl::morphology::EuclideanDistance(mask,dist);

	std::map<float,std::list<size_t> > spotlist;
    for (int i=0; i<static_cast<int>(dist.Size()); i++) {
		if (dist[i]!=0) {
			spotlist[dist[i]].push_back(i);
		}
	}

	std::map<float,std::list<size_t> >::iterator it;
	float localdata[16];
	float med;
	float stddev;
	float sum,sum2;
	float x;
	float w=1.0f;
	const size_t N=9;
	const float M=1.0f/float(N);
	const float M1=1.0f/float(N-1);
	std::list<size_t>::iterator pixiterator;
	for (it=spotlist.begin(); it!=spotlist.end(); it++) {
		for (pixiterator=it->second.begin(); pixiterator!=it->second.end(); pixiterator++) {
			size_t pixel=*pixiterator;
			ExtractLocalData(pixel, &result, localdata);
			kipl::math::median_quick_select(localdata,N,&med);
			sum=0.0f;
			sum2=0.0f;
			for (size_t i=0; i<N; i++) {
				x=localdata[i]-med;
				sum+=x;
				sum2+=x*x;
			}
			
			stddev=sqrt(M1*(sum2-M*sum*sum));
			w=mLUT(stddev);
			//result[pixel]=result[pixel]*(1-w)+w*med;
			result[pixel]+=w*(med+result[pixel]);
		}
		it->second.clear();
	}

	return result;
}

size_t SpotClean::ExtractLocalData(size_t pixel, 
								   kipl::base::TImage<float,2> *result, 
								   float *localdata)
{
	size_t N=0;
	float *pRes=result->GetDataPtr();
	if ((first_line<pixel) && (pixel<last_line)) {
		N=9;
		localdata[0]=pRes[pixel-sx-1];
		localdata[1]=pRes[pixel-sx];
		localdata[2]=pRes[pixel-sx+1];

		localdata[3]=pRes[pixel-1];
		localdata[4]=pRes[pixel];
		localdata[5]=pRes[pixel+1];
		
		localdata[6]=pRes[pixel+sx-1];
		localdata[7]=pRes[pixel+sx];
		localdata[8]=pRes[pixel+sx+1];
	}
	else {
		if ((1<pixel) && (pixel<first_line)) {
			N=9;
			localdata[3]=pRes[pixel-1];
			localdata[4]=pRes[pixel];
			localdata[5]=pRes[pixel+1];
		
			localdata[0]=localdata[6]=pRes[pixel+sx-1];
			localdata[1]=localdata[7]=pRes[pixel+sx];
			localdata[2]=localdata[8]=pRes[pixel+sx+1];
		}
		else if ((last_line<pixel) && (pixel<(result->Size()-1))) {
			N=9;
			localdata[3]=pRes[pixel-1];
			localdata[4]=pRes[pixel];
			localdata[5]=pRes[pixel+1];
		
			localdata[0]=localdata[6]=pRes[pixel-sx-1];
			localdata[1]=localdata[7]=pRes[pixel-sx];
			localdata[2]=localdata[8]=pRes[pixel-sx+1];
		}
	}

	return N;
}


kipl::base::TImage<float,2> SpotClean::FillHoles(kipl::base::TImage<float,2> img)
{
	// Make mask image
	kipl::base::TImage<float,2> mask(img.Dims()),mask2;
	float *pImg=img.GetDataPtr();
	float *pMask=mask.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++)
		pMask[i]=(pImg[i]==0);

	// Ensure that the detected regions are closed
	float kernel[]={1,1,1,1,1,1,1,1,1};
	size_t kdims[]={3,3};
	kipl::morphology::TDilate<float,2> dilate(kernel,kdims);
	mask2=dilate(mask,kipl::filters::FilterBase::EdgeMirror);

	// Compute propagation field for the filling
	kipl::base::TImage<float,2> dist;
	kipl::morphology::EuclideanDistance(mask2,dist,kipl::morphology::conn8);

	map<float,vector<size_t> > distmap;
#undef max
	for (size_t i=0; i<dist.Size(); i++) {
		if (dist[i]!=0) {
			distmap[dist[i]].push_back(i);
			pImg[i]=std::numeric_limits<float>::max( );
		}
	}

	map<float,vector<size_t> >::iterator it;
	vector<size_t>::iterator pos;
	kipl::morphology::CNeighborhood neighbor(img.Dims(),2,kipl::morphology::conn8);

	vector<pair<size_t, float> > corrections;
	vector<pair<size_t, float> >::iterator cit;

	// Loop over the levels
	for (it=distmap.begin(); it!=distmap.end(); it++) {
		corrections.clear();
		// Loop over the pixels in a level
		for (pos=it->second.begin(); pos!=it->second.end(); pos++) {
			float sum=0.0f;
			int cnt=0;
			// Scan the neighborhood for edge pixels and sum them
			for (size_t i=0; i<neighbor.N(); i++) {
				int npos=neighbor.neighbor(*pos,i);
				if ((0<=npos) && (pImg[npos]!=std::numeric_limits<float>::max())){
					cnt++;
					sum+=pImg[npos];
				}
			} 
			if (cnt!=0) {
				// Put the correction value on the replacement queue
				corrections.push_back(make_pair(*pos,sum/cnt));
			}
			else{
				// This shouldn't happen...
				cout<<"missed pixel"<<endl;
			}
		}

		// Correct the pixels of the current level
		for (cit=corrections.begin(); cit!=corrections.end(); cit++) {
			pImg[cit->first]=cit->second;
		}
		
	}	

	return img;
}

kipl::base::TImage<float,2> SpotClean::DetectionImage(kipl::base::TImage<float,2> img, eSpotDetection method, size_t dims)
{
	kipl::base::TImage<float,2> det_img;
	logger(kipl::logging::Logger::LogVerbose,"Compute unbiased");
	det_img=UnbiasedImage(img,method,dims);

	float *pDet=det_img.GetDataPtr();
	logger(kipl::logging::Logger::LogVerbose,"Computing det img");
	switch (method) {
		case SpotDetection_Mean:
		case SpotDetection_Median:
			for (size_t i=0; i<det_img.Size(); i++)
				pDet[i]=fabs(pDet[i]);
			break;
		case SpotDetection_StdDev_Mean:		
		case SpotDetection_StdDev_Median:
		default:
			det_img=StdDev(det_img,dims);
			break;
	}

	logger(kipl::logging::Logger::LogVerbose,"Got Detection image");
	return det_img;
}

kipl::base::TImage<float,2> SpotClean::UnbiasedImage(kipl::base::TImage<float,2> img, eSpotDetection method, size_t nFilterSize)
{
	size_t nFiltDimsV[]={1, nFilterSize};
	size_t nFiltDimsH[]={nFilterSize, 1};
	kipl::filters::TMedianFilter<float,2> medianV(nFiltDimsV);
	kipl::filters::TMedianFilter<float,2> medianH(nFiltDimsH);

	float *kernel=new float[nFilterSize];
	float val=1.0f/static_cast<float>(nFilterSize);
	for (size_t i=0; i<nFilterSize; i++)
		kernel[i]=val;
	kipl::filters::TFilter<float,2> meanV(kernel,nFiltDimsV);
	kipl::filters::TFilter<float,2> meanH(kernel,nFiltDimsH);

	kipl::base::TImage<float,2> mimg;
	
	switch (method) {
		case SpotDetection_Mean:
		case SpotDetection_StdDev_Mean:
			mimg=meanV(img,eEdgeProcessingStyle);
			mimg=meanH(mimg,eEdgeProcessingStyle);
			
			break;

		case SpotDetection_Median:
		case SpotDetection_StdDev_Median:
		default:
			mimg=medianV(img);
			mimg=medianH(mimg);
	
			break;
	}

	kipl::base::TImage<float,2> unbiased=img-mimg;

	return unbiased;
}



