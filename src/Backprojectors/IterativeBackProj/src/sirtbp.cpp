#include "sirtbp.h"

SIRTbp::SIRTbp(InteractionBase *interactor) :
    BackProjectorBase("muhrec","SIRTbp",BackProjectorBase::MatrixXYZ,interactor)
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

    return parameters;
}

/// Sets the region of interest on the projections.
/// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
void SIRTbp::SetROI(size_t *roi)
{

}

/// Get the histogram of the reconstructed matrix. This should be calculated in the masked region only to avoid unnescessary zero counts.
/// \param x the bin values of the x axis
/// \param y the histogram bins
/// \param N number of bins
void SIRTbp::GetHistogram(float *x, size_t *y, size_t N)
{


}
