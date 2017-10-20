//<LICENSE>

#ifndef ROBUSTLOGNORMDLG_H
#define ROBUSTLOGNORMDLG_H
#include "StdPreprocModulesGUI_global.h"

#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <imageviewerwidget.h>
#include <plotter.h>

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

    void on_spinx0BBroi_valueChanged(int arg1);
    void on_spiny0BBroi_valueChanged(int arg1);
    void on_spinx1BBroi_valueChanged(int arg1);
    void on_spiny1BBroi_valueChanged(int arg1);

    void on_button_sampleBBpath_clicked();

    void on_buttonPreviewsampleBB_clicked();

    void on_button_BBdose_clicked();

    void on_spinx0BBdose_valueChanged(int arg1);
    void on_spinx1BBdose_valueChanged(int arg1);
    void on_spiny0BBdose_valueChanged(int arg1);
    void on_spiny1BBdose_valueChanged(int arg1);



    void on_errorButton_clicked();
    void on_combo_BBoptions_activated(const QString &arg1);
    void on_button_OB_BB_ext_clicked();
    void on_button_BBexternal_path_clicked();


    void on_combo_InterpolationMethod_activated(const QString &arg1);

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
    size_t doseBBroi[4];
    size_t radius;
    size_t min_area;

    float ffirstAngle;
    float flastAngle;

    std::string blackbodyexternalname;
    std::string blackbodysampleexternalname;

    size_t nBBextCount; /// number of preprocessed BB images;
    size_t nBBextFirstIndex; /// first index in filneame for preprocessed BB images

    float tau;
    bool bPBvariante;
    bool bUseNormROI; /// boolean value on the use of the norm roi
    bool bUseNormROIBB; /// boolean value on the use of the norm roi on BBs
    bool bSameMask; /// boolean value on the use of the same mask for all images with BBs
//    bool bUseBB; /// boolean value on the use of BBs, to be set when calling PrepareBBData
//    bool bUseExternalBB; /// boolean value on the use of externally produced BBs
    ImagingAlgorithms::AverageImage::eAverageMethod m_ReferenceAverageMethod;
    ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod m_ReferenceMethod;
    ImagingAlgorithms::ReferenceImageCorrection::eBBOptions m_BBOptions;
    ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX m_xInterpOrder;
    ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY m_yInterpOrder;
    ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod m_InterpMethod;
    int m_nWindow;

    RobustLogNorm module;



    kipl::base::TImage <float,2> m_Preview_OBBB;
    kipl::base::TImage <float,2> m_Preview_sampleBB;



};

#endif // ROBUSTLOGNORMDLG_H
