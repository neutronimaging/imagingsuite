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
    ProjectionFilterSingle(kipl::interactors::InteractionBase *interactor);
	virtual ~ProjectionFilterSingle(void);

    virtual int Configure(ReconConfig config, std::map<std::string,std::string> parameters);
    virtual std::map<std::string, std::string> GetParameters();
    const vector<float> & filterWeights() {return mFilter.filterWeights();}

private:
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    ImagingAlgorithms::ProjectionFilter mFilter;
};

#endif
