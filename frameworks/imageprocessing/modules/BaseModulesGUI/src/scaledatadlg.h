#ifndef SCALEDATADLG_H
#define SCALEDATADLG_H

#include <ConfiguratorDialogBase.h>

namespace Ui {
class ScaleDataDlg;
}

class ScaleDataDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ScaleDataDlg(QWidget *parent = nullptr);
    ~ScaleDataDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void ApplyParameters() ;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    Ui::ScaleDataDlg *ui;

    double m_fSlope;
    double m_fIntercept;

    bool m_bAutoScale;
};

#endif // SCALEDATADLG_H
