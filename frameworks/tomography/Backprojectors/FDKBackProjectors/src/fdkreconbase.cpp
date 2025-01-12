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

FdkReconBase::FdkReconBase(std::string /*application*/, std::string name, eMatrixAlignment alignment, kipl::interactors::InteractionBase *interactor) :
    BackProjectorModuleBase("muhrec",name,alignment,interactor),
    nProjCounter(0),
    SizeU(0),
    SizeV(0),
    SizeProj(0),
    MatrixCenterX(0),
    ProjCenter(0.0f),
    nProjectionBufferSize(2000),
    nSliceBlock(32),
    fRotation(0.0f), // not sure if i need these parameters.. for sure i would need the others that we have added in reconconfig
    filter(nullptr)

{
   logger(kipl::logging::Logger::LogMessage,"c'tor FdkProjectorBase");
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

    BackProjectorModuleBase::Configure(config,parameters);

    nProjectionBufferSize = GetIntParameter(parameters,"ProjectionBufferSize");
    nSliceBlock           = GetIntParameter(parameters,"SliceBlock");
//    GetUIntParameterVector(parameters,"SubVolume",nSubVolume,2);

    size_t NProj  = (mConfig.ProjectionInfo.nLastIndex-mConfig.ProjectionInfo.nFirstIndex+1)/mConfig.ProjectionInfo.nProjectionStep;
    proj_matrices = new float[12*NProj]; // not sure that the configure is not called every slice block.

    filter.setParameters(parameters);
    return 0;
}

/// Initializing the reconstructor
int FdkReconBase::Initialize()
{

    return 0;
}

int FdkReconBase::InitializeBuffers(int /*width*/, int /*height*/)
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
    parameters = BackProjectorModuleBase::GetParameters();

    auto fp = filter.parameters();
    parameters.insert(fp.begin(),fp.end());

    parameters["ProjectionBufferSize"]=kipl::strings::value2string(nProjectionBufferSize);
    parameters["SliceBlock"]= kipl::strings::value2string(nSliceBlock);
//    parameters["SubVolume"]=kipl::strings::value2string(nSubVolume[0])+" "+kipl::strings::value2string(nSubVolume[1]);

    return parameters;
}


/// Sets the region of interest on the projections.
/// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
void FdkReconBase::SetROI(const std::vector<size_t> &roi)
{
    // now here i Get the CBCT roi! here it should be the small one
    //    std::cout << "CBCTroi in fdkReconBase: "  << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3]<< std::endl;
    ProjCenter    = mConfig.ProjectionInfo.fCenter;
    SizeU         = roi[2]-roi[0];

    if (mConfig.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram)
        SizeV = roi[3];
    else
        SizeV = roi[3]-roi[1];

    mConfig.ProjectionInfo.roi=roi;

    volume_size = mConfig.MatrixInfo.nDims;
    // original code had this line ... volume_size[1] = mConfig.MatrixInfo.nDims[2];


    SizeProj      = SizeU*SizeV;
    size_t rest=0;
#ifdef USE_PROJ_PADDING
    rest = SizeV & 3 ;
    rest = rest !=0 ? 4 - rest : 0;
#endif
    std::vector<size_t> projDims={SizeU, SizeV + rest, nProjectionBufferSize};

    std::vector<size_t> matrixDims;
    if (MatrixAlignment==MatrixZXY)
    {
        matrixDims = { SizeV, SizeU, SizeU };

        std::swap(projDims[0],   projDims[1]);
    }
    else
    {
        matrixDims = { SizeU, SizeU, SizeV };
    }

    volume.resize(matrixDims);
    volume=0.0f;

    cbct_volume.resize(matrixDims);
    cbct_volume=0.0f;


    stringstream msg;
    msg<<"Setting up reconstructor with ROI=["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]"<<std::endl;
    msg<<"Matrix dimensions "<<volume<<std::endl;
    projections.resize(projDims); // this is wrong, but I don-t use the projection buffer size anyway.. so at this point I don't mind
    projections=0.0f;
    msg<<"Projection buffer dimensions "<<projections<<std::endl;
    logger(kipl::logging::Logger::LogVerbose,msg.str());

    BuildCircleMask();
    MatrixCenterX = volume.Size(1)/2;

}


/// Add one projection to the back-projection stack
/// \param proj The projection
/// \param angle Acquisition angle
/// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
/// \param bLastProjection termination signal. When true the back-projeciton is finalized.
size_t FdkReconBase::Process(kipl::base::TImage<float,2> proj, float angle, float weight, size_t nProj, bool bLastProjection)
{
    logger.verbose("FdkReconBase::Process 2D");
    std::ostringstream msg;
    if (volume.Size()==0)
        throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

    proj.Clone();
    ProjCenter=mConfig.ProjectionInfo.fCenter;

    filter.process(proj);
    float *pProj=nullptr;

    kipl::base::TImage<float,2> img;
    proj*=weight;
    // size_t N=0;
    if (MatrixAlignment==MatrixZXY)
    {
        kipl::base::Transpose<float,8> transpose;
        img=transpose(proj);
        pProj=img.GetDataPtr();
        // N=img.Size(0);
    }
    else
    {
        pProj=proj.GetDataPtr();
        // N=proj.Size(0);
    }

#ifdef USE_PROJ_PADDING
    for (int i=0; i<static_cast<int>(projections.Size(1)); i++)
    {
        memcpy(projections.GetLinePtr(i,nProjCounter),pProj+i*N,N*sizeof(float));
    }
#else
    memcpy(projections.GetLinePtr(0,nProjCounter),pProj,proj.Size()*sizeof(float));
#endif
    nProjCounter++;
    if (bLastProjection || (nProjectionBufferSize<=(nProjCounter)))
    {
        if (nProjectionBufferSize<=nProjCounter)
            nProjCounter--;

        msg.str("");
        msg<<"Counter="<<nProjCounter<<", buffer size="<<nProjectionBufferSize<<" last "<<(bLastProjection ? "True" : "False");
        logger(logger.LogMessage,msg.str());
        this->reconstruct(proj,angle,nProj);
        nProjCounter=0;
    }

    return nProjCounter;
}


/// Starts the back-projection process of projections stored as a 3D volume.
/// \param proj The projection data
/// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
size_t FdkReconBase::Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters)
{
    timer.Tic();
    logger.message("FdkReconBase::Process 3D");

    if (volume.Size()==0)
        throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

    kipl::base::TImage<float,2> img(proj.dims());

    size_t nProj=proj.Size(2);

    // Extract the projection parameters
    std::vector<float> weights;
    GetFloatParameterVector(parameters,"weights", weights, nProj);

    std::vector<float> angles;
    GetFloatParameterVector(parameters,"angles",  angles,  nProj);

    std::ostringstream msg;
    msg<<"Got "<<weights.size()<<" weights and "<<angles.size()<<" angles, image dims=["<<img.Size(0)<<", "<<img.Size(1)<<"]";
    logger.verbose(msg.str());

    // Process the projections
    float *pImg=img.GetDataPtr();
    float *pProj=nullptr;

    logger.verbose("Starting recon loop over projections");

    for (size_t i=0; (i<nProj) && (!UpdateStatus(static_cast<float>(i)/nProj,"FDK back-projection")); ++i)
    {
        msg.str("");
        msg<<"Reconstructing projection" << i;
        logger.verbose(msg.str());
        pProj=proj.GetLinePtr(0,i);

        std::copy_n(pProj,img.Size(),pImg);
        img *= weights[i];

        //           Process(img,angles[i],weights[i],i,i==(nProj-1)); // to ask for
        this->reconstruct(img, angles[i], nProj);
    }

//    FinalizeBuffers();
    logger.verbose("Recon loop finished");

    //         fdkTimer.Toc();
    //         std::cout << "fdkTimer: " << fdkTimer << std::endl;

    // go into the parallel case reference system
    std::vector<size_t> dims2d = {cbct_volume.Size(0), cbct_volume.Size(1)};
    kipl::base::TRotate<float> rotate;
    kipl::base::TImage<float,2> ori(dims2d);
    kipl::base::TImage<float,2> rotated(dims2d);

    ori.resize(dims2d);
    rotated.resize(dims2d);

    for (size_t k=0; k<volume.Size(2); ++k)
    {
        memcpy(ori.GetDataPtr(), cbct_volume.GetLinePtr(0,k), sizeof(float)*cbct_volume.Size(0)*cbct_volume.Size(1));
        rotated = rotate.MirrorHorizontal(ori);
        memcpy(volume.GetLinePtr(0,volume.Size(2)-k-1),rotated.GetDataPtr(),sizeof(float)*cbct_volume.Size(0)*cbct_volume.Size(1));
    }

    timer.Toc();
    //    delete [] weights;
    //    delete [] angles;
    return 0L;
}

std::vector<size_t> FdkReconBase::GetMatrixDims()
{
    std::vector<size_t> dims;
    if (MatrixAlignment==MatrixZXY)
    {
        dims= { volume.Size(1), volume.Size(2), volume.Size(0) };
    }
    else
    {
        dims = volume.dims();
    }

    return dims;
}

void FdkReconBase::GetHistogram(float *axis, size_t *hist,size_t nBins)
{
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
    for (size_t z=0; z<volume.Size(2); ++z)
    {
        for (size_t y=0; y<volume.Size(1); ++y)
        {
            float *pLine=volume.GetLinePtr(y,z);
            for (size_t x=0; x<volume.Size(0); ++x)
            {
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
    for (size_t i=1; i<nBins; ++i)
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
    for (size_t z=0; z<volume.Size(2); z++)
    {
        for (size_t y=0; y<volume.Size(1); y++)
        {
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
        for (size_t z=0; z<volume.Size(2); ++z)
        {
            for (size_t y=0; y<volume.Size(1); ++y)
            {
                float *pLine=volume.GetLinePtr(y,z);
                maxval=max(maxval,*max_element(pLine,pLine+volume.Size(0)));
            }
        }
//    }

    //    float *pLine = volume.GetDataPtr();
    //    maxval = max_element(volume.GetDataPtr(), volume.GetDataPtr()+volume.Size(0)*volume.Size(1)*volume.Size(2));

    return maxval;
}

size_t FdkReconBase::ComputeGeometryMatrices(float */*matrices*/){
    return 1;
}
