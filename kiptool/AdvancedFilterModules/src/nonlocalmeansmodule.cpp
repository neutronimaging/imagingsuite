//<LICENSE>
#include "nonlocalmeansmodule.h"
#include <ParameterHandling.h>
#include <strings/miscstring.h>
#include <filters/nonlocalmeans.h>

NonLocalMeansModule::NonLocalMeansModule() :
    KiplProcessModuleBase("NonLocalMeansModule", true),
    m_fSensitivity(1000.0f),
    m_nWidth(5)
{

}

NonLocalMeansModule::~NonLocalMeansModule()
{

}

int NonLocalMeansModule::Configure(std::map<std::string, std::string> parameters)
{
    m_fSensitivity  = GetFloatParameter(parameters,"sensitivity");
    m_nWidth        = GetIntParameter(parameters,"width");

    return 0;
}

std::map<std::string, std::string> NonLocalMeansModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["sensitivity"]=kipl::strings::value2string(m_fSensitivity);
    parameters["width"]=kipl::strings::value2string(m_nWidth);

    return parameters;
}

int NonLocalMeansModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    akipl::NonLocalMeans nlm(m_nWidth,m_fSensitivity);

    kipl::base::TImage<float,3> res;

    nlm(img,res);

    img=res;

    return 0;
}
