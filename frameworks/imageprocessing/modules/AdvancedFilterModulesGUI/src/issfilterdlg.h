#ifndef ISSFILTERDLG_H
#define ISSFILTERDLG_H

#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <base/timage.h>
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

    int exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img);

private slots:
    void on_pushButton_browseIterationPath_clicked();

    void on_spinBox_slice_valueChanged(int arg1);

    void on_horizontalSlider_slice_sliderMoved(int position);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::ISSFilterDlg *ui;

    bool m_bAutoScale;
    double m_fSlope;
    double m_fIntercept;

    double m_fTau;
    double m_fLambda;
    double m_fAlpha;
    int m_nIterations;
    std::string m_sIterationPath;
    bool m_bSaveIterations;
    akipl::scalespace::eRegularizationType m_eRegularization;
    akipl::scalespace::eInitialImageType m_eInitialImage;

    kipl::base::TImage<float,3> *pOriginal;
    kipl::base::TImage<float,3> filtered;
};

#endif // ISSFILTERDLG_H
