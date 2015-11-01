#ifndef RESLICERDIALOG_H
#define RESLICERDIALOG_H

#include <QDialog>

#include "Reslicer.h"

namespace Ui {
class ReslicerDialog;
}

class ReslicerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReslicerDialog(QWidget *parent = 0);
    ~ReslicerDialog();

private slots:
    void on_pushButton_browsein_clicked();

    void on_pushButton_browsout_clicked();

    void on_pushButton_startreslice_clicked();

private:
    void UpdateDialog();
    void UpdateConfig();
    Ui::ReslicerDialog *ui;

   TIFFReslicer m_reslicer;
};

#endif // RESLICERDIALOG_H
