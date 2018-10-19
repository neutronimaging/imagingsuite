//<LICENSE>
#ifndef DOUBLETHRESHOLDDLG_H
#define DOUBLETHRESHOLDDLG_H

#include <string>
#include <map>
#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <base/timage.h>

namespace Ui {
class DoubleThresholdDlg;
}

class DoubleThresholdDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DoubleThresholdDlg(QWidget *parent = nullptr);
    ~DoubleThresholdDlg();

    virtual int exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img);
private slots:
    void on_checkBox_background_toggled(bool checked);

private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;

    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    Ui::DoubleThresholdDlg *ui;

    kipl::base::TImage<float,3> * pOriginal;
    kipl::base::TImage<float,3> bilevelImg;

    double m_fHighThreshold;
    double m_fLowThreshold ;
    double m_fBackgroundValue;
    bool m_bUseBackgroundValue;

    size_t m_nHistMax;
};

#endif // DOUBLETHRESHOLDDLG_H
