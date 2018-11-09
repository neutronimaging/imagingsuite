#ifndef BASICTHRESHOLDDLG_H
#define BASICTHRESHOLDDLG_H

#include <string>
#include <map>
#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <base/timage.h>

namespace Ui {
class BasicThresholdDlg;
}

class BasicThresholdDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit BasicThresholdDlg(QWidget *parent = nullptr);
    ~BasicThresholdDlg();
    virtual int exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img);
private slots:
    void on_doubleSpinBox_threshold_valueChanged(double arg1);

private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::BasicThresholdDlg *ui;

    kipl::base::TImage<float,3> * pOriginal;
    kipl::base::TImage<float,3> bilevelImg;

    double m_fThreshold;
    size_t m_nHistMax;
};

#endif // BASICTHRESHOLDDLG_H
