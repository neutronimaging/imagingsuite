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
    nDCCount(0),
    nDCFirstIndex(1),
    nBBCount(0),
    nBBFirstIndex(1),
    nBBSampleCount(0),
    nBBSampleFirstIndex(1),
    fFlatDose(1.0f),
    fBlackDose(1.0f),
    bUseNormROI(true),
    bUseLUT(false),
    bUseWeightedMean(false),
    bUseBB(false),
    m_nWindow(5),
    m_ReferenceAverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_ReferenceMethod(ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm)
{
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



    m_nWindow = GetIntParameter(parameters,"window");
    string2enum(GetStringParameter(parameters,"avgmethod"),m_ReferenceAverageMethod);
    string2enum(GetStringParameter(parameters,"refmethod"), m_ReferenceMethod);
    bUseNormROI = kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));

    blackbodyname = GetStringParameter(parameters,"BB_OB_name");
    nBBCount = GetIntParameter(parameters,"BB_counts");
    nBBFirstIndex = GetIntParameter(parameters, "BB_first_index");
    blackbodysamplename = GetStringParameter(parameters,"BB_samplename");
    nBBSampleFirstIndex = GetIntParameter(parameters, "BB_sample_firstindex");
    nBBSampleCount = GetIntParameter(parameters,"BB_samplecounts");

    std::cout << blackbodyname << std::endl;
    std::cout << nBBCount << " " << nBBFirstIndex << std::endl;

    memcpy(nOriginalNormRegion,config.ProjectionInfo.dose_roi,4*sizeof(size_t));

    // prepare BB data
    PrepareBBData();

    return 1;
}

bool RobustLogNorm::SetROI(size_t *roi)
{
    std::stringstream msg;
    msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    LoadReferenceImages(roi);
    memcpy(nNormRegion,nOriginalNormRegion,4*sizeof(size_t));

    size_t roix=roi[2]-roi[0];
    size_t roiy=roi[3]-roi[1];

    return true;
}


std::map<std::string, std::string> RobustLogNorm::GetParameters()
{
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

    //maybe path i do not need.
//    m_corrector.LoadReferenceImages(path,flatname,nOBFirstIndex,nOBCount,
//                                    darkname,nDCFirstIndex,nDCCount,
//                                    blackbodyname,nBBFirstIndex,nBBCount,
//                                    roi,nNormRegion,nullptr);

    kipl::base::TImage<float,2> img, flat, dark, bb;



    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;
    std::string filename,ext;
    ProjectionReader reader;

    fDarkDose=0.0f;
    fFlatDose=1.0f;
    if (nOBCount!=0) {
        logger(kipl::logging::Logger::LogMessage,"Loading open beam images");

        kipl::strings::filenames::MakeFileName(flatmask,nOBFirstIndex,filename,ext,'#','0');
        img = reader.Read(filename,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                roi);

        if (bUseNormROI) {
            fFlatDose=reader.GetProjectionDose(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    nOriginalNormRegion);
        }
        else {
            fFlatDose=1.0f;
        }

        size_t obdims[]={img.Size(0), img.Size(1),nOBCount};

        kipl::base::TImage<float,3> flat3D(obdims);
        memcpy(flat3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (size_t i=1; i<nOBCount; i++) {
            kipl::strings::filenames::MakeFileName(flatmask,i+nOBFirstIndex,filename,ext,'#','0');
            img=reader.Read(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    roi);
            memcpy(flat3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));
            if (bUseNormROI) {
                fFlatDose+=reader.GetProjectionDose(filename,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        nOriginalNormRegion);
            }
            else {
                fFlatDose+=1.0f;
            }
        }
        fFlatDose/=static_cast<float>(nOBCount);

        float *tempdata=new float[nOBCount];
        flat.Resize(obdims);
        float *pFlat3D=flat3D.GetDataPtr();
        float *pFlat=flat.GetDataPtr();

        for (size_t i=0; i<flat.Size(); i++) {
            for (size_t j=0; j<nOBCount; j++) {
                tempdata[j]=pFlat3D[i+j*flat.Size()];
            }
            kipl::math::median_quick_select(tempdata, nOBCount, pFlat+i);
        }
        delete [] tempdata;

        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (size_t i=1; i<flat.Size(1); i++) {
                memcpy(flat.GetLinePtr(i), pFlat, sizeof(float)*flat.Size(0));

            }
        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Open beam image count is zero");

    if (nDCCount!=0) {
        logger(kipl::logging::Logger::LogMessage,"Loading dark images");
        kipl::strings::filenames::MakeFileName(darkmask,nDCFirstIndex,filename,ext,'#','0');
        dark = reader.Read(filename,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                roi);
        if (bUseNormROI) {
            fDarkDose=reader.GetProjectionDose(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    nOriginalNormRegion);
        }
        else {
            fDarkDose=0.0f;
        }

        for (size_t i=1; i<nDCCount; i++) {
            kipl::strings::filenames::MakeFileName(darkmask,i+nDCFirstIndex,filename,ext,'#','0');
            img=reader.Read(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    roi);
            dark+=img;
            if (bUseNormROI) {
                fDarkDose+=reader.GetProjectionDose(filename,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        nOriginalNormRegion);
            }
            else {
                fDarkDose=0.0f;
            }
        }
        fDarkDose/=static_cast<float>(nDCCount);
        dark/=static_cast<float>(nDCCount);
        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (size_t i=1; i<dark.Size(1); i++) {
                memcpy(dark.GetLinePtr(i), dark.GetLinePtr(0), sizeof(float)*dark.Size(0));
            }
        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Open beam image count is zero");

    logger(kipl::logging::Logger::LogMessage,"Load reference done");
//    SetReferenceImages(dark, flat); // that I don't think I need

    size_t subroi[4] = {0,roi[1]-m_Config.ProjectionInfo.projection_roi[1], roi[2]-roi[0]-1, roi[3]-m_Config.ProjectionInfo.projection_roi[1]-1};
    std::cout << subroi[0] << " " << subroi[1] << " " << subroi[2] << " " << subroi[3] << std::endl;


    m_corrector.SetReferenceImages(&flat, &dark, bUseBB, fFlatDose, fDarkDose, NULL); // understand connections better

//    std::cout << "computed doses: " << std::endl;
//    std::cout << fDarkDose << " " << fFlatDose << std::endl;

}

void RobustLogNorm::PrepareBBData(){

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    if (blackbodyname.empty() && nBBCount!=0)
        throw ReconException("The open beam image with BBs image mask is empty",__FILE__,__LINE__);
    if (blackbodysamplename.empty() && nBBSampleCount!=0)
        throw ReconException("The sample image with BBs image mask is empty", __FILE__,__LINE__);



    std::cout << "path: " << path << std::endl; // it is actually empty.. so i don't know if we need to use it..
    std::cout << "flatname: " << flatname << std::endl;
    std::cout << "darkname: "<< darkname << std::endl;
    std::cout << "blackbody name: " <<blackbodyname << std::endl;
//    std::cout << "roi: " << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3]  << std::endl;
    std::cout << "projection roi: " << m_Config.ProjectionInfo.projection_roi[0] << " " <<
                 m_Config.ProjectionInfo.projection_roi[1] << " " <<
                 m_Config.ProjectionInfo.projection_roi[2] << " " <<
                 m_Config.ProjectionInfo.projection_roi[3] << std::endl;


    kipl::base::TImage<float,2> img, flat, dark, bb, sample, samplebb;

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;
    std::string filename,ext;
    ProjectionReader reader;

//    fDarkDose=0.0f;
//    fFlatDose=1.0f;



    // add here or where it makes sense the boolean for using BBs

    if (nOBCount!=0) {
        logger(kipl::logging::Logger::LogMessage,"Loading open beam images for BB preparations");

        kipl::strings::filenames::MakeFileName(flatmask,nOBFirstIndex,filename,ext,'#','0');
        img = reader.Read(filename,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                m_Config.ProjectionInfo.projection_roi);


        size_t obdims[]={img.Size(0), img.Size(1),nOBCount};

        kipl::base::TImage<float,3> flat3D(obdims);
        memcpy(flat3D.GetLinePtr(0,0),img.GetDataPtr(),img.Size()*sizeof(float));

        for (size_t i=1; i<nOBCount; i++) {
            kipl::strings::filenames::MakeFileName(flatmask,i+nOBFirstIndex,filename,ext,'#','0');
            img=reader.Read(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    m_Config.ProjectionInfo.projection_roi);
            memcpy(flat3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));
        }


        float *tempdata=new float[nOBCount];
        flat.Resize(obdims);
        float *pFlat3D=flat3D.GetDataPtr();
        float *pFlat=flat.GetDataPtr();
// do i need this? I would say yes.
        for (size_t i=0; i<flat.Size(); i++) {
            for (size_t j=0; j<nOBCount; j++) {
                tempdata[j]=pFlat3D[i+j*flat.Size()];
            }
            kipl::math::median_quick_select(tempdata, nOBCount, pFlat+i);
        }
        delete [] tempdata;

    }
    else
        logger(kipl::logging::Logger::LogWarning,"Open beam image count is zero");


    if (nDCCount!=0) {
        logger(kipl::logging::Logger::LogMessage,"Loading dark images");
        kipl::strings::filenames::MakeFileName(darkmask,nDCFirstIndex,filename,ext,'#','0');
        dark = reader.Read(filename,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                m_Config.ProjectionInfo.projection_roi);


        for (size_t i=1; i<nDCCount; i++) {
            kipl::strings::filenames::MakeFileName(darkmask,i+nDCFirstIndex,filename,ext,'#','0');
            img=reader.Read(filename,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    m_Config.ProjectionInfo.projection_roi);
            dark+=img;
        }

        dark/=static_cast<float>(nDCCount);

    }
    else
        logger(kipl::logging::Logger::LogWarning,"Dark current image count is zero");



    // now load OB image with BBs

    float *bb_ob_param = new float[6];
    float *temp_parameters = new float[6];
    float *bb_sample_parameters = new float[6*nBBSampleCount];


    if (nBBCount!=0) {

        logger(kipl::logging::Logger::LogMessage,"Loading OB images with BBs");
         kipl::strings::filenames::MakeFileName(blackbodyname,nBBFirstIndex,filename,ext,'#','0');
         bb = reader.Read(filename,
                          m_Config.ProjectionInfo.eFlip,
                          m_Config.ProjectionInfo.eRotate,
                          m_Config.ProjectionInfo.fBinning,
                          m_Config.ProjectionInfo.projection_roi);


         for (size_t i=1; i<nOBCount; i++) {
             kipl::strings::filenames::MakeFileName(blackbodyname,i+nBBFirstIndex,filename,ext,'#','0');
             img=reader.Read(filename,
                     m_Config.ProjectionInfo.eFlip,
                     m_Config.ProjectionInfo.eRotate,
                     m_Config.ProjectionInfo.fBinning,
                     m_Config.ProjectionInfo.projection_roi);
             bb+=img;

         }

         bb/=static_cast<float>(nOBCount);


         kipl::base::TImage<float,2> obmask(bb.Dims());
         bb_ob_param = m_corrector.PrepareBlackBodyImage(flat,dark,bb, obmask);
         kipl::io::WriteTIFF32(obmask,"/home/carminati_c/repos/testdata/maskOb.tif");

    }

    // load sample images with BBs and sample images

    if (nBBSampleCount!=0) {
//        kipl::base::TImage<float,2> tempbb;
//        size_t bbdims[]={img.Size(0), img.Size(1),nBBSampleCount};

//        kipl::base::TImage<float,3> samplebb(bbdims); // let's assume we have 3D?
//        kipl::base::TImage<float,3> sample(bbdims);

        logger(kipl::logging::Logger::LogMessage,"Loading sample images with BB");




        for (size_t i=0; i<nBBSampleCount; i++) {

            std::cout << i << std::endl;
            kipl::strings::filenames::MakeFileName(blackbodysamplename,i+nBBSampleFirstIndex,filename,ext,'#','0');

     //       kipl::base::TImage<float,2> mask(bb.Dims());
     //       mask = 0.0f;

            std::cout << filename << std::endl;


            samplebb = reader.Read(filename,
                                   m_Config.ProjectionInfo.eFlip,
                                   m_Config.ProjectionInfo.eRotate,
                                   m_Config.ProjectionInfo.fBinning,
                                   m_Config.ProjectionInfo.projection_roi);



            if (i==5)  {kipl::io::WriteTIFF32(samplebb,"/home/carminati_c/repos/testdata/samplebb.tif");}



            // read again the sample only in the first case!
            if (i==0) {

                kipl::strings::filenames::MakeFileName(m_Config.ProjectionInfo.sFileMask,m_Config.ProjectionInfo.nFirstIndex,filename,ext,'#','0'); // index must be changed
                std::cout << filename << std::endl;

                sample= reader.Read(filename,
                                       m_Config.ProjectionInfo.eFlip,
                                       m_Config.ProjectionInfo.eRotate,
                                       m_Config.ProjectionInfo.fBinning,
                                       m_Config.ProjectionInfo.projection_roi);





                kipl::base::TImage<float,2> mask(sample.Dims());
                mask = 0.0f;



                temp_parameters= m_corrector.PrepareBlackBodyImage(sample,dark,samplebb, mask);
                mMaskBB = mask; // or memcpy
                memcpy(bb_sample_parameters, temp_parameters, sizeof(float)*6); // copio i primi 6


                kipl::io::WriteTIFF32(mask,"/home/carminati_c/repos/testdata/maskSample.tif");


            }

            else {

                temp_parameters = m_corrector.PrepareBlackBodyImagewithMask(dark,samplebb,mMaskBB);
                memcpy(bb_sample_parameters+i*6, temp_parameters, sizeof(float)*6); // copio gli altri mano mano

            }



        }

        std::cout << "PRINTING OF THE PARAMETERS:" << std::endl;
        for (size_t i=0; i<6*nBBSampleCount; i++) {
            std::cout << bb_sample_parameters[i] << " ";

            if (i % 6 == 0 ) std::cout << "\n";
        } // ok, they don't seem rubbish

        // parameters are computed!!!!

        std::cout << std::endl;
        std::cout << "ROI BEFORE SETINTERPARAMETER" << std::endl;

        std::cout <<
        m_Config.ProjectionInfo.roi[0] << " " <<
                                           m_Config.ProjectionInfo.roi[1] << " " <<
                                           m_Config.ProjectionInfo.roi[2] << " " <<
                                           m_Config.ProjectionInfo.roi[3] << " " << std::endl;


        // I have deleted -1 from the last two elements
        size_t subroi[4] = {0,m_Config.ProjectionInfo.roi[1]-m_Config.ProjectionInfo.projection_roi[1], m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0], m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.projection_roi[1]};


        if (nBBCount!=0 && nBBSampleCount!=0) {
            // for now let's deal with the most common case
             bUseBB = true;

            std::cout << "dims of projections: " << m_Config.ProjectionInfo.nDims[0] << " " <<
                         m_Config.ProjectionInfo.nDims[1] << std::endl;

            size_t nProj = m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex+1;
            std::cout << "number of projections: " << nProj << std::endl;
            // it has to be more complicated than this. TO BE FIXED

             m_corrector.SetInterpParameters(bb_ob_param, bb_sample_parameters,nBBSampleCount, nProj);
             m_corrector.SetBBInterpRoi(subroi);
        }


    }


    logger(kipl::logging::Logger::LogMessage,"Load images for BB preparation done");



    delete[] temp_parameters;
    delete[] bb_ob_param;
    delete[] bb_sample_parameters;




}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    float dose=GetFloatParameter(coeff,"dose")-fDarkDose;
    std::cout << "Process core 2D" << std::endl; // doesn't seem to enter in here actually..
    std::cout << dose << std::endl;

    // Handling te non-dose correction case
    m_corrector.Process(img,dose);

    return 0;
}

int RobustLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    int nDose=img.Size(2);
    float *doselist=nullptr;

    std::stringstream msg;

    if (bUseNormROI==true) {
        doselist=new float[nDose];
        GetFloatParameterVector(coeff,"dose",doselist,nDose);
        for (int i=0; i<nDose; i++) {
            doselist[i] = doselist[i]-fDarkDose;
//            std::cout << doselist[i]<< std::endl; // seem correct
        }
    }

    m_corrector.Process(img,doselist);

    if (doselist!=nullptr)
        delete [] doselist;

    return 0;
}

void RobustLogNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{
// to see if they are needed
//    mDark=dark;
//    mFlatField=flat;
////    m_corrector.SetReferenceImages();
////    NormBase::SetReferenceImages(dark,flat);

//    // not sure what it does. is it used? yes. it enters in here..

//    std::cout << "SetReferenceImages dark 2d and flat 2d" << std::endl;

//    float dose=1.0f/(fFlatDose-fDarkDose);
//    if (dose!=dose)
//        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

////    const int N=static_cast<int>(flat.Size());
////    float *pFlat=flat.GetDataPtr();
////    float *pDark=dark.GetDataPtr();

////    if (nDCCount!=0) {
////        #pragma omp parallel for
////        for (int i=0; i<N; i++) {
////            float fProjPixel=pFlat[i]-pDark[i];
////            if (fProjPixel<=0)
////                pFlat[i]=0;
////            else
////               pFlat[i]=log(fProjPixel*dose);

////        }
////    }
////    else {
////        #pragma omp parallel for
////        for (int i=0; i<N; i++) {
////            float fProjPixel=pFlat[i];
////            if (fProjPixel<=0)
////                pFlat[i]=0;
////            else
////                pFlat[i]=log(fProjPixel*dose);
////        }
////    }

//    // che fine fanno qst pFlat? SECONDO ME SI USANO IN REALTÀ IN REFERENCEIMAGECORRECTION::PREPAREREFERENCES

//    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
//            for (int i=1; i<flat.Size(1); i++) {
//                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
//    }

}
