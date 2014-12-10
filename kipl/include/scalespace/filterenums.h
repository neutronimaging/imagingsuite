#ifndef __FILTERENUMS_H
#define __FILTERENUMS_H
#include "../kipl_global.h"
#include <iostream>
#include <string>

namespace akipl { namespace scalespace {
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

}}

KIPLSHARED_EXPORT std::ostream & operator<<(std::ostream &s, akipl::scalespace::eFilterType ft);
KIPLSHARED_EXPORT void string2enum(std::string str, akipl::scalespace::eFilterType &ft);

KIPLSHARED_EXPORT std::ostream & operator<<(std::ostream & s, akipl::scalespace::eInitialImageType it);
KIPLSHARED_EXPORT void string2enum(std::string str, akipl::scalespace::eInitialImageType &it);
KIPLSHARED_EXPORT std::string enum2string(akipl::scalespace::eInitialImageType it);

KIPLSHARED_EXPORT std::ostream & operator<<(std::ostream & s, akipl::scalespace::eRegularizationType rt);
KIPLSHARED_EXPORT std::string enum2string(akipl::scalespace::eRegularizationType rt);
KIPLSHARED_EXPORT void string2enum(std::string str, akipl::scalespace::eRegularizationType &rt);
#endif


