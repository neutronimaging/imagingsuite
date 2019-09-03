//<LICENCE>

#include "advancedfilterexception.h"
#include "filterenums.h"

std::ostream & operator<<(std::ostream & s, advancedfilters::eInitialImageType it)
{
    s<<enum2string(it);
    return s;
}

std::string enum2string(advancedfilters::eInitialImageType it)
{
    std::string str;
	switch (it) {
        case advancedfilters::InitialImageOriginal : str="InitialImageOriginal"; break;
        case advancedfilters::InitialImageZero     : str="InitialImageZero";     break;
    default: throw AdvancedFiltersException("Unknown initial image type",__FILE__,__LINE__);
	};

    return str;
}

void string2enum(const std::string &str, advancedfilters::eInitialImageType &it)
{
	if (str=="InitialImageOriginal")
        it=advancedfilters::InitialImageOriginal;
    else if (str=="InitialImageZero")
        it=advancedfilters::InitialImageZero;
    else
       throw AdvancedFiltersException("Unknown initial image type",__FILE__,__LINE__);

}

std::ostream & operator<<(std::ostream &s, advancedfilters::eFilterType ft)
{
	switch (ft) {
        case advancedfilters::FilterISS : s<<"FilterISS"; break;
        case advancedfilters::FilterISSp : s<<"FilterISSp"; break;
        case advancedfilters::FilterDiffusion : s<<"FilterDiffusion"; break;
        case advancedfilters::FilterIteratedMedian: s<<"FilterIteratedMedian"; break;
        case advancedfilters::FilterGauss: s<<"FilterGauss"; break;
        default : throw AdvancedFiltersException("Unknown filter type",__FILE__,__LINE__);
	}

	return s;
}

void string2enum(const std::string &str, advancedfilters::eFilterType &ft)
{
    if (str=="FilterISS") ft=advancedfilters::FilterISS;
	else
    if (str=="FilterISSp") ft=advancedfilters::FilterISSp;
	else
    if (str=="FilterDiffusion") ft=advancedfilters::FilterDiffusion;
	else
    if (str=="FilterIteratedMedian") ft=advancedfilters::FilterIteratedMedian;
	else
    if (str=="FilterGauss") ft=advancedfilters::FilterGauss;
	else
    throw AdvancedFiltersException("Could not convert string to enum",__FILE__,__LINE__);

}

std::ostream & operator<<(std::ostream & s, advancedfilters::eRegularizationType rt)
{
    s<<enum2string(rt);

    return s;
}

std::string enum2string(advancedfilters::eRegularizationType rt)
{
    std::string str;

    switch (rt) {
        case advancedfilters::RegularizationTV1 :
            str="RegularizationTV1";
            break;
        case advancedfilters::RegularizationTV2 :
            str="RegularizationTV2";
            break;
        default : throw AdvancedFiltersException("Unknown Regularization type",__FILE__,__LINE__);
    }

    return str;
}

void string2enum(const std::string & str, advancedfilters::eRegularizationType &rt)
{
    if (str == "RegularizationTV1")
        rt=advancedfilters::RegularizationTV1;
    else if (str == "RegularizationTV2")
        rt=advancedfilters::RegularizationTV2;
    else
        throw AdvancedFiltersException("Could not convert string to enum",__FILE__,__LINE__);

}
