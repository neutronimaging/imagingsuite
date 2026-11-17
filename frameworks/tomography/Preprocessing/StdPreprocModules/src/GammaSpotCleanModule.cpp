//<LICENSE>

#include "../include/StdPreprocModules_global.h"
#include "../include/GammaSpotCleanModule.h"
#include <ParameterHandling.h>
#include <gammaclean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <ImagingException.h>
#include <ReconException.h>


GammaSpotCleanModule::GammaSpotCleanModule(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("GammaSpotClean", interactor),
    m_fSigma(0.8f),
    m_fThreshold3(25),
    m_fThreshold5(100),
    m_fThreshold7(400),
    m_nMedianSize(3)
{
    publications.push_back(Publication({"H. Li","B. Schillinger","E. Calzada","Y. Liu","M. Muehlbauer"},
                                    "An adaptive algorithm for gamma spots removal in CCD-based neutron radiography and tomography",
                                    "Nuclear Instruments and Methods in Physics Research Section A: Accelerators, Spectrometers, Detectors and Associated Equipment",
                                    2006,
                                    1,
                                    565,
                                    "405-413",
                                     "10.1016/j.nima.2006.04.063"));
}

GammaSpotCleanModule::~GammaSpotCleanModule()
{
}

int GammaSpotCleanModule::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
{
    m_fThreshold3        = GetFloatParameter( parameters, "threshold3");
    m_fThreshold5        = GetFloatParameter( parameters, "threshold5");
    m_fThreshold7        = GetFloatParameter( parameters, "threshold7");
    m_fSigma             = GetFloatParameter( parameters, "sigma");
    m_nMedianSize        = GetIntParameter(   parameters, "mediansize");

    PreprocModuleBase::Configure(parameters);

    return 0;
}

std::map<std::string, std::string> GammaSpotCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["threshold3"] = kipl::strings::value2string(m_fThreshold3);
    parameters["threshold5"] = kipl::strings::value2string(m_fThreshold5);
    parameters["threshold7"] = kipl::strings::value2string(m_fThreshold7);
    parameters["sigma"]      = kipl::strings::value2string(m_fSigma);
    parameters["mediansize"] = kipl::strings::value2string(m_nMedianSize);

    return parameters;
}

bool GammaSpotCleanModule::SetROI(const std::vector<size_t> &/*UNUSED*/)
{
    return false;
}

int GammaSpotCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & /*coeff*/)
{
    return ProcessSingle(img);
    //return ProcessParallel(img);
}


int GammaSpotCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & /*coeff*/)
{
   std::ostringstream msg;
   ImagingAlgorithms::GammaClean cleaner(   m_fSigma,
                                            m_fThreshold3,m_fThreshold5,m_fThreshold7,
                                            m_nMedianSize,
                                            m_bThreading,
                                            m_Interactor);

    try {
        cleaner.process(img);
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
        msg.str("");
        msg<<"Failed to process data with unknown exception.";
        throw ReconException(msg.str(),__FILE__,__LINE__);

    }

    m_DetectionImg=cleaner.detectionImage(img);

    msg.str(""); msg<<"Processed image"<<img;
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return 0;
}

int GammaSpotCleanModule::ProcessSingle(kipl::base::TImage<float,3> & img)
{
    ImagingAlgorithms::GammaClean cleaner(  m_fSigma,
                                            m_fThreshold3,m_fThreshold5,m_fThreshold7,
                                            m_nMedianSize,
                                            m_bThreading,
                                            m_Interactor);
                                            
    std::ostringstream msg;
    
    try {
        cleaner.process(img);
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
        msg.str("");
        msg<<"Failed to process data with unknown exception.";
        throw ReconException(msg.str(),__FILE__,__LINE__);

    }

    msg.str(""); msg<<"Processed image"<<img;
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return 0;
}

int GammaSpotCleanModule::ProcessParallel(kipl::base::TImage<float,3> & /*img*/)
{

    return 0;
}

kipl::base::TImage<float,2> GammaSpotCleanModule::DetectionImage(kipl::base::TImage<float,2> img)
{
    return m_DetectionImg;
}
