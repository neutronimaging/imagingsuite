#include <strings/miscstring.h>

//#include <ReconException.h>
//#include <ProjectionReader.h>
//#include <ReconConfig.h>
#include <math/image_statistics.h>
#include <math/median.h>
#include <ParameterHandling.h>

#include <base/textractor.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

#include "bblognorm.h"
#include "../include/ImagingException.h"
#include <imagereader.h>

BBLogNorm::BBLogNorm(kipl::interactors::InteractionBase *interactor) : KiplProcessModuleBase("BBLogNorm", false, interactor),
    // to check which one do i need: to be removed: m_nWindow and bUseWeightedMean
    nOBCount(0),
    nOBFirstIndex(1),
    nBBSampleCount(0),
    nBBSampleFirstIndex(1),
    radius(2),
    nDCCount(0),
    nDCFirstIndex(1),
    nBBCount(0),
    nBBFirstIndex(1),
    fFlatDose(1.0f),
    fBlackDose(1.0f),
    fDarkDose(0.0f),
    fdarkBBdose(0.0f),
    fFlatBBdose(1.0f),
    bUseNormROI(true),
    bUseLUT(false),
    bUseWeightedMean(false),
    bUseBB(false),
    bUseNormROIBB(false),
    m_nWindow(5),
    tau(0.97f),
    bPBvariante(true),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    m_xInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x),
    m_yInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y),
    m_InterpMethod(ImagingAlgorithms::ReferenceImageCorrection::Polynomial),
    ferror(0.0f),
    ffirstAngle(0.0f),
    flastAngle(360.0f),
    nBBextCount(0),
    nBBextFirstIndex(0),
    bUseExternalBB(false),
    bSameMask(true),
    bUseManualThresh(false),
    min_area(20),
    thresh(0),
    bSaveBG(false),
    m_Interactor(interactor)
{

    doseBBroi[0] = doseBBroi[1] = doseBBroi[2] = doseBBroi[3]=0;
    dose_roi[0] = dose_roi[1] = dose_roi[2] = dose_roi[3]=0;
    BBroi[0] = BBroi[1] = BBroi[2] = BBroi[3] = 0;
    fScanArc[0] = 0.0;
    fScanArc[1] = 360.0;
    blackbodyname = "./";
    blackbodysamplename = "./";
    blackbodyexternalname = "./";
    blackbodysampleexternalname = "./";
    path="./";
    flatname="./";
    darkname="./";
    pathBG="./";
    flatname_BG="flat_background.tif";
    filemask_BG="sample_background_####.tif";




}

BBLogNorm::~BBLogNorm()
{

}

bool BBLogNorm::updateStatus(float val, string msg){

    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }
    return false;

}

int BBLogNorm::Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters)
{

    std::stringstream msg;
    msg<<"Configuring BBLogNorm::Configure";
    logger(kipl::logging::Logger::LogDebug,msg.str());

    m_Config    = config;

    m_corrector.SetInteractor(m_Interactor); // outside the constructor.. to check if still OK.

//    path        = config.ProjectionInfo.sReferencePath;
//    flatname    = config.ProjectionInfo.sOBFileMask;
//    darkname    = config.ProjectionInfo.sDCFileMask;

//    nOBCount      = config.ProjectionInfo.nOBCount;
//    nOBFirstIndex = config.ProjectionInfo.nOBFirstIndex;

//    nDCCount      = config.ProjectionInfo.nDCCount;
//    nDCFirstIndex = config.ProjectionInfo.nDCFirstIndex;

    path = GetStringParameter(parameters,"path"); // not sure it is used
    flatname = GetStringParameter(parameters, "OB_PATH");
    darkname = GetStringParameter(parameters, "DC_PATH");
    nOBFirstIndex = GetIntParameter(parameters, "OB_first_index");
    nOBCount = GetIntParameter(parameters,"OB_counts");
    nDCFirstIndex = GetIntParameter(parameters, "DC_first_index");
    nDCCount = GetIntParameter(parameters,"DC_counts");
    GetUIntParameterVector(parameters, "dose_roi", dose_roi, 4);
    GetFloatParameterVector(parameters, "fScanArc", fScanArc, 2);



    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
    string2enum(GetStringParameter(parameters,"BBOption"), m_BBOptions);
    string2enum(GetStringParameter(parameters, "X_InterpOrder"), m_xInterpOrder);
    string2enum(GetStringParameter(parameters, "Y_InterpOrder"), m_yInterpOrder);
    string2enum(GetStringParameter(parameters,"InterpolationMethod"), m_InterpMethod);
    bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));


    blackbodyname = GetStringParameter(parameters,"BB_OB_name");
    nBBCount = GetIntParameter(parameters,"BB_counts");
    nBBFirstIndex = GetIntParameter(parameters, "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters,"BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters, "BB_sample_firstindex");
    nBBSampleCount = GetIntParameter(parameters,"BB_samplecounts");

    blackbodyexternalname = GetStringParameter(parameters, "BB_OB_ext_name");
    blackbodysampleexternalname = GetStringParameter(parameters, "BB_sample_ext_name");
    nBBextCount = GetIntParameter(parameters, "BB_ext_samplecounts");
    nBBextFirstIndex = GetIntParameter(parameters, "BB_ext_firstindex");

    tau = GetFloatParameter(parameters, "tau");
    radius = GetIntParameter(parameters, "radius");
    min_area = GetIntParameter(parameters, "min_area");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
    ffirstAngle = GetFloatParameter(parameters, "firstAngle");
    flastAngle = GetFloatParameter(parameters, "lastAngle");
    bSameMask = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));
    bUseManualThresh = kipl::strings::string2bool(GetStringParameter(parameters,"ManualThreshold"));
    thresh = GetFloatParameter(parameters,"thresh");

    bSaveBG = kipl::strings::string2bool(GetStringParameter(parameters,"SaveBG"));
    pathBG = GetStringParameter(parameters,"path_BG");
    flatname_BG=GetStringParameter(parameters,"flatname_BG");
    filemask_BG=GetStringParameter(parameters,"filemask_BG");



    m_corrector.SetManualThreshold(bUseManualThresh,thresh);
//    std::cout << bUseManualThresh << " " << thresh << std::endl;

    memcpy(nOriginalNormRegion,dose_roi,4*sizeof(size_t));

    if ( m_Config.mImageInformation.bUseROI) {
        msg<<"using image roi";
        logger(kipl::logging::Logger::LogDebug,msg.str());
    }
    else
    {

//        msg<<"image roi not checked: " << m_Config.mImageInformation.nROI[0] << " " << m_Config.mImageInformation.nROI[1]<< " " << m_Config.mImageInformation.nROI[2] << " " << m_Config.mImageInformation.nROI[3]<<endl;
        kipl::base::TImage<float,2 > myimg;
        ImageReader reader;
        std:: string filename,ext;
        std::string fmask = m_Config.mImageInformation.sSourceFileMask;
        int firstIndex = m_Config.mImageInformation.nFirstFileIndex;
        kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
        myimg = reader.Read(filename, kipl::base::ImageFlipNone, kipl::base::ImageRotateNone, 1.0f, nullptr,0L);
        m_Config.mImageInformation.nROI[0]=m_Config.mImageInformation.nROI[1]=1;
        m_Config.mImageInformation.nROI[2]=myimg.Size(0)-1;
        m_Config.mImageInformation.nROI[3]=myimg.Size(1)-1;
        m_Config.mImageInformation.bUseROI=true;

        msg<<"not using image roi. Reading original image size: " << m_Config.mImageInformation.nROI[2] << " " << m_Config.mImageInformation.nROI[3];
        logger(kipl::logging::Logger::LogDebug,msg.str());

    }

    size_t roi_bb_x= BBroi[2]-BBroi[0];
    size_t roi_bb_y = BBroi[3]-BBroi[1];

    if (roi_bb_x>0 && roi_bb_y>0) {}
    else {
        memcpy(BBroi,m_Config.mImageInformation.nROI, sizeof(size_t)*4);  // use the same as projections in case.. if i don't I got an Exception
    }

    //check on dose BB roi size
    if ((doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0){
        bUseNormROIBB=false;
//        throw ImagingException("No roi is selected for dose BB correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else {
        bUseNormROIBB = true;
    }

    if ((dose_roi[2]-dose_roi[0])<=0 || (dose_roi[3]-dose_roi[1])<=0 ){
        bUseNormROI=false;
        throw ImagingException("No roi is selected for dose correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else{
        bUseNormROI=true;
    }

    if (enum2string(m_ReferenceMethod)=="LogNorm"){
        m_corrector.SetComputeMinusLog(true);
    }

    if (enum2string(m_ReferenceMethod)=="Norm")
    {
//        std::cout << "(enum2string(m_ReferenceMethod)==Norm)" << std::endl;
        m_corrector.SetComputeMinusLog(false);
    }

    switch (m_BBOptions){
    case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
        bUseBB = false;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Average): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB): {
        bUseBB = false; // to evaluate
        bUseExternalBB = true;
        break;
    }
    default: throw ImagingException("Unknown BBOption method in BBLogNorm::Configure",__FILE__,__LINE__);

    }

    if (bSaveBG){
        m_corrector.SaveBG(bSaveBG,pathBG,flatname_BG,filemask_BG);
    }

//    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
//            PrepareBBData();
//    }

//     SetROI(m_Config.mImageInformation.nROI);


//    std::stringstream msg;
    msg<<"Configuring done";
    logger(kipl::logging::Logger::LogDebug,msg.str());


//    std::cout << "Configuring done"<< std::endl;

    return 1;
}


int BBLogNorm::ConfigureDLG(KiplProcessConfig config, std::map<std::string, std::string> parameters)
{

    m_Config    = config;
    path        = config.mImageInformation.sSourcePath;
    flatname    = GetStringParameter(parameters, "OB_PATH");
    darkname    = GetStringParameter(parameters, "DC_PATH");

    nOBCount      = GetIntParameter(parameters,"OB_counts");
    nOBFirstIndex = GetIntParameter(parameters, "OB_first_index");

    nDCCount      = GetIntParameter(parameters,"DC_counts");
    nDCFirstIndex = GetIntParameter(parameters, "DC_first_index");

    GetUIntParameterVector(parameters, "dose_roi", dose_roi, 4);
    GetFloatParameterVector(parameters, "fScanArc", fScanArc, 2);


    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
    string2enum(GetStringParameter(parameters,"BBOption"), m_BBOptions);
    string2enum(GetStringParameter(parameters, "X_InterpOrder"), m_xInterpOrder);
    string2enum(GetStringParameter(parameters, "Y_InterpOrder"), m_yInterpOrder);
    string2enum(GetStringParameter(parameters,"InterpolationMethod"), m_InterpMethod);
    bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));


    blackbodyname = GetStringParameter(parameters,"BB_OB_name");
    nBBCount = GetIntParameter(parameters,"BB_counts");
    nBBFirstIndex = GetIntParameter(parameters, "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters,"BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters, "BB_sample_firstindex");
    nBBSampleCount = GetIntParameter(parameters,"BB_samplecounts");

    blackbodyexternalname = GetStringParameter(parameters, "BB_OB_ext_name");
    blackbodysampleexternalname = GetStringParameter(parameters, "BB_sample_ext_name");
    nBBextCount = GetIntParameter(parameters, "BB_ext_samplecounts");
    nBBextFirstIndex = GetIntParameter(parameters, "BB_ext_firstindex");

    tau = GetFloatParameter(parameters, "tau");
    radius = GetIntParameter(parameters, "radius");
    min_area = GetIntParameter(parameters, "min_area");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
    ffirstAngle = GetFloatParameter(parameters, "firstAngle");
    flastAngle = GetFloatParameter(parameters, "lastAngle");
    bSameMask = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));
    bUseManualThresh = kipl::strings::string2bool(GetStringParameter(parameters,"ManualThreshold"));
    thresh = GetFloatParameter(parameters,"thresh");

    bSaveBG = kipl::strings::string2bool(GetStringParameter(parameters,"SaveBG"));
    pathBG = GetStringParameter(parameters,"path_BG");
    flatname_BG=GetStringParameter(parameters,"flatname_BG");
    filemask_BG=GetStringParameter(parameters,"filemask_BG");

    m_corrector.SetManualThreshold(bUseManualThresh,thresh);
//    std::cout << bUseManualThresh << " " << thresh << std::endl;

    memcpy(nOriginalNormRegion,dose_roi,4*sizeof(size_t));

//    size_t roi_bb_x= BBroi[2]-BBroi[0];
//    size_t roi_bb_y = BBroi[3]-BBroi[1];

//    // do i need this here?
//    if (roi_bb_x>0 && roi_bb_y>0) {}
//    else {
//        memcpy(BBroi, m_Config.mImageInformation.nROI, sizeof(size_t)*4);  // use the same as projections in case.. if i don't I got an Exception
//    }

    //check on dose BB roi size
    if ((doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0){
        bUseNormROIBB=false;
//        throw ImagingException("No roi is selected for dose BB correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else {
        bUseNormROIBB = true;
    }

    if ((dose_roi[2]-dose_roi[0])<=0 || (dose_roi[3]-dose_roi[1])<=0 ){
        bUseNormROI=false;
        throw ImagingException("No roi is selected for dose correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else{
        bUseNormROI=true;
    }

    if (enum2string(m_ReferenceMethod)=="LogNorm"){
        m_corrector.SetComputeMinusLog(true);
    }

    if (enum2string(m_ReferenceMethod)=="Norm") {
//        std::cout << "enum2string(m_ReferenceMethod)==Norm" << std::endl;
        m_corrector.SetComputeMinusLog(false);
    }

    switch (m_BBOptions){
    case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
        bUseBB = false;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Average): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
        bUseBB = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB): {
        bUseBB = false; // to evaluate
        bUseExternalBB = true;
        break;
    }
    default: throw ImagingException("Unknown BBOption method in BBLogNorm::Configure",__FILE__,__LINE__);

    }


//    std::cout<<"end of BBLogNorm::ConfigureDlg, empty for now"<<std::endl;


    return 1;
}

bool BBLogNorm::SetROI(size_t *roi) { // that is not loaded.

    std::stringstream msg;
    msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogDebug,msg.str());
    memcpy(nNormRegion,nOriginalNormRegion,4*sizeof(size_t)); //nNormRegion seems not used

    LoadReferenceImages(roi);
    return true;
}


std::map<std::string, std::string> BBLogNorm::GetParameters() {

    std::map<std::string, std::string> parameters;

    std::stringstream msg;
    msg<<"Getting Parameters for BBLogNorm";
    logger(kipl::logging::Logger::LogDebug,msg.str());

    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["avgmethod"] = enum2string(m_ReferenceAverageMethod);
    parameters["refmethod"] = enum2string(m_ReferenceMethod);
    parameters["BB_OB_name"] = blackbodyname;
    parameters["BB_counts"] = kipl::strings::value2string(nBBCount);
    parameters["BB_first_index"] = kipl::strings::value2string(nBBFirstIndex);
    parameters["BB_samplename"] = blackbodysamplename;
    parameters["BB_samplecounts"] = kipl::strings::value2string(nBBSampleCount);
    parameters["BB_sample_firstindex"] = kipl::strings::value2string(nBBSampleFirstIndex);
    parameters["tau"] = kipl::strings::value2string(tau);
    parameters["radius"] = kipl::strings::value2string(radius);
    parameters["BBroi"] = kipl::strings::value2string(BBroi[0])+" "+kipl::strings::value2string(BBroi[1])+" "+kipl::strings::value2string(BBroi[2])+" "+kipl::strings::value2string(BBroi[3]);
    parameters["doseBBroi"] = kipl::strings::value2string(doseBBroi[0])+" "+kipl::strings::value2string(doseBBroi[1])+" "+kipl::strings::value2string(doseBBroi[2])+" "+kipl::strings::value2string(doseBBroi[3]);
    parameters["PBvariante"] = kipl::strings::bool2string(bPBvariante);
    parameters["BBOption"] = enum2string(m_BBOptions);
    parameters["firstAngle"] = kipl::strings::value2string(ffirstAngle);
    parameters["lastAngle"] = kipl::strings::value2string(flastAngle);
    parameters["X_InterpOrder"] = enum2string(m_xInterpOrder);
    parameters["Y_InterpOrder"] = enum2string(m_yInterpOrder);
    parameters["InterpolationMethod"] = enum2string(m_InterpMethod);
    parameters["BB_OB_ext_name"] = blackbodyexternalname;
    parameters["BB_sample_ext_name"] = blackbodysampleexternalname;
    parameters["BB_ext_samplecounts"] = kipl::strings::value2string(nBBextCount);
    parameters["BB_ext_firstindex"] = kipl::strings::value2string(nBBextFirstIndex);
    parameters["SameMask"] = kipl::strings::bool2string(bSameMask);
    parameters["min_area"] = kipl::strings::value2string(min_area);
    parameters["ManualThreshold"] = kipl::strings::bool2string(bUseManualThresh);
    parameters["thresh"]= kipl::strings::value2string(thresh);

    parameters["OB_PATH"]= flatname;
    parameters["DC_PATH"]= darkname;
    parameters["OB_first_index"] = kipl::strings::value2string(nOBFirstIndex);
    parameters["OB_counts"] = kipl::strings::value2string(nOBCount);
    parameters["DC_counts"] = kipl::strings::value2string(nDCCount);
    parameters["DC_first_index"] = kipl::strings::value2string(nDCFirstIndex);
    parameters["dose_roi"] =  kipl::strings::value2string(dose_roi[0])+" "+kipl::strings::value2string(dose_roi[1])+" "+kipl::strings::value2string(dose_roi[2])+" "+kipl::strings::value2string(dose_roi[3]);
    parameters["fScanArc"] =  kipl::strings::value2string(fScanArc[0])+" "+kipl::strings::value2string(fScanArc[1]);
    parameters["path"]= path;
    parameters["SaveBG"] = kipl::strings::bool2string(bSaveBG);
    parameters["path_BG"] = pathBG;
    parameters["flatname_BG"] = flatname_BG;
    parameters["filemask_BG"] = filemask_BG;

    msg<<"end of BBLogNorm::GetParameters";
    logger(kipl::logging::Logger::LogDebug,msg.str());

    return parameters;
}

void BBLogNorm::LoadReferenceImages(size_t *roi)
{

    std::stringstream msg;
    msg<<"Loading reference images with roi: "<< roi[0] << " " << roi[1] << " " << roi[2] <<" " << roi[3];
    logger(kipl::logging::Logger::LogDebug,msg.str());

    if (flatname.empty() && nOBCount!=0)
        throw ImagingException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ImagingException("The dark current image mask is empty",__FILE__,__LINE__);


    kipl::base::TImage<float,2> myflat, mydark;

    fDarkDose=0.0f;
    fFlatDose=1.0f;
    std::string flatmask=flatname;
    std::string darkmask=darkname;

    myflat = ReferenceLoader(flatmask,nOBFirstIndex,nOBCount,roi,1.0f,0.0f,m_Config, fFlatDose); // i don't use the bias.. beacuse i think i use it later on
    mydark = ReferenceLoader(darkmask,nDCFirstIndex,nDCCount,roi,0.0f,0.0f,m_Config, fDarkDose);
    SetReferenceImages(mydark,myflat);

//    switch (m_BBOptions){
//    case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
//        bUseBB = false;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::Average): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB): {
//        bUseBB = false; // to evaluate
//        bUseExternalBB = true;
//        break;
//    }
//    default: throw ReconException("Unknown BBOption method in RobustLogNorm::Configure",__FILE__,__LINE__);

//    }


    if (bUseExternalBB && nBBextCount!=0){
        LoadExternalBBData(roi); // they must be ready for SetReferenceImages
    }


//    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
//        PrepareBBData();
//    }

     m_corrector.SetReferenceImages(&mflat, &mdark, (bUseBB && nBBCount!=0 && nBBSampleCount!=0), (bUseExternalBB && nBBextCount!=0), fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), roi, dose_roi);

     msg<<"References loaded";
     logger(kipl::logging::Logger::LogDebug,msg.str());

//     std::cout << "References loaded" << std::endl;

}

void BBLogNorm::LoadExternalBBData(size_t *roi){


    if (blackbodyexternalname.empty())
        throw ImagingException("The pre-processed open beam with BB image mask is empty", __FILE__, __LINE__);
    if (blackbodysampleexternalname.empty() || nBBextCount==0)
        throw ImagingException("The pre-processed sample with BB image mask is empty", __FILE__, __LINE__);


    kipl::base::TImage<float,2> bb_ext;
    kipl::base::TImage<float,3> bb_sample_ext;
    float dose;
    float *doselist = new float[nBBextCount];

    bb_ext = BBExternalLoader(blackbodyexternalname, roi, dose);
//    kipl::io::WriteTIFF32(bb_ext,"bb_ext.tif");
//    std::cout << "dose: " << dose << std::endl;
    bb_sample_ext = BBExternalLoader(blackbodysampleexternalname, nBBextCount, roi, nBBextFirstIndex,doselist);

    m_corrector.SetExternalBBimages(bb_ext, bb_sample_ext, dose, doselist);

}

void BBLogNorm::PrepareBBData(){

    logger(kipl::logging::Logger::LogDebug,"PrepareBBData begin--");
//    std::cout << "PrepareBBData begin--" << std::endl;
    if (flatname.empty() && nOBCount!=0)
        throw ImagingException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ImagingException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ImagingException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);
    if (blackbodysamplename.empty() && nBBSampleCount!=0)
        throw ImagingException("The sample image with BBs image mask is empty", __FILE__,__LINE__);



    int diffroi[4] = {int(BBroi[0]), int(BBroi[1]), int(BBroi[2]), int(BBroi[3])};

    m_corrector.setDiffRoi(diffroi);
    m_corrector.SetRadius(radius);
    m_corrector.SetTau(tau);
    m_corrector.SetPBvariante(bPBvariante);
    m_corrector.SetInterpolationOrderX(m_xInterpOrder);
    m_corrector.SetInterpolationOrderY(m_yInterpOrder);
    m_corrector.SetMinArea(min_area);
    m_corrector.SetInterpolationMethod(m_InterpMethod);

//    float *bb_ob_param;
//    float *bb_sample_parameters;

 size_t nProj=(m_Config.mImageInformation.nLastFileIndex-m_Config.mImageInformation.nFirstFileIndex+1); // here I only need the number of images loaded.. and not these computation relevant to tomography
    // this should come somehow from the KipTool mainwindow I guess


        switch (m_InterpMethod) {
        case (ImagingAlgorithms::ReferenceImageCorrection::Polynomial): {
            PreparePolynomialInterpolationParameters();

            if (nBBCount!=0 && nBBSampleCount!=0) {
                 m_corrector.SetInterpParameters(ob_bb_param, sample_bb_param,nBBSampleCount, nProj, m_BBOptions);
            }

            break;
        }
        case(ImagingAlgorithms::ReferenceImageCorrection::ThinPlateSplines):{
            std::cout << "ThinPlateSplines" << std::endl;
             logger(kipl::logging::Logger::LogDebug,"ThinPlateSplines");

            int nBBs = PrepareSplinesInterpolationParameters();

             logger(kipl::logging::Logger::LogDebug,"After PrepareSplinesInterpolationParameters ");

            if (nBBCount!=0 && nBBSampleCount!=0) {
                m_corrector.SetSplinesParameters(ob_bb_param, sample_bb_param, nBBSampleCount, nProj, m_BBOptions, nBBs); // i also need the coordinates.
            }
            break;
        }
        default: throw ImagingException("Unknown m_InterpMethod in BBLogNorm::PrepareBBData()", __FILE__, __LINE__);
        }


//        std::cout << "end of prepare BB data" << std::endl;



}

void BBLogNorm::PreparePolynomialInterpolationParameters()
{

    kipl::base::TImage<float,2> flat, dark, bb, sample, samplebb;

    std::string flatmask=flatname;
    std::string darkmask=darkname;


    fdarkBBdose=0.0f;
    fFlatBBdose=1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,nDCFirstIndex,nDCCount,0.0f,0.0f, fdarkBBdose);
    flat = BBLoader(flatmask,nOBFirstIndex,nOBCount,1.0f,0.0f, fFlatBBdose); // to check if i have to use dosebias to remove the fdarkBBdose

    // now load OB image with BBs


    float *bb_ob_param = new float[6];
    float *bb_sample_parameters;

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,fdarkBBdose, fBlackDose); // this is for mask computation and dose correction (small roi)

    kipl::base::TImage<float,2> obmask(bb.Dims());


    bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, ferror);


    if (bPBvariante) {
     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(bb_ob_param,doseBBroi);
     float mydose = computedose(mybb);
     fBlackDose = fBlackDose + ((1.0/tau-1.0)*mydose);
    }


    if(bUseNormROI && bUseNormROIBB){
     fBlackDose = fBlackDose<1 ? 1.0f : fBlackDose;
     for (size_t i=0; i<6; i++){
         bb_ob_param[i]= bb_ob_param[i]*(fFlatBBdose-fdarkBBdose);
         bb_ob_param[i]= bb_ob_param[i]/(fBlackDose*tau); // now the dose is already computed for I_OB_BB
     }
    }

    ob_bb_param = new float[6];
    memcpy(ob_bb_param, bb_ob_param, sizeof(float)*6);
//    ob_bb_param[0] = 0.0f;
//    ob_bb_param[1] = 0.0f;
//    ob_bb_param[2] = 0.0f;
//    ob_bb_param[3] = 0.0f;
//    ob_bb_param[4] = 0.0f;
//    ob_bb_param[5] = 0.0f;


    // load sample images with BBs and sample images

    float *temp_parameters;
    size_t nProj=(m_Config.mImageInformation.nLastFileIndex-m_Config.mImageInformation.nFirstFileIndex+1);
    size_t step = (nProj)/(nBBSampleCount);

    float angles[4] = {fScanArc[0], fScanArc[1], ffirstAngle, flastAngle};
    m_corrector.SetAngles(angles, nProj, nBBSampleCount);

//                          std::cout << "doselist: " << std::endl;

    float *doselist = new float[nProj];
    for (size_t i=0; (i<nProj && (updateStatus(float(i)/nProj,"Loading dose for BB images")==false) ) ; i++) {
        doselist[i] = DoseBBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+i, 1.0f, fdarkBBdose); // D(I*n-Idc) in the doseBBroi
    }

    m_corrector.SetDoseList(doselist);
    delete [] doselist;



// here Exceptions need to be added to veirfy if the selected module is compatible with the number of loaded images
         switch (m_BBOptions) {

         case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
                 bb_sample_parameters = new float[6*nBBSampleCount];
                 sample_bb_param = new float[6*nBBSampleCount];
                 temp_parameters = new float[6];

                 if (nBBSampleCount!=0) {

                     logger(kipl::logging::Logger::LogDebug,"Loading sample images with BB");

//                     float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
//                     m_corrector.SetAngles(angles, nProj, nBBSampleCount);

////                     std::cout << "doselist: " << std::endl;

//                     float *doselist = new float[nProj];
//                     for (size_t i=0; i<int(nProj); i++) {
//                         doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
//                     }

//                     m_corrector.SetDoseList(doselist);
//                     delete [] doselist;

                     for (size_t i=0; (i<nBBSampleCount && (updateStatus(float(i)/nBBSampleCount,"Calculating Polynomial interpolation")==false)); i++) {
                         samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, fBlackDoseSample);

                         float dosesample; // used for the correct dose roi computation (doseBBroi)
                         float current_dose; // current dose for sample with BBs
                         int index;

                         index = GetnProjwithAngle((angles[2]+(angles[3]-angles[2])/(nBBSampleCount-1)*i));

                         // in the first case compute the mask again (should not be necessary in well acquired datasets) however this requires that the first image (or at least one) of sample image with BB has the same angle of the sample image without BB
                         if (i==0) {

                             if (bSameMask){
                                mMaskBB = obmask;
                                temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);
                             }
                             else {
                                 sample = BBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+index, 1, 1.0f,fdarkBBdose, dosesample);
                                 kipl::base::TImage<float,2> mask(sample.Dims());
                                 mask = 0.0f;
                                 temp_parameters = m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                                 mMaskBB = mask; // or memcpy
                             }



                             if (bUseNormROIBB && bUseNormROI){
             //                     prenormalize interpolation parameters with dose

                                 current_dose = fBlackDoseSample;

                                 if (bPBvariante) {

                                         kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                         float mydose = computedose(mybb);
                                         current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                     }


                                 current_dose = current_dose<1 ? 1.0f : current_dose;



                                 for(size_t j=0; j<6; j++) {

                                           temp_parameters[j]/=(current_dose);

                                 }
                             }

                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);

                         }

                         else {

                             temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                             current_dose = fBlackDoseSample;

                             if (bPBvariante) {
                                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                     float mydose = computedose(mybb);
                                     current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                  }

                             current_dose = current_dose<1 ? 1.0f : current_dose;

                             for(size_t j=0; j<6; j++) {
                              temp_parameters[j]/=(current_dose);
                             }


                             memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);

                         }



                 }
                 }

                 memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*6*nBBSampleCount);
                 break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::Average): {

                     bb_sample_parameters = new float[6];
                     sample_bb_param = new float[6];
                     temp_parameters = new float[6];
                     float * mask_parameters = new float[6];

//                     float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
//                     m_corrector.SetAngles(angles, nProj, nBBSampleCount);

////                          std::cout << "doselist: " << std::endl;

//                     float *doselist = new float[nProj];
//                     for (size_t i=0; i<nProj; i++) {
//                         doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
//                     }

//                     m_corrector.SetDoseList(doselist);
//                     delete [] doselist;


                     kipl::base::TImage<float,2> samplebb_temp;
                     float dose_temp;
                     samplebb_temp = BBLoader(blackbodysamplename, nBBSampleFirstIndex,1,1.0f,0.0f,  dose_temp);
                     samplebb = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, fBlackDoseSample);


                     float dosesample; // used for the correct dose roi computation (doseBBroi)
                     float current_dose;

                     int index;
                     index = GetnProjwithAngle(ffirstAngle);
                     sample = BBLoader(m_Config.mImageInformation.sSourceFileMask,
                                       m_Config.mImageInformation.nFirstFileIndex +index, 1, 1.0f,fdarkBBdose, dosesample); // load the projection with angle closest to the first BB sample data


                     if (bSameMask){
                        mMaskBB = obmask;}
                     else {
                          kipl::base::TImage<float,2> mask(sample.Dims());
                          mask = 0.0f;
                          mask_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb_temp, mask); // this is just to compute the mask
                          mMaskBB = mask; // or memcpy
                     }

                     temp_parameters= m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);


                     if (bUseNormROIBB && bUseNormROI){
                //                     prenormalize interpolation parameters with dose

                        current_dose = fBlackDoseSample;

                         if (bPBvariante) {

                                 kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                 float mydose = computedose(mybb);
                                 current_dose+= ((1.0/tau-1.0)*mydose);
                              }


                         current_dose = current_dose<1 ? 1.0f : current_dose;

                         for(size_t j=0; j<6; j++) {

                                   temp_parameters[j]/=current_dose;
//                                   temp_parameters[j]*=(dosesample/tau);
                         }
//                         std::cout << std::endl;
                     }

                     memcpy(sample_bb_param, temp_parameters, sizeof(float)*6);
//                     memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*6);
                     delete [] mask_parameters;

                break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
//             std::cout << "OneToOne" << std::endl;

                bb_sample_parameters = new float[6*nBBSampleCount];
                sample_bb_param = new float[6*nBBSampleCount];
                temp_parameters = new float[6];


                float dosesample;
                float current_dose;

                for (size_t i=0; (i<nBBSampleCount && (updateStatus(float(i)/nBBSampleCount,"Calculating Polynomial interpolation")==false)); i++) {


                    samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, fBlackDoseSample);
                    sample = BBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex, 1, 1.0f,fdarkBBdose, dosesample); // continuare da qui.. valuatare se devo caricare sample tutte le volte oppure no e nel caso moltiplicare per la doselist in ReferenceImageCorrection come faccio negli altri due casi


                    // compute the mask again only for the first sample image, than assume BBs do not move during experiment
                    if (i==0) {

                        if (bSameMask){
                           mMaskBB = obmask;
                           temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark, samplebb, mMaskBB);
                        }
                        else {
                             kipl::base::TImage<float,2> mask(sample.Dims());
                             mask = 0.0f;
                             temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask); // this is just to compute the mask
                             mMaskBB = mask; // or memcpy
                        }

                        if (bUseNormROIBB && bUseNormROI){
        //                     prenormalize interpolation parameters with dose

                            current_dose= fBlackDoseSample;

                            if (bPBvariante) {

                                    kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                                    float mydose = computedose(mybb);
                                    current_dose += ((1.0/tau-1.0)*mydose);
                                 }


                            current_dose = current_dose<1 ? 1.0f : current_dose;

                            for(size_t j=0; j<6; j++) {

                                      temp_parameters[j]/=current_dose;
//                                      temp_parameters[j]*=(dosesample/tau);

                            }
                        }

                        memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);

                    }

                    else {

                        temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                        current_dose= fBlackDoseSample;

                        if (bPBvariante) {
                                kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                                float mydose = computedose(mybb);
                                current_dose+= ((1.0/tau-1.0)*mydose);
                             }

                        current_dose = current_dose<1 ? 1.0f : current_dose;


                        for(size_t j=0; j<6; j++) {
                             temp_parameters[j]/=current_dose;
//                             temp_parameters[j]*=(dosesample/tau);
                        }

                        memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);


                    }



            }

                memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*6*nBBSampleCount);
                break;
             }

         case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
               throw ImagingException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::noBB in PrepareBBdata",__FILE__,__LINE__);
               break;
         }
         case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB) : {
             throw ImagingException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::ExternalBB in PrepareBBdata",__FILE__,__LINE__);
         }

         default: throw ImagingException("trying to switch to unknown BBOption in PrepareBBdata",__FILE__,__LINE__);

         }

//         delete [] temp_parameters;
//         delete [] bb_ob_param;
//         delete [] bb_sample_parameters;
}

int BBLogNorm::PrepareSplinesInterpolationParameters() {

    kipl::base::TImage<float,2> flat, dark, bb, sample, samplebb;

    std::string flatmask=flatname;
    std::string darkmask=darkname;


    fdarkBBdose=0.0f;
    fFlatBBdose=1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,nDCFirstIndex,nDCCount,0.0f,0.0f,fdarkBBdose);
    flat = BBLoader(flatmask,nOBFirstIndex,nOBCount,1.0f,0.0f,fFlatBBdose); //
    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,fdarkBBdose,fBlackDose);

    kipl::base::TImage<float,2> obmask(bb.Dims());

    float *bb_ob_param = new float[100]; // now they are not 6.. attention
    float *bb_sample_parameters;
    std::map<std::pair<int, int>, float> values;
    std::map<std::pair<int, int>, float> values_bb;


     bb_ob_param = m_corrector.PrepareBlackBodyImagewithSplines(flat,dark,bb, obmask, values);

//     kipl::base::TImage<float,2> mythinimage = m_corrector.InterpolateBlackBodyImagewithSplines(bb_ob_param, values, BBroi);
//      kipl::io::WriteTIFF32(mythinimage,"BBob_image.tif"); // this is now correct!


     if (bPBvariante) {
      kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(bb_ob_param,values, doseBBroi);
//      kipl::io::WriteTIFF32(mybb,"BBdose_image.tif"); // also correct
      float mydose = computedose(mybb);
      fBlackDose = fBlackDose + ((1.0/tau-1.0)*mydose);
     }

     m_corrector.SetSplineObValues(values);

     if(bUseNormROI && bUseNormROIBB){
      fBlackDose = fBlackDose<1 ? 1.0f : fBlackDose;
      for (size_t i=0; i<values.size()+3; i++){
          bb_ob_param[i]= bb_ob_param[i]*(fFlatBBdose-fdarkBBdose);
          bb_ob_param[i]= bb_ob_param[i]/(fBlackDose*tau); // now the dose is already computed for I_OB_BB
      }
     }


//     std::cout << (fFlatBBdose-fdarkBBdose)/(fBlackDose*tau) << std::endl;// very close to 1

//          kipl::base::TImage<float,2> mythinimage = m_corrector.InterpolateBlackBodyImagewithSplines(bb_ob_param, values, BBroi);
//           kipl::io::WriteTIFF32(mythinimage,"BBob_image.tif"); // this is now correct! No such a big difference before and after normalization..

     ob_bb_param = new float[values.size()+3];
     memcpy(ob_bb_param, bb_ob_param, sizeof(float)*(values.size()+3));

//     for (size_t i=0; i <values.size()+3; i++) {
//         ob_bb_param[i] = 0.0f;
//     }


     // load sample images with BBs and sample images

     float *temp_parameters;
     size_t nProj=(m_Config.mImageInformation.nLastFileIndex-m_Config.mImageInformation.nFirstFileIndex+1);
     size_t step = (nProj)/(nBBSampleCount);

     float angles[4] = {fScanArc[0], fScanArc[1], ffirstAngle, flastAngle};
     m_corrector.SetAngles(angles, nProj, nBBSampleCount);

     float *doselist = new float[nProj];
     for (size_t i=0; (i<nProj && (updateStatus(float(i)/nProj,"Loading dose for BB images")==false) ); i++) {
         doselist[i] = DoseBBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+i,
                                    1.0f, fdarkBBdose); // D(I*n-Idc) in the doseBBroi
     }

     m_corrector.SetDoseList(doselist);
     delete [] doselist;


     // here Exceptions need to be added to veirfy if the selected module is compatible with the number of loaded images
              switch (m_BBOptions) {

              case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {

                      bb_sample_parameters = new float[(values.size()+3)*nBBSampleCount]; // these two are exactly the same..
                      sample_bb_param = new float[(values.size()+3)*nBBSampleCount];
                      temp_parameters = new float[(values.size()+3)];
//                      std::cout << "temp paramters before any dose normalization: " << std::endl;

                      if (nBBSampleCount!=0) {

                          logger(kipl::logging::Logger::LogDebug,"Loading sample images with BB");

//                          float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
//                          m_corrector.SetAngles(angles, nProj, nBBSampleCount);

//                          std::cout << "doselist: " << std::endl;

//                          float *doselist = new float[nProj];
//                          for (size_t i=0; i<nProj; i++) {
//                              doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
//                          }

//                          m_corrector.SetDoseList(doselist);
//                          delete [] doselist;

                          for (size_t i=0; (i<nBBSampleCount && (updateStatus(float(i)/nBBSampleCount,"Calculating ThinPlateSplines interpolation")==false)); i++) {


                              samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, fBlackDoseSample);

                              float dosesample; // used for the correct dose roi computation (doseBBroi)
                              float current_dose; // current dose for sample with BBs
                              int index;

                              index = GetnProjwithAngle((angles[2]+(angles[3]-angles[2])/(nBBSampleCount-1)*i));

                              // in the first case compute the mask again (should not be necessary in well acquired datasets) however this requires that the first image (or at least one) of sample image with BB has the same angle of the sample image without BB
                              if (i==0) {

                                  if (bSameMask){
                                     mMaskBB = obmask;
                                     values_bb = values;
                                     temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB, values_bb);
                                     m_corrector.SetSplineSampleValues(values_bb);
                                  }
                                  else {
                                      sample = BBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+index,
                                                            1, 1.0f,fdarkBBdose, dosesample);
                                      kipl::base::TImage<float,2> mask(sample.Dims());
                                      mask = 0.0f;
                                      temp_parameters = m_corrector.PrepareBlackBodyImagewithSplines(sample,dark,samplebb,mask,values_bb);
                                      m_corrector.SetSplineSampleValues(values_bb);

//                                      kipl::base::TImage<float,2> mythinimage = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters, values_bb, BBroi);
//                                       kipl::io::WriteTIFF32(mythinimage,"BBsample_image.tif"); // this is now correct!


                                      mMaskBB = mask; // or memcpy
                                  }



                                  if (bUseNormROIBB && bUseNormROI){
                  //                     prenormalize interpolation parameters with dose

                                      current_dose = fBlackDoseSample;
//                                      std:cout << "normalize dose" << std::endl;

                                      if (bPBvariante) {

//                                              kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                              kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters, values_bb, doseBBroi);
                                              float mydose = computedose(mybb);
                                              current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                          }


                                      current_dose = current_dose<1 ? 1.0f : current_dose;



                                      for(size_t j=0; j<values_bb.size()+3; j++) {

                                                temp_parameters[j]/=(current_dose);

                                      }
                                  }

                                  memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*(values_bb.size()+3));

                              }

                              else {
//                                  std::cout << "else if == 0" << std::endl;

                                  temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB,values_bb);
//                                  temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                                  current_dose = fBlackDoseSample;

                                  if (bPBvariante) {
                                          kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters,values_bb,doseBBroi);
//                                          kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                          float mydose = computedose(mybb);
                                          current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                       }

                                  current_dose = current_dose<1 ? 1.0f : current_dose;

                                  for(size_t j=0; j<(values_bb.size()+3); j++) {
                                   temp_parameters[j]/=(current_dose);
                                  }


                                  memcpy(bb_sample_parameters+i*(values_bb.size()+3), temp_parameters, sizeof(float)*(values_bb.size()+3));

                              }


                      }
                      }

                      memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*(values.size()+3)*nBBSampleCount);
                      break;
              }

              case (ImagingAlgorithms::ReferenceImageCorrection::Average): {


                          bb_sample_parameters = new float[values.size()+3]; // i am assuming there is the same number of BBs
                          sample_bb_param = new float[values.size()+3];
                          temp_parameters = new float[values.size()+3];
                          float * mask_parameters = new float[values.size()+3];

//                          float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
//                          m_corrector.SetAngles(angles, nProj, nBBSampleCount);

//                          std::cout << "doselist: " << std::endl;

//                          float *doselist = new float[nProj];
//                          for (size_t i=0; i<nProj; i++) {
//                              doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
//                          }

//                          m_corrector.SetDoseList(doselist);
//                          delete [] doselist;


                          kipl::base::TImage<float,2> samplebb_temp;
                          float dose_temp;
                          samplebb_temp = BBLoader(blackbodysamplename, nBBSampleFirstIndex,1,1.0f,0.0f, dose_temp);
                          samplebb = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, fBlackDoseSample);


                          float dosesample; // used for the correct dose roi computation (doseBBroi)
                          float current_dose;

                          int index;
                          index = GetnProjwithAngle(ffirstAngle);
                          sample = BBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+index,
                                            1, 1.0f,fdarkBBdose, dosesample); // load the projection with angle closest to the first BB sample data


                          if (bSameMask){
                             mMaskBB = obmask;
                             values_bb = values;
                             mask_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb, mMaskBB, values_bb);
//
                          }
                          else {
                               kipl::base::TImage<float,2> mask(sample.Dims());
                               mask = 0.0f;
                               mask_parameters = m_corrector.PrepareBlackBodyImagewithSplines(sample,dark, samplebb_temp, mask,values_bb);
//                               mask_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb_temp, mask); // this is just to compute the mask
                               mMaskBB = mask; // or memcpy
                          }

                          m_corrector.SetSplineSampleValues(values_bb);

                          temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB,values_bb);
//                          temp_parameters= m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);


                          if (bUseNormROIBB && bUseNormROI){
                     //                     prenormalize interpolation parameters with dose

                             current_dose = fBlackDoseSample;

                              if (bPBvariante) {

//                                      kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                      kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters,values_bb, doseBBroi);
                                      float mydose = computedose(mybb);
                                      current_dose+= ((1.0/tau-1.0)*mydose);
                                   }


                              current_dose = current_dose<1 ? 1.0f : current_dose;

                              for(size_t j=0; j<(values_bb.size()+3); j++) {

                                        temp_parameters[j]/=current_dose;
//                                        temp_parameters[j]*=(dosesample/tau);
                              }
//                              std::cout << std::endl;
                          }

                          memcpy(sample_bb_param, temp_parameters, sizeof(float)*(values.size()+3));
//                          memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*(values.size()+3));
                          delete [] mask_parameters;

                     break;
              }

              case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {

                     bb_sample_parameters = new float[(values.size()+3)*nBBSampleCount];
                     sample_bb_param = new float[(values.size()+3)*nBBSampleCount];
                     temp_parameters = new float[values.size()+3];

                     float dosesample;
                     float current_dose;

                     for (size_t i=0; (i<nBBSampleCount && (updateStatus(float(i)/nBBSampleCount,"Calculating ThinPlateSplines interpolation")==false)); i++) {


                         samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, fBlackDoseSample);
                         sample = BBLoader(m_Config.mImageInformation.sSourceFileMask, m_Config.mImageInformation.nFirstFileIndex+i,
                                             1, 1.0f,fdarkBBdose, dosesample);


                         // compute the mask again only for the first sample image, than assume BBs do not move during experiment
                         if (i==0) {

                             if (bSameMask){
                                mMaskBB = obmask;
                                values_bb = values;
                                temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB, values_bb);
//                                temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark, samplebb, mMaskBB);
                                m_corrector.SetSplineSampleValues(values_bb);
                             }
                             else {
                                  kipl::base::TImage<float,2> mask(sample.Dims());
                                  mask = 0.0f;
                                  temp_parameters = m_corrector.PrepareBlackBodyImagewithSplines(sample,dark,samplebb,mask,values_bb);
//                                  temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask); // this is just to compute the mask
                                  mMaskBB = mask; // or memcpy
                                  m_corrector.SetSplineSampleValues(values_bb);
                             }

                             if (bUseNormROIBB && bUseNormROI){
//                       prenormalize interpolation parameters with dose

                                 current_dose= fBlackDoseSample;


                                 if (bPBvariante) {
                                         kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters,values_bb,doseBBroi);
//                                         kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                                         float mydose = computedose(mybb);
                                         current_dose += ((1.0/tau-1.0)*mydose);
                                      }


                                 current_dose = current_dose<1 ? 1.0f : current_dose;

                                 for(size_t j=0; j<(values.size()+3); j++) {

                                           temp_parameters[j]/=current_dose;
//                                           temp_parameters[j]*=(dosesample/tau);

                                 }
                             }

                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*(values.size()+3));


                         }

                         else {

                               temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB,values_bb);
//                             temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                             current_dose= fBlackDoseSample;

                             if (bPBvariante) {
                                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters, values_bb, doseBBroi);
//                                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                                     float mydose = computedose(mybb);
                                     current_dose+= ((1.0/tau-1.0)*mydose);
                                  }

                             current_dose = current_dose<1 ? 1.0f : current_dose;


                             for(size_t j=0; j<(values.size()+3); j++) {
                                  temp_parameters[j]/=current_dose;
//                                  temp_parameters[j]*=(dosesample/tau);
                             }

                             memcpy(bb_sample_parameters+i*(values.size()+3), temp_parameters, sizeof(float)*(values.size()+3));


                         }

                         float dose = dosesample/(current_dose*tau);

//                         ofstream spline_values;
//                         spline_values.open(("tps_points_"+std::to_string(i)+".txt").c_str());
//                           for (std::map<std::pair<int, int>, float>::const_iterator it = values_bb.begin(); it != values_bb.end();  ++it)
//                           {
//                               spline_values << it->first.first << " " << it->first.second << " " << it->second << std::endl;
//                           }

//                           spline_values.close();
//                         std::cout << "image #: " << i << std::endl;
                     }

//                     std::cout << "before mempcy" << std::endl;
                     memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*(values.size()+3)*nBBSampleCount);
//                     std::cout << "after memcpy" << std::endl;
                     break;
                  }

              case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
                    throw ImagingException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::noBB in PrepareBBdata",__FILE__,__LINE__);
                    break;
              }
              case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB) : {
                  throw ImagingException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::ExternalBB in PrepareBBdata",__FILE__,__LINE__);
              }

              default: throw ImagingException("trying to switch to unknown BBOption in PrepareBBdata",__FILE__,__LINE__);

              }

//              delete [] temp_parameters;
//              delete [] bb_ob_param;
//              delete [] bb_sample_parameters;


    return values.size();

}

int BBLogNorm::GetnProjwithAngle(float angle){

    // range of projection angles
    double nProj=((double)m_Config.mImageInformation.nLastFileIndex-(double)m_Config.mImageInformation.nFirstFileIndex+1);

    int index =0;
    float curr_angle;

    for (size_t i=0; i<nProj; i++){
        curr_angle = fScanArc[0]+(fScanArc[1]-fScanArc[0])/(nProj-1)*i;


        if (curr_angle<=angle+0.5f & curr_angle>=angle-0.5f) {
            break;
        }
        else index++;

    }


    return index;
}

float BBLogNorm::GetInterpolationError(kipl::base::TImage<float,2> &mask){

//    std::cout << "GetInterpolationError begin.." << std::endl;
    if (flatname.empty() && nOBCount!=0)
        throw ImagingException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ImagingException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ImagingException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);

    kipl::base::TImage<float,2> flat, dark, bb;

    std::string flatmask=flatname;
    std::string darkmask=darkname;



    float darkdose = 0.0f;
    float flatdose = 1.0f;
    float blackdose = 1.0f;

//    std::cout << "before BB loader" <<std::endl;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,nDCFirstIndex,nDCCount,0.0f,0.0f,darkdose);
    flat = BBLoader(flatmask,nOBFirstIndex,nOBCount,1.0f,0.0f,flatdose);



    // now load OB image with BBs

    float *bb_ob_param = new float[6];

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,0.0f,blackdose);

//    std::cout << "after BB loader" <<std::endl;

    int diffroi[4] = {static_cast<int>(BBroi[0]),
                      static_cast<int>(BBroi[1]),
                      static_cast<int>(BBroi[2]),
                      static_cast<int>(BBroi[3])}; // it is now just the BBroi position, makes more sense

//    std::cout << "before set corrector" <<std::endl;

    m_corrector.SetRadius(radius);
    m_corrector.SetMinArea(min_area);
    m_corrector.SetInterpolationOrderX(m_xInterpOrder);
    m_corrector.SetInterpolationOrderY(m_yInterpOrder);
    m_corrector.setDiffRoi(diffroi); // left to compute the interpolation parameters in the abssolute image coordinates


//    std::cout << "after set corrector" <<std::endl;

    std::stringstream msg;
    msg.str(""); msg<<"Min area set to  "<<min_area;
    logger(kipl::logging::Logger::LogDebug,msg.str());

    float error;
    kipl::base::TImage<float,2> obmask(bb.Dims());
    bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, error);
    mask = obmask;

//    std::cout << "after PrepareBlackBodyImage" <<std::endl;

    delete [] bb_ob_param;


    return error;
}

kipl::base::TImage<float,2> BBLogNorm::GetMaskImage(){
    return mMaskBB;
}

float BBLogNorm::computedose(kipl::base::TImage<float,2>&img){

    float *pImg=img.GetDataPtr();
    float *means=new float[img.Size(1)];
    memset(means,0,img.Size(1)*sizeof(float));

    for (size_t y=0; y<img.Size(1); y++) {
        pImg=img.GetLinePtr(y);

        for (size_t x=0; x<img.Size(0); x++) {
            means[y]+=pImg[x];
        }
        means[y]=means[y]/static_cast<float>(img.Size(0));
    }

    float dose;
    kipl::math::median(means,img.Size(1),&dose);
    delete [] means;
    return dose;


}

int BBLogNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{

    return 0;
}

int BBLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) {

    // here I should call as well the SetRoi

//    SetROI(m_Config.mImageInformation.nROI);

//    size_t roi_bb_x= BBroi[2]-BBroi[0];
//    size_t roi_bb_y = BBroi[3]-BBroi[1];

//    if (roi_bb_x>0 && roi_bb_y>0) {}
//    else {
//        memcpy(BBroi,m_Config.mImageInformation.nROI, sizeof(size_t)*4);  // use the same as projections in case.. if i don't I got an Exception
//    }

//    //check on dose BB roi size
//    if ((doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0){
//        bUseNormROIBB=false;
//    }
//    else {
//        bUseNormROIBB = true;
//    }

//    if ((dose_roi[2]-dose_roi[0])<=0 || (dose_roi[3]-dose_roi[1])<=0 ){
//        bUseNormROI=false;
//        throw ImagingException("No roi is selected for dose correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
//    }
//    else{
//        bUseNormROI=true;
//    }

//    if (enum2string(m_ReferenceMethod)=="LogNorm"){
//        m_corrector.SetComputeMinusLog(true);
//    }
//    else {
//        m_corrector.SetComputeMinusLog(false);
//    }

//    switch (m_BBOptions){
//    case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
//        bUseBB = false;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::Average): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
//        bUseBB = true;
//        bUseExternalBB = false;
//        break;
//    }
//    case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB): {
//        bUseBB = false; // to evaluate
//        bUseExternalBB = true;
//        break;
//    }
//    default: throw ImagingException("Unknown BBOption method in BBLogNorm::Configure",__FILE__,__LINE__);

//    }


//    SetROI(m_Config.mImageInformation.nROI);

//    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
//            PrepareBBData();
//    }


    std::stringstream msg;
    msg.str(""); msg<<"ProcessCore";
    logger(kipl::logging::Logger::LogDebug,msg.str());

    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
            PrepareBBData();
    }

     SetROI(m_Config.mImageInformation.nROI);




    int nDose=img.Size(2);
    float *doselist=nullptr;


    ImageReader reader;


    if (bUseNormROI==true) {
        doselist=new float[nDose];
//        GetFloatParameterVector(coeff,"dose",doselist,nDose); // i don't have this fucking parameter
        std:: string filename,ext;
        std::string fmask = m_Config.mImageInformation.sSourceFileMask;
        int firstIndex = m_Config.mImageInformation.nFirstFileIndex;


        for (int i=0; i<nDose; i++) {
            kipl::strings::filenames::MakeFileName(fmask,firstIndex+i,filename,ext,'#','0');

            msg.str(""); msg<<"Reading dose for: " << filename;
            logger(kipl::logging::Logger::LogDebug,msg.str());

            float dose = reader.GetProjectionDose(filename,
                                                   kipl::base::ImageFlipNone,
                                                  kipl::base::ImageRotateNone,
                                                  1.0f,
                                                  dose_roi);
            doselist[i] = dose-fDarkDose;
        }
    }

        m_corrector.Process(img,doselist);

    if (doselist!=nullptr)
        delete [] doselist;

    return 0;
}

void BBLogNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{

    mdark = dark;
    mdark.Clone();

    mflat = flat;
    mflat.Clone();



//    m_corrector.SetReferenceImages(&mflat, &mdark, (bUseBB && nBBCount!=0 && nBBSampleCount!=0), (bUseExternalBB && nBBextCount!=0), fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), m_Config.ProjectionInfo.roi, m_Config.ProjectionInfo.dose_roi);

    // I assume I don't need it now
//    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
//            for (int i=1; i<static_cast<int>(flat.Size(1)); i++) {
//                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
//    }

}


kipl::base::TImage<float,2> BBLogNorm::ReferenceLoader(std::string fname,
                                                      int firstIndex, int N, size_t *roi,
                                                      float initialDose,
                                                      float doseBias,
                                                      KiplProcessConfig &config,
                                                      float &dose)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, refimg;

    float tmpdose = 0.0f;

    if (fname.empty() && N!=0)
        throw ImagingException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::string fmask=fname;

    std::string filename,ext;
    ImageReader reader;
    size_t found;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogDebug,msg.str());

        //TODO: handle NeXus in the future
//        found = fmask.find("hdf");
//         if (found==std::string::npos ) {

            kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');

            msg.str(""); msg<<"Reading images with name: " << filename << " and roi: "<< roi[0] <<
                              " " << roi[1] << " " << roi[2] << " " << roi[3];
            logger(kipl::logging::Logger::LogDebug,msg.str());

            img = reader.Read(filename,
                               kipl::base::ImageFlipNone,
                              kipl::base::ImageRotateNone,
                              1.0f,
                              roi);

            msg.str(""); msg<<"Reading dose";
            logger(kipl::logging::Logger::LogDebug,msg.str());
            tmpdose=bUseNormROI ? reader.GetProjectionDose(filename,
                                                           kipl::base::ImageFlipNone,
                                                          kipl::base::ImageRotateNone,
                                                          1.0f,
                                                          nOriginalNormRegion) : initialDose;
//         }
//        else {
//            img = reader.ReadNexus(fmask, firstIndex,
//                                   kipl::base::ImageFlipNone,
//                                  kipl::base::ImageRotateNone,
//                                  1.0f,
//                    roi);


//            tmpdose=bUseNormROI ? reader.GetProjectionDoseNexus(fmask, firstIndex,
//                                                                kipl::base::ImageFlipNone,
//                                                               kipl::base::ImageRotateNone,
//                                                               1.0f,
//                        nOriginalNormRegion) : initialDose;

//        }

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;


        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

//            if (found==std::string::npos ) {

                img=reader.Read(filename,
                                kipl::base::ImageFlipNone,
                               kipl::base::ImageRotateNone,
                               1.0f,
                        roi);
                memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                tmpdose = bUseNormROI ? reader.GetProjectionDose(filename,
                                                                 kipl::base::ImageFlipNone,
                                                                kipl::base::ImageRotateNone,
                                                                1.0f,
                            nOriginalNormRegion) : initialDose;
//            }
//            else{
//                    img=reader.ReadNexus(fmask,i+firstIndex,
//                                         kipl::base::ImageFlipNone,
//                                        kipl::base::ImageRotateNone,
//                                        1.0f,
//                            roi);
//                    memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

//                    tmpdose = bUseNormROI ? reader.GetProjectionDoseNexus(fmask,i+firstIndex,
//                                                                          kipl::base::ImageFlipNone,
//                                                                         kipl::base::ImageRotateNone,
//                                                                         0.0f,
//                                nOriginalNormRegion) : initialDose;
//            }

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;

        }


        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogDebug,msg.str());

        float *tempdata=new float[N];
        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;

        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);

        delete [] tempdata;


//        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
//             float *pFlat=refimg.GetDataPtr();
//            for (size_t i=1; i<refimg.Size(1); i++) {
//                memcpy(refimg.GetLinePtr(i), pFlat, sizeof(float)*refimg.Size(0));

//            }
//        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Reference image count is zero");

    msg.str(""); msg<<"Loaded reference image (dose="<<dose<<")";
    logger(kipl::logging::Logger::LogDebug,msg.str());
    return refimg;
}


kipl::base::TImage<float,2> BBLogNorm::BBLoader(std::string fname,
                                                      int firstIndex, int N,
                                                      float initialDose,
                                                      float doseBias,
                                                      float &dose)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, refimg;

    float tmpdose = 0.0f;

    if (fname.empty() && N!=0)
        throw ImagingException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::string fmask=fname;

    std::string filename,ext;
    ImageReader reader;
    size_t found;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogDebug,msg.str());

        found = fmask.find("hdf");

//        if (found==std::string::npos ) {
            kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
            img = reader.Read(filename,
                              kipl::base::ImageFlipNone,
                             kipl::base::ImageRotateNone,
                             1.0f,
                    BBroi);

            tmpdose=bUseNormROIBB ? reader.GetProjectionDose(filename,
                                                             kipl::base::ImageFlipNone,
                                                            kipl::base::ImageRotateNone,
                                                            1.0f,
                        doseBBroi) : initialDose;
//        }
//        else{
//                img=reader.ReadNexus(fmask,firstIndex,
//                        m_Config.ProjectionInfo.eFlip,
//                        m_Config.ProjectionInfo.eRotate,
//                        m_Config.ProjectionInfo.fBinning,
//                        BBroi);

//                tmpdose = bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,firstIndex,
//                            config.ProjectionInfo.eFlip,
//                            config.ProjectionInfo.eRotate,
//                            config.ProjectionInfo.fBinning,
//                            doseBBroi) : initialDose;
//        }

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;


        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {

//            if (found==std::string::npos ) {
                kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

                img=reader.Read(filename,
                                kipl::base::ImageFlipNone,
                               kipl::base::ImageRotateNone,
                               1.0f,
                        BBroi);
                memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                tmpdose = bUseNormROIBB ? reader.GetProjectionDose(filename,
                                                                   kipl::base::ImageFlipNone,
                                                                  kipl::base::ImageRotateNone,
                                                                  1.0f,
                            doseBBroi) : initialDose;
//            }
//            else{
//                        img=reader.ReadNexus(fmask,i+firstIndex,
//                                m_Config.ProjectionInfo.eFlip,
//                                m_Config.ProjectionInfo.eRotate,
//                                m_Config.ProjectionInfo.fBinning,
//                                BBroi);
//                        memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

//                        tmpdose = bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,i+firstIndex,
//                                    config.ProjectionInfo.eFlip,
//                                    config.ProjectionInfo.eRotate,
//                                    config.ProjectionInfo.fBinning,
//                                    doseBBroi) : initialDose;
//                }

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;
        }


        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogDebug,msg.str());

        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;


        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);


//        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
//             float *pFlat=refimg.GetDataPtr();
//            for (size_t i=1; i<refimg.Size(1); i++) {
//                memcpy(refimg.GetLinePtr(i), pFlat, sizeof(float)*refimg.Size(0));

//            }
//        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Reference image count is zero");

    msg.str(""); msg<<"Loaded reference image (dose="<<dose<<")";
    logger(kipl::logging::Logger::LogDebug,msg.str());
    return refimg;
}

float BBLogNorm::DoseBBLoader(std::string fname,
                             int firstIndex,
                             float initialDose,
                             float doseBias)
{

    std::string fmask=fname;
    float tmpdose = 0.0f;
    float dose;

    std::string filename,ext;
    ImageReader reader;
    size_t found;

    found=fname.find("hdf");

//    if (found!=std::string::npos)
//    {
//        tmpdose=bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,firstIndex,
//                                                              kipl::base::ImageFlipNone,
//                                                             kipl::base::ImageRotateNone,
//                                                             0.0f,
//                    doseBBroi) : initialDose;
//    }
//    else
//    {
        kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
        tmpdose=bUseNormROIBB ? reader.GetProjectionDose(filename,
                                                         kipl::base::ImageFlipNone,
                                                        kipl::base::ImageRotateNone,
                                                        1.0f,
                                                        doseBBroi) : initialDose;
//    }

    tmpdose   = tmpdose - doseBias;
    dose      = tmpdose;

    return dose;

}

kipl::base::TImage <float,2> BBLogNorm::BBExternalLoader(std::string fname, size_t *roi, float &dose)
{


    kipl::base::TImage<float,2> img;

    if (fname.empty())
        throw ImagingException("The reference image file name mask is empty",__FILE__,__LINE__);

    ImageReader reader;

        img = reader.Read(fname,
                          kipl::base::ImageFlipNone,
                         kipl::base::ImageRotateNone,
                         1.0f,
                roi);

        dose = bUseNormROI ? reader.GetProjectionDose(fname,
                                                      kipl::base::ImageFlipNone,
                                                     kipl::base::ImageRotateNone,
                                                     1.0f,
                    nOriginalNormRegion) : 0.0f;


        return img;

}

kipl::base::TImage <float,3> BBLogNorm::BBExternalLoader(std::string fname, int N, size_t *roi, int firstIndex,float *doselist){


    kipl::base::TImage <float, 2> tempimg;
    kipl::base::TImage<float, 3> img;



    if (fname.empty() && N!=0)
        throw ImagingException("The reference image file name mask is empty",__FILE__,__LINE__);

    float *mylist = new float[N];

    std::string fmask=fname;

    std::string filename,ext;
    ImageReader reader;

    if (N!=0) {


        for (int i=0; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');
//            std::cout << filename << std::endl;

            tempimg=reader.Read(filename,
                                kipl::base::ImageFlipNone,
                               kipl::base::ImageRotateNone,
                               1.0f,
                    roi);

            if (i==0){
                size_t dims[]={tempimg.Size(0), tempimg.Size(1),static_cast<size_t>(N)};
                img.Resize(dims);
            }


            mylist[i] = bUseNormROI ? reader.GetProjectionDose(filename,
                                                               kipl::base::ImageFlipNone,
                                                              kipl::base::ImageRotateNone,
                                                              1.0f,
                        nOriginalNormRegion) : 0.0f;


            memcpy(img.GetLinePtr(0,i),tempimg.GetDataPtr(),tempimg.Size()*sizeof(float));

        }
        memcpy(doselist, mylist, sizeof(float)*N);

//        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
//             float *pFlat=img.GetDataPtr();
//            for (size_t i=1; i<img.Size(1); i++) {
//                memcpy(img.GetLinePtr(i), pFlat, sizeof(float)*img.Size(0));

//            } // not sure what is this
//        }


    }


     delete [] mylist;


    return img;

}

