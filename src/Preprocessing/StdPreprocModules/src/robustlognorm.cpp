#include <strings/miscstring.h>

#include <ReconException.h>
#include <ProjectionReader.h>
#include <ReconConfig.h>

#include <ParameterHandling.h>

#include "../include/robustlognorm.h"

RobustLogNorm::RobustLogNorm() :
    NormBase("RobustLogNorm"),
    m_nWindow(5),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageAverage)
{

}

RobustLogNorm::~RobustLogNorm()
{

}


int RobustLogNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    NormBase::Configure(config,parameters);

    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"refavgmethod"),m_ReferenceAverageMethod);

    return 0;
}

std::map<std::string, std::string> RobustLogNorm::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters=NormBase::GetParameters();
    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["refavgmethod"] = enum2string(m_ReferenceAverageMethod);

    return parameters;
}

void RobustLogNorm::LoadReferenceImages(size_t *roi)
{
    m_corrector.LoadReferenceImages(flatname,nOBFirstIndex,nOBCount,
                                    darkname,nDCFirstIndex,nDCCount,
                                    blackbodyname,nBBFirstIndex,nBBCount,
                                    roi,nNormRegion,nullptr);
}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    float dose=GetFloatParameter(coeff,"dose")-fDarkDose;

    // Handling te non-dose correction case
    m_corrector.Process(img,dose);

    return 0;
}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    int nDose=img.Size(2);
    float *doselist=nullptr;

    std::stringstream msg;

    if (bUseNormROI==true) {
        doselist=new float[nDose];
        GetFloatParameterVector(coeff,"dose",doselist,nDose);
        for (int i=0; i<nDose; i++) {
            doselist[i] = doselist[i]-fDarkDose;
        }
    }

    m_corrector.Process(img,doselist);

    if (doselist!=nullptr)
        delete [] doselist;

    return 0;
}

void RobustLogNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{


}
