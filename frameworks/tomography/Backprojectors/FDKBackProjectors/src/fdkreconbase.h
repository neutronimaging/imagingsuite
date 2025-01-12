//<LICENSE>
#ifndef FDKRECONBASE_H
#define FDKRECONBASE_H

#include "fdkbackproj_global.h"
#include <BackProjectorModuleBase.h>
#include <ParameterHandling.h>
#include <string>
#include <list>
//<LICENSE>

#include <vector>
#include <base/timage.h>
#include <math/LinearAlgebra.h>
#include <ReconConfig.h>
#include <interactors/interactionbase.h>
#include <logging/logger.h>
#include <projectionfilter.h>

class FDKBACKPROJSHARED_EXPORT FdkReconBase : public BackProjectorModuleBase
{
public:
    FdkReconBase(std::string application, std::string name, eMatrixAlignment alignment, kipl::interactors::InteractionBase *interactor=nullptr);
    ~FdkReconBase(void);

    /// Sets up the back-projector with new parameters
    /// \param config Reconstruction parameter set
    /// \param parameters Additional set of configuration parameters
    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

    /// Initializing the reconstructor
    int Initialize() override;
    virtual int InitializeBuffers(int width, int height);
    virtual int FinalizeBuffers() ;

    /// Add one projection to the back-projection stack
    /// \param proj The projection
    /// \param angle Acquisition angle
    /// \param weight Intensity scaling factor for interpolation when the angles are non-uniformly distributed
    /// \param bLastProjection termination signal. When true the back-projeciton is finalized.
    virtual size_t Process(kipl::base::TImage<float,2> proj, float angle, float weight, size_t nProj, bool bLastProjection);
	using BackProjectorModuleBase::Process;
    /// Starts the back-projection process of projections stored as a 3D volume. Projections are then passed to the FDK backprojector
    /// \param proj The projection data
    /// \param parameters A list of parameters, the list shall contain at least the parameters angles and weights each containing a space separated list with as many values as projections
    size_t Process(kipl::base::TImage<float,3> projections, std::map<std::string, std::string> parameters) override;


    /// Gets a list parameters required by the module.
    /// \returns The parameter list
    std::map<std::string, std::string> GetParameters() override;

    /// Sets the region of interest on the projections.
    /// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
    void SetROI(const std::vector<size_t> &roi) override;

    /// Get the histogram of the reconstructed matrix. This should be calculated in the masked region only to avoid unnescessary zero counts.
    /// \param x the bin values of the x axis
    /// \param y the histogram bins
    /// \param N number of bins
    void GetHistogram(float *axis, size_t *hist,size_t nBins) override;

    std::vector<size_t> GetMatrixDims() override;

    virtual float Min();

    virtual float Max();




protected:
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj)=0;   
    virtual size_t ComputeGeometryMatrices(float *matrices);

    struct ProjectionInfo {
        ProjectionInfo(const float _angle, const float _weight) :
            angle(_angle), weight(_weight) {}
        ProjectionInfo(const ProjectionInfo & p) : angle(p.angle), weight(p.weight){}
        const ProjectionInfo & operator=(const ProjectionInfo & p) {angle=p.angle; weight=p.weight; return *this;}
        float angle;
        float weight;
    };

    std::vector<std::pair<ProjectionInfo, kipl::base::TImage<float, 2> > > ProjectionList;
    kipl::base::TImage<float,3> projections;
    kipl::base::TImage<float,3> cbct_volume;
    size_t nProjCounter; //!< Counts the projections in the buffer
    size_t SizeU;
    size_t SizeV;
    size_t SizeProj;
    size_t MatrixCenterX;

    float ProjCenter;
//    float fWeights[1024];
//    float fSin[1024];
//    float fCos[1024];
//    float fStartU[1024];
//    float fLocalStartU[1024];
    float *proj_matrices;

    size_t nProjectionBufferSize;
    size_t nSliceBlock;
//    size_t nSubVolume[2];
    std::vector<size_t> volume_size;
    float spacing[3];
    float fRotation;

    ImagingAlgorithms::ProjectionFilter filter;

};

#endif // FDKRECONBASE_H
