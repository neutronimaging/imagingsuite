#ifndef ROBUSTLOGNORMDLG_H
#define ROBUSTLOGNORMDLG_H
#include "StdPreprocModulesGUI_global.h"

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <QDialog>
#include <imageviewerwidget.h>
#include <plotter.h>
#include <QtAddons_global.h>

#include <robustlognorm.h>


namespace Ui {
class RobustLogNormDlg;
}

class RobustLogNormDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit RobustLogNormDlg(QWidget *parent = NULL);
     ~RobustLogNormDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private slots:
    void on_button_OBBBpath_clicked();

    void on_spinFirstOBBB_editingFinished();

    void on_spinLastOBBB_editingFinished();

    void on_buttonPreviewOBBB_clicked();

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    void BrowseOBBBPath();

    Ui::RobustLogNormDlg *ui;
    ReconConfig *m_Config;

    size_t nBBCount; /// number of open beam images with BB
    size_t nBBFirstIndex; /// first index in filename for OB images with BB
    std::string blackbodyname;

    kipl::base::TImage <float,2> m_Preview_OBBB;

};

#endif // ROBUSTLOGNORMDLG_H
