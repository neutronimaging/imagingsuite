#include "iterativereconbase.h"

IterativeReconBase::IterativeReconBase(std::string application, std::string name, eMatrixAlignment alignment, kipl::interactors::InteractionBase *interactor) :
    BackProjectorModuleBase(application,name,alignment,interactor),
    m_fp(nullptr),
    m_bp(nullptr),
    m_nIterations(10)
{

}

IterativeReconBase::~IterativeReconBase()
{

}

/// Sets up the back-projector with new parameters
/// \param config Reconstruction parameter set
/// \param parameters Additional set of configuration parameters
int IterativeReconBase::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{

    return 0;
}

/// Initializing the reconstructor
int IterativeReconBase::Initialize()
{

    return 0;
}

/// Gets a list parameters required by the module.
/// \returns The parameter list
std::map<std::string, std::string> IterativeReconBase::GetParameters()
{
   std::map<std::string, std::string> params;

   return params;
}

/// Sets the region of interest on the projections.
/// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
void IterativeReconBase::SetROI(size_t *roi)
{

}

/// Add one projection to the back-projection stack
/// \param proj The projection
/// \param angle Acquisition angle
/// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
/// \param bLastProjection termination signal. When true the back-projeciton is finalized.
size_t IterativeReconBase::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{
    return 0L;
}

/// Starts the back-projection process of projections stored as a 3D volume.
/// \param proj The projection data
/// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
size_t IterativeReconBase::Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters)
{

    return 0L;
}
