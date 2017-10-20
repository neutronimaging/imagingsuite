//
// This file is part of the Inspector library by Anders Kaestner
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

#ifndef __PREPROCMODULEBASE_H
#define __PREPROCMODULEBASE_H
#include "ReconFramework_global.h"
//#include "stdafx.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <profile/Timer.h>
#include <string>
#include <strings/miscstring.h>
#include <map>

#include <ProcessModuleBase.h>
#include <interactors/interactionbase.h>

#include "ReconConfig.h"

/// Base for preprocessing modules to provide basic ct preproc functionality
class  RECONFRAMEWORKSHARED_EXPORT PreprocModuleBase : public ProcessModuleBase
{
public:
    /// Constructor that essentially forwards the parameterto the base.
    /// \param name The name of the module.
    PreprocModuleBase(std::string name="PreprocModuleBase", kipl::interactors::InteractionBase *interactor=nullptr);

    /// Configures the module with parameters.
    /// \param config The config struct with general information.
    /// \param parameters Module specific parameters.
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters)=0;

    /// Sets the region of interest for the current processing block
    /// \param roi vector with the roi coordinates (x0,y0,x1,y1)
    /// \returns True if the roi information was used.
    virtual bool SetROI(size_t * roi);

    /// Destructor to clean up
	virtual ~PreprocModuleBase(void);

    /// The current repository version of the module.
    /// \returns A version string.
    virtual std::string Version();

protected:
    /// Extracts a sinogram from the projection data block.
    /// \param projections The projection data block
    /// \param sinogram The exracted sinogram
    /// \param idx index of the sinogram to extract
    /// \returns Always zero.
	int ExtractSinogram(kipl::base::TImage<float,3> &projections, kipl::base::TImage<float,2> &sinogram, size_t idx);

    /// Inserts a sinogram in the projection data block
    /// \param sinogram The sinogram to insert in the data block.
    /// \param projections The projection data block.
    /// \param idx Index of the location to insert the sinogram.
	int InsertSinogram(kipl::base::TImage<float,2> &sinogram, kipl::base::TImage<float,3> &projections, size_t idx);

    /// Hides the Configure method in the base class
    /// \param parameters A list of parameters to configure the module.
    virtual int Configure(std::map<std::string, std::string> parameters);

    /// The numerical version number from the repository
	virtual int SourceVersion();
};


#endif
