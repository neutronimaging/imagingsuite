//<LICENSE>

#ifndef DOUBLETHRESHOLDDLG_H
#define DOUBLETHRESHOLDDLG_H

#include <string>
#include <map>

#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <base/timage.h>
#include <segmentation/thresholds.h>

namespace Ui {
class DoubleThresholdDlg;
}

class DoubleThresholdDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DoubleThresholdDlg(QWidget *parent = nullptr);
    virtual ~DoubleThresholdDlg();

    virtual int exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img);
private slots:
    void on_doubleSpinBox_upper_valueChanged(double arg1);

    void on_doubleSpinBox_lower_valueChanged(double arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_apply_clicked();

private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;

    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    Ui::DoubleThresholdDlg *ui;

    kipl::base::TImage<float,3> * pOriginal;
    kipl::base::TImage<float,3> bilevelImg;

    float m_fHighThreshold;
    float m_fLowThreshold ;
    kipl::segmentation::CmpType m_compare;

    size_t m_nHistMax;
};

#endif // DOUBLETHRESHOLDDLG_H
