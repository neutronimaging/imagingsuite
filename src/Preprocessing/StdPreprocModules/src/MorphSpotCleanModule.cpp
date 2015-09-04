//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/MorphSpotCleanModule.h"
#include <ParameterHandling.h>
#include <MorphSpotClean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <ImagingException.h>
#include <ReconException.h>
#include <ModuleException.h>


MorphSpotCleanModule::MorphSpotCleanModule() :
    PreprocModuleBase("MorphSpotClean"),
    m_eConnectivity(kipl::morphology::conn4),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectPeaks),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
    m_fThreshold(0.1f),
    m_fSigma(0.01f),
    m_nEdgeSmoothLength(5),
    m_nMaxArea(30),
    m_fMinLevel(-0.1),
    m_fMaxLevel(12)
{
}

MorphSpotCleanModule::~MorphSpotCleanModule()
{
}

int MorphSpotCleanModule::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
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

    return 0;
}

std::map<std::string, std::string> MorphSpotCleanModule::GetParameters()
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

    return parameters;
}

bool MorphSpotCleanModule::SetROI(size_t * UNUSED(roi))
{
    return false;
}

int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    clog<<"ProcessCore"<<endl;
#ifdef __APPLE__
    return ProcessSingle(img);
#else
    return ProcessSingle(img);
    //return ProcessParallel(img);
#endif

    return 0;
}


int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    std::ostringstream msg;
    ImagingAlgorithms::MorphSpotClean cleaner;
    cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
    cleaner.setConnectivity(m_eConnectivity);

    try {
    cleaner.Process(img,m_fThreshold, m_fSigma);

    }
    catch (ImagingException & e) {
        msg.str();
        msg<<"Failed to process data with ImagingException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException & e) {
        msg.str();
        msg<<"Failed to process data with KiplException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception & e) {
        msg.str();
        msg<<"Failed to process data with STL exception : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {


    }

//    msg.str(""); msg<<"Processed image"<<img;
//    logger(kipl::logging::Logger::LogMessage,msg.str());
    return 0;
}

int MorphSpotCleanModule::ProcessSingle(kipl::base::TImage<float,3> & img)
{
    std::ostringstream msg;
    const int N = static_cast<int>(img.Size(2));

    int i;

    kipl::base::TImage<float,2> proj(img.Dims());
    ImagingAlgorithms::MorphSpotClean cleaner;
    cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
    cleaner.setConnectivity(m_eConnectivity);

    msg.str("");
    try {
    for (i=0; i<N; i++) {
        memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
        cleaner.Process(proj,m_fThreshold,m_fSigma);
        memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),proj.Size()*sizeof(float));
    }
    }
    catch (ImagingException & e) {
        msg<<"Imaging exception while processing projection "<<i<<"("<<N<<") "<<std::endl<<e.what();
        throw ModuleException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException & e) {
        msg<<"kipl exception while processing projection "<<i<<"("<<N<<") "<<std::endl<<e.what();
        throw ModuleException(msg.str(),__FILE__,__LINE__);
    }
    catch (exception & e) {
        msg<<"STL exception while processing projection "<<i<<std::endl<<e.what();
        throw ModuleException(msg.str(),__FILE__,__LINE__);
    }

    return 0;
}

int MorphSpotCleanModule::ProcessParallel(kipl::base::TImage<float,3> & img)
{
    const int N = static_cast<int>(img.Size(2));

    int i;
    std::clog<<"Starting parallel processing"<<std::endl;
#pragma omp parallel
    {
        kipl::base::TImage<float,2> proj(img.Dims());
        ImagingAlgorithms::MorphSpotClean cleaner;
        cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
        cleaner.setConnectivity(m_eConnectivity);

#pragma omp for
        for (i=0; i<N; i++) {
            memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
            cleaner.Process(proj,m_fThreshold,m_fSigma);
            memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),proj.Size()*sizeof(float));
        }
    }

    return 0;
}

kipl::base::TImage<float,2> MorphSpotCleanModule::DetectionImage(kipl::base::TImage<float,2> img)
{
    ImagingAlgorithms::MorphSpotClean cleaner;
    cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
    return cleaner.DetectionImage(img);
}
