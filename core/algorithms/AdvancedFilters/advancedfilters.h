#ifndef ADVANCEDFILTERS_H
#define ADVANCEDFILTERS_H
#include "advancedfilters_global.h"
#include "ISSfilterQ3D.h"
#include "NonLinDiffAOS.h"

namespace advancedfilters {

typedef advancedfilters::ISSfilterQ3D<float> ISSfilterQ3Df;

typedef advancedfilters::NonLinDiffusionFilter<float,2> NonLinDiffusionFilter2Df;
typedef advancedfilters::NonLinDiffusionFilter<float,3> NonLinDiffusionFilter3Df;

}
#endif // ADVANCEDFILTERS_H
