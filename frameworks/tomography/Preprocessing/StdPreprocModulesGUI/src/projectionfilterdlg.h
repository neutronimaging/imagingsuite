//<LICENSE>

#ifndef PROJECTIONFILTERDLG_H
#define PROJECTIONFILTERDLG_H

#include "StdPreprocModulesGUI_global.h"

#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>

//#include <ProjectionFilter.h>
#include <projectionfilter.h>

namespace Ui {
class ProjectionFilterDlg;
}

class ProjectionFilterDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ProjectionFilterDlg(QWidget *parent = nullptr);
    ~ProjectionFilterDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private:
    using ConfiguratorDialogBase::exec;
    Ui::ProjectionFilterDlg *ui;

    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    ReconConfig *m_ReconConfig;

    ImagingAlgorithms::ProjectionFilterType m_eFilterType;
    float m_fCutOff;
    float m_fOrder;
    bool m_bUseBias;
    float m_fBiasWeight;
    float m_fPadding;
};

#endif // PROJECTIONFILTERDLG_H
