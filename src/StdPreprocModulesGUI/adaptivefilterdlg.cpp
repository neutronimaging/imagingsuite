#include "adaptivefilterdlg.h"
#include "ui_adaptivefilterdlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>

AdaptiveFilterDlg::AdaptiveFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("AdaptiveFilter",true,false,true,parent),
    ui(new Ui::AdaptiveFilterDlg)
{
    ui->setupUi(this);
}

AdaptiveFilterDlg::~AdaptiveFilterDlg()
{
    delete ui;
}

int AdaptiveFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
{
    mConfig=config;
    m_fLambda     = GetFloatParameter(parameters,"lambda");
    m_fSigma      = GetFloatParameter(parameters,"sigma");
    m_nFilterSize = GetIntParameter(parameters,"size");

    UpdateDialog();
    ApplyParameters();


}

void AdaptiveFilterDlg::ApplyParameters()
{

}

void AdaptiveFilterDlg::UpdateDialog()
{

}

void AdaptiveFilterDlg::UpdateParameters()
{

}

void AdaptiveFilterDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["lambda"] = kipl::strings::value2string(m_fLambda);
    parameters["sigma"]  = kipl::strings::value2string(m_fSigma);
    parameters["size"]   = kipl::strings::value2string(m_nFilterSize);
}
