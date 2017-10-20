#include "genericbp.h"

#include <ParameterHandling.h>
#include <interactors/interactionbase.h>
#include <strings/miscstring.h>

GenericBP::GenericBP(kipl::interactors::InteractionBase *interactor) :
    BackProjectorModuleBase("muhrec","GenericBP",BackProjectorModuleBase::MatrixXYZ,interactor),
    m_sStringParameter("string"),
    m_fFloatParameter(1.23f),
    m_bBooleanParameter(true),
    m_nIntParameter(123)
{

}

GenericBP::~GenericBP()
{

}

size_t GenericBP::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{
 // Back project single projections at angle and weighted. This is an inifite task that is terminated when bLastProjection is true.

    return 0;
}

size_t GenericBP::Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters)
{
    // Back project a set of projections stored in a volume as xy-slices
    // In the parameter list there will be the parameters angles and weights are provided from the framework.
    // The method Process for single projections is usually called here
    return 0;
}

int GenericBP::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    mConfig = config;

    m_sStringParameter  = GetStringParameter(parameters,"par1");
    m_fFloatParameter   = GetIntParameter(parameters,"par2");
    m_bBooleanParameter = kipl::strings::string2bool(GetStringParameter(parameters,"par3"));

    return 0;
}

std::map<std::string, std::string> GenericBP::GetParameters()
{
    std::map<std::string, std::string> parameters;

    // The default values for the back projector are defined by initialization of the c'tor.
    parameters["par1"] = m_sStringParameter;
    parameters["par2"] = kipl::strings::value2string(m_fFloatParameter);
    parameters["par3"] = kipl::strings::bool2string(m_bBooleanParameter);

    return parameters;
}

void GenericBP::SetROI(size_t *roi)
{
    mConfig.ProjectionInfo.roi[0]=roi[0];
    mConfig.ProjectionInfo.roi[1]=roi[1];
    mConfig.ProjectionInfo.roi[2]=roi[2];
    mConfig.ProjectionInfo.roi[3]=roi[3];

    // Initialize region to reconstruct
}

void GenericBP::GetHistogram(float *x, size_t *y, size_t N)
{
    // x and y shall be allocated before calling GetHistogram.
    memset(x,0,sizeof(float)*N);
    memset(y,0,sizeof(size_t)*N);

    // Compute histogram of the reconstructed matrix.
}
