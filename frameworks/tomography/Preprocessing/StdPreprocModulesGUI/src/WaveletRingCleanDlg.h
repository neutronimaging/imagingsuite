//<LICENSE>

#ifndef WAVELETRINGCLEANDLG_H
#define WAVELETRINGCLEANDLG_H
#include "StdPreprocModulesGUI_global.h"

#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfigBase.h>
#include <ConfiguratorDialogBase.h>

#include <ReconConfig.h>

#include <imageviewerwidget.h>
#include <plotter.h>

#include <WaveletRingClean.h>

namespace Ui {
class WaveletRingCleanDlg;
}

class WaveletRingCleanDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit WaveletRingCleanDlg(QWidget *parent = 0);
    ~WaveletRingCleanDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private slots:
    void on_button_apply_clicked();

private:
    Ui::WaveletRingCleanDlg *ui;

    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    void PrepareWaveletComboBox();

    ReconConfig *m_Config;

    WaveletRingClean m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;

    kipl::base::TImage<float,2> m_OriginalSino;
    kipl::base::TImage<float,2> m_ProcessedSino;
    kipl::base::TImage<float,2> m_DifferenceSino;

    int         m_nLevels;
    float       m_fSigma;
    std::string m_sWaveletName;
    bool        m_bParallel;
    ImagingAlgorithms::StripeFilterOperation m_eCleaningMethod;
};

#endif // WAVELETRINGCLEANDLG_H
