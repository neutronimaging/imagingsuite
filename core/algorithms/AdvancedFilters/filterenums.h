//<LICENSE>

#ifndef FILTERENUMS_H
#define FILTERENUMS_H

#include "advancedfilters_global.h"

#include <iostream>
#include <string>

namespace advancedfilters {
enum eInitialImageType {
		InitialImageOriginal,
		InitialImageZero
};

enum eFilterType {
	FilterISS,
	FilterISSp,
	FilterDiffusion,
	FilterIteratedMedian,
	FilterGauss
};

enum eRegularizationType {
    RegularizationTV1 = 0,
    RegularizationTV2 = 1
};

}

ADVANCEDFILTERSSHARED_EXPORT std::ostream & operator<<(std::ostream &s, advancedfilters::eFilterType ft);
ADVANCEDFILTERSSHARED_EXPORT void string2enum(const std::string &str, advancedfilters::eFilterType &ft);

ADVANCEDFILTERSSHARED_EXPORT std::ostream & operator<<(std::ostream & s, advancedfilters::eInitialImageType it);
ADVANCEDFILTERSSHARED_EXPORT void string2enum(const std::string & str, advancedfilters::eInitialImageType &it);
ADVANCEDFILTERSSHARED_EXPORT std::string enum2string(advancedfilters::eInitialImageType it);

ADVANCEDFILTERSSHARED_EXPORT std::ostream & operator<<(std::ostream & s, advancedfilters::eRegularizationType rt);
ADVANCEDFILTERSSHARED_EXPORT std::string enum2string(advancedfilters::eRegularizationType rt);
ADVANCEDFILTERSSHARED_EXPORT void string2enum(const std::string &str, advancedfilters::eRegularizationType &rt);
#endif


