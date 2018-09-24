#ifndef ISSFILTERDLG_H
#define ISSFILTERDLG_H

#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <scalespace/ISSfilterQ3D.h>


namespace Ui {
class ISSFilterDlg;
}

class ISSFilterDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ISSFilterDlg(QWidget *parent = nullptr);
    virtual ~ISSFilterDlg();

private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;

    Ui::ISSFilterDlg *ui;

    bool m_bAutoScale;
    double m_fSlope;
    double m_fIntercept;

    double m_fTau;
    double m_fLambda;
    double m_fAlpha;
    int m_nIterations;
//    std::string m_sIterationPath;
//    bool m_bSaveIterations;
    akipl::scalespace::eRegularizationType m_eRegularization;
    akipl::scalespace::eInitialImageType m_eInitialImage;
};

#endif // ISSFILTERDLG_H
