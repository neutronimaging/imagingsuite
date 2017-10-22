//<LICENSE>

#ifndef MEDIANMIXRINGCLEAN_H
#define MEDIANMIXRINGCLEAN_H
#include "StdPreprocModulesGUI_global.h"

//#include <QDialog>
#include <ConfiguratorDialogBase.h>
#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfigBase.h>
#include <imageviewerwidget.h>
#include <plotter.h>

#include <MedianMixRingClean.h>

namespace Ui {
class MedianMixRingClean;
}

class MedianMixRingCleanDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit MedianMixRingCleanDlg(QWidget *parent = 0);
    ~MedianMixRingCleanDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private slots:
    void on_entry_lambda_valueChanged(double arg1);

    void on_entry_sigma_valueChanged(double arg1);

    void on_button_apply_clicked();

private:
    Ui::MedianMixRingClean *ui;

    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void DrawCursors(float lambda, float sigma);

    ReconConfig *m_Config;

    MedianMixRingClean m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;

    kipl::base::TImage<float,2> m_OriginalImage;
    kipl::base::TImage<float,2> m_ProcessedImage;
    kipl::base::TImage<float,2> m_DifferenceImage;

    float m_fSigma;
    float m_fLambda;
};

#endif // MEDIANMIXRINGCLEAN_H
