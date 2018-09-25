#ifndef DOSECORRECTIONDLG_H
#define DOSECORRECTIONDLG_H

#include <ConfiguratorDialogBase.h>

namespace Ui {
class DoseCorrectionDlg;
}

class DoseCorrectionDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DoseCorrectionDlg(QWidget *parent = nullptr);
    ~DoseCorrectionDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void updateImage(kipl::base::TImage<float,3> & img);

    Ui::DoseCorrectionDlg *ui;

    double m_fSlope;
    double m_fIntercept;

    size_t m_nROI[4];
};

#endif // DOSECORRECTIONDLG_H
