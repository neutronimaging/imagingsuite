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

#ifndef TRANSLATEDPROJECTIONWEIGHTING_H_
#define TRANSLATEDPROJECTIONWEIGHTING_H_

#include <base/timage.h>
#include <PreprocModuleBase.h>
#include <ReconConfig.h>
#include "PreprocEnums.h"

class TranslatedProjectionWeighting : public PreprocModuleBase
{
public:	
	TranslatedProjectionWeighting();
	virtual ~TranslatedProjectionWeighting();
	
	int Setup(ReconConfig *config, eWeightFunctionType wf=WeightFunction_Sigmoid, float width=0.5f);


	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);

protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	void ComputeSigmoidWeights();
	void ComputeLinearWeights();
	kipl::base::TImage<float,2> fWeights;
	ReconConfig mConfig;
	eWeightFunctionType eWeightFunction;
	float mWidth;
	size_t mROI[4];
};

#endif /*TRANSLATEDPROJECTIONWEIGHTING_H_*/
