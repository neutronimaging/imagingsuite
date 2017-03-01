//<LICENCE>

#ifndef IMAGETHRESHOLDMULTIRESSEG_HPP
#define IMAGETHRESHOLDMULTIRESSEG_HPP

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "../../base/timage.h"
#include "../../base/imagesamplers.h"
#include "../../logging/logger.h"

//#include "../segmentationbase.h"

namespace akipl { namespace segmentation {
template<class ImgType, class SegType, size_t NDim>
MultiResolution<ImgType,SegType,NDim>::MultiResolution(
	SegmentationBase<ImgType,SegType,NDim> & Seg,
    SegmentUpdateBase<ImgType,SegType,NDim> & Upd, 
    std::ostream &os) : akipl::segmentation::SegmentationBase<ImgType,SegType,NDim>("Pyramid segmenter"), segmenter(Seg), updater(Upd)
{
	doHistogram=true;
	useStayAway=false;
	maxCVal=1;
	minCVal=0;
	intervalHi=1;
	intervalLo=0;
	this->sampler=0;
	nLevels=4;
	sampler=0;
	samplerSigma=1.0;
}

template<class ImgType, class SegType, size_t NDim>
int MultiResolution<ImgType,SegType,NDim>::process(const kipl::base::TImage<ImgType, NDim> &img,
													kipl::base::TImage<SegType,NDim> &seg, 
													int levels, 
													int classes)
{
	std::stringstream msgstr;
	
	this->logger(kipl::logging::Logger::LogVerbose,"Building the pyramid...");
	
	PrepareScales(img,levels);
	this->logger(kipl::logging::Logger::LogVerbose,"Pyramid is done.");
	/*
	int r,c,i;
	r=2;
	c=levels+1;
	typename vector<kipl::base::TImage<ImgType,NDim> >::iterator imgIt;
	
	imgIt=scalevec.end();
	imgIt--;
	minCVal=0; maxCVal=classes-1;
	
	kipl::base::TImage<SegType,NDim> current,prev;
	this->logger(kipl::logging::Logger::LogVerbose,"Do toplevel segmentation.");
	
	segmenter.setClasses(classes);
	updater.setClasses(classes);
	segmenter(*imgIt,current);
	
	kipl::base::TImage<SegType,2> slice;

	this->logger(kipl::logging::Logger::LogVerbose,"Propagate segments to the remaining levels.");
	
	for (i=1; imgIt!=scalevec.begin(); imgIt--, i++) {
		
		msgstr.str("");
		msgstr<<"Propagating to level "<<levels-i<<endl;
		this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
		prev=current;
//		prev.mirroredge=true;
		updater.update(*(imgIt), prev, *(imgIt-1),current);
	}
	// Do the final image
	this->logger(kipl::logging::Logger::LogVerbose,"Last image");
	
	updater.update(*imgIt, current, img,seg); 

	this->logger(kipl::logging::Logger::LogVerbose,"Segmentation is done");
	*/
	return 0;
}

template<class ImgType, class SegType, size_t NDim>
MultiResolution<ImgType,SegType,NDim>::~MultiResolution()
{}

template<class ImgType, class SegType, size_t NDim>
int MultiResolution<ImgType,SegType,NDim>::PrepareScales(const kipl::base::TImage<ImgType,NDim> img, 
		int levels)
{
	std::stringstream msgstr;
	msgstr<<"Building "<<levels<<" levels";
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
	
	int r,c,i;
	r=2;
	c=levels+1;
	msgstr.str("");
	msgstr<<"level "<<0<<" of " <<levels;
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
	kipl::base::TImage<ImgType,NDim> a,b;
	kipl::base::TImage<ImgType,2> slice;
	size_t bins[3]={2,2,2};

	kipl::base::ReBin(img,b,bins);
	std::cout<<b<<std::endl;
	//kipl::io::WriteMAT(b,"first.mat","f");
	/*
	switch (sampler) {
		default:
		case 0:
			kipl::base::ReBin(img,b,bins); 
			break;
		case 1:
	//		GaussianSampler(img,b,2,samplerSigma);
			break;
	}
	*/
	msgstr.str("");
	msgstr<<"level "<<1<<" of " <<levels;
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
	
	scalevec.push_back(b);
	
	for (i=2; i<levels; i++) {
		
		msgstr.str("");
		msgstr<<"level "<<i<<" of " <<levels;
		this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
		
		a=b;
		a.Clone();
		kipl::base::ReBin(a,b,bins); 
		/*
		switch (sampler) {
		default:
		case 0:
			ReBin(a,b,bins); break;
		case 1:
			GaussianSampler(a,b,2,samplerSigma);
			break;
		}
		*/
		if (img.Size()!=1) {
			scalevec.push_back(b);
		}
		else
			break;
	}
	typename vector<kipl::base::TImage<ImgType,NDim> >::iterator it;
	
	msgstr.str("");
	msgstr<<"reporting sizes:";
	stringstream fname;
	for (it=scalevec.begin(); it!=scalevec.end(); it++) {	
		msgstr<<" "<<it->Size();
		fname.str("");
		fname<<"level_"<<it->Size(0)<<".mat";
		kipl::io::WriteMAT(*it,fname.str().c_str(),"level");
	}
	
	this->logger(kipl::logging::Logger::LogVerbose,msgstr.str());
	return i;
}

template <class ImgType, class SegType, size_t NDim>
int SegmentUpdateBase<ImgType,SegType,NDim>::ComputeClassStatistics(
				const kipl::base::TImage<ImgType,NDim> &img, 
				const kipl::base::TImage<SegType,NDim> &seg)
{
	mean.clear(); mean.assign(nClasses,0.0);
	var.clear(); var.assign(nClasses,0.0);
	cnt.clear(); cnt.assign(nClasses,0.0);
	ImgType const * const pImg=img.GetDataPtr();
	SegType const * const pSeg=seg.GetDataPtr();
	size_t segIdx;
	for (size_t i=0; i<img.Size(); i++) {
		segIdx=static_cast<size_t>(pSeg[i]);
		ImgType val=pImg[i];
		mean[segIdx]+=val;	
		var[segIdx]+=val*val;
		cnt[segIdx]++;
	}
	
	for (int j=0; j<this->nClasses; j++) {
		var[j]=(var[j]-mean[j]*mean[j]/cnt[j])/(cnt[j]-1);
		mean[j]/=cnt[j];
		cnt[j]=cnt[j]/img.Size();
		if (!this->quiet) 
			this->logstream<<j<<": m="<<mean[j]<<", v="<<var[j]<<", P="<<cnt[j]<<endl;
	}
	
	return 0;
}

}}

#endif
