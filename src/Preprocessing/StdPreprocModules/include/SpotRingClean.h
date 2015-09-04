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


#ifndef SPOTRINGCLEAN_H_
#define SPOTRINGCLEAN_H_

#include <map>
#include "SpotClean2.h"
#include "NormPlugins.h"
#include <base/timage.h>
#include <containers/ArrayBuffer.h>
#include <ReconHelpers.h>

//class SpotRingClean : public SpotClean2
//{
//	public:
//		SpotRingClean(std::string name="SpotRingClean");
//
//		virtual bool SetROI(size_t *roi);
//
//		virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
//		virtual std::map<std::string, std::string> GetParameters();
//
//		kipl::base::TImage<float,2> GetSpotImage() {return spotimg;}
//		kipl::base::TImage<float,2> GetCumulatedImage() {return cumimg;}
//
//		kipl::base::TImage<float,2> RingDetectionImage(kipl::base::TImage<float,2> img);
//	protected:
//		virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, float> &coeff);
//		kipl::containers::ArrayBuffer<PixelInfo2> mRingList;
//		NormBase * mNorm;
//		std::map<float,ProjectionInfo> mProjectionList;
//		kipl::base::TImage<float,2> spotimg;
//		kipl::base::TImage<float,2> cumimg;
//
//};

#endif /* SPOTRINGCLEAN_H_ */
