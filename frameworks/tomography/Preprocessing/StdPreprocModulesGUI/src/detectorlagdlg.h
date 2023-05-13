//<LICENSE>

#ifndef DETECTORLAGDLG_H
#define DETECTORLAGDLG_H

#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>

namespace Ui {
class DetectorLagDialog;
}

class DetectorLagDialog : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DetectorLagDialog(QWidget *parent = nullptr);
    ~DetectorLagDialog();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & UNUSED_img);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::DetectorLagDialog *ui;


    float m_fSlope; // Example Parameters
    float m_fOffset;
};

#endif // DETECTORLAGDLG_H
