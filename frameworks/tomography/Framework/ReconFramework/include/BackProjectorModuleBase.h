//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//

#ifndef __BACKPROJECTORBASE_H
#define __BACKPROJECTORBASE_H
#include "ReconFramework_global.h"
//#include "stdafx.h"
#include <string>

#include <base/timage.h>
#include <logging/logger.h>
#include <profile/Timer.h>
#include <interactors/interactionbase.h>

#include "ReconConfig.h"

/// Abstract base class for backprojection modules. It can used as base as is but is mostly refined by a second based class.
class RECONFRAMEWORKSHARED_EXPORT BackProjectorModuleBase
{
protected:
    kipl::logging::Logger logger; ///< Logger instance for back-projection objects
public:
    /// Enum to select data alignment strategy in memory, ZXY is mostly more efficient than XYZ
	enum eMatrixAlignment {
        MatrixXYZ,      ///< The data is arranged such that the adjacent elements are following the x-axis. The largest distances are on z-axis.
        MatrixZXY       ///< The data is arranged such that the adjacent elements are following the z-axis. The largest distances are on y-axis.
	};
    /// The alignment type is algorithm dependent and should only be set once for a class
	const eMatrixAlignment MatrixAlignment;

    /// Constructor to specify basic characteristics of the backprojector
    /// \param application
    /// \param name Specifies the name of the backprojector, is mainly used by the logger.
    /// \param alignment Specifies the memory alignment supported by the backprojector.
    /// \param interactor reference to an interactor object to report the backprojection progress etc.
    BackProjectorModuleBase(std::string application, std::string name, eMatrixAlignment alignment, kipl::interactors::InteractionBase *interactor=nullptr);

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
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters)=0;

    /// Initializing the back-projector
	virtual int Initialize()=0;

    /// Gets a list parameters required by the module.
    /// \returns The parameter list
	virtual std::map<std::string, std::string> GetParameters()=0;

    /// Sets the region of interest on the projections.
    /// \param roi A four-entry array of ROI coordinates (x0,y0,x1,y1)
	virtual void SetROI(size_t *roi)=0;

    /// Get the reconstructed matrix
    /// \returns The matrix as TImage object
	kipl::base::TImage<float,3> GetVolume() {return volume;}

    /// Gets a slice in the xy-plane from the reconstructed matrix
    /// \param idx index of the slice to get (todo check if idx is local or global)
    /// \returns The requested slice as a TImage object
	kipl::base::TImage<float,2> GetSlice(size_t idx);

    /// Get the number of reconstucted slices
    /// \returns the number of slices
	size_t GetNSlices();

    /// Get the name of the backprojection module
    /// \returns the name of the module as a std::string
	std::string Name() {return m_sModuleName;}

    /// Get the size of the matrix
    /// \param dims a three-element array to store the size of the matrix
	virtual void GetMatrixDims(size_t *dims) {dims[0]=volume.Size(0); dims[1]=volume.Size(1); dims[2]=volume.Size(2);}

    /// Get the histogram of the reconstructed matrix. This should be calculated in the masked region only to avoid unnescessary zero counts.
    /// \param x the bin values of the x axis
    /// \param y the histogram bins
    /// \param N number of bins
	virtual void GetHistogram(float *x, size_t *y, size_t N)=0;

    /// \brief Returns the reconstructed voxel size. Default value is the projection pixel size, this must be adjusted by the magnification for cone beam.
   virtual float GetVoxelSize() {return mConfig.ProjectionInfo.fResolution[0];}

    /// Destructor to clean up allocated memory.
    virtual ~BackProjectorModuleBase(void);

    /// Execution time of the latest back-projection run.
    /// \returns The execution time in seconds.
	double ExecTime() {return timer.ElapsedSeconds(); }

protected:
    /// \brief Clears circle mask and matrix dimensions
    virtual void ClearAll();

    /// \brief Builds a circle mask based on the defined reconstruction geometry. A list containing start-stop pairs per line is built.
    void BuildCircleMask();


    /// Interface to a progress bar in a GUI.
    /// \param val a fraction value 0.0-1.0 to tell the progress of the back-projection.
    /// \param msg a message string to add information to the progress bar.
    /// \returns The abort status of interactor object. True means abort back-projection and false continue.
	bool UpdateStatus(float val, std::string msg);
    ReconConfig mConfig; ///< Configuration information

    std::string m_sModuleName;                   ///< The name of the back-projection module.
    kipl::base::TImage<float,3> volume;          ///< The matrix where the back-projection is stored during the process.
    std::vector<std::pair<size_t,size_t> > mask; ///< List of start and stop positions forming a masked area where the data is reconstructed.
    kipl::profile::Timer timer;                  ///< A timer used to measure the exectution time of the back-projctor.
    size_t MatrixDims[3];

    std::string m_sApplication;                  ///< The name of the application calling the module
    kipl::interactors::InteractionBase *m_Interactor;               ///< Interface to a progress bar in the GUI.
};

#endif
