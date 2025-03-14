//<LICENSE>

#ifndef _PROJECTIONFILTER_H_
#define _PROJECTIONFILTER_H_

#include "StdPreprocModules_global.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <PreprocModuleBase.h>

#include <logging/logger.h>
#include <base/timage.h>
#include <fft/fftbase.h>
#include <interactors/interactionbase.h>

#include <projectionfilter.h>

class STDPREPROCMODULESSHARED_EXPORT ProjectionFilterSingle :
    public PreprocModuleBase
{
public:
    using PreprocModuleBase::Configure;
    ProjectionFilterSingle(kipl::interactors::InteractionBase *interactor);
	virtual ~ProjectionFilterSingle(void);

    int Configure(ReconConfig config, std::map<std::string,std::string> parameters) override;
    std::map<std::string, std::string> GetParameters();
    const vector<float> & filterWeights() {return mFilter.filterWeights();}

private:
    using PreprocModuleBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;
    ImagingAlgorithms::ProjectionFilter mFilter;
};

#endif
