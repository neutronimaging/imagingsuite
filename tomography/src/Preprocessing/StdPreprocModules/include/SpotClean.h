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

#ifndef _SPOTCLEAN_H_
#define _SPOTCLEAN_H_

#include <PreprocModuleBase.h>
#include <base/timage.h>
#include <filters/filterbase.h>
#include <math/LUTCollection.h>
#include <map>
#include <list>
#include "../include/PreprocEnums.h"
#include <ReconConfig.h>

class PixelInfo {
public:
	PixelInfo() :pos(0), stddev(0.0f), medval(0.0f) {}
	PixelInfo(int nPos,float fStdDev, float fMedVal) : pos(nPos), stddev(fStdDev), medval(fMedVal) {}

	int pos;
	float stddev;
	float medval;
}; 

class SpotClean :
	public PreprocModuleBase
{
public:
	SpotClean(void);
	virtual ~SpotClean(void);

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();

	kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img, eSpotDetection method, size_t dims);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> &coeff);
private:

	kipl::base::TImage<float,2> FillHoles(kipl::base::TImage<float,2> img);
	kipl::base::TImage<float,2> CleanIteration(kipl::base::TImage<float,2> img);
	kipl::base::TImage<float,2> ProcessPixelList(kipl::base::TImage<float,2> img);
	kipl::base::TImage<float,2> ProcessLevelSets(kipl::base::TImage<float,2> img);
	kipl::base::TImage<float,2> BoxFilter(kipl::base::TImage<float,2> img, size_t dim);


	kipl::base::TImage<float,2> UnbiasedImage(kipl::base::TImage<float,2> img, eSpotDetection method, size_t dims);
	kipl::base::TImage<float,2> StdDev(kipl::base::TImage<float,2> img, size_t dim);

	bool DilatePixelList(kipl::base::TImage<float,2> *img, std::list<size_t> *list, const int halfkernel);
	size_t ExtractLocalData(size_t pixel, kipl::base::TImage<float,2> *result, float *localdata);
	

	PixelInfo LocalStatistics(float *img, 
									 const int pos, 
									 const int imgSize, 
									 const int sx,
									 const int halfkernel,
									 const float k0,
									 const float k1);

	double ChangeStatistics(kipl::base::TImage<float,2> img);

	kipl::base::TImage<float,2> mask;
	kipl::base::TImage<float,2> medianImage;
	size_t nFilterDims[2];
	float *pKernel;
	size_t nKernelSize;
	size_t nIterations;
	size_t nCurrentIteration;
	size_t sx;
	size_t first_line;
	size_t last_line;
	float fThreshold;
	float fWidth;
	eSpotCleanAlgorithm mAlgorithm;
	eSpotDetection mDetection;
	size_t nWindowSize;

	std::list<size_t> processList;
	kipl::math::SigmoidLUT mLUT;
	
	kipl::filters::FilterBase::EdgeProcessingStyle eEdgeProcessingStyle; 
};


#endif
