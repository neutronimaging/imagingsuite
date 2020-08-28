#include "scaledatadlg.h"
#include "ui_scaledatadlg.h"


#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ParameterHandling.h>

ScaleDataDlg::ScaleDataDlg(QWidget *parent) :
    ConfiguratorDialogBase("ScaleData", true, false, false,parent),
    ui(new Ui::ScaleDataDlg)
{
    ui->setupUi(this);
}

ScaleDataDlg::~ScaleDataDlg()
{
    delete ui;
}

int ScaleDataDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fSlope = static_cast<double>(GetFloatParameter(parameters,"slope"));
    m_fIntercept = static_cast<double>(GetFloatParameter(parameters,"intercept"));
    m_bAutoScale = kipl::strings::string2bool(GetStringParameter(parameters,"autoscale"));

    UpdateDialog();

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
        return true;
    }
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }


    return 0;
}

void ScaleDataDlg::UpdateDialog()
{
    ui->doubleSpinBox_slope->setValue(m_fSlope);
    ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    ui->checkBox->setChecked(m_bAutoScale);
}

void ScaleDataDlg::UpdateParameters()
{
    m_fSlope=ui->doubleSpinBox_slope->value();
    m_fIntercept= ui->doubleSpinBox_intercept->value();
    m_bAutoScale=ui->checkBox->isChecked();
}


void ScaleDataDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["slope"]     = kipl::strings::value2string(m_fSlope);
    parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
    parameters["autoscale"] = kipl::strings::bool2string(m_bAutoScale);
}

void ScaleDataDlg::ApplyParameters()
{

}
