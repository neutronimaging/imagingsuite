#ifndef ADDLOADERDIALOG_H
#define ADDLOADERDIALOG_H

#include <QDialog>

#include <datasetbase.h>

namespace Ui {
class AddLoaderDialog;
}

class AddLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLoaderDialog(QWidget *parent = 0);
    ~AddLoaderDialog();
    ImageLoader m_loader;
    virtual int exec();
private slots:
    void on_pushButton_Browse_clicked();

private:
    void UpdateDialog();
    void UpdateConfig();

    Ui::AddLoaderDialog *ui;
};

#endif // ADDLOADERDIALOG_H
