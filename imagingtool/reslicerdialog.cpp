#include <QDir>
#include <QFileDialog>
#include <QString>

#include <io/DirAnalyzer.h>

#include "reslicerdialog.h"
#include "ui_reslicerdialog.h"



ReslicerDialog::ReslicerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReslicerDialog)
{
    ui->setupUi(this);
    UpdateDialog();
}

ReslicerDialog::~ReslicerDialog()
{
    delete ui;
}

void ReslicerDialog::on_pushButton_browsein_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file from data set",
                                      ui->lineEdit_infilemask->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();

        #ifdef _MSC_VER
        const char slash='\\';
        #else
        const char slash='/';
        #endif
        ptrdiff_t pos=pdir.find_last_of(slash);

        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->lineEdit_infilemask->setText(QString::fromStdString(fi.m_sMask));
    }
}

void ReslicerDialog::on_pushButton_browsout_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination directory",
                                      ui->lineEdit_outfilemask->text());
    if (!projdir.isEmpty()) {
        ui->lineEdit_outfilemask->setText(projdir);
    }
}

void ReslicerDialog::on_pushButton_startreslice_clicked()
{
    m_reslicer.process();
}

void ReslicerDialog::UpdateDialog()
{
    ui->lineEdit_infilemask->setText(QString::fromStdString(m_reslicer.m_sSourceMask));
    ui->lineEdit_outfilemask->setText(QString::fromStdString(m_reslicer.m_sDestinationPath));

    ui->spinBox_firstslice->setValue(m_reslicer.m_nFirst);
    ui->spinBox_lastslice->setValue(m_reslicer.m_nLast);
    ui->spinBox_firstXZ->setValue(m_reslicer.m_nFirstXZ);
    ui->spinBox_lastXZ->setValue(m_reslicer.m_nLastXZ);
    ui->spinBox_firstYZ->setValue(m_reslicer.m_nFirstYZ);
    ui->spinBox_lastYZ->setValue(m_reslicer.m_nLastYZ);

    ui->checkBox_XZ->setChecked(m_reslicer.m_bResliceXZ);
    ui->checkBox_YZ->setChecked(m_reslicer.m_bResliceYZ);
}

void ReslicerDialog::UpdateConfig()
{
    m_reslicer.m_sSourceMask      = ui->lineEdit_infilemask->text().toStdString();
    m_reslicer.m_sDestinationPath = ui->lineEdit_outfilemask->text().toStdString();

    m_reslicer.m_nFirst           = ui->spinBox_firstslice->value();
    m_reslicer.m_nLast            = ui->spinBox_lastslice->value();
    m_reslicer.m_nFirstXZ         = ui->spinBox_firstXZ->value();
    m_reslicer.m_nLastXZ          = ui->spinBox_lastXZ->value();
    m_reslicer.m_nFirstYZ         = ui->spinBox_firstYZ->value();
    m_reslicer.m_nLastYZ          = ui->spinBox_lastYZ->value();

    m_reslicer.m_bResliceXZ       = ui->checkBox_XZ->isChecked();
    m_reslicer.m_bResliceYZ       = ui->checkBox_YZ->isChecked();
}
