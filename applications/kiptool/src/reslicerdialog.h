#ifndef RESLICERDIALOG_H
#define RESLICERDIALOG_H

#include <QDialog>

#include <logging/logger.h>

#include "Reslicer.h"

namespace Ui {
class ReslicerDialog;
}

class ReslicerDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ReslicerDialog(QWidget *parent = nullptr);
    ~ReslicerDialog();

private slots:

    void on_pushButton_browsout_clicked();

    void on_pushButton_startreslice_clicked();

    void on_pushButton_preview_clicked();

    void on_pushButton_getROI_clicked();

    void on_spinBox_firstXZ_valueChanged(int arg1);

    void on_spinBox_lastXZ_valueChanged(int arg1);

    void on_spinBox_firstYZ_valueChanged(int arg1);

    void on_spinBox_lastYZ_valueChanged(int arg1);

private:
    void UpdateDialog();
    void UpdateConfig();
    Ui::ReslicerDialog *ui;

   TIFFReslicer m_reslicer;
   QRect m_currentROI;
};

#endif // RESLICERDIALOG_H
