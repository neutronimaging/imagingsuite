//<LICENSE>
#ifndef CLAMPDATADLG_H
#define CLAMPDATADLG_H

#include <ConfiguratorDialogBase.h>

namespace Ui {
class ClampDataDlg;
}

class ClampDataDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ClampDataDlg(QWidget *parent = nullptr);
    virtual ~ClampDataDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    Ui::ClampDataDlg *ui;

    double m_fMin;
    double m_fMax;
};

#endif // CLAMPDATADLG_H
