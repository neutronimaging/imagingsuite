#ifndef GLOBALSETTINGSDIALOG_H
#define GLOBALSETTINGSDIALOG_H

#include <QDialog>

#include <ReconConfig.h>

namespace Ui {
class GlobalSettingsDialog;
}

class GlobalSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalSettingsDialog(QWidget *parent = nullptr);
    ~GlobalSettingsDialog();
    void setConfig(ReconConfig &config);
    void updateConfig(ReconConfig &config);
private:
    Ui::GlobalSettingsDialog *ui;

};

#endif // GLOBALSETTINGSDIALOG_H
