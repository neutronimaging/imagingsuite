#ifndef PROJECTIONSERIESCORRECTIONDLG_H
#define PROJECTIONSERIESCORRECTIONDLG_H

#include <QDialog>
#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ReconConfig.h>
#include <imageviewerwidget.h>
#include <plotter.h>

namespace Ui {
class ProjectionSeriesCorrectionDlg;
}

class ProjectionSeriesCorrectionDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ProjectionSeriesCorrectionDlg(QWidget *parent = nullptr);
    ~ProjectionSeriesCorrectionDlg();

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
    void SetDoseList(std::vector<float> &dose);
private slots:
    void on_doubleSpinBox_threshold_valueChanged(double arg1);

    void on_doubleSpinBox_threshold_editingFinished();

private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
    void UpdateParameterList(std::map<std::string, std::string> &parameters);
    void LoadDoseList(ReconConfig &config);

    Ui::ProjectionSeriesCorrectionDlg *ui;
    ReconConfig *m_Config;

    float threshold;
    std::vector<float> doselist;
};

#endif // PROJECTIONSERIESCORRECTIONDLG_H
