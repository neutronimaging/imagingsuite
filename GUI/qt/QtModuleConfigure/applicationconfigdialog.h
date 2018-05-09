#ifndef APPLICATIONCONFIGDIALOG_H
#define APPLICATIONCONFIGDIALOG_H

#include <QDialog>

#include <applicationconfig.h>

namespace Ui {
class ApplicationConfigDialog;
}

class ApplicationConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplicationConfigDialog(QWidget *parent = 0);
    void setApplicationConfig(ApplicationConfig * cfg);

    virtual int exec();

    ~ApplicationConfigDialog();

private slots:
    void on_pushButton_AppPath_clicked();

    void on_pushButton_DataPath_clicked();

private:
    Ui::ApplicationConfigDialog *ui;
    ApplicationConfig *config;
};

#endif // APPLICATIONCONFIGDIALOG_H
