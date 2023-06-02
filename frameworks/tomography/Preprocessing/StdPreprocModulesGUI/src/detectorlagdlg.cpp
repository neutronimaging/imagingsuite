//<LICENSE>

#include "detectorlagdlg.h"
#include "ui_detectorlagdlg.h"
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ParameterHandling.h>

DetectorLagDlg::DetectorLagDlg(QWidget *parent) :
    ConfiguratorDialogBase("DetectorLag",true,false,false,parent),
    ui(new Ui::DetectorLagDlg)
{
    ui->setupUi(this);
}

DetectorLagDlg::~DetectorLagDlg()
{
    delete ui;
}


int DetectorLagDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> & UNUSED(img))
{
    m_correctionFactor = GetFloatParameter(parameters,"correctionFactor");
    m_medianKernelSize  = GetIntParameter(parameters,"medianKernelSize");

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

    return false;
}

void DetectorLagDlg::ApplyParameters()
{}

void DetectorLagDlg::UpdateDialog()
{
    ui->spinCorrectionFactor->setValue(m_correctionFactor);
    ui->spinMedianKernelSize->setValue(m_medianKernelSize);
}

void DetectorLagDlg::UpdateParameters()
{
    m_correctionFactor = ui->spinCorrectionFactor->value();
    m_medianKernelSize = ui->spinMedianKernelSize->value();
}

void DetectorLagDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["correctionFactor"] = m_correctionFactor;
    parameters["medianKernelSize"] = m_medianKernelSize;
}
