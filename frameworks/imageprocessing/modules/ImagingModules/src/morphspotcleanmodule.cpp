#include "../../src/morphspotcleanmodule.h"
#include <thread>
#include <cstdlib>
#include "ImagingModules_global.h"
#include "morphspotcleanmodule.h"
#include <ParameterHandling.h>
#include <MorphSpotClean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <ImagingException.h>
#include <KiplFrameworkException.h>
#include <ModuleException.h>

MorphSpotCleanModule::MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor) :
    KiplProcessModuleBase("MorphSpotClean",interactor),
    m_eConnectivity(kipl::morphology::conn4),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectPeaks),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
    m_fThreshold(0.1),
    m_fSigma(0.01),
    m_nEdgeSmoothLength(5),
    m_nMaxArea(30),
    m_fMinLevel(-0.1),
    m_fMaxLevel(12),
    m_bThreading(false)
{

}

MorphSpotCleanModule::~MorphSpotCleanModule()
{

}

int MorphSpotCleanModule::Configure(std::map<string, string> parameters)
{
    string2enum(GetStringParameter(parameters,"connectivity"),m_eConnectivity);
    string2enum(GetStringParameter(parameters,"cleanmethod"),m_eCleanMethod);
    string2enum(GetStringParameter(parameters,"detectionmethod"),m_eDetectionMethod);
    m_fThreshold        = GetFloatParameter(parameters,"threshold");
    m_fSigma            = GetFloatParameter(parameters,"sigma");
    m_nEdgeSmoothLength = GetIntParameter(parameters,"edgesmooth");
    m_nMaxArea          = GetIntParameter(parameters,"maxarea");
    m_fMinLevel         = GetFloatParameter(parameters,"minlevel");
    m_fMaxLevel         = GetFloatParameter(parameters,"maxlevel");
    m_bThreading        = kipl::strings::string2bool(GetStringParameter(parameters,"threading"));

    return 0;
}

std::map<string, string> MorphSpotCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["connectivity"] = enum2string(m_eConnectivity);
    parameters["cleanmethod"]  = enum2string(m_eCleanMethod);
    parameters["detectionmethod"] = enum2string(m_eDetectionMethod);
    parameters["threshold"]    = kipl::strings::value2string(m_fThreshold);
    parameters["sigma"]        = kipl::strings::value2string(m_fSigma);
    parameters["edgesmooth"]   = kipl::strings::value2string(m_nEdgeSmoothLength);
    parameters["maxarea"]      = kipl::strings::value2string(m_nMaxArea);
    parameters["minlevel"]     = kipl::strings::value2string(m_fMinLevel);
    parameters["maxlevel"]     = kipl::strings::value2string(m_fMaxLevel);
    parameters["threading"]    = kipl::strings::bool2string(m_bThreading);

    return parameters;
}

int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
{

}
