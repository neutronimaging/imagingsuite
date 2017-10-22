//<LICENSE>

#include "saveprojectionsdlg.h"
#include "ui_saveprojectionsdlg.h"
#include "QFileDialog"
#include <strings/miscstring.h>
#include <ParameterHandling.h>

SaveProjectionsDlg::SaveProjectionsDlg(QWidget *parent) :
    ConfiguratorDialogBase("SaveProjections",true,false,false,parent),
    m_sPath(QDir::homePath().toStdString()),
    m_sFileMask("/proj_####.tif"),
    m_eImageType(ReconConfig::cProjections::ImageType_Projections),
    m_eFileType(kipl::io::TIFFfloat),
    ui(new Ui::SaveProjectionsDlg)
{
    ui->setupUi(this);
}

SaveProjectionsDlg::~SaveProjectionsDlg()
{
    delete ui;
}

void SaveProjectionsDlg::on_buttonBrowse_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                                      "Select destination for the images",
                                                      ui->editPath->text());

    if (!projdir.isEmpty()) {
        ui->editPath->setText(projdir);
    }
}

int SaveProjectionsDlg::exec(ConfigBase * UNUSED(config), std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> & UNUSED(img))
{
    m_sPath = GetStringParameter(parameters,"path");
    m_sFileMask  = GetStringParameter(parameters,"filemask");
    string2enum(GetStringParameter(parameters,"imagetype"),m_eImageType);
    string2enum(GetStringParameter(parameters,"filetype"),m_eFileType);

    UpdateDialog();

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
        return true;
    }
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }

    return false;
}

void SaveProjectionsDlg::ApplyParameters()
{}

void SaveProjectionsDlg::UpdateDialog()
{
    ui->editPath->setText(QString::fromStdString(m_sPath));
    ui->editFileMask->setText(QString::fromStdString(m_sFileMask));
    ui->comboImageType->setCurrentIndex(static_cast<int>(m_eImageType));
    ui->comboFileType->setCurrentIndex(static_cast<int>(m_eFileType)-2);
}

void SaveProjectionsDlg::UpdateParameters()
{
    m_sPath = ui->editPath->text().toStdString();
    kipl::strings::filenames::CheckPathSlashes(m_sPath,true);
    m_sFileMask  = ui->editFileMask->text().toStdString();
    m_eImageType = static_cast<ReconConfig::cProjections::eImageType>(ui->comboImageType->currentIndex());
    m_eFileType = static_cast<kipl::io::eFileType>(ui->comboFileType->currentIndex()+2);
}

void SaveProjectionsDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["path"]=m_sPath;
    parameters["filemask"]=m_sFileMask;
    parameters["imagetype"]=enum2string(m_eImageType);
    parameters["filetype"]=enum2string(m_eFileType);
}
