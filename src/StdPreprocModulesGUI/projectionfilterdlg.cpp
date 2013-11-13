#include "projectionfilterdlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <QMessageBox>

ProjectionFilterDlg::ProjectionFilterDlg(QWidget * parent) :
    ConfiguratorDialogBase("ProjectionFilterDlg",false,false,parent),
    m_checkbox_usebias("Use DC bias"),
    m_label_filter("Filter window"),
    m_label_cutoff("Cut-of frequency")
{
    m_combo_filtertype.addItem(QString::fromStdString(enum2string(ProjectionFilterBase::FilterRamLak)));
    m_combo_filtertype.addItem(QString::fromStdString(enum2string(ProjectionFilterBase::FilterSheppLogan)));
    m_combo_filtertype.addItem(QString::fromStdString(enum2string(ProjectionFilterBase::FilterHanning)));
    m_combo_filtertype.addItem(QString::fromStdString(enum2string(ProjectionFilterBase::FilterHamming)));
    m_combo_filtertype.addItem(QString::fromStdString(enum2string(ProjectionFilterBase::FilterButterworth)));

    m_hbox_filter.addWidget(&m_label_filter);
    m_hbox_filter.addWidget(&m_combo_filtertype);
    m_vbox_main.addLayout(&m_hbox_filter);
    m_hbox_filter.addWidget(&m_label_cutoff);
    m_hbox_filter.addWidget(&m_spin_cutoff);
    m_vbox_main.addLayout(&m_hbox_cutoff);
    m_vbox_main.addWidget(&m_checkbox_usebias);
    m_FrameMain.setLayout(&m_vbox_main);

    m_spin_cutoff.setRange(0.0,1.5);
    m_spin_cutoff.setSingleStep(0.05);
    setWindowTitle("Configure the reconstruction filter");
    UpdateDialog();

    show();
}

ProjectionFilterDlg::~ProjectionFilterDlg()
{

}

int ProjectionFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> UNUSED(img))
{
    m_Config=dynamic_cast<ReconConfig *>(config);

    try {
        string2enum(GetStringParameter(parameters,"filtertype"), m_eFilterType);
        m_fCutOff = GetFloatParameter(parameters,"cutoff");
        m_fOrder = GetFloatParameter(parameters,"order");
        m_bUseBias = kipl::strings::string2bool(GetStringParameter(parameters,"usebias"));
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
    logger(kipl::logging::Logger::LogMessage, "You shouldn't be here...");
}

void ProjectionFilterDlg::UpdateDialog()
{
    m_combo_filtertype.setCurrentIndex(static_cast<int>(m_eFilterType));
    m_spin_cutoff.setValue(m_fCutOff);
    m_checkbox_usebias.setChecked(m_bUseBias);

}

void ProjectionFilterDlg::UpdateParameters()
{
    m_eFilterType = static_cast<ProjectionFilterBase::FilterType>(m_combo_filtertype.currentIndex());
    m_fCutOff     = m_spin_cutoff.value();
   // m_fOrder      = m_spin ;
    m_bUseBias    = m_checkbox_usebias.checkState();
}

void ProjectionFilterDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["filtertype"]=enum2string(m_eFilterType);
    parameters["cutoff"]=kipl::strings::value2string(m_fCutOff);
    parameters["order"]=kipl::strings::value2string(m_fOrder);
    parameters["usebias"]=kipl::strings::bool2string(m_bUseBias);
}

void ProjectionFilterDlg::UpdateDialogFromParameterList(std::map<std::string, std::string> & UNUSED(parameters))
{

}
