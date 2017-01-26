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
    // to check which one do i need:
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
    bPBvariante(false),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm),
    m_BBOptions(ImagingAlgorithms::ReferenceImageCorrection::Interpolate),
    ferror(0.0f)
{
    doseBBroi[0] = doseBBroi[1] = doseBBroi[2] = doseBBroi[3]=0;
    BBroi[0] = BBroi[1] = BBroi[2] = BBroi[3] = 0;
    blackbodyname = "somename";
    blackbodysamplename = "somename";

}

RobustLogNorm::~RobustLogNorm()
{

}


int RobustLogNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{


    std::cout << "ciao robust log norm" << std::endl;

    // from NormPlugins
    m_Config    = config;
    path        = config.ProjectionInfo.sReferencePath;
    flatname    = config.ProjectionInfo.sOBFileMask;
    darkname    = config.ProjectionInfo.sDCFileMask;

    nOBCount      = config.ProjectionInfo.nOBCount;
    nOBFirstIndex = config.ProjectionInfo.nOBFirstIndex;

    nDCCount      = config.ProjectionInfo.nDCCount;
    nDCFirstIndex = config.ProjectionInfo.nDCFirstIndex;


//    std::cout << "ciao robust log norm" << std::endl;

    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
    string2enum(GetStringParameter(parameters,"BBOption"), m_BBOptions);
    bUseNormROI = kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));

    bPBvariante = kipl::strings::string2bool(GetStringParameter(parameters,"PBvariante"));


//    std::cout << "ciao robust log norm" << std::endl;

    blackbodyname = GetStringParameter(parameters,"BB_OB_name");
    nBBCount = GetIntParameter(parameters,"BB_counts");
    nBBFirstIndex = GetIntParameter(parameters, "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters,"BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters, "BB_sample_firstindex");
    nBBSampleCount = GetIntParameter(parameters,"BB_samplecounts");
    tau = GetFloatParameter(parameters, "tau");
    radius = GetIntParameter(parameters, "radius");
    GetUIntParameterVector(parameters, "BBroi", BBroi, 4);
    GetUIntParameterVector(parameters, "doseBBroi", doseBBroi, 4);
    bUseNormROIBB = kipl::strings::string2bool(GetStringParameter(parameters,"useBBnormregion"));

//    std::cout << "ciao robust log norm" << std::endl;

    std::cout << blackbodyname << std::endl;
    std::cout << nBBCount << " " << nBBFirstIndex << std::endl;

    memcpy(nOriginalNormRegion,config.ProjectionInfo.dose_roi,4*sizeof(size_t));

    size_t roi_bb_x= BBroi[2]-BBroi[0];
    size_t roi_bb_y = BBroi[3]-BBroi[1];

//    std::cout << "ciao robust log norm" << std::endl;

    if (roi_bb_x>0 && roi_bb_y>0) {
        std::cout << "have roi for BB!" << std::endl;
    }
    else {
        std::cout << "does not have roi for BB, copy projection roi" << std::endl;
        memcpy(BBroi, m_Config.ProjectionInfo.projection_roi, sizeof(size_t)*4);  // use the same as projections in case
    }

    //check on dose BB roi size
    if ((doseBBroi[2]-doseBBroi[0])<=0 || (doseBBroi[3]-doseBBroi[1])<=0){
        bUseNormROIBB=false;
    }



    // prepare BB data
    if (nBBCount!=0 && nBBSampleCount!=0) {
        PrepareBBData();
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

//    parameters=PreprocModuleBase::GetParameters();
    parameters["window"] = kipl::strings::value2string(m_nWindow);
    parameters["avgmethod"] = enum2string(m_ReferenceAverageMethod);
    parameters["refmethod"] = enum2string(m_ReferenceMethod);
    parameters["usenormregion"] = kipl::strings::bool2string(bUseNormROI);
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
    parameters["useBBnormregion"] = kipl::strings::bool2string(bUseNormROIBB);
    parameters["PBvariante"] = kipl::strings::bool2string(bPBvariante);
    parameters["BBOption"] = enum2string(m_BBOptions);


//    parameters["corrector"] = enum2string(m_corrector);

    return parameters;
}

void RobustLogNorm::LoadReferenceImages(size_t *roi)
{

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    std::cout << "path: " << path << std::endl; // it is actually empty.. so i don't know if we need to use it..
    std::cout << "flatname: " << flatname << std::endl;
    std::cout << "darkname: "<< darkname << std::endl;
    std::cout << "blackbody name: " <<blackbodyname << std::endl;
    std::cout << "roi: " << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3]  << std::endl;
    std::cout << "projection roi: " << m_Config.ProjectionInfo.projection_roi[0] << " " <<
                 m_Config.ProjectionInfo.projection_roi[1] << " " <<
                 m_Config.ProjectionInfo.projection_roi[2] << " " <<
                 m_Config.ProjectionInfo.projection_roi[3] << std::endl;

    // new code
    kipl::base::TImage<float,2> flat, dark;

    fDarkDose=0.0f;
    fFlatDose=1.0f;
    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;

    dark = ReferenceLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,0.0f,0.0f,m_Config,fDarkDose);
    flat = ReferenceLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,1.0f,0.0f,m_Config,fFlatDose); // i don't use the bias.. beacuse i think i use it later on

//    std::cout << "computed doses: " << std::endl;
//    std::cout << fDarkDose << " " << fFlatDose << std::endl;


    if (bUseBB) {
            size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};
            m_corrector.SetBBInterpRoi(subroi);
    }

    m_corrector.SetReferenceImages(&flat, &dark, bUseBB, fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), m_Config.ProjectionInfo.dose_roi);

    // OLD CODE to delete at the end
    //    //maybe path i do not need.
    ////    m_corrector.LoadReferenceImages(path,flatname,nOBFirstIndex,nOBCount,
    ////                                    darkname,nDCFirstIndex,nDCCount,
    ////                                    blackbodyname,nBBFirstIndex,nBBCount,
    ////                                    roi,nNormRegion,nullptr);

    //    kipl::base::TImage<float,2> img, flat, dark, bb;

    //    std::string flatmask=path+flatname;
    //    std::string darkmask=path+darkname;
    //    std::string filename,ext;
    //    ProjectionReader reader;

    //    fDarkDose=0.0f;
    //    fFlatDose=1.0f;
    //    if (nOBCount!=0) {
    //        logger(kipl::logging::Logger::LogMessage,"Loading open beam images");

    //        float *fFlatDoses=new float[nOBCount];
    //        float dose=0.0f;

    //        kipl::strings::filenames::MakeFileName(flatmask,nOBFirstIndex,filename,ext,'#','0');
    //        img = reader.Read(filename,
    //                m_Config.ProjectionInfo.eFlip,
    //                m_Config.ProjectionInfo.eRotate,
    //                m_Config.ProjectionInfo.fBinning,
    //                roi);

    //        dose=bUseNormROI ? reader.GetProjectionDose(filename,
    //                    m_Config.ProjectionInfo.eFlip,
    //                    m_Config.ProjectionInfo.eRotate,
    //                    m_Config.ProjectionInfo.fBinning,
    //                    nOriginalNormRegion) : 1.0f;

    //        fFlatDose     = dose;
    //        fFlatDoses[0] = dose;

    ////        if (bUseNormROI) {
    ////            fFlatDose=reader.GetProjectionDose(filename,
    ////                    m_Config.ProjectionInfo.eFlip,
    ////                    m_Config.ProjectionInfo.eRotate,
    ////                    m_Config.ProjectionInfo.fBinning,
    ////                    nOriginalNormRegion);
    ////        }
    ////        else {
    ////            fFlatDose=1.0f;
    ////        }

    //        size_t obdims[]={img.Size(0), img.Size(1),nOBCount};

    //        kipl::base::TImage<float,3> flat3D(obdims);
    //        memcpy(flat3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

    //        for (size_t i=1; i<nOBCount; i++) {
    //            kipl::strings::filenames::MakeFileName(flatmask,i+nOBFirstIndex,filename,ext,'#','0');
    //            img=reader.Read(filename,
    //                    m_Config.ProjectionInfo.eFlip,
    //                    m_Config.ProjectionInfo.eRotate,
    //                    m_Config.ProjectionInfo.fBinning,
    //                    roi);
    //            memcpy(flat3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

    //            dose = bUseNormROI ? reader.GetProjectionDose(filename,
    //                        m_Config.ProjectionInfo.eFlip,
    //                        m_Config.ProjectionInfo.eRotate,
    //                        m_Config.ProjectionInfo.fBinning,
    //                        nOriginalNormRegion) : 1.0f;

    //            fFlatDose    += dose;
    //            fFlatDoses[i] = fFlatDoses[0]/dose;

    ////            if (bUseNormROI) {
    ////                fFlatDose+=reader.GetProjectionDose(filename,
    ////                        m_Config.ProjectionInfo.eFlip,
    ////                        m_Config.ProjectionInfo.eRotate,
    ////                        m_Config.ProjectionInfo.fBinning,
    ////                        nOriginalNormRegion);
    ////            }
    ////            else {
    ////                fFlatDose+=1.0f;
    ////            }
    //        }

    //        fFlatDoses[0]=1.0f;

    //        fFlatDose/=static_cast<float>(nOBCount);

    ////        float *tempdata=new float[nOBCount];
    //        flat.Resize(obdims);
    ////        float *pFlat3D=flat3D.GetDataPtr();
    //        float *pFlat=flat.GetDataPtr();

    ////        for (size_t i=0; i<flat.Size(); i++) {
    ////            for (size_t j=0; j<nOBCount; j++) {
    ////                tempdata[j]=pFlat3D[i+j*flat.Size()];
    ////            }
    ////            kipl::math::median_quick_select(tempdata, nOBCount, pFlat+i);
    ////        }

    //        ImagingAlgorithms::AverageImage avg;
    //        flat = avg(flat3D, m_ReferenceAverageMethod, nullptr); // check this fFlatDoses if it is correct

    ////        delete [] tempdata;

    //        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
    //            for (size_t i=1; i<flat.Size(1); i++) {
    //                memcpy(flat.GetLinePtr(i), pFlat, sizeof(float)*flat.Size(0));

    //            }
    //        }
    //    }
    //    else
    //        logger(kipl::logging::Logger::LogWarning,"Open beam image count is zero");

    //    if (nDCCount!=0) {
    //        logger(kipl::logging::Logger::LogMessage,"Loading dark images");
    //        kipl::strings::filenames::MakeFileName(darkmask,nDCFirstIndex,filename,ext,'#','0');
    //        dark = reader.Read(filename,
    //                m_Config.ProjectionInfo.eFlip,
    //                m_Config.ProjectionInfo.eRotate,
    //                m_Config.ProjectionInfo.fBinning,
    //                roi);
    //        if (bUseNormROI) {
    //            fDarkDose=reader.GetProjectionDose(filename,
    //                    m_Config.ProjectionInfo.eFlip,
    //                    m_Config.ProjectionInfo.eRotate,
    //                    m_Config.ProjectionInfo.fBinning,
    //                    nOriginalNormRegion);
    //        }
    //        else {
    //            fDarkDose=0.0f;
    //        }

    //        for (size_t i=1; i<nDCCount; i++) {
    //            kipl::strings::filenames::MakeFileName(darkmask,i+nDCFirstIndex,filename,ext,'#','0');
    //            img=reader.Read(filename,
    //                    m_Config.ProjectionInfo.eFlip,
    //                    m_Config.ProjectionInfo.eRotate,
    //                    m_Config.ProjectionInfo.fBinning,
    //                    roi);
    //            dark+=img;
    //            if (bUseNormROI) {
    //                fDarkDose+=reader.GetProjectionDose(filename,
    //                        m_Config.ProjectionInfo.eFlip,
    //                        m_Config.ProjectionInfo.eRotate,
    //                        m_Config.ProjectionInfo.fBinning,
    //                        nOriginalNormRegion);
    //            }
    //            else {
    //                fDarkDose=0.0f;
    //            }
    //        }
    //        fDarkDose/=static_cast<float>(nDCCount);
    //        dark/=static_cast<float>(nDCCount);
    //        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
    //            for (size_t i=1; i<dark.Size(1); i++) {
    //                memcpy(dark.GetLinePtr(i), dark.GetLinePtr(0), sizeof(float)*dark.Size(0));
    //            }
    //        }
    //    }
    //    else
    //        logger(kipl::logging::Logger::LogWarning,"Dark current image count is zero");

    //    logger(kipl::logging::Logger::LogMessage,"Load reference done");
    ////    SetReferenceImages(dark, flat); // that I don't think I need

    ////    size_t subroi[4] = {0,roi[1]-m_Config.ProjectionInfo.projection_roi[1], roi[2]-roi[0]-1, roi[3]-m_Config.ProjectionInfo.projection_roi[1]-1};
    ////    std::cout << subroi[0] << " " << subroi[1] << " " << subroi[2] << " " << subroi[3] << std::endl;
    //    if (bUseBB) {
    //            size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};
    //            m_corrector.SetBBInterpRoi(subroi);
    //    }


    //    m_corrector.SetReferenceImages(&flat, &dark, bUseBB, fFlatDose, fDarkDose, (bUseNormROIBB && bUseNormROI), m_Config.ProjectionInfo.dose_roi);

    ////    std::cout << "computed doses: " << std::endl;
    ////    std::cout << fDarkDose << " " << fFlatDose << std::endl;

    //}

}


void RobustLogNorm::PrepareBBData(){


    std::cout << "PrepareBBData begin--" << std::endl;
    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);
    if (blackbodysamplename.empty() && nBBSampleCount!=0)
        throw ReconException("The sample image with BBs image mask is empty", __FILE__,__LINE__);



//    std::cout << "path: " << path << std::endl; // it is actually empty.. so i don't know if we need to use it..
//    std::cout << "flatname: " << flatname << std::endl;
//    std::cout << "darkname: "<< darkname << std::endl;
//    std::cout << "blackbody name: " <<blackbodyname << std::endl;
//    std::cout << "projection roi: " << m_Config.ProjectionInfo.projection_roi[0] << " " <<
//                 m_Config.ProjectionInfo.projection_roi[1] << " " <<
//                 m_Config.ProjectionInfo.projection_roi[2] << " " <<
//                 m_Config.ProjectionInfo.projection_roi[3] << std::endl;

//    std::cout << "BBroi: " << BBroi[0] << " " << BBroi[1] << " " << BBroi[2] << " " << BBroi[3] << std::endl;

    int diffroi[4] = {int(BBroi[0]-m_Config.ProjectionInfo.projection_roi[0]), int(BBroi[1]-m_Config.ProjectionInfo.projection_roi[1]), int(BBroi[2]-m_Config.ProjectionInfo.projection_roi[2]), int(BBroi[3]-m_Config.ProjectionInfo.projection_roi[3])};

//    std::cout << "diff roi: " << diffroi[0] << " " << diffroi[1] << " " << diffroi[2] << " " << diffroi[3] << std::endl;

//    std::cout << "BB NORM ROI: " << doseBBroi[0] << " " << doseBBroi[1] << " " << doseBBroi[2] << " " << doseBBroi[3] << std::endl;

    m_corrector.setDiffRoi(diffroi);
    m_corrector.SetRadius(radius);
    m_corrector.SetTau(tau);
    m_corrector.SetPBvariante(bPBvariante);


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
    bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask);




    kipl::base::TImage<float,2> myint(bb.Dims());
    myint = m_corrector.InterpolateBlackBodyImage(bb_ob_param, BBroi);
    kipl::base::TImage<float,2> BB_DC(bb.Dims());
    memcpy(BB_DC.GetDataPtr(), bb.GetDataPtr(), sizeof(float)*bb.Size());
    //         BB_DC=bb;
    BB_DC-=dark;
    ferror = m_corrector.ComputeInterpolationError(myint, obmask, BB_DC); // the error also I want to change the way it is done..

    //         std::cout << "FIRST COMPUTATION OF INTERPOLATION ERROR TO BE THEN PUT IN THE GUI: " << ferror << std::endl;



    if (bPBvariante) {
     kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(bb_ob_param,doseBBroi);
     float mydose = computedose(mybb);
     std::cout << "----------------before PB variante: " << fBlackDose << std::endl;
     fBlackDose+= ((1/tau-1)*mydose);
     std::cout << "black dose with PB variante: " << fBlackDose << std::endl;

    }


    if(bUseNormROI && bUseNormROIBB){
     fBlackDose = fBlackDose<1 ? 1.0f : fBlackDose;
     for (size_t i=0; i<6; i++){
         bb_ob_param[i]*=(fFlatBBdose-fDarkDose);
         bb_ob_param[i]/=(fBlackDose*tau); // now the dose is already computed for I_Ob_BB
     }
    }

    std::cout << "blackdose: " << fBlackDose << std::endl;

    //         kipl::io::WriteTIFF32(obmask,"/home/carminati_c/repos/testdata/maskOb.tif");

    //    }

    // load sample images with BBs and sample images

    float *temp_parameters;
    float *bb_sample_parameters;



         switch (m_BBOptions) {

         case (ImagingAlgorithms::ReferenceImageCorrection::Interpolate): {
                 std::cout << "ciao interpolate" << std::endl;
                 bb_sample_parameters = new float[6*nBBSampleCount];
                 temp_parameters = new float[6];

                 if (nBBSampleCount!=0) {

                     logger(kipl::logging::Logger::LogMessage,"Loading sample images with BB");

                     for (size_t i=0; i<nBBSampleCount; i++) {

                         std::cout << i << std::endl;
                         samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);
                         std::cout << "fBlackDose sample at sample n. " << i << " without PB variante: " << fBlackDoseSample << std::endl;

                         // read again the sample only in the first case!
                         if (i==0) {
                             float dosesample; // i don't need this

                             sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex, 1, 1.0f,0.0f,m_Config, dosesample);

                             kipl::base::TImage<float,2> mask(sample.Dims());
                             mask = 0.0f;



                             temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                             mMaskBB = mask; // or memcpy



                             std::cout << "temp parameters BEFORE normalization: " << std::endl;
                             std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;
                             if (bUseNormROIBB && bUseNormROI){
             //                     prenormalize interpolation parameters with dose


                                 if (bPBvariante) {

                                         kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                         float mydose = computedose(mybb);
                                         fBlackDoseSample+= ((1/tau-1)*mydose);
                                         std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                                      }


                                 fBlackDoseSample = fBlackDoseSample<1 ? 1.0f : fBlackDoseSample;

                                 for(size_t j=0; j<6; j++) {

                                           temp_parameters[j]/=fBlackDoseSample;

                                 }
                             }

                             std::cout << "temp parameters AFTER normalization: " << std::endl;
                             std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;

                             std::cout <<  "f balck dose sample " << fBlackDoseSample<< std::endl;
                             memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6); // copio i primi 6


             //                kipl::io::WriteTIFF32(mask,"/home/carminati_c/repos/testdata/maskSample.tif");


                         }

                         else {

                             temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);

                             for(size_t j=0; j<6; j++) {
                              temp_parameters[j]/=fBlackDoseSample;
                             }

                             memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6); // copio gli altri mano mano

                         }



                 }
                 }


                 break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::Average): {

                     bb_sample_parameters = new float[6];
                     temp_parameters = new float[6];
                     samplebb = BBLoader(blackbodysamplename, nBBSampleFirstIndex,nBBSampleCount,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);

                     float dosesample; // i don't need this

                     sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex, 1, 1.0f,0.0f,m_Config, dosesample);

                     kipl::base::TImage<float,2> mask(sample.Dims());
                     mask = 0.0f;

                     temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                     mMaskBB = mask; // or memcpy



                     std::cout << "temp parameters BEFORE normalization: " << std::endl;
                     std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;
                     if (bUseNormROIBB && bUseNormROI){
                //                     prenormalize interpolation parameters with dose


                         if (bPBvariante) {

                                 kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                 float mydose = computedose(mybb);
                                 fBlackDoseSample+= ((1/tau-1)*mydose);
                                 std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                              }


                         fBlackDoseSample = fBlackDoseSample<1 ? 1.0f : fBlackDoseSample;

                         for(size_t j=0; j<6; j++) {

                                   temp_parameters[j]/=fBlackDoseSample;

                         }
                     }

                     std::cout << "temp parameters AFTER normalization: " << std::endl;
                     std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;

                     std::cout <<  "f balck dose sample " << fBlackDoseSample<< std::endl;
                     memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6); // copio i primi 6


                std::cout << "ciao average" << std::endl;
                break;
         }

         case (ImagingAlgorithms::ReferenceImageCorrection::OneToOne): {
                bb_sample_parameters = new float[6*nBBSampleCount];

                for (size_t i=0; i<nBBSampleCount; i++) {

                    std::cout << i << std::endl;
                    samplebb = BBLoader(blackbodysamplename,i+nBBSampleFirstIndex,1,1.0f,fdarkBBdose, m_Config, fBlackDoseSample);
                    std::cout << "fBlackDose sample at sample n. " << i << " without PB variante: " << fBlackDoseSample << std::endl;

                    // read again the sample only in the first case!
                    if (i==0) {
                        float dosesample; // i don't need this

                        sample = BBLoader(m_Config.ProjectionInfo.sFileMask, m_Config.ProjectionInfo.nFirstIndex, 1, 1.0f,0.0f,m_Config, dosesample);

                        kipl::base::TImage<float,2> mask(sample.Dims());
                        mask = 0.0f;



                        temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                        mMaskBB = mask; // or memcpy



                        std::cout << "temp parameters BEFORE normalization: " << std::endl;
                        std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;
                        if (bUseNormROIBB && bUseNormROI){
        //                     prenormalize interpolation parameters with dose


                            if (bPBvariante) {

                                    kipl::base::TImage<float,2> mybb = m_corrector.InterpolateBlackBodyImage(temp_parameters,doseBBroi);
                                    float mydose = computedose(mybb);
                                    fBlackDoseSample+= ((1/tau-1)*mydose);
                                    std::cout << "black dose with PB variante: " << fBlackDoseSample << std::endl;

                                 }


                            fBlackDoseSample = fBlackDoseSample<1 ? 1.0f : fBlackDoseSample;

                            for(size_t j=0; j<6; j++) {

                                      temp_parameters[j]/=fBlackDoseSample;

                            }
                        }

                        std::cout << "temp parameters AFTER normalization: " << std::endl;
                        std::cout << temp_parameters[0] << " " << temp_parameters[1] << " " <<temp_parameters[2] << " " <<temp_parameters[3] << " " <<temp_parameters[4] << " " <<temp_parameters[5] << std::endl;

                        std::cout <<  "f balck dose sample " << fBlackDoseSample<< std::endl;
                        memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6); // copio i primi 6


        //                kipl::io::WriteTIFF32(mask,"/home/carminati_c/repos/testdata/maskSample.tif");


                    }

                    else {

                        temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);

                        for(size_t j=0; j<6; j++) {
                         temp_parameters[j]/=fBlackDoseSample;
                        }

                        memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6); // copio gli altri mano mano

                    }



            }

                std::cout << "ciao one to one " << std::endl;
                break;
             }

         }



//        std::cout << "PRINTING OF THE PARAMETERS:" << std::endl;
//        for (size_t i=0; i<6*nBBSampleCount; i++) {
//            std::cout << bb_sample_parameters[i] << " ";

//            if (i % 6 == 0 ) std::cout << "\n";
//        } // ok, they don't seem rubbish

//        // parameters are computed!!!!

//        std::cout << std::endl;
//        std::cout << "ROI BEFORE SETINTERPARAMETER" << std::endl;

//        std::cout <<
//        m_Config.ProjectionInfo.roi[0] << " " <<
//                                           m_Config.ProjectionInfo.roi[1] << " " <<
//                                           m_Config.ProjectionInfo.roi[2] << " " <<
//                                           m_Config.ProjectionInfo.roi[3] << " " << std::endl;



//        size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};


        if (nBBCount!=0 && nBBSampleCount!=0) {
            // for now let's deal with the most common case
             bUseBB = true;

             size_t doseroi_bb_x = doseBBroi[2]-doseBBroi[0];
             size_t doseroi_bb_y = doseBBroi[3]-doseBBroi[1];

//             if (doseroi_bb_x>0 && doseroi_bb_y>0) {
//                 std::cout << "have dose roi for BB!" << std::endl;
//                 bUseNormROIBB = true;
//             }




//            std::cout << "dims of projections: " << m_Config.ProjectionInfo.nDims[0] << " " <<
//                         m_Config.ProjectionInfo.nDims[1] << std::endl;


            double nProj=((double)m_Config.ProjectionInfo.nLastIndex-(double)m_Config.ProjectionInfo.nFirstIndex+1)/(double)m_Config.ProjectionInfo.nProjectionStep;

//            std::cout << "number of projections: " << nProj << std::endl;


             m_corrector.SetInterpParameters(bb_ob_param, bb_sample_parameters,nBBSampleCount, nProj);
//             m_corrector.SetBBInterpRoi(subroi); // moved in load reference images if bUseBB
        }


//    std::cout << "------- BB doses ------" << std::endl;
//    std::cout << "fdarkdose: " <<fdarkBBdose << "\n blackdose: " <<fBlackDose << "\n balck dose sample: " << fBlackDoseSample << std::endl;


    logger(kipl::logging::Logger::LogMessage,"Load images for BB preparation done");



    delete[] temp_parameters;
    delete[] bb_ob_param;
    delete[] bb_sample_parameters;




}


float RobustLogNorm::GetInterpolationError(){
    return ferror;
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
    float dose=GetFloatParameter(coeff,"dose")-fDarkDose;
    std::cout << "Process core 2D" << std::endl; // doesn't seem to enter in here actually..
    std::cout << dose << std::endl;

    // Handling the non-dose correction case
    m_corrector.Process(img,dose);

    return 0;
}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) {
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

}

// problem to use it as it is: i have different roi and doseroi for images (bb and not bb.. to take this into account..)
kipl::base::TImage<float,2> RobustLogNorm::ReferenceLoader(std::string fname,
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

//        float *fDoses=new float[N];

        kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
        img = reader.Read(filename,
                config.ProjectionInfo.eFlip,
                config.ProjectionInfo.eRotate,
                config.ProjectionInfo.fBinning,
                config.ProjectionInfo.roi);

        tmpdose=bUseNormROI ? reader.GetProjectionDose(filename,
                    config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    nOriginalNormRegion) : initialDose;

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;
//        fDoses[0] = tmpdose;

        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

            img=reader.Read(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.roi);
            memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

            tmpdose = bUseNormROI ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : initialDose;

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;
//            fDoses[i] = fDoses[0]/tmpdose;

//            msg.str(""); msg<<"fDose["<<i<<"]="<<fDoses[i];
//            logger(logger.LogMessage,msg.str());
        }
//        fDoses[0]=1.0f;

        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogMessage,msg.str());

        float *tempdata=new float[N];
        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;

//        refimg = avg(img3D,m_ReferenceAvagerage,fDoses);
        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);

        delete [] tempdata;
//        delete [] fDoses;

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

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0) {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger(kipl::logging::Logger::LogMessage,msg.str());

//        float *fDoses=new float[N];

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

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;
//        fDoses[0] = tmpdose;

        size_t obdims[]={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
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

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;
//            fDoses[i] = fDoses[0]/tmpdose;

//            msg.str(""); msg<<"fDose["<<i<<"]="<<fDoses[i];
//            logger(logger.LogMessage,msg.str());
        }
//        fDoses[0]=1.0f;

        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogMessage,msg.str());

//        float *tempdata=new float[N];
        refimg.Resize(img.Dims());

        ImagingAlgorithms::AverageImage avg;

//        refimg = avg(img3D,m_ReferenceAvagerage,fDoses);
        refimg = avg(img3D,m_ReferenceAverageMethod,nullptr);

//        delete [] tempdata;
//        delete [] fDoses;

        // what is this? do i need it?
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
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return refimg;
}
