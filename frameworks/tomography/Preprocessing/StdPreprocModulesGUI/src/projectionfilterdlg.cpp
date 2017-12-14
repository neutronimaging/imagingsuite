//<LICENSE>

#include "projectionfilterdlg.h"
#include "ui_projectionfilterdlg.h"

#include <QMessageBox>

#include <strings/miscstring.h>
#include <ParameterHandling.h>

ProjectionFilterDlg::ProjectionFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("ProjectionFilterDlg", true,false, false, parent),
    ui(new Ui::ProjectionFilterDlg)
{
    ui->setupUi(this);
    UpdateDialog();
}

ProjectionFilterDlg::~ProjectionFilterDlg()
{
    delete ui;
}

int ProjectionFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img)
{
    m_Config=dynamic_cast<ReconConfig *>(config);

    try {
        string2enum(GetStringParameter(parameters,"filtertype"), m_eFilterType);
        m_fCutOff = GetFloatParameter(parameters,"cutoff");
        m_fOrder = GetFloatParameter(parameters,"order");
        m_bUseBias = kipl::strings::string2bool(GetStringParameter(parameters,"usebias"));
        m_fBiasWeight = GetFloatParameter(parameters,"biasweight");
        m_fPadding = GetFloatParameter(parameters,"paddingdoubler");
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Missing parameter");
        msgbox.setText("A parameter was missing during dialog setup.");
        msgbox.setDetailedText(QString::fromStdString(e.what()));
        msgbox.exec();
        return QDialog::Rejected;
    }

    UpdateDialog();
    ApplyParameters();

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
    }
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }

    return res;
}

void ProjectionFilterDlg::ApplyParameters()
{
    logger(kipl::logging::Logger::LogMessage, "ApplyParameters:You shouldn't be here...");
}

void ProjectionFilterDlg::UpdateDialog()
{
    ui->combo_filterwindow->setCurrentIndex(static_cast<int>(m_eFilterType));
    ui->entry_cutoff->setValue(m_fCutOff);
    ui->check_bias->setChecked(m_bUseBias);
    ui->entry_biasfactor->setValue(m_fBiasWeight);
    ui->entry_padding->setValue(m_fPadding);
}

void ProjectionFilterDlg::UpdateParameters()
{
    m_eFilterType = static_cast<ProjectionFilterBase::FilterType>(ui->combo_filterwindow->currentIndex());
    m_fCutOff       = ui->entry_cutoff->value();
    m_fBiasWeight   = ui->entry_biasfactor->value();
    m_fPadding      = ui->entry_padding->value();
    m_bUseBias      = ui->check_bias->isChecked();
}

void ProjectionFilterDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["filtertype"]=enum2string(m_eFilterType);
    parameters["cutoff"]=kipl::strings::value2string(m_fCutOff);
    parameters["order"]=kipl::strings::value2string(m_fOrder);
    parameters["usebias"]=kipl::strings::bool2string(m_bUseBias);
    parameters["biasweight"]=kipl::strings::value2string(m_fBiasWeight);
    parameters["paddingdoubler"]=kipl::strings::bool2string(m_fPadding);
}
