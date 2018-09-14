
#ifndef BBLOGNORMDLG_H
#define BBLOGNORMDLG_H

#include "imagingmodulesgui_global.h"

#include <QDialog>

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <KiplProcessConfig.h>
#include <imageviewerwidget.h>
#include <plotter.h>

#include <bblognorm.h>


namespace Ui {
class bblognormDlg;
}

class bblognormDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit bblognormDlg(QWidget *parent = 0);
    ~bblognormDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

private:
    Ui::bblognormDlg *ui;

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    virtual void UpdateParameterList(std::map<std::string, std::string> &parameters);
};

#endif // BBLOGNORMDLG_H
