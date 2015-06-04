#ifndef POLYNOMIALCORRECTIONDLG_H
#define POLYNOMIALCORRECTIONDLG_H

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <QDialog>
#include <PolynomialCorrectionModule.h>

namespace Ui {
class PolynomialCorrectionDlg;
}

class PolynomialCorrectionDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit PolynomialCorrectionDlg(QWidget *parent = 0);
    ~PolynomialCorrectionDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private slots:
    void on_spinOrder_valueChanged(int arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::PolynomialCorrectionDlg *ui;

    ReconConfig *m_Config;
    PolynomialCorrection corrector;
    kipl::base::TImage<float,3> m_Projections;

    size_t m_nPolyOrder;
    float m_fCoefs[10];
};

#endif // POLYNOMIALCORRECTIONDLG_H
