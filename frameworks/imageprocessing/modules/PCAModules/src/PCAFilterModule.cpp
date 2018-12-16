//<LICENSE>
#include "stdafx.h"
#include "PCAFilterModule.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#include <pca/pca.h>
#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>
#include <math/covariance.h>
#include <containers/PlotData.h>

PCAFilterModule::PCAFilterModule() :
KiplProcessModuleBase("PCAFilterModule", true)
{

}

PCAFilterModule::~PCAFilterModule()
{
}


int PCAFilterModule::Configure(KiplProcessConfig m_Config, std::map<std::string, std::string> parameters)
{

//	m_bComplement = kipl::strings::string2bool(GetStringParameter(parameters,"complement"));
//	m_fMaxRadius  = GetFloatParameter(parameters,"maxradius");

    m_nLevel = GetIntParameter(parameters,"level");
    string2enum(GetStringParameter(parameters,"decomposition"),m_eDecompositionType);
    string2enum(GetStringParameter(parameters,"covariance"),m_eCovarianceType);

    m_bCenterData    = kipl::strings::string2bool(GetStringParameter(parameters,"center"));
    m_bNormalizeData = kipl::strings::string2bool(GetStringParameter(parameters,"normalize"));

	return 0;
}

std::map<std::string, std::string> PCAFilterModule::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["level"]=kipl::strings::value2string(m_nLevel);
    parameters["decomposition"]=enum2string(m_eDecompositionType);
    parameters["covariance"]=enum2string(m_eCovarianceType);
    parameters["center"]=kipl::strings::bool2string(m_bCenterData);
    parameters["normalize"]=kipl::strings::bool2string(m_bNormalizeData);

	return parameters;
}

int PCAFilterModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    kipl::pca::PCA pca;

    pca.setCovarianceType(m_eCovarianceType);
    pca.setDecompositionType(m_eDecompositionType);
    pca.setCenterNormalize(m_bCenterData,m_bNormalizeData);

    pca.filter(img,img,m_nLevel);

    kipl::containers::PlotData<float,float> eigplot(img.Size(2),"Eigen plot");
    kipl::containers::PlotData<float,float> normeigplot(img.Size(2),"Normalized eigen plot");
    TNT::Array1D<double> eigval=pca.eigenvalues();

    for (size_t i=0; i<eigplot.Size(); i++) {
        eigplot.GetX()[i]=static_cast<float>(i);
        eigplot.GetY()[i]=static_cast<float>(eigval[i]);
        normeigplot.GetX()[i]=static_cast<float>(i);
        normeigplot.GetY()[i]=static_cast<float>(eigval[i]/eigval[0]);
    }

    m_PlotList["Eigen values"]            = eigplot;
    m_PlotList["Normalized eigen values"] = normeigplot;

	return 0;
}
