#include <QFileDialog>
#include <QString>

#include <io/DirAnalyzer.h>

#include "addloaderdialog.h"
#include "ui_addloaderdialog.h"

AddLoaderDialog::AddLoaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLoaderDialog)
{
    ui->setupUi(this);
}

AddLoaderDialog::~AddLoaderDialog()
{
    delete ui;
}

void AddLoaderDialog::on_pushButton_Browse_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file from the data set",
                                      ui->lineEdit_FileMask->text());
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

        ui->lineEdit_FileMask->setText(QString::fromStdString(fi.m_sMask));

    }
}


int AddLoaderDialog::exec()
{
    int res=0;

    UpdateDialog();

    res=QDialog::exec();

    if (res==QDialog::Accepted)
        UpdateConfig();

    return res;
}

void AddLoaderDialog::UpdateDialog()
{
    ui->lineEdit_FileMask->setText(QString::fromStdString(m_loader.m_filemask));
    ui->spinBox_First->setValue(m_loader.m_nFirst);
    ui->spinBox_Last->setValue(m_loader.m_nLast);
    ui->spinBox_Step->setValue(m_loader.m_nStep);
}

void AddLoaderDialog::UpdateConfig()
{
    m_loader.m_filemask = ui->lineEdit_FileMask->text().toStdString();
    m_loader.m_nFirst   = ui->spinBox_First->value();
    m_loader.m_nLast    = ui->spinBox_Last->value();
    m_loader.m_nStep    = ui->spinBox_Step->value();
}
