#include "../../include/base/KiplException.h"
#include "../../include/scalespace/filterenums.h"

std::ostream & operator<<(std::ostream & s, akipl::scalespace::eInitialImageType it)
{
    s<<enum2string(it);
    return s;
}

std::string enum2string(akipl::scalespace::eInitialImageType it)
{
    std::string str;
	switch (it) {
        case akipl::scalespace::InitialImageOriginal : str="InitialImageOriginal"; break;
        case akipl::scalespace::InitialImageZero     : str="InitialImageZero";     break;
    default: throw kipl::base::KiplException("Unknown initial image type",__FILE__,__LINE__);
	};

    return str;
}

void string2enum(std::string str, akipl::scalespace::eInitialImageType &it) 
{
	if (str=="InitialImageOriginal")
		it=akipl::scalespace::InitialImageOriginal;
    else if (str=="InitialImageZero")
		it=akipl::scalespace::InitialImageZero;
    else
       throw kipl::base::KiplException("Unknown initial image type",__FILE__,__LINE__);

}

std::ostream & operator<<(std::ostream &s, akipl::scalespace::eFilterType ft)
{
	switch (ft) {
		case akipl::scalespace::FilterISS : s<<"FilterISS"; break;
		case akipl::scalespace::FilterISSp : s<<"FilterISSp"; break;
		case akipl::scalespace::FilterDiffusion : s<<"FilterDiffusion"; break;
		case akipl::scalespace::FilterIteratedMedian: s<<"FilterIteratedMedian"; break;
		case akipl::scalespace::FilterGauss: s<<"FilterGauss"; break;
		default : throw kipl::base::KiplException("Unknown filter type",__FILE__,__LINE__);
	}

	return s;
}

void string2enum(std::string str, akipl::scalespace::eFilterType &ft)
{
	if (str=="FilterISS") ft=akipl::scalespace::FilterISS;
	else
	if (str=="FilterISSp") ft=akipl::scalespace::FilterISSp;
	else
	if (str=="FilterDiffusion") ft=akipl::scalespace::FilterDiffusion;
	else
	if (str=="FilterIteratedMedian") ft=akipl::scalespace::FilterIteratedMedian;
	else
	if (str=="FilterGauss") ft=akipl::scalespace::FilterGauss;
	else
	throw kipl::base::KiplException("Could not convert string to enum",__FILE__,__LINE__);

}

std::ostream & operator<<(std::ostream & s, akipl::scalespace::eRegularizationType rt)
{
    s<<enum2string(rt);

    return s;
}

std::string enum2string(akipl::scalespace::eRegularizationType rt)
{
    std::string str;

    switch (rt) {
        case akipl::scalespace::RegularizationTV1 :
            str="RegularizationTV1";
            break;
        case akipl::scalespace::RegularizationTV2 :
            str="RegularizationTV2";
            break;
        default : throw kipl::base::KiplException("Unknown Regularization type",__FILE__,__LINE__);
    }

    return str;
}

void string2enum(std::string str, akipl::scalespace::eRegularizationType &rt)
{
    if (str == "RegularizationTV1")
        rt=akipl::scalespace::RegularizationTV1;
    else if (str == "RegularizationTV2")
        rt=akipl::scalespace::RegularizationTV2;
    else
        throw kipl::base::KiplException("Could not convert string to enum",__FILE__,__LINE__);

}
