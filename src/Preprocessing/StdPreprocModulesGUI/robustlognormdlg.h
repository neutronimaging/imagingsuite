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

    void on_buttonPreviewOBBB_clicked();

    void on_button_BBroi_clicked();

    void on_spinx0BBroi_valueChanged();
    void on_spiny0BBroi_valueChanged();
    void on_spinx1BBroi_valueChanged();
    void on_spiny1BBroi_valueChanged();

    void on_button_sampleBBpath_clicked();

    void on_buttonPreviewsampleBB_clicked();

    void on_button_BBdose_clicked();

    void on_spinx0BBdose_valueChanged();
    void on_spinx1BBdose_valueChanged();
    void on_spiny0BBdose_valueChanged();
    void on_spiny1BBdose_valueChanged();



    void on_errorButton_clicked();

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void UpdateParameterList(std::map<std::string, std::string> &parameters);

    void BrowseOBBBPath();
    void UpdateBBROI();

    void BrowseSampleBBPath();
    void UpdateDoseROI();

    Ui::RobustLogNormDlg *ui;
    ReconConfig *m_Config;

    size_t nBBCount; /// number of open beam images with BB
    size_t nBBFirstIndex; /// first index in filename for OB images with BB
    std::string blackbodyname;

    size_t nBBSampleCount; /// number of sample images with BB
    size_t nBBSampleFirstIndex; /// first index in filename for sample images with BB
    std::string blackbodysamplename;
    size_t BBroi[4];
    size_t doseBB[4];
    size_t radius;


    kipl::base::TImage <float,2> m_Preview_OBBB;
    kipl::base::TImage <float,2> m_Preview_sampleBB;



};

#endif // ROBUSTLOGNORMDLG_H
