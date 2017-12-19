//<LICENSE>

#ifndef SPOTCLEAN2DLG_H
#define SPOTCLEAN2DLG_H
#include "StdPreprocModulesGUI_global.h"

#include <QDialog>

#include <base/timage.h>
#include <morphology/morphology.h>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>

#include <imageviewerwidget.h>
#include <plotter.h>

#include <SpotClean2.h>

namespace Ui {
class SpotClean2Dlg;
}

class SpotClean2Dlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit SpotClean2Dlg(QWidget *parent = 0);
    ~SpotClean2Dlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private:
    Ui::SpotClean2Dlg *ui;

protected:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    ReconConfig *m_Config;

    SpotClean2 m_Cleaner;

    kipl::base::TImage<float,3> m_Projections;

    kipl::base::TImage<float,2> m_OriginalImage;
    kipl::base::TImage<float,2> m_ProcessedImage;
    kipl::base::TImage<float,2> m_DetectionImage;

    ImagingAlgorithms::DetectionMethod m_eDetectionMethod;

    float m_fGamma;
    float m_fSigma;
    int   m_nIterations;
    float m_fMaxLevel;
    float m_fMinLevel;
    size_t m_nMaxArea;
private slots:
    void on_combo_detection_display_currentIndexChanged(int index);
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_button_apply_clicked();
};

#endif // SPOTCLEAN2DLG_H
