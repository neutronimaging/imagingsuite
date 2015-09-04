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
#include <map>
#include <string>
#include <exception>

#include <base/timage.h>
#include <base/KiplException.h>
#include <io/io_tiff.h>
#include <filters/medianfilter.h>

#include "../include/SpotRingClean.h"
#include "../include/NormPlugins.h"
#include <ProjectionReader.h>
#include <ReconHelpers.h>
#include <ReconException.h>


//SpotRingClean::SpotRingClean(std::string name) :
//	SpotClean2(name),
//	mRingList(0),
//	mNorm(NULL)
//{
//
//}
//
//int SpotRingClean::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
//{
//	int res=SpotClean2::Configure(config, parameters);
//
//	NormPluginBuilder(&config,&mNorm);
//
//	std::map<std::string, std::string> normparameters;
//	mNorm->Configure(m_Config,normparameters);
//
//	BuildFileList(&m_Config,&mProjectionList);
//
//	return res;
//}
//
//int SpotRingClean::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, float> &coeff)
//{
//	// Subtract the spot image
//	throw ReconException("Spot ring clean is not fully implemented.");
//	kipl::containers::ArrayBuffer<PixelInfo2> spots(img.Size());
//	spots.copy(&mRingList);
//	float *pImg=img.GetDataPtr();
//	PixelInfo2 * pPixels=spots.dataptr();
//
//	for (size_t i=0; i<spots.size(); i++)
//		pImg[pPixels[i].pos]=mark;
//
//	kipl::base::TImage<float,2> result=CleanByArray(img,&spots);
//
//	img=result;
//
//	return 0;
//}
//
//bool SpotRingClean::SetROI(size_t *roi)
//{
//	// todo: fix configs
////	ostringstream msg;
////	msg<<"g="<<mConfig.Reconstructor.RingClean.fGamma<<", s="<<mConfig.Reconstructor.RingClean.fSigma;
////	logger(kipl::logging::Logger::LogVerbose,msg.str());
////
////	// Initialize reader
////	ProjectionReader reader;
////
////	// Initialize norm plug-in
////	mNorm->SetROI(roi);
////
////	//kipl::base::TImage<float,2> cumimg;
////	kipl::base::TImage<float,2> img;
////	cumimg=0.0f;
////
////	std::map<std::string,double> params;
////	std::string fname;
////	size_t inc=mProjectionList.size()/mConfig.Reconstructor.RingClean.nProjections;
////
////	std::map<float,ProjectionInfo>::iterator it;
////	try {
////		for (it=mProjectionList.begin(); it!=mProjectionList.end(); ) {
////			// Load image
////			img=reader.Read(it->second.name,roi);
////			// Compute the norm
////
////			mNorm->Process(img,params);
////			if (it==mProjectionList.begin())
////				cumimg=img;
////			else
////				cumimg+=img;
////
////			for (size_t i=0; (i<inc) && (it!=mProjectionList.end()); i++)
////				it++;
////		}
////	}
////	catch (std::exception &e) {
////		throw ReconException(e.what(),__FILE__,__LINE__);
////	}
////	catch (kipl::base::KiplException &e) {
////		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
////	}
////	catch (ReconException &e) {
////		throw ReconException(e.what(),__FILE__,__LINE__);
////	}
////	catch (...) {
////		throw ReconException("Unknown exception",__FILE__,__LINE__);
////	}
////
////	float *pCumimg=cumimg.GetDataPtr();
////	float scale=1.0f/mConfig.Reconstructor.RingClean.nProjections;
////
////	for (size_t i=0; i<cumimg.Size(); i++) {
////		pCumimg[i]*=scale;
////	}
////
////	size_t dims[2]={3,3};
////	kipl::filters::TMedianFilter<float,2> medfilt(dims);
////	spotimg=medfilt(cumimg,kipl::filters::FilterBase::EdgeMirror);
////	float *pSpotimg=spotimg.GetDataPtr();
////	for (size_t i=0; i<cumimg.Size(); i++) {
////		pSpotimg[i]=fabs(pCumimg[i]-pSpotimg[i]);
////	}
////
//////	kipl::io::WriteTIFF32(spotimg,"spotimg.tif");
////	kipl::base::TImage<float,2> detimg;
////	try {
////		detimg=spotimg;
////		detimg.Clone();
////		mRingList.resize(detimg.Size()); // Set the buffer size
////		mRingList.reset();				 // Reset the buffer counter
////	}
////	catch (std::exception &e) {
////		throw ReconException(e.what(),__FILE__,__LINE__);
////	}
////	catch (kipl::base::KiplException &e) {
////		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
////	}
////	catch (ReconException &e) {
////		throw ReconException(e.what(),__FILE__,__LINE__);
////	}
////	catch (...) {
////		throw ReconException("Unknown exception",__FILE__,__LINE__);
////	}
////
//////	kipl::io::WriteTIFF32(detimg,"detimg.tif");
////
////	float *pDet=detimg.GetDataPtr();
////
////	for (size_t i=0; i<detimg.Size(); i++) {
////		if (mConfig.Reconstructor.RingClean.fGamma<fabs(pDet[i]))
////		{
////			mRingList.push_back(PixelInfo2(static_cast<int>(i),0.0f, 1.0f));
////			pDet[i]=1.0f;
////		}
////		else
////			pDet[i]=0.0f;
////	}
////
//////	kipl::io::WriteTIFF32(detimg,"spots.tif");
////
////	if (detimg.Size()<4*mRingList.size()) {
////		msg.str("");
////		msg<<"More than 25% of the pixels detected as rings with gamma="<<mConfig.Reconstructor.RingClean.fGamma<<"!!! ("<<mRingList.size()<<" of "<<detimg.Size()<<")";
////		logger(kipl::logging::Logger::LogWarning,msg.str());
////	}
//
//	return true;
//}
//
//std::map<std::string, std::string> SpotRingClean::GetParameters()
//{
//	std::map<std::string, std::string> parameters;
//
//	parameters["gamma"]="0.01";
//	parameters["sigma"]="0.001";
//	parameters["iterations"]="1";
//
//	return parameters;
//}
