#ifndef SAVEASDIALOG_H
#define SAVEASDIALOG_H

#include <string>

#include <QDialog>

namespace Ui {
class SaveAsDialog;
}

class SaveAsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveAsDialog(QWidget *parent = 0);
    ~SaveAsDialog();
    void setLimits(int first, int last);
    void setPath(std::string path, std::string mask);
    void getDialogInfo(std::string &filemask, int &first, int &last);

private slots:
    void on_checkBox_interval_toggled(bool checked);

    void on_pushButton_browse_clicked();

private:
    Ui::SaveAsDialog *ui;
};

#endif // SAVEASDIALOG_H
