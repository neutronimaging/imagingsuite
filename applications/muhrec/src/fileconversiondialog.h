//<LICENSE>

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
    explicit FileConversionDialog(QWidget *parent = nullptr);
    ~FileConversionDialog();

private slots:
    void on_pushButton_GetSkipList_clicked();

    void on_pushButton_BrowseDestination_clicked();

    void on_pushButton_StartConversion_clicked();

    void on_buttonBox_rejected();

    void on_spinCombineSize_valueChanged(int arg1);

    void on_spinCombineSize_editingFinished();

    void on_comboBox_ScanOrder_currentIndexChanged(int index);

    void on_ImageLoaderConfig_readerListModified();

private:
    int CopyImages();
    int ConvertImages();
    int Progress();
    int Process(bool sameext);
    Ui::FileConversionDialog *ui;

    std::list<std::string> flist;
    int filecnt;
};

#endif // FILECONVERSIONDIALOG_H
