#include <strings/miscstring.h>

#include <ReconException.h>
#include <ProjectionReader.h>
#include <ReconConfig.h>
#include <math/image_statistics.h>
#include <math/median.h>

#include <ParameterHandling.h>

#include "../include/robustlognorm.h"

RobustLogNorm::RobustLogNorm() :
    PreprocModuleBase("RobustLogNorm"),
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
    tau(0.99f),
    bPBvariante(true),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    m_xInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_x),
    m_yInterpOrder(ImagingAlgorithms::ReferenceImageCorrection::SecondOrder_y),
    ferror(0.0f),
    ffirstAngle(0.0f),
    flastAngle(360.0f),
    nBBextCount(0),
    nBBextFirstIndex(0),
    bUseExternalBB(false),
    bSameMask(true)
{
    doseBBroi[0] = doseBBroi[1] = doseBBroi[2] = doseBBroi[3]=0;
    BBroi[0] = BBroi[1] = BBroi[2] = BBroi[3] = 0;
    blackbodyname = "./";
    blackbodysamplename = "./";
    blackbodyexternalname = "./";
    blackbodysampleexternalname = "./";

}

RobustLogNorm::~RobustLogNorm()
{

}


int RobustLogNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
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
    string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
    string2enum(GetStringParameter(parameters,"BBOption"), m_BBOptions);
    string2enum(GetStringParameter(parameters, "X_InterpOrder"), m_xInterpOrder);
    string2enum(GetStringParameter(parameters, "Y_InterpOrder"), m_yInterpOrder);
//    bUseNormROI = kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));

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
//    bUseExternalBB = kipl::strings::string2bool(GetStringParameter(parameters,"useExternalBB"));



    tau = GetFloatParameter(parameters, "tau");
    radius = GetIntParameter(parameters, "radius");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
//    bUseNormROIBB = kipl::strings::string2bool(GetStringParameter(parameters,"useBBnormregion"));
    ffirstAngle = GetFloatParameter(parameters, "firstAngle");
    flastAngle = GetFloatParameter(parameters, "lastAngle");
    bSameMask = kipl::strings::string2bool(GetStringParameter(parameters,"SameMask"));


    memcpy(nOriginalNormRegion,config.ProjectionInfo.dose_roi,4*sizeof(size_t));

    size_t roi_bb_x= BBroi[2]-BBroi[0];
    size_t roi_bb_y = BBroi[3]-BBroi[1];

    // do i need this here?
    if (roi_bb_x>0 && roi_bb_y>0) {
//        std::cout << "have roi for BB!" << std::endl;
    }
    else {
//        std::cout << "does not have roi for BB, copy projection roi" << std::endl;
        memcpy(BBroi, m_Config.ProjectionInfo.projection_roi, sizeof(size_t)*4);  // use the same as projections in case.. if i don't I got an Exception
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




    return 1;
}

bool RobustLogNorm::SetROI(size_t *roi) {
    std::stringstream msg;
    msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    LoadReferenceImages(roi);
    memcpy(nNormRegion,nOriginalNormRegion,4*sizeof(size_t));

    size_t roix=roi[2]-roi[0];
    size_t roiy=roi[3]-roi[1];

    return true;
}


std::map<std::string, std::string> RobustLogNorm::GetParameters() {
    std::map<std::string, std::string> parameters;

    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["avgmethod"] = enum2string(m_ReferenceAverageMethod);
    parameters["refmethod"] = enum2string(m_ReferenceMethod);
//    parameters["usenormregion"] = kipl::strings::bool2string(bUseNormROI);
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
//    parameters["useBBnormregion"] = kipl::strings::bool2string(bUseNormROIBB);
    parameters["PBvariante"] = kipl::strings::bool2string(bPBvariante);
    parameters["BBOption"] = enum2string(m_BBOptions);
    parameters["firstAngle"] = kipl::strings::value2string(ffirstAngle);
    parameters["lastAngle"] = kipl::strings::value2string(flastAngle);
    parameters["X_InterpOrder"] = enum2string(m_xInterpOrder);
    parameters["Y_InterpOrder"] = enum2string(m_yInterpOrder);

    parameters["BB_OB_ext_name"] = blackbodyexternalname;
    parameters["BB_sample_ext_name"] = blackbodysampleexternalname;
    parameters["BB_ext_samplecounts"] = kipl::strings::value2string(nBBextCount);
    parameters["BB_ext_firstindex"] = kipl::strings::value2string(nBBextFirstIndex);
    parameters["SameMask"] = kipl::strings::bool2string(bSameMask);
//    parameters["useExternalBB"] = kipl::strings::bool2string(bUseExternalBB);
//    parameters["useBB"] = kipl::strings::bool2string(bUseBB);

    return parameters;
}

void RobustLogNorm::LoadReferenceImages(size_t *roi)
{

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);


    kipl::base::TImage<float,2> flat, dark;

    fDarkDose=0.0f;
    fFlatDose=1.0f;
    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;

    dark = ReferenceLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,roi,0.0f,0.0f,m_Config,fDarkDose);
    flat = ReferenceLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,roi,1.0f,0.0f,m_Config,fFlatDose); // i don't use the bias.. beacuse i think i use it later on

//    std::cout << "computed doses: " << std::endl;
//    std::cout << fDarkDose << " " << fFlatDose << std::endl;

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
    default: throw ReconException("Unknown BBOption method in RobustLogNorm::Configure",__FILE__,__LINE__);

    }



    if (bUseExternalBB && nBBextCount!=0){
        LoadExternalBBData(roi); // they must be ready for SetReferenceImages
    }

    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
        PrepareBBData();
//        size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};
        size_t subroi[4] = {0,roi[1]-m_Config.ProjectionInfo.projection_roi[1], roi[2]-m_Config.ProjectionInfo.roi[0], roi[3]-m_Config.ProjectionInfo.projection_roi[1]};
        m_corrector.SetBBInterpRoi(subroi);
//        m_corrector.SetBBInterpRoi(roi);
    }


     SetReferenceImages(dark,flat);

}

void RobustLogNorm::LoadExternalBBData(size_t *roi){


    if (blackbodyexternalname.empty())
        throw ReconException("The pre-processed open beam with BB image mask is empty", __FILE__, __LINE__);
    if (blackbodysampleexternalname.empty() || nBBextCount==0)
        throw ReconException("The pre-processed sample with BB image mask is empty", __FILE__, __LINE__);

//    if (nBBextCount!=((m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex-1)/m_Config.ProjectionInfo.nProjectionStep)){
//        std::cout << nBBextCount << std::endl;
//        std::cout << ((m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex-1)/m_Config.ProjectionInfo.nProjectionStep)<< std::endl;

//            throw ReconException("Number of externally processed BB images is not the same as the number of projections", __FILE__, __LINE__);
//}

    kipl::base::TImage<float,2> bb_ext;
    kipl::base::TImage<float,3> bb_sample_ext;
    float dose;
    float *doselist = new float[nBBextCount];

    bb_ext = BBExternalLoader(blackbodyexternalname, m_Config, roi, dose);
//    kipl::io::WriteTIFF32(bb_ext,"bb_ext.tif");
//    std::cout << "dose: " << dose << std::endl;
    bb_sample_ext = BBExternalLoader(blackbodysampleexternalname, nBBextCount, roi, nBBextFirstIndex, m_Config, doselist);

    m_corrector.SetExternalBBimages(bb_ext, bb_sample_ext, dose, doselist);

}

void RobustLogNorm::PrepareBBData(){


//    std::cout << "PrepareBBData begin--" << std::endl;
    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);
    if (blackbodysamplename.empty() && nBBSampleCount!=0)
        throw ReconException("The sample image with BBs image mask is empty", __FILE__,__LINE__);



    int diffroi[4] = {int(BBroi[0]-m_Config.ProjectionInfo.projection_roi[0]), int(BBroi[1]-m_Config.ProjectionInfo.projection_roi[1]), int(BBroi[2]-m_Config.ProjectionInfo.projection_roi[2]), int(BBroi[3]-m_Config.ProjectionInfo.projection_roi[3])};


    m_corrector.setDiffRoi(diffroi);
    m_corrector.SetRadius(radius);
    m_corrector.SetTau(tau);
    m_corrector.SetPBvariante(bPBvariante);
    m_corrector.SetInterpolationOrderX(m_xInterpOrder);
    m_corrector.SetInterpolationOrderY(m_yInterpOrder);


    kipl::base::TImage<float,2> img, flat, dark, bb, sample, samplebb;

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;


    fdarkBBdose=0.0f;
    fFlatBBdose=1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,fdarkBBdose);
    flat = BBLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,fFlatBBdose); // to check if i have to use dosebias to remove the fdarkBBdose

    // now load OB image with BBs

    float *bb_ob_param = new float[6];

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,fdarkBBdose,m_Config,fBlackDose);

    kipl::base::TImage<float,2> obmask(bb.Dims());
    bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, ferror);
//    std::cout << "FIRST COMPUTATION OF INTERPOLATION ERROR TO BE THEN PUT IN THE GUI: " << ferror << std::endl;






    if (bPBvariante) {
     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(bb_ob_param,doseBBroi);
     float mydose = computedose(mybb);
//     std::cout << "----------------before PB variante: " << fBlackDose << std::endl;
     fBlackDose+= ((1/tau-1)*mydose);
//     std::cout << "black dose with PB variante: " << fBlackDose << std::endl;

    }


    if(bUseNormROI && bUseNormROIBB){
     fBlackDose = fBlackDose<1 ? 1.0f : fBlackDose;
     for (size_t i=0; i<6; i++){
         bb_ob_param[i]*=(fFlatBBdose-fDarkDose);
         bb_ob_param[i]/=(fBlackDose*tau); // now the dose is already computed for I_OB_BB
     }
    }

//    std::cout << "blackdose: " << fBlackDose << std::endl;

//             kipl::io::WriteTIFF32(obmask,"/home/carminati_c/repos/testdata/maskOb.tif");



    // load sample images with BBs and sample images

    float *temp_parameters;
    float *bb_sample_parameters;
    size_t nProj=(m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex+1)/m_Config.ProjectionInfo.nProjectionStep;
    size_t step = (nProj)/(nBBSampleCount);


// here Exceptions need to be added to veirfy if the selected module is compatible with the number of loaded images
         switch (m_BBOptions) {

         case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
//                 std::cout << "ciao interpolate" << std::endl;
                 bb_sample_parameters = new float[6*nBBSampleCount];
                 temp_parameters = new float[6];

                 if (nBBSampleCount!=0) {

                     logger(kipl::logging::Logger::LogMessage,"Loading sample images with BB");

                     //     first I pass the neede parameters to ReferenceImageCorrection
                     float angles[4] = {m_Config.ProjectionInfo.fScanArc[0], m_Config.ProjectionInfo.fScanArc[1], ffirstAngle, flastAngle};
                     m_corrector.SetAngles(angles, nProj, nBBSampleCount);

                     float *doselist = new float[nProj];
                     for (size_t i=0; i<int(nProj); i++) {
                         doselist[i] = DoseBBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+i, 1.0f, fdarkBBdose, m_Config); //
                     }

                     m_corrector.SetDoseList(doselist);
                     delete [] doselist;

                     for (size_t i=0; i<nBBSampleCount; i++) {
                         samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);

                         float dosesample; // used for the correct dose roi computation (doseBBroi)
                         float current_dose; // current dose for sample with BBs
                         int index;

                         index = GetnProjwithAngle((angles[2]+(angles[3]-angles[2])/(nBBSampleCount-1)*i));

                         // in the first case compute the mask again (should not be necessary in well acquired datasets) however this requires that the first image (or at least one) of sample image with BB has the same angle of the sample image without BB
                         if (i==0) {

                             if (bSameMask){
                                mMaskBB = obmask;
                                temp_parameters= m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb, mMaskBB);
                             }
                             else {
                                 sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample);
                                 kipl::base::TImage<float,2> mask(sample.Dims());
                                 mask = 0.0f;
                                 temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                                 mMaskBB = mask; // or memcpy
                             }



                             if (bUseNormROIBB && bUseNormROI){
             //                     prenormalize interpolation parameters with dose

                                 current_dose = fBlackDoseSample;

                                 if (bPBvariante) {

                                         kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                         float mydose = computedose(mybb);
//                                         std::cout << "black dose before PB variante: " << fBlackDoseSample << std::endl;
                                         current_dose+= ((1/tau-1)*mydose);
//                                         std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                                      }


                                 current_dose = current_dose<1 ? 1.0f : current_dose;

//                                 std::cout << "black dose with PB variante after value check: " << fBlackDoseSample << std::endl;

                                 for(size_t j=0; j<6; j++) {

                                           temp_parameters[j]/=(current_dose);
//                                           temp_parameters[j]*=(dosesample/tau); // it is now in the m_corrector.SetInterpParameters

                                 }
                             }

                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);

                         }

                         else {

                             temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);

                             current_dose = fBlackDoseSample;

                             // I was missing the correct dose for indexes of i after 1....
                             if (bPBvariante) {

                                     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                     float mydose = computedose(mybb);
                                     current_dose+= ((1/tau-1)*mydose);

                                  }

                             current_dose = current_dose<1 ? 1.0f : current_dose;

                             for(size_t j=0; j<6; j++) {
                              temp_parameters[j]/=(current_dose);
//                              temp_parameters[j]*=(dosesample/tau);
                             }

                             memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);

                         }



                 }
                 }


                 break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::Average): {

                     bb_sample_parameters = new float[6];
                     temp_parameters = new float[6];
                     float * mask_parameters = new float[6];

                     kipl::base::TImage<float,2> samplebb_temp;
                     float dose_temp;
                     samplebb_temp = BBLoader(blackbodysamplename, nBBSampleFirstIndex,1,1.0f,0.0f, m_Config, dose_temp);
                     samplebb = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);


                     float dosesample; // used for the correct dose roi computation (doseBBroi)
                     float current_dose;

                     int index;
                     index = GetnProjwithAngle(ffirstAngle);
                     sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex+index, 1, 1.0f,fdarkBBdose,m_Config, dosesample); // load the projection with angle closest to the first BB sample data


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
                                 current_dose+= ((1/tau-1)*mydose);
//                                 std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                              }


                         current_dose = current_dose<1 ? 1.0f : current_dose;

                         for(size_t j=0; j<6; j++) {

                                   temp_parameters[j]/=current_dose;
                                   temp_parameters[j]*=(dosesample/tau);

                         }
                     }

                     memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);
                     delete [] mask_parameters;

                break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
                bb_sample_parameters = new float[6*nBBSampleCount];
                float dosesample;
                float current_dose;

                for (size_t i=0; i<nBBSampleCount; i++) {


                    samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);
                    sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex, 1, 1.0f,fdarkBBdose,m_Config, dosesample);


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

                                    kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                    float mydose = computedose(mybb);
                                    current_dose+= ((1/tau-1)*mydose);
//                                    std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                                 }


                            current_dose = current_dose<1 ? 1.0f : current_dose;

                            for(size_t j=0; j<6; j++) {

                                      temp_parameters[j]/=current_dose;
                                      temp_parameters[j]*=(dosesample/tau);

                            }
                        }

                        memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6);

                    }

                    else {

                        temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                        current_dose= fBlackDoseSample;

                        if (bPBvariante) {

                                kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                float mydose = computedose(mybb);
                                current_dose+= ((1/tau-1)*mydose);
//                                    std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                             }

                        current_dose = current_dose<1 ? 1.0f : current_dose;


                        for(size_t j=0; j<6; j++) {
                             temp_parameters[j]/=current_dose;
                             temp_parameters[j]*=(dosesample/tau);
                        }

                        memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6);


                    }



            }

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


        if (nBBCount!=0 && nBBSampleCount!=0) {

             m_corrector.SetInterpParameters(bb_ob_param, bb_sample_parameters,nBBSampleCount, nProj, m_BBOptions);
//             m_corrector.SetBBInterpRoi(subroi); // moved in load reference images if bUseBB
        }


//    std::cout << "------- BB doses ------" << std::endl;
//    std::cout << "fdarkdose: " <<fdarkBBdose << "\n blackdose: " <<fBlackDose << "\n balck dose sample: " << fBlackDoseSample << std::endl;


    logger(kipl::logging::Logger::LogMessage,"Preparing BB images done");



    delete[] temp_parameters;
    delete[] bb_ob_param;
    delete[] bb_sample_parameters;




}

int RobustLogNorm::GetnProjwithAngle(float angle){

    // range of projection angles
    double nProj=((double)m_Config.ProjectionInfo.nLastIndex-(double)m_Config.ProjectionInfo.nFirstIndex+1)/(double)m_Config.ProjectionInfo.nProjectionStep;

    int index =0;
    float curr_angle;

    for (size_t i=0; i<nProj; i++){
        curr_angle = m_Config.ProjectionInfo.fScanArc[0]+(m_Config.ProjectionInfo.fScanArc[1]-m_Config.ProjectionInfo.fScanArc[0])/(nProj-1)*i;


        if (curr_angle<=angle+0.5f & curr_angle>=angle-0.5f) {
//            std::cout << "curr angle in GetnProjwithAngle: " << curr_angle << " " << angle << std::endl;
            break;
        }
        else index++;

    }


    return index;
}

float RobustLogNorm::GetInterpolationError(kipl::base::TImage<float,2> &mask){

//    std::cout << "GetInterpolationError begin.." << std::endl;
    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);

    kipl::base::TImage<float,2> flat, dark, bb;

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;


//    fdarkBBdose=0.0f;
//    fFlatBBdose=1.0f;

    float darkdose = 0.0f;
    float flatdose = 1.0f;
    float blackdose = 1.0f;

    // reload the OB and DC into the BBroi and doseBBroi
    dark = BBLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,darkdose);
    flat = BBLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,flatdose); // to check if i have to use dosebias to remove the fdarkBBdose

    // now load OB image with BBs

    float *bb_ob_param = new float[6];

    bb = BBLoader(blackbodyname,nBBFirstIndex,nBBCount,1.0f,0.0f,m_Config,blackdose);

    int diffroi[4] = {int(BBroi[0]-m_Config.ProjectionInfo.projection_roi[0]), int(BBroi[1]-m_Config.ProjectionInfo.projection_roi[1]), int(BBroi[2]-m_Config.ProjectionInfo.projection_roi[2]), int(BBroi[3]-m_Config.ProjectionInfo.projection_roi[3])};

//    std::cout<< "diffroi: " << std::endl;
//    std::cout << diffroi[0] << " " << diffroi[1] << " " << diffroi[2] << " " << diffroi[3] << std::endl;

    m_corrector.SetRadius(radius);
    m_corrector.SetInterpolationOrderX(m_xInterpOrder);
    m_corrector.SetInterpolationOrderY(m_yInterpOrder);
    m_corrector.setDiffRoi(diffroi);

    float error;
    kipl::base::TImage<float,2> obmask(bb.Dims());
    bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask, error);
    mask = obmask;
//    std::cout << bb_ob_param[0] << " " << bb_ob_param[1] << " " << bb_ob_param[2] << " " << bb_ob_param[3] << " " <<
//                                   bb_ob_param[4] << " " << bb_ob_param[5] << std::endl;

//    std::cout << "error in get interpolation error: " << error << std::endl;

    delete [] bb_ob_param;


    return error;
}

kipl::base::TImage<float,2> RobustLogNorm::GetMaskImage(){
    return mMaskBB;
}

float RobustLogNorm::computedose(kipl::base::TImage<float,2>&img){

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

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
//    float dose=GetFloatParameter(coeff,"dose")-fDarkDose;
//    std::cout << "Process core 2D" << std::endl; // doesn't seem to enter in here actually..
//    std::cout << dose << std::endl;

//    // Handling the non-dose correction case
//    m_corrector.Process(img,dose);

    return 0;
}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) {


//    if (bUseBB && nBBCount!=0 && nBBSampleCount!=0) {
//        PrepareBBData();
//        size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};
//        m_corrector.SetBBInterpRoi(subroi);
//    }




//    m_corrector.SetReferenceImages(&mflat, &mdark, (bUseBB && nBBCount!=0 && nBBSampleCount!=0), (bUseExternalBB && nBBextCount!=0), fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), m_Config.ProjectionInfo.dose_roi);

    int nDose=img.Size(2);
    float *doselist=nullptr;

    std::stringstream msg;

    if (bUseNormROI==true) {
        doselist=new float[nDose];
        GetFloatParameterVector(coeff,"dose",doselist,nDose);
        for (int i=0; i<nDose; i++) {
            doselist[i] = doselist[i]-fDarkDose;
//            std::cout << doselist[i]<< std::endl;
        }
    }

    m_corrector.Process(img,doselist);

    if (doselist!=nullptr)
        delete [] doselist;

    return 0;
}

void RobustLogNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{

    mdark = dark;
    mdark.Clone();

    mflat = flat;
    mflat.Clone();



    m_corrector.SetReferenceImages(&mflat, &mdark, (bUseBB && nBBCount!=0 && nBBSampleCount!=0), (bUseExternalBB && nBBextCount!=0), fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), m_Config.ProjectionInfo.dose_roi);

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (int i=1; i<static_cast<int>(flat.Size(1)); i++) {
                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
    }

}

// problem to use it as it is: i have different roi and doseroi for images (bb and not bb.. to take this into account..)
kipl::base::TImage<float,2> RobustLogNorm::ReferenceLoader(std::string fname,
                                                      int firstIndex, int N, size_t *roi,
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
    size_t found;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogMessage,msg.str());

        found = fmask.find("hdf");
         if (found==std::string::npos ) {

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


        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

            if (found==std::string::npos ) {

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
        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;

        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);

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


kipl::base::TImage<float,2> RobustLogNorm::BBLoader(std::string fname,
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
    size_t found;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogMessage,msg.str());

        found = fmask.find("hdf");

        if (found==std::string::npos ) {
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


        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {

            if (found==std::string::npos ) {
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
            else{
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

        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;


        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);


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

float RobustLogNorm::DoseBBLoader(std::string fname,
                             int firstIndex,
                             float initialDose,
                             float doseBias,
                             ReconConfig &config) {

    std::string fmask=fname;
    float tmpdose = 0.0f;
    float dose;

    std::string filename,ext;
    ProjectionReader reader;
    size_t found;

    found=fname.find("hdf");

    if (found!=std::string::npos)
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

kipl::base::TImage <float,2> RobustLogNorm::BBExternalLoader(std::string fname, ReconConfig &config, size_t *roi, float &dose){


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

//        std::cout << "dose in BB external loader " <<dose << std::endl;

        return img;

}

kipl::base::TImage <float,3> RobustLogNorm::BBExternalLoader(std::string fname, int N, size_t *roi, int firstIndex, ReconConfig &config, float *doselist){


    kipl::base::TImage <float, 2> tempimg;
    kipl::base::TImage<float, 3> img;



    if (fname.empty() && N!=0)
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    float *mylist = new float[N];

    std::string fmask=fname;

    std::string filename,ext;
    ProjectionReader reader;

    if (N!=0) {


        for (int i=0; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');
//            std::cout << filename << std::endl;

            tempimg=reader.Read(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    roi);

            if (i==0){
                size_t dims[]={tempimg.Size(0), tempimg.Size(1),static_cast<size_t>(N)};
                img.Resize(dims);
            }

//            std::cout << "before the GetProjectiondose " << std::endl;
//            std::cout << "bUseNormROIBB " << bUseNormROIBB << std::endl;

            mylist[i] = bUseNormROI ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : 0.0f;
//            std::cout << "dose in BBexternal loader: " << mylist[i] << std::endl;


            memcpy(img.GetLinePtr(0,i),tempimg.GetDataPtr(),tempimg.Size()*sizeof(float));

        }
        memcpy(doselist, mylist, sizeof(float)*N);


    }


     delete [] mylist;


    return img;

}

