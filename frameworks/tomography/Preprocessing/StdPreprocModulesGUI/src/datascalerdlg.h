//<LICENSE

#ifndef DATASCALERDLG_H
#define DATASCALERDLG_H
#include "StdPreprocModulesGUI_global.h"
#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>


namespace Ui {
class DataScalerDlg;
}

class DataScalerDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit DataScalerDlg(QWidget *parent = 0);
    ~DataScalerDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & UNUSED_img);

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::DataScalerDlg *ui;

    float m_fSlope;
    float m_fOffset;

};

#endif // DATASCALERDLG_H
