//LICENSE

#ifndef VOSTRIPECLEANDLG_H
#define VOSTRIPECLEANDLG_H

#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>

#include <VoStripeCleanModule.h>

namespace Ui {
class VoStripeCleanDlg;
}

class VoStripeCleanDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit VoStripeCleanDlg(QWidget *parent = nullptr);
    ~VoStripeCleanDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private:
    Ui::VoStripeCleanDlg *ui;

    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    ReconConfig *m_Config;
    VoStripeCleanModule m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;

    kipl::base::TImage<float,2> m_OriginalSino;
    kipl::base::TImage<float,2> m_ProcessedSino;
    kipl::base::TImage<float,2> m_DifferenceSino;

    bool   useUnresponsiveStripes;
    bool   useStripeSorting;
    bool   useLargeStripes;

    size_t filterSize_unresponsive;
    size_t filterSize_sorting;
    size_t filterSize_large;
    float  snr_unresponsive;
    float  snr_large;
};

#endif // VOSTRIPECLEANDLG_H
