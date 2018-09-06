//<LICENSE>
#include "DoubleThreshold.h"

#include <sstream>

#include <segmentation/thresholds.h>
#include <base/timage.h>
#include <strings/miscstring.h>

#include <ParameterHandling.h>

DoubleThreshold::DoubleThreshold():
	KiplProcessModuleBase("DoubleThreshold"),
	m_fHighThreshold(1000.0f),
	m_fLowThreshold(500.0f),
	m_fBackgroundValue(0.0f),
	m_bUseBackgroundValue(false)
{
	// TODO Auto-generated constructor stub

}

DoubleThreshold::~DoubleThreshold() {
	// TODO Auto-generated destructor stub
}

int DoubleThreshold::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{
	m_fHighThreshold      = GetFloatParameter(parameters,"highthreshold");
	m_fLowThreshold       = GetFloatParameter(parameters,"lowthreshold");
	m_fBackgroundValue    = GetFloatParameter(parameters,"backgroundvalue");
	m_bUseBackgroundValue = kipl::strings::string2bool(GetStringParameter(parameters,"usebackgroundvalue"));

	return 0;
}

std::map<std::string, std::string> DoubleThreshold::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["highthreshold"]   = kipl::strings::value2string(m_fHighThreshold);
	parameters["lowthreshold"]    = kipl::strings::value2string(m_fLowThreshold);
	parameters["backgroundvalue"] = kipl::strings::value2string(m_fBackgroundValue);
	parameters["usebackgroundvalue"] = kipl::strings::bool2string(m_bUseBackgroundValue);

	return parameters;
}

int DoubleThreshold::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<char,3> mask(img.Dims());

	kipl::segmentation::DoubleThreshold(img,mask,
			m_fLowThreshold,m_fHighThreshold,
			kipl::segmentation::cmp_greater,
			kipl::morphology::conn26);

	return 0;
}

