//<LICENSE>

#ifndef GENERALFILTERDLG_H
#define GENERALFILTERDLG_H

#include <QDialog>

#include <ConfiguratorDialogBase.h>

#include <GeneralFilter.h>

namespace Ui {
class GeneralFilterDlg;
}

class GeneralFilterDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit GeneralFilterDlg(QWidget *parent = 0);
    ~GeneralFilterDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img) override;

private:
    using ConfiguratorDialogBase::ApplyParameters;
    using ConfiguratorDialogBase::UpdateDialog;
    using ConfiguratorDialogBase::UpdateParameters;
    
    void ApplyParameters() override;
    void UpdateDialog() override;
    void UpdateParameters() override;
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    Ui::GeneralFilterDlg *ui;
    GeneralFilter::eGeneralFilter filterType;
    float filterSize;
};

#endif // GENERALFILTERDLG_H
