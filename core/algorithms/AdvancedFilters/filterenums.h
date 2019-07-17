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

enum eGradientType {
    Simple_Grad_Centered = 0,   /// 1 0 -1 gradient
    Jahne_Grad           = 1,   /// Gradient according to Jahne
    Simple_Grad_Minus    = 2,   /// Gradient -1 1
    Diff_Grad_Minus      = 3,   /// Gradient -0.5 0.5
    BinDiff_Grad         = 4	/// Gradient Bz By Dx etc.
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


