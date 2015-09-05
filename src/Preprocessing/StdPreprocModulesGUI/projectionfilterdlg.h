#ifndef PROJECTIONFILTERDLG_H
#define PROJECTIONFILTERDLG_H
#include <ConfiguratorDialogBase.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>

#include <ProjectionFilter.h>
#include <ReconConfig.h>

class ProjectionFilterDlg : public ConfiguratorDialogBase
{
public:
    ProjectionFilterDlg(QWidget * parent = NULL);
    virtual ~ProjectionFilterDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img);
private:
    virtual int exec() {return QDialog::exec();}
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void UpdateDialogFromParameterList(std::map<std::string, std::string> &parameters);

    QVBoxLayout m_vbox_main;
    QHBoxLayout m_hbox_filter;
    QHBoxLayout m_hbox_cutoff;
    QCheckBox m_checkbox_usebias;
    QDoubleSpinBox m_spin_cutoff;
    QComboBox m_combo_filtertype;
    QLabel m_label_filter;
    QLabel m_label_cutoff;

    bool m_bUseBias;
    float m_fCutOff;
    float m_fOrder;
    ProjectionFilterBase::FilterType m_eFilterType;

    ReconConfig *m_Config;
};

#endif // PROJECTIONFILTERDLG_H
