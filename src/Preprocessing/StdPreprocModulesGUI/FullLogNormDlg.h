//<LICENSE>

#ifndef FULLLOGNORMDLG_H
#define FULLLOGNORMDLG_H

//#include <QDialog>
#include "StdPreprocModulesGUI_global.h"

#include <averageimage.h>
#include <ConfiguratorDialogBase.h>

namespace Ui {
class FullLogNormDlg;
}

class FullLogNormDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit FullLogNormDlg(QWidget *parent = nullptr);
    virtual ~FullLogNormDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void UpdateDialogFromParameterList(std::map<std::string, std::string> &parameters);

    Ui::FullLogNormDlg *ui;
    bool m_bUseDose;
    bool m_bUseLUT;
    ImagingAlgorithms::AverageImage::eAverageMethod m_eAverageMethod;

};

#endif // FULLLOGNORMDLG_H
