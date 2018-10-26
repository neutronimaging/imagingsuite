// Statistics.cpp: implementation of the Statistics class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include "../../include/math/statistics.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace kipl {
namespace math {

KIPLSHARED_EXPORT Statistics::Statistics() :
			m_fMax(-std::numeric_limits<double>::max()),
			m_fMin(std::numeric_limits<double>::max()),
			m_fSum2(0.0),
			m_fSum(0.0),
			m_nNdata(0)
{
}


KIPLSHARED_EXPORT Statistics::Statistics(const Statistics & s) :
			m_fMax(s.m_fMax),
			m_fMin(s.m_fMin),
			m_fSum2(s.m_fSum2),
			m_fSum(s.m_fSum),
			m_nNdata(s.m_nNdata)
{
}

Statistics & Statistics::operator=(const Statistics &s)
{
    m_fMax   = s.m_fMax;
    m_fMin   = s.m_fMin;
    m_fSum2  = s.m_fSum2;
    m_fSum   = s.m_fSum;
    m_nNdata = s.m_nNdata;

    return *this;
}

Statistics::~Statistics()
{
}


double KIPLSHARED_EXPORT Statistics::E() const
{
    return m_nNdata == 0UL ? 0.0 : m_fSum/m_nNdata;
}

double KIPLSHARED_EXPORT Statistics::V() const
{
    return m_nNdata == 0UL ? 0.0 : (m_fSum2-m_fSum*m_fSum/m_nNdata)/(m_nNdata);
}

double KIPLSHARED_EXPORT Statistics::s() const
{
    return m_nNdata == 0UL ? 0.0 : sqrt((m_fSum2-m_fSum*m_fSum/m_nNdata)/(m_nNdata));
}

Statistics KIPLSHARED_EXPORT Statistics::operator +(Statistics & s)
{
	Statistics temp;
	temp.m_fSum=m_fSum+s.m_fSum;
	temp.m_fSum2=m_fSum2+s.m_fSum2;
	temp.m_nNdata=m_nNdata+s.m_nNdata;

	return temp;

}


int KIPLSHARED_EXPORT Statistics::reset()
{
	m_fSum=0.0;
	m_fSum2=0.0;
	m_nNdata=0;
	m_fMin=std::numeric_limits<double>::max();
	m_fMax=-std::numeric_limits<double>::max();

	return 1;
}

double KIPLSHARED_EXPORT Statistics::Max() const
{
    return m_nNdata == 0UL ? 0.0 : m_fMax;
}

double KIPLSHARED_EXPORT Statistics::Min() const
{
    return m_nNdata == 0UL ? 0.0 : m_fMin;
}

size_t KIPLSHARED_EXPORT Statistics::n() const
{
  return m_nNdata;
}

}
} // End namespace statistics

std::ostream & operator <<(std::ostream & os, kipl::math::Statistics & s)
{
    os<<"E="<<s.E()<<std::endl;
    os<<"V="<<s.V()<<std::endl;
    os<<"s="<<s.s()<<std::endl;
    os<<"n="<<s.n()<<std::endl;

	return os;
}
