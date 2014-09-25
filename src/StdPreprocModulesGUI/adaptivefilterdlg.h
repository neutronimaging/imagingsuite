#ifndef ADAPTIVEFILTERDLG_H
#define ADAPTIVEFILTERDLG_H

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <QDialog>

namespace Ui {
class AdaptiveFilterDlg;
}

class AdaptiveFilterDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit AdaptiveFilterDlg(QWidget *parent = 0);
    ~AdaptiveFilterDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::AdaptiveFilterDlg *ui;

    float m_fLambda;
    float m_fSigma;
    float m_nFilterSize;
};

#endif // ADAPTIVEFILTERDLG_H
