#include "robustlognormdlg.h"
#include "ui_robustlognormdlg.h"

#include <ParameterHandling.h>
#include <ModuleException.h>
#include "StdPreprocModulesGUI_global.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>

#include <QFileDialog>
#include <QImage>

#include <io/DirAnalyzer.h>

RobustLogNormDlg::RobustLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("RobustLogNorm",true,false,true,parent),
    ui(new Ui::RobustLogNormDlg),
    nBBCount(0),
    nBBFirstIndex(1)
{
    try{
            ui->setupUi(this);
    }

    catch (ModuleException & e)
    {
        logger(kipl::logging::Logger::LogWarning,e.what());
    }


}

RobustLogNormDlg::~RobustLogNormDlg()
{
    delete ui;
}


int RobustLogNormDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img) {

    UpdateDialog();
    UpdateParameters();

    try {
        ApplyParameters();
    }
    catch (kipl::base::KiplException &e) {
        logger(kipl::logging::Logger::LogError,e.what());
        return false;
    }

    int res=QDialog::exec();


    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
        return true;
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }

    return false;


}

void RobustLogNormDlg::ApplyParameters(){

}

void RobustLogNormDlg::UpdateDialog(){

    ui->spinFirstOBBB->setValue(nBBFirstIndex);
    ui->spinCountsOBBB->setValue(nBBCount);

}

void RobustLogNormDlg::UpdateParameters(){

    nBBFirstIndex = ui->spinFirstOBBB->value();
    nBBCount = ui->spinCountsOBBB->value();
    blackbodyname = ui->edit_OB_BB_mask->text().toStdString();

    std::cout << "blackbody name in update paramaters: " << blackbodyname << std::endl;

}

void RobustLogNormDlg::UpdateParameterList(std::map<string, string> &parameters){

    parameters["BB_OB_name"] = blackbodyname;
    parameters["BB_counts"] = kipl::strings::value2string(nBBCount);
    parameters["BB_first_index"] = kipl::strings::value2string(nBBFirstIndex);

}

void RobustLogNormDlg::on_button_OBBBpath_clicked()
{
    BrowseOBBBPath();
}

void RobustLogNormDlg::BrowseOBBBPath(){

    QString ob_bb_dir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open beam images with BB",
                                                   ui->edit_OB_BB_mask->text());

    if (!ob_bb_dir.isEmpty()) {
        std::string pdir=ob_bb_dir.toStdString();

        #ifdef _MSC_VER
        const char slash='\\';
        #else
        const char slash='/';
        #endif
        ptrdiff_t pos=pdir.find_last_of(slash);

        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
        std::string fname=pdir.substr(pos+1);
        kipl::io::DirAnalyzer da;
        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->edit_OB_BB_mask->setText(QString::fromStdString(fi.m_sMask));
    }


}

void RobustLogNormDlg::on_spinFirstOBBB_editingFinished()
{

}

void RobustLogNormDlg::on_spinLastOBBB_editingFinished()
{

}

void RobustLogNormDlg::on_buttonPreviewOBBB_clicked()
{

    // does not work!!!
    // try to read the first image and then display it!
//    std::string filename, ext;
//    kipl::strings::filenames::MakeFileName(blackbodyname,nBBFirstIndex,filename,ext,'#','0');
//    QImage image("/home/carminati_c/repos/testdata/BB.tif");
//    ui->label_ImageOBBB->setPixmap(QPixmap::fromImage(image));
}
