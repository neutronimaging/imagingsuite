#ifndef CONFIGUREGEOMETRYDIALOG_H
#define CONFIGUREGEOMETRYDIALOG_H

#include <QDialog>
#include <ReconConfig.h>
#include <base/timage.h>
#include <logging/logger.h>

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
private:
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
};

#endif // CONFIGUREGEOMETRYDIALOG_H
