#ifndef EDGEFILEITEMDIALOG_H
#define EDGEFILEITEMDIALOG_H

#include <QDialog>

namespace Ui {
class EdgeFileItemDialog;
}

class EdgeFileItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EdgeFileItemDialog(QWidget *parent = 0);
    ~EdgeFileItemDialog();

    void setInfo(float distance, QString fname);
    void getInfo(float &distance, QString &fname);

private slots:
    void on_buttonBrowse_clicked();

private:
    Ui::EdgeFileItemDialog *ui;
};

#endif // EDGEFILEITEMDIALOG_H
