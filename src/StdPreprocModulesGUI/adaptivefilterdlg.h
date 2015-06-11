#ifndef ADAPTIVEFILTERDLG_H
#define ADAPTIVEFILTERDLG_H
#include "StdPreprocModulesGUI_global.h"
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

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_comboCompare_currentIndexChanged(int index);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::AdaptiveFilterDlg *ui;

    kipl::base::TImage<float,3> m_Projections;
    kipl::base::TImage<float,2> m_Sino;
    kipl::base::TImage<float,2> m_ProcessedSino;

    float m_fLambda;
    float m_fSigma;
    float m_nFilterSize;
};

#endif // ADAPTIVEFILTERDLG_H
