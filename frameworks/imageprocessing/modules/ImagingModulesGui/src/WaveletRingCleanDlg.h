//<LICENSE>

#ifndef WAVELETRINGCLEANDLG_H
#define WAVELETRINGCLEANDLG_H
//#include "StdPreprocModulesGUI_global.h"
#include "imagingmodulesgui_global.h"

#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfigBase.h>
#include <ConfiguratorDialogBase.h>

//#include <ReconConfig.h>
#include <KiplProcessConfig.h>

#include <imageviewerwidget.h>
#include <plotter.h>

#include <stripefiltermodule.h>

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

    KiplProcessConfig *m_Config;

    StripeFilterModule m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;

    kipl::base::TImage<float,3> m_OriginalImg;
    kipl::base::TImage<float,3> m_ProcessedImg;
    kipl::base::TImage<float,3> m_DifferenceSino;

    int         m_nLevels;
    float       m_fSigma;
    std::string m_sWaveletName;
    bool        m_bParallel;
    ImagingAlgorithms::eStripeFilterOperation op;
    kipl::base::eImagePlanes plane;
};

#endif // WAVELETRINGCLEANDLG_H
