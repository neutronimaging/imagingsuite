//<LICENCE>

#include <iostream>
#include <time.h>
#include <chrono>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif

#include "../../include/profile/Timer.h"

namespace kipl { namespace profile {

Timer::Timer() :
    m_nTic(std::chrono::high_resolution_clock::now()),
    m_nToc(std::chrono::high_resolution_clock::now())
//    m_nDiff(std::chrono::duration<double, std::milli>(0)),
//    m_nCumulative(std::chrono::duration<double, std::milli>(0))
{
}

void Timer::Tic()
{
    m_nToc=m_nTic=std::chrono::high_resolution_clock::now();
}

double Timer::Toc()
{
    m_nToc=std::chrono::high_resolution_clock::now();
    m_nDiff = std::chrono::duration<double, std::milli>(m_nToc-m_nTic);
    m_nCumulative+=m_nDiff;

    return m_nDiff.count();
}


}}

std::ostream & operator<<(std::ostream &s, kipl::profile::Timer &t)
{
    if (t.cumulativeTime()==t.elapsedTime())
//		s<<t.CumulativeElapsedSeconds()<<"s ("<<t.CumulativeElapsedTics()<<" tics)"<<std::flush;
//	else
//		s<<"Total time: "<<t.CumulativeElapsedSeconds()<<"s, Wall time: "<<t.CumulativeWallTime()<<"s"<<std::flush;
	
    return s;
}
