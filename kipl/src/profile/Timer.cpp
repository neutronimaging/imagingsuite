//<LICENCE>

#include <iostream>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif

#include "../../include/profile/Timer.h"

namespace kipl { namespace profile {

Timer::Timer() : 
m_nTic(0), 
m_nToc(0),
m_fTic(0.0), 
m_fToc(0.0),
m_nDiff(0),
m_nCumulative(0),
m_fDiff(0.0),
m_fCumulative(0.0)

{
}

void Timer::Tic()
{
#ifdef _OPENMP
	m_fTic=omp_get_wtime();
#endif
	m_nToc=m_nTic=clock();

}

clock_t Timer::Toc()
{
#ifdef _OPENMP
	m_fToc=omp_get_wtime();
#endif
	m_nToc=clock();
	
	m_nDiff=_diff(m_nToc,m_nTic);
	m_nCumulative+=m_nDiff;

	m_fDiff=m_fToc-m_fTic;
	m_fCumulative+=m_fDiff;

	return m_nDiff;
}

inline clock_t Timer::_diff (clock_t v1, clock_t v2)
{
    return v1<v2 ? v2-v1 : v1-v2;
}



}}

std::ostream & operator<<(std::ostream &s, kipl::profile::Timer &t)
{
	if (t.CumulativeWallTime()==0.0)
		s<<t.CumulativeElapsedSeconds()<<"s ("<<t.CumulativeElapsedTics()<<" tics)"<<std::flush;
	else
		s<<"Total time: "<<t.CumulativeElapsedSeconds()<<"s, Wall time: "<<t.CumulativeWallTime()<<"s"<<std::flush;
	
	return s;
}
