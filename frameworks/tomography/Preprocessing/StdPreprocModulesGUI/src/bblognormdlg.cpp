//<LICENSE>

#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

#include <strings/miscstring.h>
#include <strings/filenames.h>
#include <base/thistogram.h>
#include <io/DirAnalyzer.h>

#include <ParameterHandling.h>
#include <ModuleException.h>
#include <base/KiplException.h>
#include <ReconException.h>
#include <ImagingException.h>

#include <ProjectionReader.h>

#include "bblognormdlg.h"
#include "ui_bblognormdlg.h"

BBLogNormDlg::BBLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("BBLogNorm",true,false,false,parent),
    ui(new Ui::BBLogNormDlg),
    nBBCount(0),
    nBBFirstIndex(1),
    nBBSampleCount(0),
    nBBSampleFirstIndex(1),
    BBroi(4,0UL),
    doseBBroi(4,0UL),
    radius(2),
    min_area(20),
    ffirstAngle(0.0f),
    flastAngle(360.0f),
//    bUseNormROI(true),
//    bUseNormROIBB(false),
    blackbodyexternalname("none"),
    blackbodysampleexternalname("none"),
    blackbodyexternalmaskname("none"),
    nBBextCount(1),
    nBBextFirstIndex(0),
    tau(0.99f),
    thresh(0),
    bPBvariante(true),
    bSameMask(true),
    bUseManualThresh(false),
    bExtSingleFile(true),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    m_xInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x),
    m_yInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y),
    m_InterpMethod(ImagingAlgorithms::ReferenceImageCorrection::Polynomial),
    m_nWindow(5)
{

    blackbodyname = "somename";
    blackbodysamplename = "somename";

    blackbodyexternalname = "./";
    blackbodysampleexternalname = "./";

    try{
            ui->setupUi(this);
            ui->roiwidget_BB->registerViewer(ui->ob_bb_Viewer);
            ui->roiwidget_BB->setTitle("BB ROI");
            ui->roiwidget_BB->setROIColor("green");

            ui->roiwidget_dose->registerViewer(ui->sample_bb_Viewer);
            ui->roiwidget_dose->setTitle("dose ROI");
            ui->roiwidget_dose->setROIColor("red");

    }

    catch (ModuleException & e)
    {
        logger(kipl::logging::Logger::LogWarning,e.what());
    }



}

BBLogNormDlg::~BBLogNormDlg()
{
    delete ui;
}


int BBLogNormDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> & /*img*/) 
{
    m_Config=dynamic_cast<ReconConfig *>(config);

    try{
        nBBFirstIndex = GetIntParameter(parameters,"BB_first_index");
        nBBCount = GetIntParameter(parameters,"BB_counts");
        blackbodyname = GetStringParameter(parameters,"BB_OB_name");
        nBBSampleFirstIndex = GetIntParameter(parameters,"BB_sample_firstindex");
        nBBSampleCount = GetIntParameter(parameters,"BB_samplecounts");
        blackbodysamplename = GetStringParameter(parameters, "BB_samplename");
        GetUIntParameterVector(parameters,"BBroi",BBroi,4);
        GetUIntParameterVector(parameters,"doseBBroi",doseBBroi,4);
        radius = GetIntParameter(parameters,"radius");
//        bUseNormROIBB = kipl::strings::string2bool(GetStringParameter(parameters,"useBBnormregion"));
        tau = GetFloatParameter(parameters, "tau");
        bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));
        string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
        string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
        string2enum(GetStringParameter(parameters,"BBOption"), m_BBOptions);
        m_nWindow = GetIntParameter(parameters,"window");
        ffirstAngle = GetFloatParameter(parameters,"firstAngle");
        flastAngle = GetFloatParameter(parameters,"lastAngle");
        string2enum(GetStringParameter(parameters,"X_InterpOrder"), m_xInterpOrder);
        string2enum(GetStringParameter(parameters,"Y_InterpOrder"), m_yInterpOrder);
        string2enum(GetStringParameter(parameters,"InterpolationMethod"), m_InterpMethod);

        blackbodyexternalname       = GetStringParameter(parameters, "BB_OB_ext_name");
        blackbodysampleexternalname = GetStringParameter(parameters, "BB_sample_ext_name");
        blackbodyexternalmaskname   = GetStringParameter(parameters, "BB_mask_ext_name");
        nBBextCount                 = GetIntParameter(parameters,    "BB_ext_samplecounts");
        nBBextFirstIndex            = GetIntParameter(parameters,    "BB_ext_firstindex");
        bSameMask                   = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));
        bExtSingleFile              = kipl::strings::string2bool(GetStringParameter(parameters, "singleBBext"));
        bUseManualThresh            = kipl::strings::string2bool(GetStringParameter(parameters,"ManualThreshold"));
        thresh                      = GetFloatParameter(parameters,"thresh");
        min_area                    = GetIntParameter(parameters, "min_area");

//        bUseExternalBB = kipl::strings::string2bool(GetStringParameter(parameters,"useExternalBB")); // not sure I need those here
//        bUseBB = kipl::strings::string2bool(GetStringParameter(parameters, "useBB"));




    }
    catch(ModuleException &e){
        logger(kipl::logging::Logger::LogWarning,e.what());
        return false;

            QMessageBox error_dlg;
            error_dlg.setWindowTitle("Error");
            error_dlg.setText("Module exception");
            error_dlg.setDetailedText(QString::fromStdString(e.what()));

    }

    UpdateDialog();

    try {
        ApplyParameters();
    }
    catch (kipl::base::KiplException &e) {
        logger(kipl::logging::Logger::LogError,e.what());
        return false;
    }


    if (enum2string(m_BBOptions)=="noBB" || enum2string(m_BBOptions)=="ExternalBB") {
        ui->tabWidget->setTabEnabled(1, false);
    } // not sure this is the right place

    if (enum2string(m_BBOptions)!="ExternalBB"){
        ui->group_externalBB->setEnabled(false);
    }
    else {
        ui->group_externalBB->setEnabled(true);
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

    return res;


}

void BBLogNormDlg::ApplyParameters(){


    ui->buttonPreviewOBBB->click();
    ui->buttonPreviewsampleBB->click();


    std::map<std::string, std::string> parameters;
    UpdateParameterList(parameters);



}

void BBLogNormDlg::UpdateDialog(){


    ui->spinFirstOBBB->setValue(nBBFirstIndex);
    ui->spinCountsOBBB->setValue(nBBCount);
    ui->edit_OB_BB_mask->setText(QString::fromStdString(blackbodyname));
    ui->roiwidget_BB->setROI(BBroi);
    ui->spinFirstsampleBB->setValue(nBBSampleFirstIndex);
    ui->spinCountsampleBB->setValue(nBBSampleCount);
    ui->edit_sample_BB_mask->setText(QString::fromStdString(blackbodysamplename));
    ui->roiwidget_dose->setROI(doseBBroi);
    ui->spinRadius->setValue(static_cast<int>(radius));
    ui->combo_averagingMethod->setCurrentText(QString::fromStdString(enum2string(m_ReferenceAverageMethod)));
    ui->combo_referencingmethod->setCurrentText(QString::fromStdString(enum2string(m_ReferenceMethod)));
    ui->combo_BBoptions->setCurrentText(QString::fromStdString(enum2string(m_BBOptions)));
    ui->combo_IntMeth_X->setCurrentText(QString::fromStdString(enum2string(m_xInterpOrder)));
    ui->combo_IntMeth_Y->setCurrentText(QString::fromStdString(enum2string(m_yInterpOrder)));
    ui->edit_OBBB_ext->setText(QString::fromStdString(blackbodyexternalname));
    ui->edit_BB_external->setText(QString::fromStdString(blackbodysampleexternalname));
    ui->spin_first_extBB->setValue(nBBextFirstIndex);
    ui->spin_count_ext_BB->setValue(nBBextCount);
    ui->combo_InterpolationMethod->setCurrentText(QString::fromStdString(enum2string(m_InterpMethod)));
    ui->checkBox_thresh->setChecked(bUseManualThresh);
    ui->spinThresh->setValue(thresh);
    ui->spinFirstAngle->setValue(ffirstAngle);
    ui->spinLastAngle->setValue(flastAngle);
    ui->spin_minarea->setValue(min_area);
    ui->check_singleext->setChecked(bExtSingleFile);



}

void BBLogNormDlg::UpdateParameters(){


    nBBFirstIndex = ui->spinFirstOBBB->value();
    nBBCount = ui->spinCountsOBBB->value();
    blackbodyname = ui->edit_OB_BB_mask->text().toStdString();
    ui->roiwidget_BB->getROI(BBroi);

    nBBSampleFirstIndex =ui->spinFirstsampleBB->value();
    nBBSampleCount = ui->spinCountsampleBB->value();
    blackbodysamplename = ui->edit_sample_BB_mask->text().toStdString();

    ui->roiwidget_dose->getROI(doseBBroi);

    radius = static_cast<size_t>(ui->spinRadius->value());
    string2enum(ui->combo_averagingMethod->currentText().toStdString(), m_ReferenceAverageMethod);
    string2enum(ui->combo_referencingmethod->currentText().toStdString(), m_ReferenceMethod);
    string2enum(ui->combo_BBoptions->currentText().toStdString(), m_BBOptions);
    string2enum(ui->combo_IntMeth_X->currentText().toStdString(), m_xInterpOrder);
    string2enum(ui->combo_IntMeth_Y->currentText().toStdString(), m_yInterpOrder);
    string2enum(ui->combo_InterpolationMethod->currentText().toStdString(), m_InterpMethod);

    ffirstAngle = ui->spinFirstAngle->value();
    flastAngle = ui->spinLastAngle->value();

    blackbodyexternalname = ui->edit_OBBB_ext->text().toStdString();
    blackbodysampleexternalname = ui->edit_BB_external->text().toStdString();
    nBBextFirstIndex = ui->spin_first_extBB->value();
    nBBextCount = ui->spin_count_ext_BB->value();

    min_area = ui->spin_minarea->value();

    bUseManualThresh = ui->checkBox_thresh->isChecked();
    thresh = ui->spinThresh->value();
    bExtSingleFile = ui->check_singleext->isChecked();


}

void BBLogNormDlg::UpdateParameterList(std::map<string, string> &parameters){

    parameters["BB_OB_name"] = blackbodyname;
    parameters["BB_counts"] = kipl::strings::value2string(nBBCount);
    parameters["BB_first_index"] = kipl::strings::value2string(nBBFirstIndex);
    parameters["BBroi"] = kipl::strings::value2string(BBroi[0])+" "+kipl::strings::value2string(BBroi[1])+" "+kipl::strings::value2string(BBroi[2])+" "+kipl::strings::value2string(BBroi[3]);

    parameters["BB_samplename"] = blackbodysamplename;
    parameters["BB_samplecounts"] = kipl::strings::value2string(nBBSampleCount);
    parameters["BB_sample_firstindex"] = kipl::strings::value2string(nBBSampleFirstIndex);
    parameters["doseBBroi"] = kipl::strings::value2string(doseBBroi[0])+" "+kipl::strings::value2string(doseBBroi[1])+" "+kipl::strings::value2string(doseBBroi[2])+" "+kipl::strings::value2string(doseBBroi[3]);

    parameters["radius"] = kipl::strings::value2string(radius);

    parameters["avgmethod"] = enum2string(m_ReferenceAverageMethod);
    parameters["refmethod"] = enum2string(m_ReferenceMethod);
    parameters["BBOption"] = enum2string(m_BBOptions);
    parameters["InterpolationMethod"] = enum2string(m_InterpMethod);
    parameters["tau"] = kipl::strings::value2string(tau);
    parameters["PBvariante"] = kipl::strings::bool2string(bPBvariante);
    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["firstAngle"] = kipl::strings::value2string(ffirstAngle);
    parameters["lastAngle"] = kipl::strings::value2string(flastAngle);
    parameters["X_InterpOrder"] = enum2string(m_xInterpOrder);
    parameters["Y_InterpOrder"] = enum2string(m_yInterpOrder);

    parameters["BB_OB_ext_name"]      = blackbodyexternalname;
    parameters["BB_sample_ext_name"]  = blackbodysampleexternalname;
    parameters["BB_mask_ext_name"]    = blackbodyexternalmaskname ;
    parameters["BB_ext_samplecounts"] = kipl::strings::value2string(nBBextCount);
    parameters["BB_ext_firstindex"]   = kipl::strings::value2string(nBBextFirstIndex);

    parameters["SameMask"] = kipl::strings::bool2string(bSameMask);
    parameters["ManualThreshold"] = kipl::strings::bool2string(bUseManualThresh);
    parameters["min_area"] = kipl::strings::value2string(min_area);
    parameters["thresh"]= kipl::strings::value2string(thresh);
    parameters["singleBBext"] = kipl::strings::bool2string(bExtSingleFile);
}

void BBLogNormDlg::on_button_OBBBpath_clicked()
{
    BrowseOBBBPath();
    ui->buttonPreviewOBBB->click();
}

void BBLogNormDlg::BrowseOBBBPath(){

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

        if (fi.m_sExt=="hdf"){
            ui->edit_OB_BB_mask->setText(QString::fromStdString(pdir));

            ProjectionReader reader;
            size_t Nofimgs[2];
            reader.GetNexusInfo(pdir,Nofimgs,nullptr);
            ui->spinFirstOBBB->setValue(Nofimgs[0]);
            ui->spinCountsOBBB->setValue(Nofimgs[1]);
        }
        else {
            ui->edit_OB_BB_mask->setText(QString::fromStdString(fi.m_sMask));
            int c=0;
            int f=0;
            int l=0;
            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);
            ui->spinFirstOBBB->setValue(f);
            ui->spinCountsOBBB->setValue(c);
        }
    }


}


void BBLogNormDlg::on_buttonPreviewOBBB_clicked()
{

    UpdateParameters();


    std::string filename, ext;
    kipl::strings::filenames::MakeFileName(blackbodyname,nBBFirstIndex,filename,ext,'#','0');
    size_t found=blackbodyname.find("hdf");
    ProjectionReader reader;
    if ((QFile::exists(QString::fromStdString(filename)) || QFile::exists(QString::fromStdString(blackbodyname))) && blackbodyname!="./")
        {
            if (found==std::string::npos )
            {
                m_Preview_OBBB = reader.Read(filename,
                                             m_Config->ProjectionInfo.eFlip,
                                             m_Config->ProjectionInfo.eRotate,
                                             m_Config->ProjectionInfo.fBinning,
                                             {});
            }
            else {
                 m_Preview_OBBB = reader.ReadNexus(blackbodyname, 0,
                                                 m_Config->ProjectionInfo.eFlip,
                                                 m_Config->ProjectionInfo.eRotate,
                                                 m_Config->ProjectionInfo.fBinning,
                                                 {});
            }
            float lo,hi;

        //    if (x < 0) {
                const size_t NHist=512;
                size_t hist[NHist];
                float axis[NHist];
                size_t nLo=0;
                size_t nHi=0;
                kipl::base::Histogram(m_Preview_OBBB.GetDataPtr(),m_Preview_OBBB.Size(),hist,NHist,0.0f,0.0f,axis);
                kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
                lo=axis[nLo];
                hi=axis[nHi];

                ui->ob_bb_Viewer->set_image(m_Preview_OBBB.GetDataPtr(), m_Preview_OBBB.dims(), lo,hi);
    }


}




void BBLogNormDlg::on_button_sampleBBpath_clicked()
{
    BrowseSampleBBPath();
    ui->buttonPreviewsampleBB->click();
}

void BBLogNormDlg::BrowseSampleBBPath(){

    QString sample_bb_dir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open beam images with BB",
                                                   ui->edit_sample_BB_mask->text());

    if (!sample_bb_dir.isEmpty()) {
        std::string pdir=sample_bb_dir.toStdString();

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

        if (fi.m_sExt=="hdf"){
            ui->edit_sample_BB_mask->setText(QString::fromStdString(pdir));
            ProjectionReader reader;
            size_t Nofimgs[2];
            double Angles[2];
            reader.GetNexusInfo(pdir, Nofimgs, Angles);
            ui->spinFirstsampleBB->setValue(Nofimgs[0]);
            ui->spinCountsampleBB->setValue(Nofimgs[1]);
            ui->spinFirstAngle->setValue(Angles[0]);
            ui->spinLastAngle->setValue(Angles[1]);
        }
        else {
            ui->edit_sample_BB_mask->setText(QString::fromStdString(fi.m_sMask));
            int c=0;
            int f=0;
            int l=0;
            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);
            ui->spinFirstsampleBB->setValue(f);
            ui->spinCountsampleBB->setValue(c);
        }
    }

}

void BBLogNormDlg::on_buttonPreviewsampleBB_clicked()
{
    UpdateParameters();


    std::string filename, ext;
    kipl::strings::filenames::MakeFileName(blackbodysamplename,nBBSampleFirstIndex,filename,ext,'#','0');
    ProjectionReader reader;

    if (QFile::exists(QString::fromStdString(filename)))
    {
        m_Preview_sampleBB = reader.Read(filename,
                                     m_Config->ProjectionInfo.eFlip,
                                     m_Config->ProjectionInfo.eRotate,
                                     m_Config->ProjectionInfo.fBinning,
                                     {});


        float lo,hi;

    //    if (x < 0) {
            const size_t NHist=512;
            size_t hist[NHist];
            float axis[NHist];
            size_t nLo=0;
            size_t nHi=0;
            kipl::base::Histogram(m_Preview_OBBB.GetDataPtr(),m_Preview_OBBB.Size(),hist,NHist,0.0f,0.0f,axis);
            kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
            lo=axis[nLo];
            hi=axis[nHi];

            ui->sample_bb_Viewer->set_image(m_Preview_sampleBB.GetDataPtr(), m_Preview_sampleBB.dims(), lo,hi);

    //    }
    //    else {

    //        ui->projectionViewer->get_levels(&lo,&hi);
    //        ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
    //    }
    }

    if (QFile::exists(QString::fromStdString(blackbodysamplename)) && blackbodysamplename!="./") {
        m_Preview_sampleBB = reader.ReadNexus(blackbodysamplename, nBBSampleFirstIndex,
                                              m_Config->ProjectionInfo.eFlip,
                                              m_Config->ProjectionInfo.eRotate,
                                              m_Config->ProjectionInfo.fBinning,
                                              {});
        float lo,hi;

    //    if (x < 0) {
            const size_t NHist=512;
            size_t hist[NHist];
            float axis[NHist];
            size_t nLo=0;
            size_t nHi=0;
            kipl::base::Histogram(m_Preview_OBBB.GetDataPtr(),m_Preview_OBBB.Size(),hist,NHist,0.0f,0.0f,axis);
            kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
            lo=axis[nLo];
            hi=axis[nHi];

            ui->sample_bb_Viewer->set_image(m_Preview_sampleBB.GetDataPtr(), m_Preview_sampleBB.dims(), lo,hi);

    //    }
    //    else {

    //        ui->projectionViewer->get_levels(&lo,&hi);
    //        ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
    //    }

    }

}



void BBLogNormDlg::on_errorButton_clicked()
{

    // has to run only if bb ob image and bb roi are loaded

    QRect rect=ui->ob_bb_Viewer->get_marked_roi();


    if (rect.width()*rect.height()!=0) {

//        ReconConfig *rc=dynamic_cast<ReconConfig *>(m_Config);

        std::map<std::string, std::string> parameters;
        UpdateParameters();
        UpdateParameterList(parameters);
//        std::cout << "trying to compute error" << std::endl;
//        std::cout << "tau: " << GetFloatParameter(parameters, "tau") << std::endl;

        kipl::base::TImage<float,2> mymask;
        float error;

        try {
            module.ConfigureDLG(*(dynamic_cast<ReconConfig *>(m_Config)),parameters);

        }
        catch(ModuleException &e){
            QMessageBox errdlg(this);
            errdlg.setText("Failed to configure the module dialog, check the parameters");
            errdlg.setDetailedText(QString::fromStdString(e.what()));
            logger(kipl::logging::Logger::LogWarning,e.what());
            errdlg.exec();
            return ;
        }
        catch(kipl::base::KiplException &e) {
            QMessageBox errdlg(this);
            errdlg.setText("Failed to configure the module dialog, check the parameters");
            errdlg.setDetailedText(QString::fromStdString(e.what()));
            logger(kipl::logging::Logger::LogWarning,e.what());
            errdlg.exec();
            return ;
        }
        catch(...){
            QMessageBox errdlg(this);
            errdlg.setText("Failed to configure the module dialog.. generic exception.");
            return ;
        }

        try {
            error = module.GetInterpolationError(mymask);
        }
        catch(ImagingException &e ){
            QMessageBox errdlg(this);
            errdlg.setText("Failed to compute interpolation error. Hint: try to change the threshold by using the manual threshold option");
            errdlg.setDetailedText(QString::fromStdString(e.what()));
            logger(kipl::logging::Logger::LogWarning,e.what());
            errdlg.exec();
            return ;

        }
        catch(kipl::base::KiplException &e) {
            QMessageBox errdlg(this);
            errdlg.setText("Failed to compute interpolation error. Hint: try to change the threshold by using the manual threshold option");
            errdlg.setDetailedText(QString::fromStdString(e.what()));
            logger(kipl::logging::Logger::LogWarning,e.what());
            errdlg.exec();
            return ;
        }
        catch(...){
            QMessageBox errdlg(this);
            errdlg.setText("Failed to compute interpolation error.. generic exception.");
            return ;
        }


        // display interpolation error

        ui->label_error->setText(QString::number(error));

        // display computed mask
    //    kipl::base::TImage<float,2> mymask = module.GetMaskImage();
        float lo,hi;
        const size_t NHist=512;
        size_t hist[NHist];
        float axis[NHist];
        size_t nLo=0;
        size_t nHi=0;
        kipl::base::Histogram(mymask.GetDataPtr(),mymask.Size(),hist,NHist,0.0f,0.0f,axis);
        kipl::base::FindLimits(hist, NHist, 99.0f, &nLo, &nHi);
        lo=axis[nLo];
        hi=axis[nHi];
        ui->mask_Viewer->set_image(mymask.GetDataPtr(), mymask.dims(), lo,hi);
    }

}

void BBLogNormDlg::on_combo_BBoptions_activated(const QString &arg1)
{

//    std::cout << "arg1: " << arg1.toStdString() << std::endl;
    if (arg1.toStdString()=="noBB" || arg1.toStdString()=="ExternalBB"){
        ui->tabWidget->setTabEnabled(1,false);
    }
    else {
        ui->tabWidget->setTabEnabled(1,true);
    }

    if(arg1.toStdString()!="ExternalBB"){
        ui->group_externalBB->setEnabled(false);
    }
    else {
        ui->group_externalBB->setEnabled(true);
    }
}

void BBLogNormDlg::on_button_OB_BB_ext_clicked()
{
    QString ob_bb_ext_dir=QFileDialog::getOpenFileName(this,
                                      "Select the open beam images with BB",
                                                   ui->edit_OBBB_ext->text());

    if (!ob_bb_ext_dir.isEmpty()) {
        std::string pdir=ob_bb_ext_dir.toStdString();

        ui->edit_OBBB_ext->setText(QString::fromStdString(pdir)); // I want the entire filename
    }

}

void BBLogNormDlg::on_button_BBexternal_path_clicked()
{
    QString sample_bb_ext_dir=QFileDialog::getOpenFileName(this,
                                      "Select one sample beam images with BB",
                                                   ui->edit_BB_external->text());
    std::ostringstream msg;
    msg<< "bExtSingleFile " <<  bExtSingleFile ;
    logger.message(msg.str());

    if (!sample_bb_ext_dir.isEmpty()) {

        std::string pdir=sample_bb_ext_dir.toStdString();
        if (bExtSingleFile)
        {
            ui->edit_BB_external->setText(QString::fromStdString(pdir));
        }
        else {

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

            ui->edit_BB_external->setText(QString::fromStdString(fi.m_sMask));

            int c=0;
            int f=0;
            int l=0;
            da.AnalyzeMatchingNames(fi.m_sMask,c,f,l);
            ui->spin_first_extBB->setValue(f);
            ui->spin_count_ext_BB->setValue(c);

        }

    }


}


void BBLogNormDlg::on_combo_InterpolationMethod_activated(const QString &arg1)
{
    if (arg1.toStdString()=="Polynomial") {
        ui->combo_IntMeth_X->setEnabled(true);
        ui->combo_IntMeth_Y->setEnabled(true);
        ui->label_23->setEnabled(true);
        ui->label_24->setEnabled(true);
//        ui->errorBrowser->setEnabled(true);
    }

    if (arg1.toStdString()=="ThinPlateSplines"){
        ui->combo_IntMeth_X->setEnabled(false);
        ui->combo_IntMeth_Y->setEnabled(false);
        ui->label_23->setEnabled(false);
        ui->label_24->setEnabled(false);
//        ui->errorBrowser->setEnabled(false);
    }

}

void BBLogNormDlg::on_checkBox_thresh_clicked(bool checked)
{
    bUseManualThresh = checked;

}


void BBLogNormDlg::on_spinThresh_valueChanged(double arg1)
{
    thresh = arg1;
}

void BBLogNormDlg::on_checkBox_thresh_stateChanged(int /*arg1*/)
{

}

void BBLogNormDlg::on_pushButton_filenameOBBB_clicked()
{
    ui->edit_OB_BB_mask->setText(QString::fromStdString(m_Config->ProjectionInfo.sFileMask));
}

void BBLogNormDlg::on_pushButton_filenameBB_clicked()
{
    ui->edit_sample_BB_mask->setText(ui->edit_OB_BB_mask->text());
}


void BBLogNormDlg::on_check_singleext_clicked(bool checked)
{
    bExtSingleFile = checked;
    // triggers the re-browsing of the bb ext file
    ui->button_BBexternal_path->click();

}

void BBLogNormDlg::on_check_singleext_stateChanged(int /*arg1*/)
{
    if (ui->check_singleext->isChecked())
    {
        bExtSingleFile = true;
    }
    else {
        bExtSingleFile = false;
    }

}

void BBLogNormDlg::on_pushButton_ext_OB_back_clicked()
{
    ui->edit_OBBB_ext->setText(QString::fromStdString(m_Config->ProjectionInfo.sFileMask));
}

void BBLogNormDlg::on_pushButton_ext_sample_back_clicked()
{
    ui->edit_BB_external->setText(QString::fromStdString(m_Config->ProjectionInfo.sFileMask));
}
