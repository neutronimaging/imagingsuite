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
    fFlatDose(1.0f),
    fBlackDose(1.0f),
    bUseNormROI(true),
    bUseLUT(false),
    bUseWeightedMean(false),
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

    std::cout << blackbodyname << std::endl;
    std::cout << nBBCount << " " << nBBFirstIndex << std::endl;

    memcpy(nOriginalNormRegion,config.ProjectionInfo.dose_roi,4*sizeof(size_t));

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
    SetReferenceImages(dark, flat);

    // now load OB image with BBs

    if (nBBCount!=0) {
         logger(kipl::logging::Logger::LogMessage,"Loading OB images with BBs");
         kipl::strings::filenames::MakeFileName(blackbodyname,nOBFirstIndex,filename,ext,'#','0');
         bb = reader.Read(filename,
                          m_Config.ProjectionInfo.eFlip,
                          m_Config.ProjectionInfo.eRotate,
                          m_Config.ProjectionInfo.fBinning,
                          roi);

// not used for now but to take into account for the near future
//         if (bUseNormROI) {
//             fDarkDose=reader.GetProjectionDose(filename,
//                     m_Config.ProjectionInfo.eFlip,
//                     m_Config.ProjectionInfo.eRotate,
//                     m_Config.ProjectionInfo.fBinning,
//                     nOriginalNormRegion);
//         }
//         else {
//             fDarkDose=0.0f;
//         }

         for (size_t i=1; i<nOBCount; i++) {
             kipl::strings::filenames::MakeFileName(blackbodyname,i+nBBFirstIndex,filename,ext,'#','0');
             img=reader.Read(filename,
                     m_Config.ProjectionInfo.eFlip,
                     m_Config.ProjectionInfo.eRotate,
                     m_Config.ProjectionInfo.fBinning,
                     roi);
             bb+=img;
//             if (bUseNormROI) {
//                 fDarkDose+=reader.GetProjectionDose(filename,
//                         m_Config.ProjectionInfo.eFlip,
//                         m_Config.ProjectionInfo.eRotate,
//                         m_Config.ProjectionInfo.fBinning,
//                         nOriginalNormRegion);
//             }
//             else {
//                 fDarkDose=0.0f;
//             }
         }

         bb/=static_cast<float>(nOBCount);
         // repeat sinograms? to see if to be added

    }


    m_corrector.SetReferenceImages(&flat, &dark, &bb, fFlatDose, fDarkDose, NULL); // understand connections better

//    std::cout << "computed doses: " << std::endl;
//    std::cout << fDarkDose << " " << fFlatDose << std::endl;

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
// to see if they are neededs
    mDark=dark;
    mFlatField=flat;
//    m_corrector.SetReferenceImages();
//    NormBase::SetReferenceImages(dark,flat);

    // not sure what it does. is it used? yes. it enters in here..

    std::cout << "SetReferenceImages dark 2d and flat 2d" << std::endl;

    float dose=1.0f/(fFlatDose-fDarkDose);
    if (dose!=dose)
        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

//    const int N=static_cast<int>(flat.Size());
//    float *pFlat=flat.GetDataPtr();
//    float *pDark=dark.GetDataPtr();

//    if (nDCCount!=0) {
//        #pragma omp parallel for
//        for (int i=0; i<N; i++) {
//            float fProjPixel=pFlat[i]-pDark[i];
//            if (fProjPixel<=0)
//                pFlat[i]=0;
//            else
//               pFlat[i]=log(fProjPixel*dose);

//        }
//    }
//    else {
//        #pragma omp parallel for
//        for (int i=0; i<N; i++) {
//            float fProjPixel=pFlat[i];
//            if (fProjPixel<=0)
//                pFlat[i]=0;
//            else
//                pFlat[i]=log(fProjPixel*dose);
//        }
//    }

    // che fine fanno qst pFlat? SECONDO ME SI USANO IN REALTÀ IN REFERENCEIMAGECORRECTION::PREPAREREFERENCES

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (int i=1; i<flat.Size(1); i++) {
                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
    }

}
