/*
 * RemoveBackground.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: anders
 */

#include "RemoveBackground.h"

#include <sstream>

#include <segmentation/thresholds.h>
#include <morphology/morphdist.h>
#include <base/timage.h>
#include <strings/miscstring.h>

#include <ParameterHandling.h>

RemoveBackground::RemoveBackground()  :
	KiplProcessModuleBase("RemoveBackground"),
	m_fThreshold(1.0f),
	m_bDoMasking(false),
	m_fPruneRadius(5.0f),
	m_fMaskValue(0.0f)
{
}

RemoveBackground::~RemoveBackground() {
}

int RemoveBackground::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{
	m_fThreshold   = GetFloatParameter(parameters,"threshold");
	m_bDoMasking   = kipl::strings::string2bool(GetStringParameter(parameters,"domasking"));
	m_fPruneRadius = GetFloatParameter(parameters,"pruneradius");
	m_fMaskValue   = GetFloatParameter(parameters,"maskvalue");

	return 0;
}

std::map<std::string, std::string> RemoveBackground::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["threshold"]   = kipl::strings::value2string(m_fThreshold);
	parameters["domasking"]   = kipl::strings::bool2string(m_bDoMasking);
	parameters["pruneradius"] = kipl::strings::value2string(m_fPruneRadius);
	parameters["maskvalue"]   = kipl::strings::value2string(m_fMaskValue);

	return parameters;
}

int RemoveBackground::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<char,3>   mask(img.Dims());
	kipl::base::TImage<float,3>  distance(img.Dims());

	size_t N=img.Size();

	Threshold(img.GetDataPtr(),mask.GetDataPtr(),N,m_fThreshold, kipl::segmentation::cmp_greater);

	// Expand region
    kipl::morphology::old::EuclideanDistance(mask,distance,kipl::morphology::conn26);
	Threshold(distance.GetDataPtr(),mask.GetDataPtr(),img.Size(),m_fPruneRadius, kipl::segmentation::cmp_greater);

	// Shrink region
    kipl::morphology::old::EuclideanDistance(mask,distance,kipl::morphology::conn26);
	Threshold(distance.GetDataPtr(),mask.GetDataPtr(),img.Size(),m_fPruneRadius, kipl::segmentation::cmp_greater);

	float *pImg  = img.GetDataPtr();
	char  *pMask = mask.GetDataPtr();

	if (m_bDoMasking) {
		logger(kipl::logging::Logger::LogMessage,"Masking out background");
		for (size_t i=0; i<img.Size(); i++) {
			pImg[i]= pMask[i] != 0 ? pImg[i] : m_fMaskValue;
		}

	}
	else {
		logger(kipl::logging::Logger::LogMessage,"Transferring mask to image");
		for (size_t i=0; i<img.Size(); i++) {
			pImg[i]= static_cast<float>(pMask[i])  ;
		}
	}

	return 0;
}

int RemoveBackground::Threshold(float * data,
			char *result,
			size_t N,
			float th,
			kipl::segmentation::CmpType cmp)
	{
		std::ostringstream msg;


		char cmptrue,cmpfalse;

		if (cmp==kipl::segmentation::cmp_greater) {
			cmptrue=1;
			cmpfalse=0;
		}
		else {
			cmptrue=0;
			cmpfalse=1;
		}

		msg<<"Thresholding with th="<<th<<", N="<<N<<", cmptrue="<<int(cmptrue)<<", cmpfalse="<<int(cmpfalse);
		logger(kipl::logging::Logger::LogMessage,msg.str());

		size_t cnt=0;

		for (size_t i=0; i<N; i++) {
			if (th<data[i]) {
				result[i]=cmptrue;
				cnt++;
			}
			else
				result[i]=cmpfalse;

		}

		msg.str("");
		msg<<"Thresholding set "<<cnt<<" of "<<N<<" elements to true";
		logger(kipl::logging::Logger::LogMessage,msg.str());
		return 0;

}


