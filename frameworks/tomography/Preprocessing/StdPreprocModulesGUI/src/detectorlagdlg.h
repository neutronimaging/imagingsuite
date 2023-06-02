//<LICENSE>

#ifndef DETECTORLAGDLG_H
#define DETECTORLAGDLG_H
#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>


namespace Ui {
class DetectorLagDlg;
}

class DetectorLagDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DetectorLagDlg(QWidget *parent = nullptr);
    ~DetectorLagDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & UNUSED_img);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::DetectorLagDlg *ui;

    float m_correctionFactor;
    int m_medianKernelSize;
};

#endif // DETECTORLAGDLG_H
