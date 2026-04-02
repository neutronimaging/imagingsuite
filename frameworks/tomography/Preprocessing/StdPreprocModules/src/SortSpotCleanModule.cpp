//<LICENSE>
#include <thread>
#include <cstdlib>
#include <functional>
#include <algorithm>
#include "../include/StdPreprocModules_global.h"
#include "../include/SortSpotCleanModule.h"
#include <ParameterHandling.h>
#include <MorphSpotClean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ImagingException.h>
#include <ReconException.h>
#include <ModuleException.h>
#include <base/tpermuteimage.h>

SortSpotCleanModule::SortSpotCleanModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("SortSpotCleanModule",interactor),
    m_cleaner(true,32,true,interactor),
    m_fThreshold(0.95f),
    m_fQuantile(0.95f),
    m_nPatchSize(32),
    m_eQuantileMethod(ImagingAlgorithms::eSortSpotQuantile::BrightSpots),
    m_eConnectivity(kipl::base::eConnectivity::conn4),
    m_bRemoveInfNaN(false),
    m_bClampData(false),
    m_fMinLevel(-1.0f),
    m_fMaxLevel(7.0f)
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

SortSpotCleanModule::~SortSpotCleanModule()
{
}

int SortSpotCleanModule::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    std::ostringstream msg;
    setNumberOfThreads(config.System.nMaxThreads);

    try
    {
        m_fThreshold      = GetFloatParameter(parameters,"threshold");
        m_fQuantile       = GetFloatParameter(parameters,"quantile");
        m_nPatchSize      = GetIntParameter(parameters,"patchsize");
    
        string2enum(GetStringParameter(parameters,"connectivity"),m_eConnectivity);
        string2enum(GetStringParameter(parameters,"quantilemethod"),m_eQuantileMethod);

        m_bRemoveInfNaN   = kipl::strings::string2bool(GetStringParameter(parameters,"removeinfnan"));
        m_bClampData      = kipl::strings::string2bool(GetStringParameter(parameters,"clampdata"));
        m_fMinLevel       = GetFloatParameter(parameters,"minlevel");
        m_fMaxLevel       = GetFloatParameter(parameters,"maxlevel");
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

std::map<std::string, std::string> SortSpotCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    try
    {
        parameters["threshold"]    = std::to_string(m_fThreshold);
        parameters["quantile"]     = std::to_string(m_fQuantile);
        parameters["patchsize"]    = std::to_string(m_nPatchSize);

        parameters["connectivity"] = enum2string(m_eConnectivity);
        parameters["quantilemethod"]     = enum2string(m_eQuantileMethod);

        parameters["removeinfnan"] = kipl::strings::bool2string(m_bRemoveInfNaN);
        parameters["clampdata"]    = kipl::strings::bool2string(m_bClampData);
        parameters["minlevel"]     = std::to_string(m_fMinLevel);
        parameters["maxlevel"]     = std::to_string(m_fMaxLevel);
    }
    catch (kipl::base::KiplException &e)
    {
        throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
    }

    return parameters;
}

bool SortSpotCleanModule::SetROI(const std::vector<size_t> &roi)
{
    std::ignore = roi;
    return false;
}

int SortSpotCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & /*coeff*/)
{

    logger.message("ProcessCore 3D");

    
    std::ostringstream msg;

    try {
        m_cleaner.process(img,m_fQuantile,m_fThreshold,m_eQuantileMethod);
    }
    catch (ImagingException & e) {
        msg.str("");
        msg<<"Failed to process data with ImagingException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException & e) {
        msg.str("");
        msg<<"Failed to process data with KiplException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception & e) {
        msg.str("");
        msg<<"Failed to process data with STL exception : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {
        throw ReconException("Failed to process with unknown exception",__FILE__,__LINE__);
    }

    return 0;
}


int SortSpotCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & /*coeff*/)
{
    std::ostringstream msg;

    try {
        m_cleaner.process(img,m_fQuantile,m_fThreshold,m_eQuantileMethod);
    }
    catch (ImagingException & e) {
        msg.str("");
        msg<<"Failed to process data with ImagingException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException & e) {
        msg.str("");
        msg<<"Failed to process data with KiplException : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception & e) {
        msg.str("");
        msg<<"Failed to process data with STL exception : "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {


    }

    return 0;
}

// pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> SortSpotCleanModule::DetectionImage(kipl::base::TImage<float,2> img, ImagingAlgorithms::eMorphDetectionMethod dm, bool removeBias)
// {
//     ImagingAlgorithms::MorphSpotClean cleaner;
//     cleaner.setCleanMethod(dm,m_eCleanMethod);
//     return cleaner.detectionImage(img,removeBias);
// }
