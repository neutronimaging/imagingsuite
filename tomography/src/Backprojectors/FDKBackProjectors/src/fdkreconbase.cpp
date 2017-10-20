//<LICENSE>

#include "fdkreconbase.h"

#include "../include/ReconException.h"
#include "../include/ReconHelpers.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <base/tpermuteimage.h>
#include <base/trotate.h>
#include <math/mathconstants.h>

FdkReconBase::FdkReconBase(std::string application, std::string name, eMatrixAlignment alignment, kipl::interactors::InteractionBase *interactor) :
    BackProjectorModuleBase("muhrec",name,alignment,interactor),
    m_fp(nullptr),
    m_bp(nullptr),
    nProjCounter(0),
    SizeU(0),
    SizeV(0),
    SizeProj(0),
    MatrixCenterX(0),
    ProjCenter(0.0f),
    nProjectionBufferSize(0),
    nSliceBlock(3000),
    fRotation(0.0f) // not sure if i need these parameters.. for sure i would need the others that we have added in reconconfig
{
   logger(kipl::logging::Logger::LogMessage,"c'tor FdkProjectorBase");

   // set ebeamgeometry to conical
//   mConfig.ProjectionInfo.beamgeometry = "Cone";
//   mConfig.ProjectionInfo.beamgeometry=mConfig.ProjectionInfo.BeamGeometry_Cone;

//   std::cout << "beam geoemtry in fdk recon base: " << std::endl;
//   std::cout<< mConfig.ProjectionInfo.beamgeometry << std::endl;
   // e' in set roi
//       size_t mysize[3] = {512, 512, 85};
//       volume.Resize(mysize);
//       volume = 0.0f;

}

FdkReconBase::~FdkReconBase(void)
{

}

/// Sets up the back-projector with new parameters
/// \param config Reconstruction parameter set
/// \param parameters Additional set of configuration parameters
int FdkReconBase::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{

    mConfig=config;

//    config.ProjectionInfo.beamgeometry = mConfig.ProjectionInfo.BeamGeometry_Cone;

    nProjectionBufferSize=GetIntParameter(parameters,"ProjectionBufferSize");
    nSliceBlock=GetIntParameter(parameters,"SliceBlock");
    GetUIntParameterVector(parameters,"SubVolume",nSubVolume,2);

//    if (mConfig.MatrixInfo.bUseVOI) {
        volume_size[0] = mConfig.MatrixInfo.voi[1]-mConfig.MatrixInfo.voi[0];
        volume_size[1] = mConfig.MatrixInfo.voi[3]-mConfig.MatrixInfo.voi[2];
        volume_size[2] = mConfig.MatrixInfo.voi[5]-mConfig.MatrixInfo.voi[4];

//        std::cout << "volume size: " << volume_size[0] << " " << volume_size[1] << " " << volume_size[2] << std::endl;
//    }
//    else {
//        volume_size[0] = mConfig.MatrixInfo.nDims[0];
//        volume_size[1] = mConfig.MatrixInfo.nDims[1];
//        volume_size[2] = mConfig.MatrixInfo.nDims[2];
//    }

    volume.Resize(volume_size);
    volume = 0.0f;

    cbct_volume.Resize(volume_size);
    cbct_volume = 0.0f;

//   std::cout << "projections size: " << std::endl;
//   std::cout << projections.Size(0) << " " << projections.Size(1) << " "<< projections.Size(2) << std::endl;

    return 0;
}

/// Initializing the reconstructor
int FdkReconBase::Initialize()
{

    return 0;
}

int FdkReconBase::InitializeBuffers(int width, int height)
{
    return 0;
}

int FdkReconBase::FinalizeBuffers() {
    return 0;
}
/// Gets a list parameters required by the module.
/// \returns The parameter list
std::map<std::string, std::string> FdkReconBase::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["ProjectionBufferSize"]=kipl::strings::value2string(nProjectionBufferSize);
    parameters["SliceBlock"]= kipl::strings::value2string(nSliceBlock);
    parameters["SubVolume"]=kipl::strings::value2string(nSubVolume[0])+" "+kipl::strings::value2string(nSubVolume[1]);
//    parameters["SDD"] = kipl::strings::value2string(mConfig.ProjectionInfo.fSDD);
//    parameters["SOD"] = kipl::strings::value2string(mConfig.ProjectionInfo.fSOD);
//    parameters["HorizontalCenter"] = kipl::strings::value2string(mConfig.ProjectionInfo.fpPoint[0]);
//    parameters["VerticalCenter"] = kipl::strings::value2string(mConfig.ProjectionInfo.fpPoint[1]);
//    parameters["volumeSize"] = kipl::strings::value2string(volume_size[0])+" " +kipl::strings::value2string(volume_size[1])+" " +kipl::strings::value2string(volume_size[2]);
//    parameters["volumeSpacing"] = kipl::strings::value2string(spacing[0])+" "+kipl::strings::value2string(spacing[1])+" "+kipl::strings::value2string(spacing[2]);

    return parameters;
}


/// Sets the region of interest on the projections.
/// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
void FdkReconBase::SetROI(size_t *roi)
{

//    // commented: not used for now
//    ProjCenter    = mConfig.ProjectionInfo.fCenter;
//    SizeU         = roi[2]-roi[0];
//    if (mConfig.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram)
//        SizeV = roi[3];
//    else
//        SizeV = roi[3]-roi[1];

//    mConfig.ProjectionInfo.roi[0]=roi[0];
//    mConfig.ProjectionInfo.roi[1]=roi[1];
//    mConfig.ProjectionInfo.roi[2]=roi[2];
//    mConfig.ProjectionInfo.roi[3]=roi[3];

//    SizeProj      = SizeU*SizeV;
//    size_t rest=0;
//#ifdef USE_PROJ_PADDING
//    rest = SizeV & 3 ;
//    rest = rest !=0 ? 4 - rest : 0;
//#endif
//    size_t projDims[3]={SizeU, SizeV + rest, nProjectionBufferSize};

//    if (MatrixAlignment==MatrixZXY) {
//        MatrixDims[0]=SizeV;
//        MatrixDims[1]=SizeU;
//        MatrixDims[2]=SizeU;

//        std::swap(projDims[0],   projDims[1]);
//    }
//    else {
//        MatrixDims[0]=SizeU;
//        MatrixDims[1]=SizeU;
//        MatrixDims[2]=SizeV;
//    }

//    volume.Resize(MatrixDims);
//    volume=0.0f;

//    size_t mysize[3] = {500, 500, 1000};

//    volume.Resize(mysize);
//    volume = 0.0f;

//    std::cout << volume.Size(0) << " " << volume.Size(1) << " " << volume.Size(2) << std::endl;

//    stringstream msg;

//    msg<<"Setting up reconstructor with ROI=["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]"<<std::endl;
//    msg<<"Matrix dimensions "<<volume<<std::endl;
//    projections.Resize(projDims);
//    projections=0.0f;
//    msg<<"Projection buffer dimensions "<<projections<<std::endl;
//    logger(kipl::logging::Logger::LogVerbose,msg.str());

    BuildCircleMask();
//    MatrixCenterX = volume.Size(1)/2;

}


/// Add one projection to the back-projection stack
/// \param proj The projection
/// \param angle Acquisition angle
/// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
/// \param bLastProjection termination signal. When true the back-projeciton is finalized.
size_t FdkReconBase::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{

    // -------- not used -----
//       logger(kipl::logging::Logger::LogMessage,"FdkReconBase::Process 2");
//       msg<<"FdkReconBase::Process 2" <<std::endl;
//       if (volume.Size()==0)
//           throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

//       proj.Clone();
//       ProjCenter=mConfig.ProjectionInfo.fCenter;
//       fWeights[nProjCounter]  = weight;
//       fSin[nProjCounter]      = sin(angle*fPi/180.0f);
//       fCos[nProjCounter]      = cos(angle*fPi/180.0f);
//       fStartU[nProjCounter]   = MatrixCenterX*(fSin[nProjCounter]-fCos[nProjCounter])+ProjCenter;
//       float *pProj=NULL; // QUA SEMBREREBEB FARE QLC CON IL CENTRO!!

//            this->reconstruct(proj, angle);

//        nProjCounter++;



//       kipl::base::TImage<float,2> img;
//       proj*=weight;
//       size_t N=0;
//       if (MatrixAlignment==MatrixZXY) {
//           kipl::base::Transpose<float,8> transpose;
//           img=transpose(proj);
//           pProj=img.GetDataPtr();
//           N=img.Size(0);
//       }
//       else {
//           pProj=proj.GetDataPtr();
//           N=proj.Size(0);
//       }

//   #ifdef USE_PROJ_PADDING
//       for (int i=0; i<static_cast<int>(projections.Size(1)); i++) {
//           memcpy(projections.GetLinePtr(i,nProjCounter),pProj+i*N,N*sizeof(float));
//       }
//   #else
//       memcpy(projections.GetLinePtr(0,nProjCounter),pProj,proj.Size()*sizeof(float));
//   #endif
//       nProjCounter++;
//       if (bLastProjection || (nProjectionBufferSize<=nProjCounter)) {
//           if (nProjectionBufferSize<=nProjCounter)
//               nProjCounter--;
//           this->reconstruct(proj, angle); //
//           nProjCounter=0;
//       }

//       std::cout << "nProjCounter " << nProjCounter << std::endl;


//       return nProjCounter;
    return 0L;
}


/// Starts the back-projection process of projections stored as a 3D volume.
/// \param proj The projection data
/// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
size_t FdkReconBase::Process(kipl::base::TImage<float,3> projections, std::map<std::string, std::string> parameters)
{
       logger(kipl::logging::Logger::LogMessage,"FdkReconBase::Process 1");

       if (volume.Size()==0)
           throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

//       BuildCircleMask();

       kipl::base::TImage<float,2> img(projections.Dims());

       size_t nProj=projections.Size(2);
       std::cout<< "NProj:   "<<nProj << std::endl; // numero totale di proiezioni

       // Extract the projection parameters
//       float *weights=new float[nProj+16];
       float *weights=new float[nProj];
       GetFloatParameterVector(parameters,"weights",weights,nProj);


       float *angles=new float[nProj];
       GetFloatParameterVector(parameters,"angles",angles,nProj);

       // Process the projections
       float *pImg=img.GetDataPtr();
       float *pProj=nullptr;



       // Update piercing point position in projection rois

       std::cout << "projection roi: " << mConfig.ProjectionInfo.roi[0] << " " << mConfig.ProjectionInfo.roi[1] << " " << mConfig.ProjectionInfo.roi[2] << " " << mConfig.ProjectionInfo.roi[3] << std::endl;
       std::cout << "piercing point: " << mConfig.ProjectionInfo.fpPoint[0] << " " << mConfig.ProjectionInfo.fpPoint[1] << std::endl;
       std::cout << "nProjectionBufferSize: " << nProjectionBufferSize << std::endl;
       mConfig.ProjectionInfo.fpPoint[0] -= mConfig.ProjectionInfo.roi[0];
       mConfig.ProjectionInfo.fpPoint[1] -= mConfig.ProjectionInfo.roi[1];


        std::cout << "Processing projection with angle: " << std::endl;

        kipl::profile::Timer fdkTimer;
        fdkTimer.Tic();
        InitializeBuffers(img.Size(0),img.Size(1));
        for (int i=0; (i<nProj) && (!m_Interactor->SetProgress(float(i)/nProj,"FDK back-projection")); i++) {

           pProj=projections.GetLinePtr(0,i);
           memcpy(pImg,pProj,sizeof(float)*img.Size());

           img *= weights[i];
//           std::cout << "weigth: " << weights[i] << std::endl;

          this->reconstruct(img, angles[i], nProj);


       }

        FinalizeBuffers();
         fdkTimer.Toc();
         std::cout << "fdkTimer: " << fdkTimer << std::endl;

//         for (int k=0; k<volume.Size(2); ++k){
//             memcpy(volume.GetLinePtr(0,k), cbct_volume.GetLinePtr(0,k), sizeof(float)*volume.Size(0)*volume.Size(1));
//         }

         // go into the parallel case reference system
         kipl::base::TRotate<float> rotate;
         kipl::base::TImage<float,2> ori, rotated;
         size_t dims2d[2] = {cbct_volume.Size(0), cbct_volume.Size(1)};
         ori.Resize(dims2d);
         rotated.Resize(dims2d);

         for (int k=0; k<volume.Size(2); ++k){
             memcpy(ori.GetDataPtr(), cbct_volume.GetLinePtr(0,k), sizeof(float)*cbct_volume.Size(0)*cbct_volume.Size(1));
             rotated = rotate.MirrorHorizontal(ori);
             memcpy(volume.GetLinePtr(0,volume.Size(2)-k-1),rotated.GetDataPtr(),sizeof(float)*cbct_volume.Size(0)*cbct_volume.Size(1));
//             memcpy(volume.GetLinePtr(0,volume.Size(2)-k-1),cbct_volume.GetLinePtr(0,k),sizeof(float)*cbct_volume.Size(0)*cbct_volume.Size(1));
         }



       delete [] weights;
       delete [] angles;

    return 0L;
}

void FdkReconBase::GetHistogram(float *axis, size_t *hist,size_t nBins) {


    float matrixMin=Min();
    float matrixMax=Max();

    ostringstream msg;
    msg<<"Preparing histogram; #bins: "<<nBins<<", Min: "<<matrixMin<<", Max: "<<matrixMax;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    memset(axis,0,sizeof(float)*nBins);
    memset(hist,0,sizeof(size_t)*nBins);
    float scale=(matrixMax-matrixMin)/(nBins+1);
    float invScale=1.0f/scale;
    size_t index=0;
    for (size_t z=0; z<volume.Size(2); z++) {
        for (size_t y=0; y<volume.Size(1); y++) {
            float *pLine=volume.GetLinePtr(y,z);
            for (size_t x=0; x<volume.Size(0); x++) {
                index=static_cast<size_t>(invScale*(pLine[x]-matrixMin));
                if (pLine[x]<matrixMin)
                    index=0;
                else if (nBins<=index)
                    index=nBins-1;
                hist[index]++;
            }
        }
    }

    axis[0]=matrixMin+scale/2.0f;
    for (size_t i=1; i<nBins; i++)
        axis[i]=axis[i-1]+scale;

}

float FdkReconBase::Min()
{
    float minval=std::numeric_limits<float>::max();


//    if (MatrixAlignment==FdkReconBase::MatrixXYZ) {
//        logger(kipl::logging::Logger::LogWarning,"Min is not implemented for MatrixXYZ");
//        throw ReconException("Min is not implemented for MatrixXYZ",__FILE__,__LINE__);
//    }
//    else {
    // should be computed on mask not to have too many values of the background.. but I haven't computed the mask yet..
        for (size_t z=0; z<volume.Size(2); z++) {
            for (size_t y=0; y<volume.Size(1); y++) {
                float *pLine=volume.GetLinePtr(y,z);
                minval=min(minval,*min_element(pLine,pLine+volume.Size(0)));
            }
        }
//    }
//        float *pLine = volume.GetDataPtr();
//        minval = min(minval, *min(pLine, pLine+volume.Size(0)*volume.Size(1)*volume.Size(2)));


    return minval;
}

float FdkReconBase::Max()
{
    float maxval=-std::numeric_limits<float>::max();

//    if (MatrixAlignment==FdkReconBase::MatrixXYZ) {
//        logger(kipl::logging::Logger::LogWarning,"Max is not implemented for MatrixXYZ");
//        throw ReconException("Max is not implemented for MatrixXYZ",__FILE__,__LINE__);
//    }
//    else {
        for (size_t z=0; z<volume.Size(2); z++) {
            for (size_t y=0; y<volume.Size(1); y++) {
                float *pLine=volume.GetLinePtr(y,z);
                maxval=max(maxval,*max_element(pLine,pLine+volume.Size(0)));
            }
        }
//    }

    //    float *pLine = volume.GetDataPtr();
    //    maxval = max_element(volume.GetDataPtr(), volume.GetDataPtr()+volume.Size(0)*volume.Size(1)*volume.Size(2));

    return maxval;
}
