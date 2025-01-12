//<LICENCE>

#include <string>
#include <iostream>

#include "../../include/math/covariance.h"
#include "../../include/base/KiplException.h"


std::string enum2string(kipl::math::eCovarianceType ct)
{
    switch (ct) {
    case kipl::math::CovarianceMatrix: return "covariance"; break;
    case kipl::math::CorrelationMatrix: return "correlation"; break;
    default: throw kipl::base::KiplException("Unknown covariance matrix type",__FILE__,__LINE__);
    }

    return "no enum";
}

void string2enum(std::string str, kipl::math::eCovarianceType &dt)
{
    if (str=="covariance")
        dt=kipl::math::CovarianceMatrix;
    else if (str=="correlation") dt=kipl::math::CorrelationMatrix;
    else throw kipl::base::KiplException("Could not convert string to covariance matrix type",__FILE__,__LINE__);
}

std::ostream & operator<<(std::ostream &s, kipl::math::eCovarianceType dt)
{
    s<<enum2string(dt);

    return s;
}
