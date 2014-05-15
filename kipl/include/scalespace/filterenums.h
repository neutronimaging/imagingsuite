#ifndef __FILTERENUMS_H
#define __FILTERENUMS_H

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

std::ostream & operator<<(std::ostream &s, akipl::scalespace::eFilterType ft);
void string2enum(std::string str, akipl::scalespace::eFilterType &ft);

std::ostream & operator<<(std::ostream & s, akipl::scalespace::eInitialImageType it);
void string2enum(std::string str, akipl::scalespace::eInitialImageType &it);
std::string enum2string(akipl::scalespace::eInitialImageType it);

std::ostream & operator<<(std::ostream & s, akipl::scalespace::eRegularizationType rt);
std::string enum2string(akipl::scalespace::eRegularizationType rt);
void string2enum(std::string str, akipl::scalespace::eRegularizationType &rt);
#endif


