//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include "../include/GammaSpotCleanModule.h"
#include <ParameterHandling.h>
#include <gammaclean.h>
#include <base/timage.h>
#include <strings/miscstring.h>
#include <ImagingException.h>
#include <ReconException.h>


GammaSpotCleanModule::GammaSpotCleanModule() :
    PreprocModuleBase("GammaSpotClean"),
    m_fSigma(0.8f),
    m_fThreshold3(25),
    m_fThreshold5(100),
    m_fThreshold7(400),
    m_nMedianSize(3)
{
}

GammaSpotCleanModule::~GammaSpotCleanModule()
{
}

int GammaSpotCleanModule::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
{

    m_fThreshold3        = GetFloatParameter(parameters,"threshold3");
    m_fThreshold5        = GetFloatParameter(parameters,"threshold5");
    m_fThreshold7        = GetFloatParameter(parameters,"threshold7");
    m_fSigma             = GetFloatParameter(parameters,"sigma");
    m_nMedianSize        = GetIntParameter(parameters,"mediansize");

    return 0;
}

std::map<std::string, std::string> GammaSpotCleanModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["threshold3"] = kipl::strings::value2string(m_fThreshold3);
    parameters["threshold5"] = kipl::strings::value2string(m_fThreshold5);
    parameters["threshold7"] = kipl::strings::value2string(m_fThreshold7);
    parameters["sigma"] = kipl::strings::value2string(m_fSigma);
    parameters["mediansize"] = kipl::strings::value2string(m_nMedianSize);

    return parameters;
}

bool GammaSpotCleanModule::SetROI(size_t * UNUSED(roi))
{
    return false;
}

int GammaSpotCleanModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
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


int GammaSpotCleanModule::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
//    std::ostringstream msg;
//    ImagingAlgorithms::GammaClean cleaner;
//    cleaner.Configure(m_fSigma,m_fThreshold3,m_fThreshold5,m_fThreshold7,m_nMedianSize);

//    try {
//        cleaner.Process(img);
//    }
//    catch (ImagingException & e) {
//        msg.str();
//        msg<<"Failed to process data with ImagingException : "<<std::endl<<e.what();
//        throw ReconException(msg.str(),__FILE__,__LINE__);
//    }
//    catch (kipl::base::KiplException & e) {
//        msg.str();
//        msg<<"Failed to process data with KiplException : "<<std::endl<<e.what();
//        throw ReconException(msg.str(),__FILE__,__LINE__);
//    }
//    catch (std::exception & e) {
//        msg.str();
//        msg<<"Failed to process data with STL exception : "<<std::endl<<e.what();
//        throw ReconException(msg.str(),__FILE__,__LINE__);
//    }
//    catch (...) {


//    }

//    msg.str(""); msg<<"Processed image"<<img;
//    logger(kipl::logging::Logger::LogMessage,msg.str());
    return 0;
}

int GammaSpotCleanModule::ProcessSingle(kipl::base::TImage<float,3> & img)
{
//    const int N = static_cast<int>(img.Size(2));

//    int i;

//    kipl::base::TImage<float,2> proj(img.Dims());
//    ImagingAlgorithms::GammaClean cleaner;
//    cleaner.Configure(m_fSigma,m_fThreshold3,m_fThreshold5,m_fThreshold7,m_nMedianSize);

//    for (i=0; i<N; i++) {
//        memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
//        cleaner.Process(proj);
//        memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),proj.Size()*sizeof(float));
//    }

    return 0;
}

int GammaSpotCleanModule::ProcessParallel(kipl::base::TImage<float,3> & img)
{
//    const int N = static_cast<int>(img.Size(2));

//    int i;
//    std::clog<<"Starting parallel processing"<<std::endl;
//#pragma omp parallel
//    {
//        kipl::base::TImage<float,2> proj(img.Dims());
//        ImagingAlgorithms::GammaClean cleaner;
//        cleaner.Configure(m_fSigma,m_fThreshold3,m_fThreshold5,m_fThreshold7,m_nMedianSize);

//#pragma omp for
//        for (i=0; i<N; i++) {
//            memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),proj.Size()*sizeof(float));
//            cleaner.Process(proj);
//            memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),proj.Size()*sizeof(float));
//        }
//    }

    return 0;
}

kipl::base::TImage<float,2> GammaSpotCleanModule::DetectionImage(kipl::base::TImage<float,2> img)
{
//    ImagingAlgorithms::GammaClean cleaner;
//    cleaner.Configure(m_fSigma,m_fThreshold3,m_fThreshold5,m_fThreshold7,m_nMedianSize);
//    return cleaner.DetectionImage(img);

    return img;
}
