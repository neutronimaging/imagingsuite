#ifndef FILECONVERSIONDIALOG_H
#define FILECONVERSIONDIALOG_H

#include <QDialog>

#include <logging/logger.h>

namespace Ui {
class FileConversionDialog;
}

class FileConversionDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit FileConversionDialog(QWidget *parent = 0);
    ~FileConversionDialog();

private slots:
    void on_pushButton_GetSkipList_clicked();

    void on_pushButton_BrowseDestination_clicked();

    void on_pushButton_StartConversion_clicked();

private:
    void CopyImages(std::list<std::string> &flist);
    void ConvertImages(std::list<std::string> &flist);
    Ui::FileConversionDialog *ui;
};

#endif // FILECONVERSIONDIALOG_H
