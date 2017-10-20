
#include <interactors/interactionbase.h>
#include "sirtbp.h"



SIRTbp::SIRTbp(kipl::interactors::InteractionBase *interactor) :
    IterativeReconBase("muhrec","SIRTbp",BackProjectorModuleBase::MatrixXYZ,interactor)
{

}

SIRTbp::~SIRTbp()
{

}

/// Add one projection to the back-projection stack
/// \param proj The projection
/// \param angle Acquisition angle
/// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
/// \param bLastProjection termination signal. When true the back-projeciton is finalized.
size_t SIRTbp::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{

    return 0L;
}

/// Starts the back-projection process of projections stored as a 3D volume.
/// \param proj The projection data
/// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
size_t SIRTbp::Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters)
{
//    //    forwproj2D  - forward projector
//    //    backproj2D - backprojector
//    //    x - initial guess
//    //    b - projection data (sinogram)

//    // Matalab initialization code
//    //    [nsino1, nsino2] = size(b);
//    //    N = size(x,1);
//    //    if (N == 1)
//    //        disp('Initialize image!');
//    //    end
//    //    row_sum = forwproj2D(ones(N,N), theta, nsino2);
//    //    column_sum = backproj2D(ones(nsino1, nsino2), theta, N);
//    //    R = zeros(nsino1, nsino2);
//    //    C = zeros(N,N);

//    //    roi1 = find(row_sum(:) > 0);
//    //    roi2 = find(column_sum(column_sum(:) > 0));

//    //    R(roi1) = 1./row_sum(roi1);
//    //    C(roi2) = 1./column_sum(roi2);

//    size_t dims[3]={proj.Size(0), proj.Size(0), proj.Size(2)};
//    volume.Resize(dims);
//    kipl::base::TImage<float,2> x(dims);
//    kipl::base::TImage<float,2> bp(dims);
//    kipl::base::TImage<float,2> r(dims); // todo fix dimensions
//    kipl::base::TImage<float,2> res; // todo fix dimensions
//    kipl::base::TImage<float,2> sino; // todo fix dimensions
//    for (int i=0; i<m_nIterations; i++) {
//        // Matlab iteration code
//        //         r =  forwproj2D(x, theta, nsino2); % forward proj
//        //         res = (b-r);
//        //         res = res.*R;
//        //         bp = backproj2D(res, theta, N);
//        //         bp = bp.*C;
//        //         x = x + bp;
//        //         %x(x<0)=0;
//        m_fp->project(x,theta,r);
//        res = sino - r;
//        res *=R;
//        m_bp->backproject(res,dims[0]/2.0f,theta,bp);
//        bp*=C;
//        x+=bp;
//    }

    return 0L;
}

size_t SIRTbp::reconstruct(kipl::base::TImage<float,3> proj,std::list<float> & angles)
{
    return 0L;
}
/// Sets up the back-projector with new parameters
/// \param config Reconstruction parameter set
/// \param parameters Additional set of configuration parameters
int SIRTbp::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    mConfig=config;

    return 0;
}

/// Initializing the back-projector
int SIRTbp::Initialize()
{

    return 0;
}


/// Gets a list parameters required by the module.
/// \returns The parameter list
std::map<std::string, std::string> SIRTbp::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters=IterativeReconBase::GetParameters();


    return parameters;
}

/// Sets the region of interest on the projections.
/// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
void SIRTbp::SetROI(size_t *roi)
{
    IterativeReconBase::SetROI(roi);
}

/// Get the histogram of the reconstructed matrix. This should be calculated in the masked region only to avoid unnescessary zero counts.
/// \param x the bin values of the x axis
/// \param y the histogram bins
/// \param N number of bins
void SIRTbp::GetHistogram(float *x, size_t *y, size_t N)
{

}
