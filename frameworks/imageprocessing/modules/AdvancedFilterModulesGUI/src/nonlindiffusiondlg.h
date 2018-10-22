#ifndef NONLINDIFFUSIONDLG_H
#define NONLINDIFFUSIONDLG_H

#include <QDialog>
#include <ConfiguratorDialogBase.h>

#include <base/timage.h>

namespace Ui {
class NonLinDiffusionDlg;
}

class NonLinDiffusionDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit NonLinDiffusionDlg(QWidget *parent = nullptr);
    ~NonLinDiffusionDlg();

    int exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img);



private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void normalizeImage(kipl::base::TImage<float,3> & img, bool forward);
    Ui::NonLinDiffusionDlg *ui;

    bool m_bAutoScale;
    float m_fSlope;
    float m_fIntercept;

    float m_fTau;
    float m_fSigma;
    float m_fLambda;

    int m_nIterations;
    std::string m_sIterationPath;
    bool m_bSaveIterations;

    kipl::base::TImage<float,3> *pOriginal;
    kipl::base::TImage<float,3> filtered;
};

#endif // NONLINDIFFUSIONDLG_H
