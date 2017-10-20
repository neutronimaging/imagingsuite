#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

#include <logging/logger.h>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void UpdateDialog();
    void UpdateConfig();

    virtual int exec();

    int m_MemoryLimit;
    kipl::logging::Logger::LogLevel m_LogLevel;


private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
