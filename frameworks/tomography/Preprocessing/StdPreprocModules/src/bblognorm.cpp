//<LICENSE>
#include <strings/miscstring.h>

#include <ReconException.h>
#include <ImagingException.h>
#include <ModuleException.h>
#include <ProjectionReader.h>
#include <ReconConfig.h>
#include <math/image_statistics.h>
#include <math/median.h>
#include <strings/filenames.h>
#include <analyzefileext.h>

#include <ParameterHandling.h>

#include "../include/bblognorm.h"


BBLogNorm::BBLogNorm(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("BBLogNorm", interactor),
    // to check which one do i need: to be removed: m_nWindow and bUseWeightedMean
    m_Config(""),
    blackbodyexternalmaskname("none"),
    nBBextCount(1),
    nBBextFirstIndex(0),
    nOBCount(0),
    nOBFirstIndex(1),
    nDCCount(0),
    nDCFirstIndex(1),
    nBBCount(0),
    nBBFirstIndex(1),
    nBBSampleCount(0),
    nBBSampleFirstIndex(1),
    fFlatDose(1.0f),
    fDarkDose(0.0f),
    fBlackDose(1.0f),
    fdarkBBdose(0.0f),
    fFlatBBdose(1.0f),
    tau(0.97f),
    thresh(0),
    bUseNormROI(true),
    bUseLUT(false),
    bUseWeightedMean(false),
    bUseBB(false),
    bUseExternalBB(false),
    bUseNormROIBB(false),
    bSameMask(true),
    bUseManualThresh(false),
    bPBvariante(true),
    bExtSingleFile(true),
    nNormRegion(4,0UL),
    nOriginalNormRegion(4,0UL),
    BBroi(4,0UL),
    doseBBroi(4,0UL),
    radius(2),
    min_area(20),
    ferror(0.0f),
    ffirstAngle(0.0f),
    flastAngle(360.0f),
    m_nWindow(5),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    m_xInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x),
    m_yInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y),
    m_InterpMethod(ImagingAlgorithms::ReferenceImageCorrection::Polynomial),
    m_maskCreationMethod(ImagingAlgorithms::ReferenceImageCorrection::otsuMask)
{
    blackbodyname = "./";
    blackbodysamplename = "./";
    blackbodyexternalname = "./";
    blackbodysampleexternalname = "./";

    publications.push_back(Publication({"C. Carminati","P. Boillat","F. Schmid",
                                        "P. Vontobel","J. Hovind","M. Morgano",
                                        "M. Raventos","M. Siegwart","D. Mannes",
                                        "C. Gruenzweig","P. Trtik","E. Lehmann",
                                        "M. Strobl","A. Kaestner"},
                                       "Implementation and assessment of the black body bias correction in quantitative neutron imaging",
                                       "PLOS ONE",
                                       2019,
                                       14,
                                       1,
                                       "e0210300",
                                       "10.1371/journal.pone.0210300"
                                       ));

    publications.push_back(Publication({"P. Boillat","C. Carminati","F. Schmid",
                                        "C. Gruenzweig","J. Hovind","A. Kaestner",
                                        "D. Mannes","M. Morgano","M. Siegwart",
                                        "P. Trtik","P. Vontobel","E.H. Lehmann"},
                                     "Chasing quantitative biases in neutron imaging with scintillator-camera detectors: a practical method with black body grids",
                                     "Optics Express",
                                     2018,
                                     26,
                                     12,
                                     "15769",
                                     "10.1364/oe.26.015769"
                               ));
}

BBLogNorm::~BBLogNorm()
{

}

bool BBLogNorm::updateStatus(float val, string msg)
{

    if (m_Interactor!=nullptr)
    {
        return m_Interactor->SetProgress(val,msg);
    }
    return false;

}



int BBLogNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{

    m_Config      = config;
    path          = config.ProjectionInfo.sReferencePath;
    flatname      = config.ProjectionInfo.sOBFileMask;
    darkname      = config.ProjectionInfo.sDCFileMask;

    nOBCount      = config.ProjectionInfo.nOBCount;
    nOBFirstIndex = config.ProjectionInfo.nOBFirstIndex;

    nDCCount      = config.ProjectionInfo.nDCCount;
    nDCFirstIndex = config.ProjectionInfo.nDCFirstIndex;


    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters, "avgmethod"),           m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters, "refmethod"),           m_ReferenceMethod);
    string2enum(GetStringParameter(parameters, "BBOption"),            m_BBOptions);
    string2enum(GetStringParameter(parameters, "X_InterpOrder"),       m_xInterpOrder);
    string2enum(GetStringParameter(parameters, "Y_InterpOrder"),       m_yInterpOrder);
    string2enum(GetStringParameter(parameters, "InterpolationMethod"), m_InterpMethod);
    string2enum(GetStringParameter(parameters, "MaskCreationMethod"),  m_maskCreationMethod);
    bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));


    blackbodyname       = GetStringParameter(parameters, "BB_OB_name");
    nBBCount            = GetIntParameter(parameters,    "BB_counts");
    nBBFirstIndex       = GetIntParameter(parameters,    "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters, "BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters,    "BB_sample_firstindex");
    nBBSampleCount      = GetIntParameter(parameters,    "BB_samplecounts");

    blackbodyexternalname       = GetStringParameter(parameters, "BB_OB_ext_name");
    blackbodysampleexternalname = GetStringParameter(parameters, "BB_sample_ext_name");
    blackbodyexternalmaskname   = GetStringParameter(parameters, "BB_mask_ext_name");
    nBBextCount                 = GetIntParameter(parameters,    "BB_ext_samplecounts");
    nBBextFirstIndex            = GetIntParameter(parameters,    "BB_ext_firstindex");

    tau              = GetFloatParameter(parameters, "tau");
    radius           = GetIntParameter(parameters,   "radius");
    min_area         = GetIntParameter(parameters,   "min_area");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
    ffirstAngle      = GetFloatParameter(parameters, "firstAngle");
    flastAngle       = GetFloatParameter(parameters, "lastAngle");
    bSameMask        = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));
    bExtSingleFile   = kipl::strings::string2bool(GetStringParameter(parameters, "singleBBext"));
    thresh           = GetFloatParameter(parameters,"thresh");

    m_corrector.SaveBG(false, blackbodyname, blackbodyname, blackbodyname); // fake names
    m_corrector.SetManualThreshold(thresh);


    nOriginalNormRegion = config.ProjectionInfo.dose_roi;

    size_t roi_bb_x= BBroi[2]-BBroi[0];
    size_t roi_bb_y = BBroi[3]-BBroi[1];

    // do i need this here?
    if (roi_bb_x>0 && roi_bb_y>0)
    {}
    else
    {
        BBroi = m_Config.ProjectionInfo.projection_roi;  // use the same as projections in case.. if i don't I got an Exception
    }

    //check on dose BB roi size
    if ((doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0){
        bUseNormROIBB=false;
    }
    else {
        bUseNormROIBB = true;
    }

    if ((m_Config.ProjectionInfo.dose_roi[2]-m_Config.ProjectionInfo.dose_roi[0])<=0 || (m_Config.ProjectionInfo.dose_roi[3]-m_Config.ProjectionInfo.dose_roi[1])<=0 ){
        bUseNormROI=false;
        throw ReconException("No roi is selected for dose correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else{
        bUseNormROI=true;
    }

    if (enum2string(m_ReferenceMethod)=="LogNorm"){
        m_corrector.SetComputeMinusLog(true);
    }
    else {
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
    default: throw ReconException("Unknown BBOption method in BBLogNorm::Configure",__FILE__,__LINE__);

    }

    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
            PrepareBBData();
    }


    return 1;
}

int BBLogNorm::ConfigureDLG(ReconConfig config, std::map<std::string, std::string> parameters)
{

    m_Config    = config;
    path        = config.ProjectionInfo.sReferencePath;
    flatname    = config.ProjectionInfo.sOBFileMask;
    darkname    = config.ProjectionInfo.sDCFileMask;

    nOBCount      = config.ProjectionInfo.nOBCount;
    nOBFirstIndex = config.ProjectionInfo.nOBFirstIndex;

    nDCCount      = config.ProjectionInfo.nDCCount;
    nDCFirstIndex = config.ProjectionInfo.nDCFirstIndex;


    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"avgmethod"),           m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"),           m_ReferenceMethod);
    string2enum(GetStringParameter(parameters,"BBOption"),            m_BBOptions);
    string2enum(GetStringParameter(parameters, "X_InterpOrder"),      m_xInterpOrder);
    string2enum(GetStringParameter(parameters, "Y_InterpOrder"),      m_yInterpOrder);
    string2enum(GetStringParameter(parameters,"InterpolationMethod"), m_InterpMethod);
    bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));


    blackbodyname       = GetStringParameter(parameters, "BB_OB_name");
    nBBCount            = GetIntParameter(parameters,    "BB_counts");
    nBBFirstIndex       = GetIntParameter(parameters,    "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters, "BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters,    "BB_sample_firstindex");
    nBBSampleCount      = GetIntParameter(parameters,    "BB_samplecounts");

    blackbodyexternalname       = GetStringParameter(parameters, "BB_OB_ext_name");
    blackbodysampleexternalname = GetStringParameter(parameters, "BB_sample_ext_name");
    nBBextCount                 = GetIntParameter(parameters,    "BB_ext_samplecounts");
    nBBextFirstIndex            = GetIntParameter(parameters,    "BB_ext_firstindex");

    tau              = GetFloatParameter(parameters, "tau");
    radius           = GetIntParameter(parameters,   "radius");
    min_area         = GetIntParameter(parameters,   "min_area");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
    ffirstAngle      = GetFloatParameter(parameters, "firstAngle");
    flastAngle       = GetFloatParameter(parameters, "lastAngle");
    bSameMask        = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));
    thresh           = GetFloatParameter(parameters, "thresh");
    bExtSingleFile   = kipl::strings::string2bool(GetStringParameter(parameters, "singleBBext"));

    m_corrector.SetManualThreshold(thresh);

    nOriginalNormRegion = config.ProjectionInfo.dose_roi;

    size_t roi_bb_x= BBroi[2]-BBroi[0];
    size_t roi_bb_y = BBroi[3]-BBroi[1];

    // do i need this here?
    if (roi_bb_x>0 && roi_bb_y>0) {}
    else {
        BBroi = m_Config.ProjectionInfo.projection_roi;
    }

    //check on dose BB roi size
    if ( (doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0 )
    {
        bUseNormROIBB = false;
    }
    else  {
        bUseNormROIBB = true;
    }

    if (    (m_Config.ProjectionInfo.dose_roi[2]-m_Config.ProjectionInfo.dose_roi[0])<=0
         || (m_Config.ProjectionInfo.dose_roi[3]-m_Config.ProjectionInfo.dose_roi[1])<=0 )
    {
        bUseNormROI=false;
        throw ReconException("No roi is selected for dose correction. This is necessary for accurate BB referencing",__FILE__, __LINE__);
    }
    else
    {
        bUseNormROI=true;
    }

    if (enum2string(m_ReferenceMethod)=="LogNorm")
    {
        m_corrector.SetComputeMinusLog(true);
    }
    else
    {
        m_corrector.SetComputeMinusLog(false);
    }

    switch (m_BBOptions)
    {
    case (ImagingAlgorithms::ReferenceImageCorrection::noBB):
    {
        bUseBB         = false;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate):
    {
        bUseBB         = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::Average):
    {
        bUseBB         = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne):
    {
        bUseBB         = true;
        bUseExternalBB = false;
        break;
    }
    case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB):
    {
        bUseBB         = false; // to evaluate
        bUseExternalBB = true;
        break;
    }
    default: throw ReconException("Unknown BBOption method in BBLogNorm::Configure",__FILE__,__LINE__);

    }

    return 1;
}

bool BBLogNorm::SetROI(const std::vector<size_t> &roi) {

    std::stringstream msg;
    msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger.message(msg.str());

    LoadReferenceImages(roi);
    nNormRegion = nOriginalNormRegion; //nNormRegion seems not used
    return true;
}


std::map<std::string, std::string> BBLogNorm::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["window"]               = kipl::strings::value2string(m_nWindow);
    parameters["avgmethod"]            = enum2string(m_ReferenceAverageMethod);
    parameters["refmethod"]            = enum2string(m_ReferenceMethod);
    parameters["BB_OB_name"]           = blackbodyname;
    parameters["BB_counts"]            = kipl::strings::value2string(nBBCount);
    parameters["BB_first_index"]       = kipl::strings::value2string(nBBFirstIndex);
    parameters["BB_samplename"]        = blackbodysamplename;
    parameters["BB_samplecounts"]      = kipl::strings::value2string(nBBSampleCount);
    parameters["BB_sample_firstindex"] = kipl::strings::value2string(nBBSampleFirstIndex);
    parameters["tau"]                  = kipl::strings::value2string(tau);
    parameters["radius"]               = kipl::strings::value2string(radius);
    parameters["BBroi"]                = kipl::strings::value2string(BBroi[0])+" "+kipl::strings::value2string(BBroi[1])+" "+kipl::strings::value2string(BBroi[2])+" "+kipl::strings::value2string(BBroi[3]);
    parameters["doseBBroi"]            = kipl::strings::value2string(doseBBroi[0])+" "+kipl::strings::value2string(doseBBroi[1])+" "+kipl::strings::value2string(doseBBroi[2])+" "+kipl::strings::value2string(doseBBroi[3]);
    parameters["PBvariante"]           = kipl::strings::bool2string(bPBvariante);
    parameters["BBOption"]             = enum2string(m_BBOptions);
    parameters["firstAngle"]           = kipl::strings::value2string(ffirstAngle);
    parameters["lastAngle"]            = kipl::strings::value2string(flastAngle);
    parameters["X_InterpOrder"]        = enum2string(m_xInterpOrder);
    parameters["Y_InterpOrder"]        = enum2string(m_yInterpOrder);
    parameters["InterpolationMethod"]  = enum2string(m_InterpMethod);
    parameters["BB_OB_ext_name"]       = blackbodyexternalname;
    parameters["BB_sample_ext_name"]   = blackbodysampleexternalname;
    parameters["BB_mask_ext_name"]     = blackbodyexternalmaskname;
    parameters["MaskCreationMethod"]   = enum2string(m_maskCreationMethod);
    parameters["BB_ext_samplecounts"]  = kipl::strings::value2string(nBBextCount);
    parameters["BB_ext_firstindex"]    = kipl::strings::value2string(nBBextFirstIndex);
    parameters["SameMask"]             = kipl::strings::bool2string(bSameMask);
    parameters["min_area"]             = kipl::strings::value2string(min_area);
    // parameters["ManualThreshold"]      = kipl::strings::bool2string(bUseManualThresh);
    parameters["thresh"]               = kipl::strings::value2string(thresh);
    parameters["singleBBext"]          = kipl::strings::bool2string(bExtSingleFile);

    return parameters;
}

void BBLogNorm::LoadReferenceImages(const std::vector<size_t> &roi)
{

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);


    kipl::base::TImage<float,2> myflat, mydark;

    fDarkDose=0.0f;
    fFlatDose=1.0f;
    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;

    mydark = ReferenceLoader(darkmask,
                             m_Config.ProjectionInfo.nDCFirstIndex,
                             m_Config.ProjectionInfo.nDCCount,
                             roi,
                             0.0f,0.0f,
                             m_Config,fDarkDose);
    myflat = ReferenceLoader(flatmask,
                             m_Config.ProjectionInfo.nOBFirstIndex,
                             m_Config.ProjectionInfo.nOBCount,
                             roi,
                             1.0f,0.0f,
                             m_Config,fFlatDose); // i don't use the bias.. beacuse i think i use it later on

    SetReferenceImages(mydark,myflat);


    if (bUseExternalBB && nBBextCount!=0)
    {
        try
        {
             LoadExternalBBData(roi); // they must be ready for SetReferenceImages

        } catch (...)
        {
            throw ReconException("Error while loading external BB data", __FILE__, __LINE__);

        }

    }

     m_corrector.SetReferenceImages(&mflat,
                                    &mdark,
                                    (bUseBB && nBBCount!=0 && nBBSampleCount!=0),
                                    (bUseExternalBB && nBBextCount!=0),
                                    bExtSingleFile,
                                    fFlatDose,
                                    fDarkDose,
                                    (bUseNormROIBB && bUseNormROI),
                                    roi,
                                    m_Config.ProjectionInfo.dose_roi);

}

void BBLogNorm::LoadExternalBBData(const std::vector<size_t> &roi)
{

    if (blackbodyexternalname.empty())
        throw ReconException("The pre-processed open beam with BB image mask is empty", __FILE__, __LINE__);
    if (blackbodysampleexternalname.empty() || nBBextCount==0)
        throw ReconException("The pre-processed sample with BB image mask is empty", __FILE__, __LINE__);

    kipl::base::TImage<float,2> bb_ext;

    float dose;

    bb_ext = BBExternalLoader(blackbodyexternalname, m_Config, roi, dose);
//    kipl::io::WriteTIFF32(bb_ext,"bb_ext.tif");


    if (bExtSingleFile)
    {
        kipl::base::TImage<float,2> bb_sample_ext;
        float dose_s;
        bb_sample_ext = BBExternalLoader(blackbodysampleexternalname, m_Config, roi, dose_s);
        m_corrector.SetExternalBBimages(bb_ext, bb_sample_ext, dose, dose_s);
//        kipl::io::WriteTIFF32(bb_ext,"bb_ext.tif");
    }
    else
    {
        kipl::base::TImage<float,3> bb_sample_ext;
        std::vector<float> doselist(nBBextCount,0);
        bb_sample_ext = BBExternalLoader(blackbodysampleexternalname, nBBextCount, roi, nBBextFirstIndex, m_Config, doselist);
        m_corrector.SetExternalBBimages(bb_ext, bb_sample_ext, dose, doselist);
    }

}

void BBLogNorm::PrepareBBData()
{
    logger.message("PrepareBBData begin");

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);
    if (blackbodysamplename.empty() && nBBSampleCount!=0)
        throw ReconException("The sample image with BBs image mask is empty", __FILE__,__LINE__);


    std::vector<int> diffroi(BBroi.begin(),BBroi.end());

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

    size_t nProj=(m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex+1)/m_Config.ProjectionInfo.nProjectionStep;

    switch (m_InterpMethod)
    {
    case (ImagingAlgorithms::ReferenceImageCorrection::Polynomial):
    {
        PreparePolynomialInterpolationParameters();

        if (nBBCount!=0 && nBBSampleCount!=0)
        {
             m_corrector.SetInterpParameters(ob_bb_param, sample_bb_param,nBBSampleCount, nProj, m_BBOptions);
        }

        break;
    }
    case(ImagingAlgorithms::ReferenceImageCorrection::ThinPlateSplines):
    {
        logger(kipl::logging::Logger::LogMessage,"ThinPlateSplines");

        int nBBs = PrepareSplinesInterpolationParameters();

        logger(kipl::logging::Logger::LogMessage,"After PrepareSplinesInterpolationParameters ");

        if (nBBCount!=0 && nBBSampleCount!=0)
        {
            m_corrector.SetSplinesParameters(ob_bb_param, sample_bb_param, nBBSampleCount, nProj, m_BBOptions, nBBs); // i also need the coordinates.
        }
        break;
    }
    default: throw ReconException("Unknown m_InterpMethod in BBLogNorm::PrepareBBData()", __FILE__, __LINE__);
    }

    logger.message("PrepareBBData end");
}

void BBLogNorm::PreparePolynomialInterpolationParameters()
{
    logger.message("PreparePolynomialInterpolationParameters begin");
    kipl::base::TImage<float,2> flat, dark, bb, sample, samplebb;

    std::string flatmask = path+flatname;
    std::string darkmask = path+darkname;


    fdarkBBdose=0.0f;
    fFlatBBdose=1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,fdarkBBdose);
    flat = BBLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,fFlatBBdose); // to check if i have to use dosebias to remove the fdarkBBdose

    // now load OB image with BBs
    std::vector<float> bb_ob_param;
    std::vector<float> bb_sample_parameters;

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,fdarkBBdose,m_Config,fBlackDose); // this is for mask computation and dose correction (small roi)

    kipl::base::TImage<float,2> obmask(bb.dims());
    std::ostringstream msg;

    try
    {
        bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, ferror);
    }
    catch (ModuleException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(ReconException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(kipl::base::KiplException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch (std::exception & e)
    {
        msg.str("");
        msg<<"Failed to compute bb_ob_parameters with STL exception. Try to change thresholding method or value. "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }


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

    ob_bb_param=bb_ob_param;
//    = new float[6];
//    memcpy(ob_bb_param, bb_ob_param, sizeof(float)*6);



    // load sample images with BBs and sample images

    std::vector<float> temp_parameters;
    size_t nProj = (m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex+1)/m_Config.ProjectionInfo.nProjectionStep;
    size_t step  = (nProj)/(nBBSampleCount);

    float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
    m_corrector.SetAngles(angles, nProj, nBBSampleCount);


    std::vector<float> doselist(nProj);
    for (size_t i=0; i<nProj; i++)
    {
        doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
    }

    m_corrector.SetDoseList(doselist);

// here Exceptions need to be added to veirfy if the selected module is compatible with the number of loaded images
         switch (m_BBOptions)
         {

         case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate):
         {
//                 bb_sample_parameters = new float[6*nBBSampleCount];
             sample_bb_param.resize(6*nBBSampleCount);// = new float[6*nBBSampleCount];
             temp_parameters.resize(6);

             if (nBBSampleCount!=0)
             {

                 logger.message("Loading sample images with BB");

                 for (size_t i=0; i<nBBSampleCount; i++) {
                     samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);

                     float dosesample; // used for the correct dose roi computation (doseBBroi)
                     float current_dose; // current dose for sample with BBs
                     int index;

                     index = GetnProjwithAngle((angles[2]+(angles[3]-angles[2])/(nBBSampleCount-1)*i));

                     // in the first case compute the mask again (should not be necessary in well acquired datasets) however this requires that the first image (or at least one) of sample image with BB has the same angle of the sample image without BB
                     if (i==0)
                     {
                         if (bSameMask)
                         {
                            mMaskBB = obmask;
                            temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);
                         }
                         else
                         {
                             sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample);
                             kipl::base::TImage<float,2> mask(sample.dims());
                             mask = 0.0f;
                             temp_parameters = m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                             mMaskBB = mask; // or memcpy
                         }



                         if (bUseNormROIBB && bUseNormROI)
                         {
         //                     prenormalize interpolation parameters with dose

                             current_dose = fBlackDoseSample;

                             if (bPBvariante)
                             {
                                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                     float mydose = computedose(mybb);
                                     current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                             }

                             current_dose = current_dose<1 ? 1.0f : current_dose;

                             for(size_t j=0; j<6; j++)
                             {
                                       temp_parameters[j]/=(current_dose);
                             }
                         }
//                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);
                         bb_sample_parameters = temp_parameters;
                     }

                     else
                     {
                         temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                         current_dose = fBlackDoseSample;

                         if (bPBvariante)
                         {
                             kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                             float mydose = computedose(mybb);
                             current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                          }

                         current_dose = current_dose<1 ? 1.0f : current_dose;

                         for(size_t j=0; j<6; j++)
                         {
                          temp_parameters[j]/=(current_dose);
                         }
//                             memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);
                         std::copy_n(temp_parameters.begin(),6,bb_sample_parameters.begin()+i*6);

                     }



             }
             }

//                 memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*6*nBBSampleCount);
             std::copy_n(bb_sample_parameters.begin(),6*nBBSampleCount,sample_bb_param.begin());
             break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::Average):
         {
             bb_sample_parameters.resize(6); // = new float[6];
             sample_bb_param.resize(6);      //      = new float[6];
             temp_parameters.resize(6);      //     = new float[6];

             std::vector<float> mask_parameters(6);

             kipl::base::TImage<float,2> samplebb_temp;
             float dose_temp;
             samplebb_temp = BBLoader(blackbodysamplename, nBBSampleFirstIndex,1,1.0f,0.0f, m_Config, dose_temp);
             samplebb      = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);


             float dosesample; // used for the correct dose roi computation (doseBBroi)
             float current_dose;

             int index;
             index  = GetnProjwithAngle(ffirstAngle);
             sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample); // load the projection with angle closest to the first BB sample data


             if (bSameMask)
             {
                mMaskBB = obmask;
             }
             else
             {
                  kipl::base::TImage<float,2> mask(sample.dims());
                  mask = 0.0f;
                  mask_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb_temp, mask); // this is just to compute the mask
                  mMaskBB = mask; // or memcpy
             }

             temp_parameters= m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);

             if (bUseNormROIBB && bUseNormROI)
             {
        //                     prenormalize interpolation parameters with dose

                current_dose = fBlackDoseSample;

                 if (bPBvariante)
                 {
                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                     float mydose = computedose(mybb);
                     current_dose+= ((1.0/tau-1.0)*mydose);
                  }

                 current_dose = current_dose<1 ? 1.0f : current_dose;

                 for(size_t j=0; j<6; j++)
                 {
                           temp_parameters[j]/=current_dose;
                 }
             }

//                     memcpy(sample_bb_param, temp_parameters, sizeof(float)*6);
            std::copy_n(temp_parameters.begin(),6,sample_bb_param.begin());
//                     delete [] mask_parameters;

            break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne):
         {
            bb_sample_parameters.resize(6*nBBSampleCount);// = new float[6*nBBSampleCount];
            sample_bb_param.resize(6*nBBSampleCount); //= new float[6*nBBSampleCount];
            temp_parameters.resize(6);// = new float[6];

            float dosesample;
            float current_dose;

            for (size_t i=0; i<nBBSampleCount; i++)
            {
                samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);
                sample   = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex, 1, 1.0f,fdarkBBdose,m_Config, dosesample); // continuare da qui.. valuatare se devo caricare sample tutte le volte oppure no e nel caso moltiplicare per la doselist in ReferenceImageCorrection come faccio negli altri due casi

                // compute the mask again only for the first sample image, than assume BBs do not move during experiment
                if (i==0)
                {
                    if (bSameMask)
                    {
                       mMaskBB = obmask;
                       temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark, samplebb, mMaskBB);
                    }
                    else
                    {
                         kipl::base::TImage<float,2> mask(sample.dims());
                         mask = 0.0f;
                         temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask); // this is just to compute the mask
                         mMaskBB = mask; // or memcpy
                    }

                    if (bUseNormROIBB && bUseNormROI)
                    {
                        // prenormalize interpolation parameters with dose

                        current_dose= fBlackDoseSample;

                        if (bPBvariante)
                        {
                            kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                            float mydose = computedose(mybb);
                            current_dose += ((1.0/tau-1.0)*mydose);
                        }

                        current_dose = current_dose<1 ? 1.0f : current_dose;

                        for(size_t j=0; j<6; j++)
                        {
                                  temp_parameters[j]/=current_dose;
                        }
                    }

//                        memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);
                }
                else
                {
                    temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                    current_dose= fBlackDoseSample;

                    if (bPBvariante)
                    {
                        kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi); // previously doseBBroi
                        float mydose = computedose(mybb);
                        current_dose+= ((1.0/tau-1.0)*mydose);
                     }

                    current_dose = current_dose<1 ? 1.0f : current_dose;

                    for(size_t j=0; j<6; j++)
                    {
                         temp_parameters[j]/=current_dose;
                    }

//                        memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);

                    std::copy_n(temp_parameters.begin(),6,bb_sample_parameters.begin()+i*6);

                }
            }

//                memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*6*nBBSampleCount);

            std::copy_n(bb_sample_parameters.begin(),6*nBBSampleCount,sample_bb_param.begin());
            break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::noBB):
         {
               throw ReconException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::noBB in PrepareBBdata",__FILE__,__LINE__);
               break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB) :
         {
             throw ReconException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::ExternalBB in PrepareBBdata",__FILE__,__LINE__);
         }

         default: throw ReconException("trying to switch to unknown BBOption in PrepareBBdata",__FILE__,__LINE__);

    }
    logger.message("PreparePolynomialInterpolationParameters end");
}

int BBLogNorm::PrepareSplinesInterpolationParameters() {

    kipl::base::TImage<float,2> flat, dark, bb, sample, samplebb;

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;


    fdarkBBdose=0.0f;
    fFlatBBdose=1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,fdarkBBdose);
    flat = BBLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,fFlatBBdose); //
    bb   = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,fdarkBBdose,m_Config,fBlackDose);

    kipl::base::TImage<float,2> obmask(bb.dims());

    std::vector<float> bb_ob_param;// = new float[100]; // now they are not 6.. attention
    std::vector<float> bb_sample_parameters;
    std::map<std::pair<int, int>, float> values;
    std::map<std::pair<int, int>, float> values_bb;
    std::ostringstream msg;

    try {
        bb_ob_param = m_corrector.PrepareBlackBodyImagewithSplines(flat,dark,bb, obmask, values);
    }
    catch (ModuleException &e) {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(ReconException &e){
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(kipl::base::KiplException &e){
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger(kipl::logging::Logger::LogDebug,msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch (std::exception & e) {
        msg.str("");
        msg<<"Failed to compute bb_ob_parameters with STL exception. Try to change thresholding method or value. "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }



     if (bPBvariante) {
      kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(bb_ob_param,values, doseBBroi);
      float mydose = computedose(mybb);
      fBlackDose = fBlackDose + ((1.0/tau-1.0)*mydose);
     }

     m_corrector.SetSplineObValues(values);

     if(bUseNormROI && bUseNormROIBB)
     {
        fBlackDose = fBlackDose<1 ? 1.0f : fBlackDose;
        for (size_t i=0; i<values.size()+3; i++)
        {
              bb_ob_param[i]= bb_ob_param[i]*(fFlatBBdose-fdarkBBdose);
              bb_ob_param[i]= bb_ob_param[i]/(fBlackDose*tau); // now the dose is already computed for I_OB_BB
        }
     }
//     ob_bb_param = new float[values.size()+3];
//     memcpy(ob_bb_param, bb_ob_param, sizeof(float)*(values.size()+3));

     ob_bb_param = bb_ob_param;


     // load sample images with BBs and sample images

     std::vector<float> temp_parameters;
     size_t nProj=(m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex+1)/m_Config.ProjectionInfo.nProjectionStep;
     size_t step = (nProj)/(nBBSampleCount);

     float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
     m_corrector.SetAngles(angles, nProj, nBBSampleCount);

     std::vector<float> doselist(nProj);
     for (size_t i=0; i<nProj; i++)
     {
         doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); // D(I*n-Idc) in the doseBBroi
     }

     m_corrector.SetDoseList(doselist);

     // here Exceptions need to be added to veirfy if the selected module is compatible with the number of loaded images
              switch (m_BBOptions)
              {

              case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate):
              {
                      bb_sample_parameters.resize((values.size()+3)*nBBSampleCount); // these two are exactly the same..
                      sample_bb_param.resize((values.size()+3)*nBBSampleCount);
                      temp_parameters.resize(values.size()+3);

                      if (nBBSampleCount!=0)
                      {

                          logger.message("Loading sample images with BB");

                          for (size_t i=0; i<nBBSampleCount; i++)
                          {
                              samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);

                              float dosesample; // used for the correct dose roi computation (doseBBroi)
                              float current_dose; // current dose for sample with BBs
                              int index;

                              index = GetnProjwithAngle((angles[2]+(angles[3]-angles[2])/(nBBSampleCount-1)*i));

                              // in the first case compute the mask again (should not be necessary in well acquired datasets) however this requires that the first image (or at least one) of sample image with BB has the same angle of the sample image without BB
                              if (i==0)
                              {

                                  if (bSameMask)
                                  {
                                     mMaskBB = obmask;
                                     values_bb = values;
                                     temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB, values_bb);
                                     m_corrector.SetSplineSampleValues(values_bb);
                                  }
                                  else
                                  {
                                      sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample);
                                      kipl::base::TImage<float,2> mask(sample.dims());
                                      mask = 0.0f;
                                      temp_parameters = m_corrector.PrepareBlackBodyImagewithSplines(sample,dark,samplebb,mask,values_bb);
                                      m_corrector.SetSplineSampleValues(values_bb);

                                      mMaskBB = mask; // or memcpy
                                  }



                                  if (bUseNormROIBB && bUseNormROI)
                                  {
                  //                     prenormalize interpolation parameters with dose

                                      current_dose = fBlackDoseSample;
//                                      std:cout << "normalize dose" << std::endl;

                                      if (bPBvariante)
                                      {
//                                              kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                          kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters, values_bb, doseBBroi);
                                          float mydose = computedose(mybb);
                                          current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                      }

                                      current_dose = current_dose<1 ? 1.0f : current_dose;

                                      for(size_t j=0; j<values_bb.size()+3; j++)
                                      {
                                                temp_parameters[j]/=(current_dose);
                                      }
                                  }

//                                  memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*(values_bb.size()+3));


                                  std::copy_n(temp_parameters.begin(),values_bb.size()+3,bb_sample_parameters.begin());
                              }

                              else {
                                  temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB,values_bb);
//                                  temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                                  current_dose = fBlackDoseSample;

                                  if (bPBvariante)
                                  {
                                      kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImagewithSplines(temp_parameters,values_bb,doseBBroi);
//                                          kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                      float mydose = computedose(mybb);
                                      current_dose = current_dose + ((1.0/tau-1.0)*mydose);
                                   }

                                  current_dose = current_dose<1 ? 1.0f : current_dose;

                                  for(size_t j=0; j<(values_bb.size()+3); j++) {
                                   temp_parameters[j]/=(current_dose);
                                  }
//                                  memcpy(bb_sample_parameters+i*(values_bb.size()+3), temp_parameters, sizeof(float)*(values_bb.size()+3))
                                  std::copy_n(temp_parameters.begin(),values_bb.size()+3,bb_sample_parameters.begin()+i*(values_bb.size()+3));
                              }
                      }
                      }

//                      memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*(values.size()+3)*nBBSampleCount);
                      std::copy_n(bb_sample_parameters.begin(),(values.size()+3)*nBBSampleCount,sample_bb_param.begin());
                      break;
              }

              case (ImagingAlgorithms::ReferenceImageCorrection::Average):
              {


                          bb_sample_parameters.resize(values.size()+3); // i am assuming there is the same number of BBs
                          sample_bb_param.resize(values.size()+3);
                          temp_parameters.resize(values.size()+3);
                          std::vector<float> mask_parameters(values.size()+3);

                          kipl::base::TImage<float,2> samplebb_temp;
                          float dose_temp;
                          samplebb_temp = BBLoader(blackbodysamplename, nBBSampleFirstIndex,1,1.0f,0.0f, m_Config, dose_temp);
                          samplebb      = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);

                          float dosesample; // used for the correct dose roi computation (doseBBroi)
                          float current_dose;

                          int index;
                          index = GetnProjwithAngle(ffirstAngle);
                          sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample); // load the projection with angle closest to the first BB sample data


                          if (bSameMask)
                          {
                             mMaskBB = obmask;
                             values_bb = values;
                             mask_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb, mMaskBB, values_bb);
                          }
                          else
                          {
                               kipl::base::TImage<float,2> mask(sample.dims());
                               mask = 0.0f;
                               mask_parameters = m_corrector.PrepareBlackBodyImagewithSplines(sample,dark, samplebb_temp, mask,values_bb);
//                               mask_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb_temp, mask); // this is just to compute the mask
                               mMaskBB = mask; // or memcpy
                          }

                          m_corrector.SetSplineSampleValues(values_bb);

                          temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB,values_bb);
//                          temp_parameters= m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);


                          if (bUseNormROIBB && bUseNormROI)
                          {
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

                          }

//                          memcpy(sample_bb_param, temp_parameters, sizeof(float)*(values.size()+3));
                          std::copy_n(temp_parameters.begin(),values.size()+3,sample_bb_param.begin());

                     break;
              }

              case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne):
              {

                     bb_sample_parameters.resize((values.size()+3)*nBBSampleCount);
                     sample_bb_param.resize((values.size()+3)*nBBSampleCount);
                     temp_parameters.resize(values.size()+3);

                     float dosesample;
                     float current_dose;

                     for (size_t i=0; i<nBBSampleCount; i++)
                     {
                         samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);
                         sample   = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1, 1.0f,fdarkBBdose,m_Config, dosesample);

                         // compute the mask again only for the first sample image, than assume BBs do not move during experiment
                         if (i==0)
                         {
                             if (bSameMask)
                             {
                                mMaskBB = obmask;
                                values_bb = values;
                                temp_parameters = m_corrector.PrepareBlackBodyImagewithSplinesAndMask(dark,samplebb,mMaskBB, values_bb);
//                                temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark, samplebb, mMaskBB);
                                m_corrector.SetSplineSampleValues(values_bb);
                             }
                             else
                             {
                                  kipl::base::TImage<float,2> mask(sample.dims());
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

//                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*(values.size()+3));


                             std::copy_n(temp_parameters.begin(),values.size()+3,bb_sample_parameters.begin());
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

//                             memcpy(bb_sample_parameters+i*(values.size()+3), temp_parameters, sizeof(float)*(values.size()+3));

                             std::copy_n(temp_parameters.begin(),values.size()+3,bb_sample_parameters.begin()+i*(values.size()+3));
                         }

                         float dose = dosesample/(current_dose*tau);
                     }

//                     memcpy(sample_bb_param, bb_sample_parameters, sizeof(float)*(values.size()+3)*nBBSampleCount);
                     std::copy_n(bb_sample_parameters.begin(),(values.size()+3)*nBBSampleCount,sample_bb_param.begin());
                     break;
                  }

              case (ImagingAlgorithms::ReferenceImageCorrection::noBB): {
                    throw ReconException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::noBB in PrepareBBdata",__FILE__,__LINE__);
                    break;
              }
              case (ImagingAlgorithms::ReferenceImageCorrection::ExternalBB) : {
                  throw ReconException("trying to switch to case ImagingAlgorithms::ReferenceImageCorrection::ExternalBB in PrepareBBdata",__FILE__,__LINE__);
              }

              default: throw ReconException("trying to switch to unknown BBOption in PrepareBBdata",__FILE__,__LINE__);

              }



    return values.size();

}

int BBLogNorm::GetnProjwithAngle(float angle)
{

    // range of projection angles
    double nProj=((double)m_Config.ProjectionInfo.nLastIndex-(double)m_Config.ProjectionInfo.nFirstIndex+1)/(double)m_Config.ProjectionInfo.nProjectionStep;

    int index =0;
    float curr_angle;

    for (size_t i=0; i<nProj; i++)
    {
        curr_angle = m_Config.ProjectionInfo.fScanArc[0]+(m_Config.ProjectionInfo.fScanArc[1]-m_Config.ProjectionInfo.fScanArc[0])/(nProj-1)*i;


        if (curr_angle<=angle+0.5f & curr_angle>=angle-0.5f)
        {
            break;
        }
        else
            index++;
    }

    return index;
}

float BBLogNorm::GetInterpolationError(kipl::base::TImage<float,2> &mask){

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);

    kipl::base::TImage<float,2> flat, dark, bb;

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;



    float darkdose = 0.0f;
    float flatdose = 1.0f;
    float blackdose = 1.0f;


    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,darkdose);
    flat = BBLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,flatdose);



    // now load OB image with BBs

    std::vector<float> bb_ob_param;

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,0.0f,m_Config,blackdose);

    std::vector<int> diffroi(BBroi.begin(),BBroi.end()); // it is now just the BBroi position, makes more sense

    m_corrector.SetRadius(radius);
    m_corrector.SetMinArea(min_area);
    m_corrector.SetInterpolationOrderX(m_xInterpOrder);
    m_corrector.SetInterpolationOrderY(m_yInterpOrder);
    m_corrector.setDiffRoi(diffroi); // left to compute the interpolation parameters in the abssolute image coordinates

    std::stringstream msg;
    msg.str(""); msg<<"Min area set to  "<<min_area;
    logger.message(msg.str());

    float error;
    kipl::base::TImage<float,2> obmask(bb.dims());

    try {
        bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, error);
    }
    catch (ModuleException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger.debug(msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(ReconException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger.debug(msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch(kipl::base::KiplException &e)
    {
        msg.str(""); msg<<"Failed to compute bb_ob_parameters. Try to change thresholding method or value. " << e.what();
        logger.debug(msg.str());
        throw ReconException("Failed to compute bb_ob_parameters. Try to change thresholding method or value. ", __FILE__,__LINE__);
    }
    catch (std::exception & e)
    {
        msg.str("");
        msg<<"Failed to compute bb_ob_parameters with STL exception. Try to change thresholding method or value. "<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    mask = obmask;

    return error;
}

kipl::base::TImage<float,2> BBLogNorm::GetMaskImage()
{
    return mMaskBB;
}

float BBLogNorm::computedose(kipl::base::TImage<float,2>&img)
{

    float *pImg=img.GetDataPtr();
    float *means=new float[img.Size(1)];
    memset(means,0,img.Size(1)*sizeof(float));

    for (size_t y=0; y<img.Size(1); y++)
    {
        pImg=img.GetLinePtr(y);

        for (size_t x=0; x<img.Size(0); x++)
        {
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
    throw ImagingException("BB correction of 2D images is not implemented", __FILE__,__LINE__);
    return 0;
}

int BBLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) {

    int nDose=img.Size(2);
    float *doselist=nullptr;

    std::stringstream msg;

    if (bUseNormROI==true) {
        doselist=new float[nDose];
        GetFloatParameterVector(coeff,"dose",doselist,nDose);
        for (int i=0; i<nDose; i++) {
            doselist[i] = doselist[i]-fDarkDose;
        }
    }
        m_corrector.SetInteractor(m_Interactor);
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

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (int i=1; i<static_cast<int>(flat.Size(1)); i++) {
                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
    }

}


kipl::base::TImage<float,2> BBLogNorm::ReferenceLoader(std::string fname,
                                                      int firstIndex, int N,
                                                      const std::vector<size_t> &roi,
                                                      float initialDose,
                                                      float doseBias,
                                                      ReconConfig &config,
                                                      float &dose)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, refimg;

    float tmpdose = 0.0f;

    if (fname.empty() && N!=0)
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::string fmask=fname;

    std::string filename,ext;
    ProjectionReader reader;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogMessage,msg.str());

        auto exttype = readers::GetFileExtensionType(fmask);
         if (exttype != readers::ExtensionHDF5 )
         {

            kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
            img = reader.Read(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    roi);

            tmpdose=bUseNormROI ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : initialDose;
         }
        else {
            img = reader.ReadNexus(fmask, firstIndex,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    roi);


            tmpdose=bUseNormROI ? reader.GetProjectionDoseNexus(fmask, firstIndex,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : initialDose;

        }

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;


        std::vector<size_t> obdims={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

            if (exttype != readers::ExtensionHDF5 )
            {

                img=reader.Read(filename,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        roi);
                memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                tmpdose = bUseNormROI ? reader.GetProjectionDose(filename,
                            config.ProjectionInfo.eFlip,
                            config.ProjectionInfo.eRotate,
                            config.ProjectionInfo.fBinning,
                            nOriginalNormRegion) : initialDose;
            }
            else{
                    img=reader.ReadNexus(fmask,i+firstIndex,
                            m_Config.ProjectionInfo.eFlip,
                            m_Config.ProjectionInfo.eRotate,
                            m_Config.ProjectionInfo.fBinning,
                            roi);
                    memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                    tmpdose = bUseNormROI ? reader.GetProjectionDoseNexus(fmask,i+firstIndex,
                                config.ProjectionInfo.eFlip,
                                config.ProjectionInfo.eRotate,
                                config.ProjectionInfo.fBinning,
                                nOriginalNormRegion) : initialDose;
            }

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;

        }


        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogMessage,msg.str());

        float *tempdata=new float[N];
        refimg.resize(img.dims());

        ImagingAlgorithms::AverageImage avg;

        refimg = avg(img3D,m_ReferenceAverageMethod);

        delete [] tempdata;


        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
             float *pFlat=refimg.GetDataPtr();
            for (size_t i=1; i<refimg.Size(1); i++) {
                memcpy(refimg.GetLinePtr(i), pFlat, sizeof(float)*refimg.Size(0));

            }
        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Reference image count is zero");

    msg.str(""); msg<<"Loaded reference image (dose="<<dose<<")";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return refimg;
}


kipl::base::TImage<float,2> BBLogNorm::BBLoader(std::string fname,
                                                      int firstIndex, int N,
                                                      float initialDose,
                                                      float doseBias,
                                                      ReconConfig &config,
                                                      float &dose)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, refimg;

    float tmpdose = 0.0f;

    if (fname.empty() && N!=0)
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::string fmask=fname;

    std::string filename,ext;
    ProjectionReader reader;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogMessage,msg.str());

        auto exttype = readers::GetFileExtensionType(fmask);
        if (exttype != readers::ExtensionHDF5 )
        {
            kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
            img = reader.Read(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    BBroi);

            tmpdose=bUseNormROIBB ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        doseBBroi) : initialDose;
        }
        else{
                img=reader.ReadNexus(fmask,firstIndex,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        BBroi);

                tmpdose = bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,firstIndex,
                            config.ProjectionInfo.eFlip,
                            config.ProjectionInfo.eRotate,
                            config.ProjectionInfo.fBinning,
                            doseBBroi) : initialDose;
        }

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;


        std::vector<size_t> obdims={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {

            if (exttype != readers::ExtensionHDF5 )
            {
                kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

                img=reader.Read(filename,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        BBroi);
                memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                tmpdose = bUseNormROIBB ? reader.GetProjectionDose(filename,
                            config.ProjectionInfo.eFlip,
                            config.ProjectionInfo.eRotate,
                            config.ProjectionInfo.fBinning,
                            doseBBroi) : initialDose;
            }
            else
            {
                        img=reader.ReadNexus(fmask,i+firstIndex,
                                m_Config.ProjectionInfo.eFlip,
                                m_Config.ProjectionInfo.eRotate,
                                m_Config.ProjectionInfo.fBinning,
                                BBroi);
                        memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                        tmpdose = bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,i+firstIndex,
                                    config.ProjectionInfo.eFlip,
                                    config.ProjectionInfo.eRotate,
                                    config.ProjectionInfo.fBinning,
                                    doseBBroi) : initialDose;
                }

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;
        }


        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogMessage,msg.str());

        refimg.resize(img.dims());

        ImagingAlgorithms::AverageImage avg;


        refimg = avg(img3D,m_ReferenceAverageMethod);


        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            float *pFlat=refimg.GetDataPtr();
            for (size_t i=1; i<refimg.Size(1); i++)
            {
                memcpy(refimg.GetLinePtr(i), pFlat, sizeof(float)*refimg.Size(0));
            }
        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Reference image count is zero");

    msg.str(""); msg<<"Loaded reference image (dose="<<dose<<")";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return refimg;
}

float BBLogNorm::DoseBBLoader(std::string fname,
                             int firstIndex,
                             float initialDose,
                             float doseBias,
                             ReconConfig &config) {

    std::string fmask=fname;
    float tmpdose = 0.0f;
    float dose;

    std::string filename,ext;
    ProjectionReader reader;

    auto exttype = readers::GetFileExtensionType(fname);
    if (exttype == readers::ExtensionHDF5)
    {
        tmpdose=bUseNormROIBB ? reader.GetProjectionDoseNexus(fmask,firstIndex,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    doseBBroi) : initialDose;
    }
    else
    {
        kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
        tmpdose=bUseNormROIBB ? reader.GetProjectionDose(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    doseBBroi) : initialDose;
    }

    tmpdose   = tmpdose - doseBias;
    dose      = tmpdose;

    return dose;

}

kipl::base::TImage <float,2> BBLogNorm::BBExternalLoader(std::string fname, ReconConfig &config, const std::vector<size_t> &roi, float &dose){


    kipl::base::TImage<float,2> img;

    if (fname.empty())
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    ProjectionReader reader;

        img = reader.Read(fname,
                config.ProjectionInfo.eFlip,
                config.ProjectionInfo.eRotate,
                config.ProjectionInfo.fBinning,
                roi);

        dose = bUseNormROI ? reader.GetProjectionDose(fname,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    nOriginalNormRegion) : 0.0f;


        return img;

}

kipl::base::TImage <float,3> BBLogNorm::BBExternalLoader(std::string fname,
                                                         int N,
                                                         const std::vector<size_t> &roi,
                                                         int firstIndex,
                                                         ReconConfig &config,
                                                         std::vector<float> & doselist)
{
    kipl::base::TImage <float, 2> tempimg;
    kipl::base::TImage<float, 3> img;



    if (fname.empty() && N!=0)
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::vector<float> mylist(N,0);

    std::string fmask=fname;

    std::string filename,ext;
    ProjectionReader reader;

    if (N!=0) {


        for (int i=0; i<N; ++i)
        {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

            tempimg=reader.Read(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    roi);

            if (i==0)
            {
                std::vector<size_t> dims={tempimg.Size(0), tempimg.Size(1),static_cast<size_t>(N)};
                img.resize(dims);
            }

            mylist[i] = bUseNormROI ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : 0.0f;

            memcpy(img.GetLinePtr(0,i),tempimg.GetDataPtr(),tempimg.Size()*sizeof(float));

        }
        doselist = mylist;

        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
             float *pFlat=img.GetDataPtr();
            for (size_t i=1; i<img.Size(1); i++) {
                memcpy(img.GetLinePtr(i), pFlat, sizeof(float)*img.Size(0));

            } // not sure what is this
        }


    }

    return img;

}

