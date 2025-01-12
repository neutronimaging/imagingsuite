#ifndef SIRTBP_H
#define SIRTBP_H

#include "iterativebackproj_global.h"
#include "iterativereconbase.h"
#include <ParameterHandling.h>

class SIRTbp : public IterativeReconBase
{
public:
    SIRTbp(kipl::interactors::InteractionBase *interactor=nullptr);
    ~SIRTbp();

    /// Add one projection to the back-projection stack
    /// \param proj The projection
    /// \param angle Acquisition angle
    /// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
    /// \param bLastProjection termination signal. When true the back-projeciton is finalized.
    virtual size_t Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection);

    /// Starts the back-projection process of projections stored as a 3D volume.
    /// \param proj The projection data
    /// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
    virtual size_t Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters);

    /// Sets up the back-projector with new parameters
    /// \param config Reconstruction parameter set
    /// \param parameters Additional set of configuration parameters
    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

    /// Initializing the back-projector
    virtual int Initialize();

    /// Gets a list parameters required by the module.
    /// \returns The parameter list
    virtual std::map<std::string, std::string> GetParameters();

    /// Sets the region of interest on the projections.
    /// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
    virtual void SetROI(const std::vector<size_t> &roi);

    /// Get the histogram of the reconstructed matrix. This should be calculated in the masked region only to avoid unnescessary zero counts.
    /// \param x the bin values of the x axis
    /// \param y the histogram bins
    /// \param N number of bins
    virtual void GetHistogram(float *x, size_t *y, size_t N);

protected:
    virtual size_t reconstruct(kipl::base::TImage<float,3> proj,std::list<float> & angles);
    float m_fAlpha;
};

#endif // SIRTBP_H
