//<LICENSE>


// ModuleConfigurator.cpp : Defines the exported functions for the DLL application.
//

#include <tuple>
#include "ConfiguratorDialogBase.h"

ConfiguratorDialogBase::ConfiguratorDialogBase(std::string name, bool emptyDialog, bool hasApply, bool needImages, QWidget *parent) :
    QDialog(parent),
    logger(name),
    m_bNeedImages(needImages)
{
    std::ignore = hasApply;
    std::ignore = emptyDialog;

}

ConfiguratorDialogBase::~ConfiguratorDialogBase()
{
}

int ConfiguratorDialogBase::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img)
{
    std::ignore = config;
    std::ignore = parameters;
    std::ignore = img;

    return QDialog::exec();
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetProjection(kipl::base::TImage<float,3> img, size_t n)
{
    kipl::base::TImage<float,2> proj(img.dims());

	memcpy(proj.GetDataPtr(), img.GetLinePtr(0,n), sizeof(float)*proj.Size());

	return proj;
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetSinogram(kipl::base::TImage<float,3> img, size_t n)
{
    std::vector<size_t> dims={img.Size(0),img.Size(2)};
	kipl::base::TImage<float,2> sino(dims);

    if (img.Size(1)<n)
    {
        throw kipl::base::KiplException("sinogram number out of bounds");
    }

	for (size_t i=0; i<img.Size(2); i++)
    {
		memcpy(sino.GetLinePtr(i),img.GetLinePtr(n,i),sizeof(float)*img.Size(0));
    }

	return sino;
}

std::map<std::string, std::string> ConfiguratorDialogBase::parameters()
{
    std::map<std::string, std::string> pars;

    UpdateParameters();
    UpdateParameterList(pars);

    return pars;
}

void ConfiguratorDialogBase::UpdateParameterList(std::map<std::string, std::string> &parameters)
{

}