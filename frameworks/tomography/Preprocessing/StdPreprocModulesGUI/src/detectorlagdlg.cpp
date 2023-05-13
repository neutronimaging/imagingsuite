#include "detectorlagdlg.h"
#include "ui_detectorlagdlg.h"

DetectorLagDialog::DetectorLagDialog(QWidget *parent) :
    ConfiguratorDialogBase("DataScaler",true,false,false,parent),
    ui(new Ui::DetectorLagDialog)
{
    ui->setupUi(this);
}

DetectorLagDialog::~DetectorLagDialog()
{
    delete ui;
}


int DetectorLagDialog::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> & UNUSED(img))
{
    m_fOffset = GetFloatParameter(parameters,"offset");
    m_fSlope  = GetFloatParameter(parameters,"slope");

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

void DetectorLagDialog::ApplyParameters()
{}

void DetectorLagDialog::UpdateDialog()
{
    ui->spinOffset->setValue(m_fOffset);
    ui->spinSlope->setValue(m_fSlope);
}

void DetectorLagDialog::UpdateParameters()
{
    m_fOffset = ui->spinOffset->value();
    m_fSlope     = ui->spinSlope->value();
}

void DetectorLagDialog::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["offset"]=m_fOffset;
    parameters["slope"]=m_fSlope;
}
