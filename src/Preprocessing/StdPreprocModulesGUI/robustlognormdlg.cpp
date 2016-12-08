#include "robustlognormdlg.h"
#include "ui_robustlognormdlg.h"

#include <ParameterHandling.h>
#include <ModuleException.h>
#include "StdPreprocModulesGUI_global.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>

#include <QFileDialog>
#include <QImage>

#include <ProjectionReader.h>
#include <base/thistogram.h>

#include <io/DirAnalyzer.h>

RobustLogNormDlg::RobustLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("RobustLogNorm",true,false,true,parent),
    ui(new Ui::RobustLogNormDlg),
    nBBCount(0),
    nBBFirstIndex(1)
{

    blackbodyname = "prova";
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

    m_Config=dynamic_cast<ReconConfig *>(config);
    std::cout << "dopo m_Config" << std::endl;


    try{
        nBBFirstIndex = GetIntParameter(parameters,"BB_first_index");
        nBBCount = GetIntParameter(parameters,"BB_counts");
        blackbodyname = GetStringParameter(parameters,"BB_OB_name");
    }
    catch(ModuleException &e){
        logger(kipl::logging::Logger::LogWarning,e.what());
        return false;
    }

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


ReconConfig *rc=dynamic_cast<ReconConfig *>(m_Config);
std::cout << "apply parameters" << std::endl;

}

void RobustLogNormDlg::UpdateDialog(){

    ui->spinFirstOBBB->setValue(nBBFirstIndex);
    ui->spinCountsOBBB->setValue(nBBCount);
    ui->edit_OB_BB_mask->setText(QString::fromStdString(blackbodyname));
    std::cout << "update dialog" << std::endl;

}

void RobustLogNormDlg::UpdateParameters(){


    nBBFirstIndex = ui->spinFirstOBBB->value();
    nBBCount = ui->spinCountsOBBB->value();
    blackbodyname = ui->edit_OB_BB_mask->text().toStdString();

    std::cout << "update parameters " << std::endl;
    std::cout << ui->edit_OB_BB_mask->text().toStdString() << std::endl;
    std::cout << blackbodyname << std::endl;
//    std::string sPath, sFname;
//    std::string name,ext;
//    kipl::strings::filenames::MakeFileName(blackbodyname, ui->edit_OB_BB_mask->text().toStdString(),sPath,sFname,exts);
//    blackbodyname = ui->edit_OB_BB_mask->text().toStdString();

//    std::cout << "blackbody name in update paramaters: " << sFname << std::endl;

}

void RobustLogNormDlg::UpdateParameterList(std::map<string, string> &parameters){

    parameters["BB_OB_name"] = blackbodyname;
    parameters["BB_counts"] = kipl::strings::value2string(nBBCount);
    parameters["BB_first_index"] = kipl::strings::value2string(nBBFirstIndex);
    std::cout << "update parameters list" << std::endl;

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
    std::string filename, ext;
    kipl::strings::filenames::MakeFileName(blackbodyname,nBBFirstIndex,filename,ext,'#','0');
//    QImage image("/home/carminati_c/repos/testdata/BB.tif");
//    ui->label_ImageOBBB->setPixmap(QPixmap::fromImage(image));

    ProjectionReader reader;

    m_Preview_OBBB = reader.Read(filename,
                                 m_Config->ProjectionInfo.eFlip,
                                 m_Config->ProjectionInfo.eRotate,
                                 m_Config->ProjectionInfo.fBinning,
                                 m_Config->ProjectionInfo.projection_roi);

    std::cout << "image read: " << filename << std::endl;

//    float lo,hi;

////    if (x < 0) {
//        const size_t NHist=512;
//        size_t hist[NHist];
//        float axis[NHist];
//        size_t nLo=0;
//        size_t nHi=0;
//        kipl::base::Histogram(m_Preview_OBBB.GetDataPtr(),m_Preview_OBBB.Size(),hist,NHist,0.0f,0.0f,axis);
//        kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
//        lo=axis[nLo];
//        hi=axis[nHi];

//        ui->ob_bb_Viewer->set_image(m_Preview_OBBB.GetDataPtr(), m_Preview_OBBB.Dims(), lo,hi);

////    }
////    else {

////        ui->projectionViewer->get_levels(&lo,&hi);
////        ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
////    }


}
