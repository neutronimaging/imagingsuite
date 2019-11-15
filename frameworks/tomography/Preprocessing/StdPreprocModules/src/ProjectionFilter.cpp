//<LICENSE>

#include "../include/StdPreprocModules_global.h"

#include <ReconException.h>
#include <ReconHelpers.h>
#include <ParameterHandling.h>

#include <math/mathconstants.h>
#include <math/mathfunctions.h>
#include <strings/miscstring.h>
#include <math/compleximage.h>
#include <base/imagecast.h>
#include <io/io_matlab.h>
#include <visualization/GNUPlot.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

#include "../include/ProjectionFilter.h"

int ProjectionFilterSingle::Configure(ReconConfig config, std::map<std::string,std::string> parameters)
{
    mFilter.setParameters(parameters);

    return 0;
}

int ProjectionFilterSingle::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
	if (img.Size()==0)
		throw ReconException("Empty projection image",__FILE__,__LINE__);

    mFilter.process(img);
	return 0;
}

int ProjectionFilterSingle::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	if (img.Size()==0)
		throw ReconException("Empty projection image",__FILE__,__LINE__);

    mFilter.process(img);
	return 0;
}


std::map<std::string, std::string> ProjectionFilterSingle::GetParameters()
{
    return mFilter.parameters();
}

//------------------------------------------------------------
// Projection filter w. float
ProjectionFilterSingle::ProjectionFilterSingle(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("ProjectionFilterSingle",interactor),
    mFilter(interactor)
{
}

ProjectionFilterSingle::~ProjectionFilterSingle(void)
{
}
