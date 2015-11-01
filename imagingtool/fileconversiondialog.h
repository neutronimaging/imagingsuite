#ifndef FILECONVERSIONDIALOG_H
#define FILECONVERSIONDIALOG_H

#include <QDialog>

namespace Ui {
class FileConversionDialog;
}

class FileConversionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileConversionDialog(QWidget *parent = 0);
    ~FileConversionDialog();

private slots:
    void on_pushButton_GetSkipList_clicked();

private:
    Ui::FileConversionDialog *ui;
};

#endif // FILECONVERSIONDIALOG_H
