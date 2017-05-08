//<LICENSE>

#include "datascalerdlg.h"
#include "ui_datascalerdlg.h"
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ParameterHandling.h>

DataScalerDlg::DataScalerDlg(QWidget *parent) :
    ConfiguratorDialogBase("DataScaler",true,false,false,parent),
    ui(new Ui::DataScalerDlg)
{
    ui->setupUi(this);
}

DataScalerDlg::~DataScalerDlg()
{
    delete ui;
}

int DataScalerDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> & UNUSED(img))
{
    m_fOffset = GetFloatParameter(parameters,"offset");
    m_fSlope     = GetFloatParameter(parameters,"slope");

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

void DataScalerDlg::ApplyParameters()
{}

void DataScalerDlg::UpdateDialog()
{
    ui->spinOffset->setValue(m_fOffset);
    ui->spinSlope->setValue(m_fSlope);
}

void DataScalerDlg::UpdateParameters()
{
    m_fOffset = ui->spinOffset->value();
    m_fSlope     = ui->spinSlope->value();
}

void DataScalerDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["offset"]=m_fOffset;
    parameters["slope"]=m_fSlope;
}
