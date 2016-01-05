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
