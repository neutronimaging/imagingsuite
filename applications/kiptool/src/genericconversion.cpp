//<LICENSE>

#include <string>

#include <QString>
#include <QMessageBox>
#include <QFileDialog>

#include <base/timage.h>
#include <base/trotate.h>
#include <base/kiplenums.h>
#include <io/io_generic.h>
#include <io/io_tiff.h>
#include <io/DirAnalyzer.h>

#include "genericconversion.h"
#include "ui_genericconversion.h"

GenericConversion::GenericConversion(QWidget *parent) :
    QDialog(parent),
    logger("GenericConversion"),
    ui(new Ui::GenericConversion),
    m_fname("/data/img_####.bin"),
    m_destPath("/data/"),
    m_destMask("converted_####.tif"),
    m_sizeX(100),
    m_sizeY(100),
    m_stride(200),
    m_customStride(false),
    m_rotate(false),
    m_customLevels(false),
    m_type(kipl::base::UInt16),
    m_levelLo(0),
    m_levelHi(1000)
{
    ui->setupUi(this);
    UpdateDialog();
    on_checkBox_DispLevels_toggled(m_customLevels);
    on_comboBox_DataType_currentIndexChanged(3);
}

GenericConversion::~GenericConversion()
{
    delete ui;
}

void GenericConversion::UpdateDialog()
{
    ui->lineEdit_FileMask->setText(QString::fromStdString(m_fname));
    ui->lineEdit_DestPath->setText(QString::fromStdString(m_destPath));
    ui->lineEdit_DestMask->setText(QString::fromStdString(m_destMask));

    ui->spinBox_SizeX->setValue(m_sizeX);
    ui->spinBox_SizeY->setValue(m_sizeY);
    ui->spinBox_Stride->setValue(m_stride);
    ui->checkBox_Stride->setChecked(m_customStride);
    ui->checkBox_Transpose->setChecked(m_rotate);
    ui->checkBox_DispLevels->setChecked(m_customLevels);
    ui->comboBox_DataType->setCurrentIndex(m_type);
    ui->spinBox_DispLo->setValue(m_levelLo);
    ui->spinBox_DispHi->setValue(m_levelHi);
}

void GenericConversion::UpdateConfig()
{
    m_fname        = ui->lineEdit_FileMask->text().toStdString();
    m_destPath     = ui->lineEdit_DestPath->text().toStdString();
    m_destMask     = ui->lineEdit_DestMask->text().toStdString();
    m_sizeX        = ui->spinBox_SizeX->value();
    m_sizeY        = ui->spinBox_SizeY->value();
    m_offset       = ui->spinBox_Offset->value();
    m_stride       = ui->spinBox_Stride->value();
    m_customStride = ui->checkBox_Stride->isChecked();
    m_rotate       = ui->checkBox_Transpose->isChecked();
    m_customLevels = ui->checkBox_DispLevels->isChecked();
    m_type         = ui->comboBox_DataType->currentIndex();
    m_levelLo      = ui->spinBox_DispLo->value();
    m_levelHi      = ui->spinBox_DispHi->value();
    m_firstImg     = ui->spinBox_FirsFile->value();
    m_lastImg      = ui->spinBox_LastFile->value();
}

void GenericConversion::on_pushButton_BrowseFileMask_clicked()
{
    QString projdir=QFileDialog::getOpenFileName(this,
                                      "Select a file",
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

void GenericConversion::on_pushButton_Preview_clicked()
{
    UpdateConfig();
    std::list<kipl::base::TImage<float,2> > imgs;

    if (!m_customStride) {
        m_stride=static_cast<int>(m_sizeX*(m_type+1)/2);
        ui->spinBox_Stride->setValue(m_stride);
    }
//    template <typename ImgType>
//    int ReadGeneric(std::list<kipl::base::TImage<ImgType,2> > &imglist,
//                    char const * const fname,
//                    size_t size_x,
//                    size_t size_y,
//                    size_t offset,
//                    size_t stride,
//                    size_t imagesperfile,
//                    kipl::base::eDataType dt,
//                    kipl::base::eEndians endian,
//                    size_t const * const nCrop=NULL);


    try {
        std::string fname, ext;
        kipl::strings::filenames::MakeFileName(m_fname,m_firstImg,fname,ext,'#','0',false);
        kipl::io::ReadGeneric(imgs ,fname.c_str(),
                         m_sizeX,m_sizeY,
                         m_offset,m_stride,
                         1,
                         static_cast<kipl::base::eDataType>(m_type),
                         kipl::base::BigEndian,
                         nullptr);
    }
    catch (kipl::base::KiplException &e) {
        std::ostringstream errmsg;
        errmsg<<"Reading image failed."<<std::endl<<e.what();
        logger(logger.LogError,errmsg.str());
        QMessageBox msg;
        msg.setDetailedText(QString::fromStdString(errmsg.str()));
        msg.setText("Failed to read image");
        msg.exec();
        return;
    }

    kipl::base::TImage<float,2> img=imgs.front();

    std::cout<<img<<std::endl;
    kipl::base::TRotate<float> rot;
    if (m_rotate) {
        img=rot.Rotate90(imgs.front());
    }
    else {
        img=imgs.front();
    }

    if (m_customLevels)
        ui->viewer_preview->set_image(img.GetDataPtr(),img.Dims(),m_levelLo,m_levelHi);
    else
        ui->viewer_preview->set_image(img.GetDataPtr(),img.Dims());

    std::ostringstream text;

    text<<(m_stride*m_sizeY)+m_offset<<" bytes";
    ui->label_FileSize->setText(QString::fromStdString(text.str()));
}

void GenericConversion::on_pushButton_BrowseDest_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select destination directory",
                                      ui->lineEdit_DestPath->text());
    if (!projdir.isEmpty()) {
        std::string pdir=projdir.toStdString();
        kipl::strings::filenames::CheckPathSlashes(pdir,true);

        ui->lineEdit_DestPath->setText(QString::fromStdString(pdir));
    }
}

void GenericConversion::on_pushButton_ConvertAll_clicked()
{

}

void GenericConversion::on_checkBox_DispLevels_toggled(bool checked)
{
    ui->label_DispLo->setVisible(checked);
    ui->label_DispHi->setVisible(checked);
    ui->spinBox_DispLo->setVisible(checked);
    ui->spinBox_DispHi->setVisible(checked);

}

void GenericConversion::on_comboBox_DataType_currentIndexChanged(int index)
{
    m_type=index;
    m_sizeX=ui->spinBox_SizeX->value();
    m_stride=static_cast<int>(m_sizeX*(m_type+1)/2);
    ui->spinBox_Stride->setValue(m_stride);
}
