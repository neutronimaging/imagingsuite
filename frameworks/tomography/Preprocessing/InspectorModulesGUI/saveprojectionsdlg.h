//<LICENSE>

#ifndef SAVEPROJECTIONSDLG_H
#define SAVEPROJECTIONSDLG_H

#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <QDialog>

namespace Ui {
class SaveProjectionsDlg;
}

class SaveProjectionsDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit SaveProjectionsDlg(QWidget *parent = 0);
    ~SaveProjectionsDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & UNUSED_img);

protected:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);

    std::string m_sPath;
    std::string m_sFileMask;
    ReconConfig::cProjections::eImageType m_eImageType;
    kipl::io::eFileType m_eFileType;

private slots:
    void on_buttonBrowse_clicked();

private:
    Ui::SaveProjectionsDlg *ui;
};

#endif // SAVEPROJECTIONSDLG_H
