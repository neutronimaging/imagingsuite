//<LICENSE>
//#include "stdafx.h"
#include <thread>
#include <cstdlib>
#include <functional>
#include <algorithm>
#include "../include/StdPreprocModules_global.h"
#include "../include/MorphSpotCleanModule.h"
#include <ParameterHandling.h>
#include <MorphSpotClean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ImagingException.h>
#include <ReconException.h>
#include <ModuleException.h>
#include <base/tpermuteimage.h>

MorphSpotCleanModule::MorphSpotCleanModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("MorphSpotClean",interactor),
    m_eConnectivity(kipl::base::conn4),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectPeaks),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
    m_fThreshold{0.95f,0.95f},
    m_fSigma{0.025f,0.025f},
    m_bThresholdByFraction(true),
    m_nEdgeSmoothLength(5),
    m_nMaxArea(30),
    m_bRemoveInfNaN(false),
    m_bClampData(false),
    m_fMinLevel(-0.1f), // This shouldnt exist...
    m_fMaxLevel(7.0f), //This corresponds to 0.1% transmission
    m_bTranspose(false)
{
    m_bThreading = true;
    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner"}),
                                       "MuhRec - a new tomography reconstructor",
                                       "Nuclear Instruments and Methods Section A",
                                       2011,
                                       651,
                                       1,
                                       "156-160",
                                       "10.1016/j.nima.2011.01.129"));

    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner","C. Carminati"}),
                                       "MuhRec software",
                                       "Zenodo",
                                       2019,
                                       1,
                                       1,
                                       "no pages",
                                       "10.5281/zenodo.1117850"));
}

MorphSpotCleanModule::~MorphSpotCleanModule()
{
}

int MorphSpotCleanModule::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
{
    std::ostringstream msg;

    try
    {
        string2enum(GetStringParameter(parameters,"connectivity"),m_eConnectivity);
        string2enum(GetStringParameter(parameters,"cleanmethod"),m_eCleanMethod);
        string2enum(GetStringParameter(parameters,"detectionmethod"),m_eDetectionMethod);
        kipl::strings::string2vector(GetStringParameter(parameters,"threshold"),m_fThreshold);
        kipl::strings::string2vector(GetStringParameter(parameters,"sigma"),m_fSigma);
        m_bThresholdByFraction = kipl::strings::string2bool(GetStringParameter(parameters,"thresholdbyfraction"));
        m_nEdgeSmoothLength = GetIntParameter(parameters,"edgesmooth");
        m_nMaxArea          = GetIntParameter(parameters,"maxarea");
        m_bRemoveInfNaN     = kipl::strings::string2bool(GetStringParameter(parameters,"removeinfnan"));
        m_bClampData        = kipl::strings::string2bool(GetStringParameter(parameters,"clampdata"));
        m_fMinLevel         = GetFloatParameter(parameters,"minlevel");
        m_fMaxLevel         = GetFloatParameter(parameters,"maxlevel");
        m_bThreading        = kipl::strings::string2bool(GetStringParameter(parameters,"threading"));
        m_bTranspose        = kipl::strings::string2bool(GetStringParameter(parameters,"transpose"));
    }
    catch (ImagingException &e) {
        msg<<"Imaging exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (ModuleException &e) {
        msg<<"Module exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Kipl Exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg<<"STL exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    return 0;
}

std::map<std::string, std::string> MorphSpotCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    try
    {
        parameters["connectivity"]    = enum2string(m_eConnectivity);
        parameters["cleanmethod"]     = enum2string(m_eCleanMethod);
        parameters["detectionmethod"] = enum2string(m_eDetectionMethod);
        parameters["threshold"]    = kipl::strings::Vector2String(m_fThreshold);
        parameters["sigma"]        = kipl::strings::Vector2String(m_fSigma);
        parameters["thresholdbyfraction"] = kipl::strings::bool2string(m_bThresholdByFraction);
        parameters["edgesmooth"]   = kipl::strings::value2string(m_nEdgeSmoothLength);
        parameters["maxarea"]      = kipl::strings::value2string(m_nMaxArea);
        parameters["removeinfnan"] = kipl::strings::bool2string(m_bRemoveInfNaN);
        parameters["clampdata"]    = kipl::strings::bool2string(m_bClampData);
        parameters["minlevel"]     = kipl::strings::value2string(m_fMinLevel);
        parameters["maxlevel"]     = kipl::strings::value2string(m_fMaxLevel);
        parameters["threading"]    = kipl::strings::bool2string(m_bThreading);
        parameters["transpose"]    = kipl::strings::bool2string(m_bTranspose);
    }
    catch (kipl::base::KiplException &e)
    {
        throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
    }

    return parameters;
}

bool MorphSpotCleanModule::SetROI(const std::vector<size_t> &roi)
{
    return false;
}

int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    logger(logger.LogMessage,"ProcessCore");

    if (m_bThreading)
        return ProcessParallelStd(img);
    else
        return ProcessSingle(img);

    return 0;
}


int MorphSpotCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    std::ostringstream msg;
    ImagingAlgorithms::MorphSpotClean cleaner;
    cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
    cleaner.setConnectivity(m_eConnectivity);
    cleaner.setLimits(m_bClampData,m_fMinLevel,m_fMaxLevel,m_nMaxArea);
    cleaner.setCleanInfNan(m_bRemoveInfNaN);
    cleaner.setThresholdByFraction(m_bThresholdByFraction);

    try {
        cleaner.process(img,m_fThreshold, m_fSigma);
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

    return 0;
}

int MorphSpotCleanModule::ProcessSingle(kipl::base::TImage<float,3> & img)
{
    std::ostringstream msg;
    const int N = static_cast<int>(img.Size(2));

    int i;


    kipl::base::TImage<float,2> proj(img.dims());
    ImagingAlgorithms::MorphSpotClean cleaner;

    cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
    cleaner.setConnectivity(m_eConnectivity);
    cleaner.setLimits(m_bClampData,m_fMinLevel,m_fMaxLevel,m_nMaxArea);
    cleaner.setCleanInfNan(m_bRemoveInfNaN);
    cleaner.setThresholdByFraction(m_bThresholdByFraction);

    msg.str("");
    try {
        for (i=0; (i<N) && (UpdateStatus(float(i)/N,m_sModuleName)==false); i++) {

            memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
            cleaner.process(proj,m_fThreshold,m_fSigma);
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
        kipl::base::TImage<float,2> proj(img.dims());
        ImagingAlgorithms::MorphSpotClean cleaner;
        cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
        cleaner.setConnectivity(m_eConnectivity);
        cleaner.setLimits(m_bClampData,m_fMinLevel,m_fMaxLevel,m_nMaxArea);

#pragma omp for
        for (i=0; i<N; i++) {
            memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
            cleaner.process(proj,m_fThreshold,m_fSigma);
            memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),proj.Size()*sizeof(float));
        }
    }

    return 0;
}

int MorphSpotCleanModule::ProcessParallelStd(kipl::base::TImage<float,3> & img)
{
    std::ostringstream msg;
    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();


    std::vector<std::thread> threads;
    const size_t N = img.Size(2);

    size_t M=N/concurentThreadsSupported;

    msg.str("");
    msg<<N<<" projections on "<<concurentThreadsSupported<<" threads, "<<M<<" projections per thread";
    logger(logger.LogMessage,msg.str());

    for(size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        // spawn threads
        size_t rest=(i==concurentThreadsSupported-1)*(N % concurentThreadsSupported); // Take care of the rest slices
        auto pImg = &img;
        threads.push_back(std::thread([=] { ProcessParallelStdBlock(i,pImg,i*M,M+rest); }));
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    return 0;
}

int MorphSpotCleanModule::ProcessParallelStdBlock(size_t tid, kipl::base::TImage<float, 3> *img, size_t firstSlice, size_t N)
{
    std::ostringstream msg;
    size_t i;

    msg<<"Starting morphclean thread number="<<tid<<", N="<<N;
    logger.verbose(msg.str());
    msg.str("");
    msg<<"Thread "<<tid<<" progress :";

    kipl::base::TImage<float,2> proj(img->dims());

    kipl::base::Transpose<float> transpose;
    transpose.bUseReference=true;
    try
    {
        ImagingAlgorithms::MorphSpotClean cleaner;
        cleaner.setCleanMethod(m_eDetectionMethod,m_eCleanMethod);
        cleaner.setConnectivity(m_eConnectivity);
        cleaner.setLimits(m_bClampData,m_fMinLevel,m_fMaxLevel,m_nMaxArea);
        cleaner.setCleanInfNan(m_bRemoveInfNaN);
        cleaner.setThresholdByFraction(m_bThresholdByFraction);

        for (i=0; i<N; i++)
        {
            std::copy_n(img->GetLinePtr(0,firstSlice+i),proj.Size(),proj.GetDataPtr());

            if (m_bTranspose)
            {
                auto tproj = transpose(proj);
                cleaner.process(tproj,this->m_fThreshold,this->m_fSigma);
                proj = transpose(tproj);
            }
            else
            {
                cleaner.process(proj,this->m_fThreshold,this->m_fSigma);
            }

            std::copy_n(proj.GetDataPtr(),proj.Size(),img->GetLinePtr(0,firstSlice+i));

            msg<<".";
        }
    }
    catch (...)
    {
        msg<<"Thread tid="<<tid<<" failed with an exception.";
        logger(logger.LogError,msg.str());
    }

    logger.verbose(msg.str());

    return 0;
}

pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> MorphSpotCleanModule::DetectionImage(kipl::base::TImage<float,2> img, ImagingAlgorithms::eMorphDetectionMethod dm, bool removeBias)
{
    ImagingAlgorithms::MorphSpotClean cleaner;
    cleaner.setCleanMethod(dm,m_eCleanMethod);
    return cleaner.detectionImage(img,removeBias);
}
