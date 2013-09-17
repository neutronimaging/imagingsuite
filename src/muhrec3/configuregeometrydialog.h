#ifndef CONFIGUREGEOMETRYDIALOG_H
#define CONFIGUREGEOMETRYDIALOG_H

#include <QDialog>
#include <ReconConfig.h>
#include <base/timage.h>
#include <logging/logger.h>
#include <vector>

namespace Ui {
class ConfigureGeometryDialog;
}

class ConfigureGeometryDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ConfigureGeometryDialog(QWidget *parent = 0);
    ~ConfigureGeometryDialog();
    
    void SetConfig(ReconConfig * config);
    void GetConfig(ReconConfig &config);
    virtual int exec(ReconConfig &config);

protected slots:
    void FindCenter();
    void UseTilt(bool x);

private slots:
    void onOKButtonClicked(){ this->setResult(QDialog::Accepted);}
    void onCancelButtonClicked(){ this->setResult(QDialog::Rejected);}
    void onROIButtonClicked();
    void ROIChanged(int x);

private:

    void LSQ_fit1(vector<float> &v, float *k, float *m);
    kipl::base::TImage<float,2> ThresholdProjection(const kipl::base::TImage<float,2> img, float level);
    float CorrelationCenter(	kipl::base::TImage<float,2> proj_0,
                                                        kipl::base::TImage<float,2> proj_180);
    float LeastSquareCenter(	kipl::base::TImage<float,2> proj_0,
                                                        kipl::base::TImage<float,2> proj_180);
    float CenterOfGravity(const kipl::base::TImage<float,2> img, size_t start, size_t end);
    void CumulateProjection(const kipl::base::TImage<float,2> img, const kipl::base::TImage<float,2> biimg);
    pair<size_t, size_t> FindBoundary(const kipl::base::TImage<float,2> img, float level);
    pair<size_t, size_t> FindMaxBoundary();

    void UpdateConfig();
    void UpdateDialog();
    int LoadImages();

    Ui::ConfigureGeometryDialog *ui;

    ReconConfig m_Config;
    kipl::base::TImage<float,2> m_Proj0Deg;
    kipl::base::TImage<float,2> m_Proj180Deg;
    kipl::base::TImage<float,2> m_ProjDC;
    kipl::base::TImage<float,2> m_ProjOB;
    kipl::base::TImage<float,2> m_ProjCumulative;
    std::vector<float> m_vCoG;
    kipl::base::TImage<float,2> m_grayCumulate;
    kipl::base::TImage<float,2> m_biCumulate;
    std::vector<pair<size_t, size_t> > m_vBoundary;
};

#endif // CONFIGUREGEOMETRYDIALOG_H


