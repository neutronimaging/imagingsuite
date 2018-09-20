//#include <referenceimagecorrection.h>

#include "referenceimagecorrectionmodule.h"
#include <base/textractor.h>
#include <ParameterHandling.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

ReferenceImageCorrectionModule::ReferenceImageCorrectionModule() :
    KiplProcessModuleBase("ReferenceImageCorrectionModule",true),
    m_computeLogarithm(true),
    m_tau(0.99f),
    m_OBname("ob_####.tif"),
    m_OBfirst(1),
    m_OBcount(5),
    m_BBname("bb_####.tif"),
    m_BBfirst(1),
    m_BBcount(5),
    m_DCname("dc_####.tif"),
    m_DCfirst(1),
    m_DCcount(5)
{
    m_doseROI[0]=0; m_doseROI[1]=0; m_doseROI[2]=10; m_doseROI[3]=10;

}

ReferenceImageCorrectionModule::~ReferenceImageCorrectionModule()
{

}

int ReferenceImageCorrectionModule::Configure(std::map<std::string, std::string> parameters)
{
    m_computeLogarithm = kipl::strings::string2bool(GetStringParameter(parameters,"computelogarithm"));
    GetUIntParameterVector(parameters,"doseroi",m_doseROI,4);
    m_tau     = GetFloatParameter(parameters, "tau");
    m_OBname  = GetStringParameter(parameters,"obname");
    m_OBfirst = GetIntParameter(parameters,  "obfirst");
    m_OBcount = GetIntParameter(parameters,  "obcount");
    m_BBname  = GetStringParameter(parameters,"bbname");
    m_BBfirst = GetIntParameter(parameters,  "bbfirst");
    m_BBcount = GetIntParameter(parameters,  "bbcount");
    m_DCname  = GetStringParameter(parameters,"dcname");
    m_DCfirst = GetIntParameter(parameters,  "dcfirst");
    m_DCcount = GetIntParameter(parameters,  "dccount");

    return 0;
}

std::map<std::string, std::string> ReferenceImageCorrectionModule::GetParameters()
{
        std::map<std::string, std::string> parameters;

        parameters["computelogarithm"] = kipl::strings::bool2string(m_computeLogarithm);
        parameters["doseroi"] = kipl::strings::Array2String(m_doseROI,4);
        parameters["tau"]     = kipl::strings::value2string(m_tau);
        parameters["obname"]  = m_OBname;
        parameters["obfirst"] = kipl::strings::value2string(m_OBfirst);
        parameters["obcount"] = kipl::strings::value2string(m_OBcount);
        parameters["bbname"]  = m_BBname;
        parameters["bbfirst"] = kipl::strings::value2string(m_BBfirst);
        parameters["bbcount"] = kipl::strings::value2string(m_BBcount);
        parameters["dcname"]  = m_DCname;
        parameters["dcfirst"] = kipl::strings::value2string(m_DCfirst);
        parameters["dccount"] = kipl::strings::value2string(m_DCcount);
        return parameters;
}

int ReferenceImageCorrectionModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
//    ImagingAlgorithms::ReferenceImageCorrection ric;
//    size_t N=img.Size(2);

//    float *dose=new float[N];
//    std::fill_n(dose,N,1.0f);
//    GetDose(img,dose);

//    ric.LoadReferenceImages("",m_OBname,m_OBfirst,m_OBcount,
//                               m_DCname,m_DCfirst,m_DCcount,
//                               m_BBname,m_BBfirst,m_BBcount,nullptr,m_doseROI);

//    // ric.setDiffRoi();
//    ric.ComputeLogartihm(m_computeLogarithm);
//    ric.SetTau(m_tau);
//    ric.Process(img,dose);

//    delete [] dose;

    return 0;
}

void ReferenceImageCorrectionModule::GetDose(kipl::base::TImage<float,3> & img, float *dose)
{
    for (size_t i=0; i<img.Size(2); ++i)
    {
        dose[i]=1.0f;
    }
}
