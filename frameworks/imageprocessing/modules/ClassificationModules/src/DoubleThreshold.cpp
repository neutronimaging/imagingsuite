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
    m_Compare(kipl::segmentation::cmp_greatereq)
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
    string2enum(GetStringParameter(parameters,"compare"),m_Compare);

	return 0;
}

std::map<std::string, std::string> DoubleThreshold::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["highthreshold"]   = kipl::strings::value2string(m_fHighThreshold);
	parameters["lowthreshold"]    = kipl::strings::value2string(m_fLowThreshold);
    parameters["compare"]         = enum2string(m_Compare);

	return parameters;
}

int DoubleThreshold::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	kipl::base::TImage<char,3> mask(img.Dims());

	kipl::segmentation::DoubleThreshold(img,mask,
			m_fLowThreshold,m_fHighThreshold,
            m_Compare,
			kipl::morphology::conn26);

    std::copy_n(mask.GetDataPtr(),mask.Size(),img.GetDataPtr());
	return 0;
}

