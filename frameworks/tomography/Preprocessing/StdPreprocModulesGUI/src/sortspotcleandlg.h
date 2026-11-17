//<LICENSE> 
#ifndef SORTSPOTCLEANDLG_H
#define SORTSPOTCLEANDLG_H


#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfigBase.h>
#include <ConfiguratorDialogBase.h>

#include <imageviewerwidget.h>
#include <plotter.h>

#include <SortSpotCleanModule.h>
#include <sortspotclean.h>

#include <map>


namespace Ui {
class SortSpotCleanDlg;
}

class SortSpotCleanDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit SortSpotCleanDlg(QWidget *parent = nullptr);
    ~SortSpotCleanDlg();
    int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img) override;
private slots:
    void on_comboBox_Difference_currentIndexChanged(int index);

    void on_comboBox_ThresholdType_currentIndexChanged(int index);

    void on_pushButton_Apply_clicked();

    void on_spinBox_imgidx_valueChanged(int arg1);

private:
    Ui::SortSpotCleanDlg *ui;
    void ApplyParameters()  override;
    
    void UpdateDialog()     override;
    void UpdateParameters() override;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    ReconConfig         *config;
    SortSpotCleanModule  m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;
    kipl::base::TImage<float,2> m_OriginalImage;
    kipl::base::TImage<float,2> m_CleanedImage;

    kipl::base::eConnectivity                   m_dilation;
    ImagingAlgorithms::eSortSpotQuantile        m_quantileMethod;
    ImagingAlgorithms::eSortSpotThresholdMethod m_thresholdMethod;
    kipl::base::eConnectivity                   m_connectivity;
    float  m_fQuantile;
    float  m_fThreshold;
    size_t m_nPatchSize;
    bool   m_bRemoveInfNaN;
    bool   m_bClampData;
    float  m_fMinLevel;
    float  m_fMaxLevel;
};

#endif // SORTSPOTCLEANDLG_H
