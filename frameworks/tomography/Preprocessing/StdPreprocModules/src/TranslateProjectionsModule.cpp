#include "../include/TranslateProjectionsModule.h"
#include <ParameterHandling.h>

TranslateProjectionsModule::TranslateProjectionsModule() :
    PreprocModuleBase("TranslateProjections"),
    m_fSlope(1.0f),
    m_fIntercept(0.0f),
    m_fWidth(1.0f),
    m_eSampler(ImagingAlgorithms::TranslateLinear)
{

}

TranslateProjectionsModule::~TranslateProjectionsModule()
{}

int TranslateProjectionsModule::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    m_fSlope     = GetFloatParameter(parameters,"slope");
    m_fIntercept = GetFloatParameter(parameters,"intercept");
    m_fWidth     = GetFloatParameter(parameters,"width");
    string2enum(GetStringParameter(parameters,"sampler"),m_eSampler);

    return 0;
}

std::map<std::string, std::string> TranslateProjectionsModule::GetParameters()
{
    std::map<std::string, std::string> parameters;
    parameters["slope"]     = kipl::strings::value2string(m_fSlope);
    parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
    parameters["width"]     = kipl::strings::value2string(m_fWidth);
    parameters["sampler"]   = enum2string(m_eSampler);

    return parameters;
}

bool TranslateProjectionsModule::SetROI(size_t *roi)
{
    return false;
}

int TranslateProjectionsModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    ImagingAlgorithms::TranslateProjections translator;

    translator.translate(img,m_fSlope, m_fIntercept, m_eSampler, m_fWidth);

    return 0;
}


