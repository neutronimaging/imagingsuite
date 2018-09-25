//<LICENSE>
#include "clampdatadlg.h"
#include "ui_clampdatadlg.h"

#include <ParameterHandling.h>

#include <strings/miscstring.h>

ClampDataDlg::ClampDataDlg(QWidget *parent) :
    ConfiguratorDialogBase("ClampData", true, false, false,parent),
    ui(new Ui::ClampDataDlg)
{
    ui->setupUi(this);
}

ClampDataDlg::~ClampDataDlg()
{
    delete ui;
}

int ClampDataDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fMin = static_cast<double>(GetFloatParameter(parameters,"min"));
    m_fMax = static_cast<double>(GetFloatParameter(parameters,"max"));

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

void ClampDataDlg::UpdateDialog()
{
    ui->doubleSpinBox_min->setValue(m_fMin);
    ui->doubleSpinBox_max->setValue(m_fMax);
}

void ClampDataDlg::UpdateParameters()
{
    m_fMin = ui->doubleSpinBox_min->value();
    m_fMax = ui->doubleSpinBox_max->value();
}

void ClampDataDlg::ApplyParameters()
{

}

void ClampDataDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters.clear();

    parameters["min"] = kipl::strings::value2string(m_fMin);
    parameters["max"] = kipl::strings::value2string(m_fMax);
}


