//<LICENSE>

#include "StdPreprocModulesGUI_global.h"

#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

#include <strings/miscstring.h>
#include <base/thistogram.h>
#include <io/DirAnalyzer.h>

#include <ParameterHandling.h>
#include <ModuleException.h>

#include <ProjectionReader.h>

#include "robustlognormdlg.h"
#include "ui_robustlognormdlg.h"

RobustLogNormDlg::RobustLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("RobustLogNorm",true,false,true,parent),
    ui(new Ui::RobustLogNormDlg),
    nBBCount(0),
    nBBFirstIndex(1),
    nBBSampleCount(0),
    nBBSampleFirstIndex(1),
    radius(2),
//    bUseNormROI(true),
//    bUseNormROIBB(false),
    tau(0.99f),
    bPBvariante(true),
    bSameMask(true),
    m_nWindow(5),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    m_xInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x),
    m_yInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y),
    m_InterpMethod(ImagingAlgorithms::ReferenceImageCorrection::Polynomial),
    ffirstAngle(0.0f),
    flastAngle(360.0f),
    nBBextCount(0),
    nBBextFirstIndex(0),
    min_area(20)
{

    blackbodyname = "somename";
    blackbodysamplename = "somename";
    doseBBroi[0] = doseBBroi[1] = doseBBroi[2] = doseBBroi[3] = 0;
    BBroi[0] = BBroi[1] = BBroi[2] = BBroi[3] = 0;

    blackbodyexternalname = "./";
    blackbodysampleexternalname = "./";

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

        blackbodyexternalname = GetStringParameter(parameters,"BB_OB_ext_name");
        blackbodysampleexternalname = GetStringParameter(parameters,"BB_sample_ext_name");
        nBBextCount = GetIntParameter(parameters,"BB_ext_samplecounts");
        nBBextFirstIndex = GetIntParameter(parameters,"BB_ext_firstindex");
        bSameMask = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));

        min_area = GetIntParameter(parameters, "min_area");

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

void RobustLogNormDlg::ApplyParameters(){

    //std::cout << "apply parameters" << std::endl;

    ui->buttonPreviewOBBB->click();
    ui->buttonPreviewsampleBB->click();
    ui->errorButton->click();
    //    UpdateParameters();
    UpdateDoseROI();
    UpdateBBROI();


    std::map<std::string, std::string> parameters;
    UpdateParameterList(parameters);

    try {
        module.Configure(*(dynamic_cast<ReconConfig *>(m_Config)),parameters);
    }

    catch (kipl::base::KiplException &e) {
        QMessageBox errdlg(this);
        errdlg.setText("Failed to configure RobustLogNorm module.");

        logger(kipl::logging::Logger::LogWarning,e.what());
        return ;

    }


}

void RobustLogNormDlg::UpdateDialog(){

    UpdateDoseROI();
    UpdateBBROI();

    ui->spinFirstOBBB->setValue(nBBFirstIndex);
    ui->spinCountsOBBB->setValue(nBBCount);
    ui->edit_OB_BB_mask->setText(QString::fromStdString(blackbodyname));
    ui->spinx0BBroi->setValue(BBroi[0]);
    ui->spinx1BBroi->setValue(BBroi[2]);
    ui->spiny0BBroi->setValue(BBroi[1]);
    ui->spiny1BBroi->setValue(BBroi[3]);

    ui->spinFirstsampleBB->setValue(nBBSampleFirstIndex);
    ui->spinCountsampleBB->setValue(nBBSampleCount);
    ui->edit_sample_BB_mask->setText(QString::fromStdString(blackbodysamplename));
    ui->spinx0BBdose->setValue(doseBBroi[0]);
    ui->spinx1BBdose->setValue(doseBBroi[2]);
    ui->spiny0BBdose->setValue(doseBBroi[1]);
    ui->spiny1BBdose->setValue(doseBBroi[3]);

    ui->spinRadius->setValue(radius);
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



//    std::cout << "ui->combo_averagingMethod->currentIndex():  " << ui->combo_averagingMethod->currentText().toStdString()<< std::endl;
    ui->spinFirstAngle->setValue(ffirstAngle);
    ui->spinLastAngle->setValue(flastAngle);
    ui->spin_minarea->setValue(min_area);


//    std::cout << "update dialog" << std::endl;

}

void RobustLogNormDlg::UpdateParameters(){


    nBBFirstIndex = ui->spinFirstOBBB->value();
    nBBCount = ui->spinCountsOBBB->value();
    blackbodyname = ui->edit_OB_BB_mask->text().toStdString();
    BBroi[0] = ui->spinx0BBroi->value();
    BBroi[1] = ui->spiny0BBroi->value();
    BBroi[2] = ui->spinx1BBroi->value();
    BBroi[3] = ui->spiny1BBroi->value();

    nBBSampleFirstIndex =ui->spinFirstsampleBB->value();
    nBBSampleCount = ui->spinCountsampleBB->value();
    blackbodysamplename = ui->edit_sample_BB_mask->text().toStdString();
    doseBBroi[0] = ui->spinx0BBdose->value();
    doseBBroi[1] = ui->spiny0BBdose->value();
    doseBBroi[2] = ui->spinx1BBdose->value();
    doseBBroi[3] = ui->spiny1BBdose->value();

//    if ( (doseBBroi[3]-doseBBroi[1])>0 && (doseBBroi[2]-doseBBroi[0]>0)) {
//        bUseNormROIBB = true;
//        std::cout << "----------------" << bUseNormROIBB << std::endl;
//    } else {
//        bUseNormROIBB = false;
//        std::cout << "---------------------" << bUseNormROIBB << std::endl;
//    }

    radius = ui->spinRadius->value();
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

//    std::cout << "update parameters " << std::endl;
//    std::cout << ui->edit_OB_BB_mask->text().toStdString() << std::endl;
//    std::cout << blackbodyname << std::endl;

}

void RobustLogNormDlg::UpdateParameterList(std::map<string, string> &parameters){

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
//    parameters["useBBnormregion"] = kipl::strings::bool2string(bUseNormROIBB);
    parameters["PBvariante"] = kipl::strings::bool2string(bPBvariante);
//    parameters["usenormregion"] = kipl::strings::bool2string(bUseNormROI);
    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["firstAngle"] = kipl::strings::value2string(ffirstAngle);
    parameters["lastAngle"] = kipl::strings::value2string(flastAngle);
    parameters["X_InterpOrder"] = enum2string(m_xInterpOrder);
    parameters["Y_InterpOrder"] = enum2string(m_yInterpOrder);

    parameters["BB_OB_ext_name"] = blackbodyexternalname;
    parameters["BB_sample_ext_name"] = blackbodysampleexternalname;
    parameters["BB_ext_samplecounts"] = kipl::strings::value2string(nBBextCount);
    parameters["BB_ext_firstindex"] = kipl::strings::value2string(nBBextFirstIndex);
    parameters["SameMask"] = kipl::strings::bool2string(bSameMask);

    parameters["min_area"] = kipl::strings::value2string(min_area);

//    parameters["useBB"] = kipl::strings::bool2string(bUseBB);
//    parameters["useExternalBB"] = kipl::strings::bool2string(bUseExternalBB);



//    std::cout << "update parameters list" << std::endl;

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

        if (fi.m_sExt=="hdf"){
            ui->edit_OB_BB_mask->setText(QString::fromStdString(pdir));

            ProjectionReader reader;
            size_t Nofimgs[2];
            reader.GetNexusInfo(pdir,Nofimgs,NULL);
            ui->spinFirstOBBB->setValue(Nofimgs[0]);
            ui->spinCountsOBBB->setValue(Nofimgs[1]);
        }
        else {
            ui->edit_OB_BB_mask->setText(QString::fromStdString(fi.m_sMask));
        }
    }


}


void RobustLogNormDlg::on_buttonPreviewOBBB_clicked()
{

    UpdateParameters();


    std::string filename, ext;
    kipl::strings::filenames::MakeFileName(blackbodyname,nBBFirstIndex,filename,ext,'#','0');
    size_t found=blackbodyname.find("hdf");
    ProjectionReader reader;
    if (QFile::exists(QString::fromStdString(filename)) || QFile::exists(QString::fromStdString(blackbodyname)) && blackbodyname!="./")
        {
            if (found==std::string::npos )
            {
                m_Preview_OBBB = reader.Read(filename,
                                             m_Config->ProjectionInfo.eFlip,
                                             m_Config->ProjectionInfo.eRotate,
                                             m_Config->ProjectionInfo.fBinning,
                                             NULL);
            }
            else {
                 m_Preview_OBBB = reader.ReadNexus(blackbodyname, 0,
                                                 m_Config->ProjectionInfo.eFlip,
                                                 m_Config->ProjectionInfo.eRotate,
                                                 m_Config->ProjectionInfo.fBinning,
                                                 NULL);
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

                ui->ob_bb_Viewer->set_image(m_Preview_OBBB.GetDataPtr(), m_Preview_OBBB.Dims(), lo,hi);
    }


}

void RobustLogNormDlg::on_button_BBroi_clicked()
{
    QRect rect=ui->ob_bb_Viewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
//        ui->spinx0BBroi->blockSignals(true);
//        ui->spiny0BBroi->blockSignals(true);
//        ui->spinx1BBroi->blockSignals(true);
//        ui->spiny1BBroi->blockSignals(true);
        ui->spinx0BBroi->setValue(rect.x());
        ui->spiny0BBroi->setValue(rect.y());
        ui->spinx1BBroi->setValue(rect.x()+rect.width());
        ui->spiny1BBroi->setValue(rect.y()+rect.height());
//        ui->spinx0BBroi->blockSignals(false);
//        ui->spiny0BBroi->blockSignals(false);
//        ui->spinx1BBroi->blockSignals(false);
//        ui->spiny1BBroi->blockSignals(false);
        UpdateBBROI();
    }
}

void RobustLogNormDlg::UpdateBBROI()
{
    QRect rect;

    rect.setCoords(ui->spinx0BBroi->value(),
                   ui->spiny0BBroi->value(),
                   ui->spinx1BBroi->value(),
                   ui->spiny1BBroi->value());

    ui->ob_bb_Viewer->set_rectangle(rect,QColor("green").light(),0);
}

void RobustLogNormDlg::on_spinx0BBroi_valueChanged(int arg1)
{
    UpdateBBROI();
}

void RobustLogNormDlg::on_spinx1BBroi_valueChanged(int arg1){ UpdateBBROI(); }
void RobustLogNormDlg::on_spiny0BBroi_valueChanged(int arg1){ UpdateBBROI(); }
void RobustLogNormDlg::on_spiny1BBroi_valueChanged(int arg1){ UpdateBBROI(); }



void RobustLogNormDlg::on_button_sampleBBpath_clicked()
{
    BrowseSampleBBPath();
}

void RobustLogNormDlg::BrowseSampleBBPath(){

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
        }
    }

}

void RobustLogNormDlg::on_buttonPreviewsampleBB_clicked()
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
                                     NULL);


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

            ui->sample_bb_Viewer->set_image(m_Preview_sampleBB.GetDataPtr(), m_Preview_sampleBB.Dims(), lo,hi);

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
                                              NULL);
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

            ui->sample_bb_Viewer->set_image(m_Preview_sampleBB.GetDataPtr(), m_Preview_sampleBB.Dims(), lo,hi);

    //    }
    //    else {

    //        ui->projectionViewer->get_levels(&lo,&hi);
    //        ui->projectionViewer->set_image(m_PreviewImage.GetDataPtr(),m_PreviewImage.Dims(),lo,hi);
    //    }

    }

}

void RobustLogNormDlg::on_button_BBdose_clicked()
{
    QRect rect=ui->sample_bb_Viewer->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spinx0BBdose->blockSignals(true);
        ui->spiny0BBdose->blockSignals(true);
        ui->spinx1BBdose->blockSignals(true);
        ui->spiny1BBdose->blockSignals(true);
        ui->spinx0BBdose->setValue(rect.x());
        ui->spiny0BBdose->setValue(rect.y());
        ui->spinx1BBdose->setValue(rect.x()+rect.width());
        ui->spiny1BBdose->setValue(rect.y()+rect.height());
        ui->spinx0BBdose->blockSignals(false);
        ui->spiny0BBdose->blockSignals(false);
        ui->spinx1BBdose->blockSignals(false);
        ui->spiny1BBdose->blockSignals(false);
        UpdateDoseROI();
    }
}


void RobustLogNormDlg::UpdateDoseROI(){
    QRect rect;

    rect.setCoords(ui->spinx0BBdose->value(),
                   ui->spiny0BBdose->value(),
                   ui->spinx1BBdose->value(),
                   ui->spiny1BBdose->value());

    ui->sample_bb_Viewer->set_rectangle(rect,QColor("red"),0);

}

void RobustLogNormDlg::on_spinx0BBdose_valueChanged(int arg1)
{
    UpdateDoseROI();
}

void RobustLogNormDlg::on_spinx1BBdose_valueChanged(int arg1)
{
    UpdateDoseROI();
}

void RobustLogNormDlg::on_spiny0BBdose_valueChanged(int arg1)
{
    UpdateDoseROI();
}

void RobustLogNormDlg::on_spiny1BBdose_valueChanged(int arg1)
{
    UpdateDoseROI();
}


void RobustLogNormDlg::on_errorButton_clicked()
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
            module.Configure(*(dynamic_cast<ReconConfig *>(m_Config)),parameters);
            error = module.GetInterpolationError(mymask);
        }
        catch(kipl::base::KiplException &e) {
            QMessageBox errdlg(this);
            errdlg.setText("Failed to compute interpoltion error.");
            logger(kipl::logging::Logger::LogWarning,e.what());
            return ;
        }
        catch(...){
            QMessageBox errdlg(this);
            errdlg.setText("Failed to compute interpoltion error.. generic exception.");
            return ;
        }


//        std::cout << error << std::endl;

        // display interpolation error
        ui->errorBrowser->setText(QString::number(error));

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
        ui->mask_Viewer->set_image(mymask.GetDataPtr(), mymask.Dims(), lo,hi);
    }

}

void RobustLogNormDlg::on_combo_BBoptions_activated(const QString &arg1)
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

void RobustLogNormDlg::on_button_OB_BB_ext_clicked()
{
    QString ob_bb_ext_dir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open beam images with BB",
                                                   ui->edit_OBBB_ext->text());

    if (!ob_bb_ext_dir.isEmpty()) {
        std::string pdir=ob_bb_ext_dir.toStdString();

//        #ifdef _MSC_VER
//        const char slash='\\';
//        #else
//        const char slash='/';
//        #endif
//        ptrdiff_t pos=pdir.find_last_of(slash);

//        QString path(QString::fromStdString(pdir.substr(0,pos+1)));
//        std::string fname=pdir.substr(pos+1);
//        kipl::io::DirAnalyzer da;
//        kipl::io::FileItem fi=da.GetFileMask(pdir);

        ui->edit_OBBB_ext->setText(QString::fromStdString(pdir)); // I want the entire filename
    }

}

void RobustLogNormDlg::on_button_BBexternal_path_clicked()
{
    QString sample_bb_ext_dir=QFileDialog::getOpenFileName(this,
                                      "Select location of the open beam images with BB",
                                                   ui->edit_BB_external->text());

    if (!sample_bb_ext_dir.isEmpty()) {
        std::string pdir=sample_bb_ext_dir.toStdString();

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
    }


}


void RobustLogNormDlg::on_combo_InterpolationMethod_activated(const QString &arg1)
{
    if (arg1.toStdString()=="Polynomial") {
        ui->combo_IntMeth_X->setEnabled(true);
        ui->combo_IntMeth_Y->setEnabled(true);
        ui->label_23->setEnabled(true);
        ui->label_24->setEnabled(true);
        ui->errorBrowser->setEnabled(true);
    }

    if (arg1.toStdString()=="ThinPlateSplines"){
        ui->combo_IntMeth_X->setEnabled(false);
        ui->combo_IntMeth_Y->setEnabled(false);
        ui->label_23->setEnabled(false);
        ui->label_24->setEnabled(false);
        ui->errorBrowser->setEnabled(false);
    }

}
