#include "../../src/morphspotcleanmodule.h"

MorphSpotCleanModule::MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor=nullptr) :
    PreprocModuleBase("MorphSpotClean",interactor),
    m_eConnectivity(kipl::morphology::conn4),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectPeaks),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
    m_fThreshold(0.1f),
    m_fSigma(0.01f),
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

}

std::map<string, string> MorphSpotCleanModule::GetParameters()
{

}

int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float, 3> &img, std::map<string, string> &coeff)
{

}
