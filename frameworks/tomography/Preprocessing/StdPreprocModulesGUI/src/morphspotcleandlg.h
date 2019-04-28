//<LICENSE>

#ifndef MORPHSPOTCLEANDLG_H
#define MORPHSPOTCLEANDLG_H
#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfigBase.h>
#include <ConfiguratorDialogBase.h>

#include <imageviewerwidget.h>
#include <plotter.h>

#include <MorphSpotCleanModule.h>
#include <MorphSpotClean.h>


namespace Ui {
class MorphSpotCleanDlg;
}

class MorphSpotCleanDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit MorphSpotCleanDlg(QWidget *parent = nullptr);
    virtual ~MorphSpotCleanDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private slots:

    void on_buttonApply_clicked();

    void on_comboDetectionDisplay_currentIndexChanged(int index);

    void on_comboDetectionMethod_currentIndexChanged(int index);

private:
    Ui::MorphSpotCleanDlg *ui;

private:
    virtual void ApplyParameters();
    void prepareDetectionPlot(kipl::base::TImage<float,2> &img,int det,size_t N,std::string curvelabel, std::string threslabel);
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);


    ReconConfig *m_Config;
    MorphSpotCleanModule m_Cleaner;
    kipl::base::TImage<float,3> m_Projections;
    kipl::morphology::MorphConnect m_eConnectivity;
    ImagingAlgorithms::eMorphDetectionMethod m_eDetectionMethod;
    ImagingAlgorithms::eMorphCleanMethod m_eCleanMethod;

    kipl::base::TImage<float,2> m_OriginalImage;
    kipl::base::TImage<float,2> m_ProcessedImage;
    kipl::base::TImage<float,2> m_DetectionImageHoles;
    kipl::base::TImage<float,2> m_DetectionImagePeaks;

    float m_fThreshold[2];
    float m_fSigma[2];
    int   m_nEdgeSmoothLength;
    int   m_nMaxArea;
    bool  m_bClampData;
    float m_fMinLevel;
    float m_fMaxLevel;
    float m_bThreading;
};

#endif // MORPHSPOTCLEANDLG_H
