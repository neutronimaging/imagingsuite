//<LICENCE>

#include <iostream>
#include <time.h>
#include <chrono>
//#ifdef _MSC_VER
//#else
//#include <unistd.h>
//#endif

#include "../../include/profile/Timer.h"

namespace kipl { namespace profile {

/// \brief Default c'tor which calibrates all timers upon first call
Timer::Timer() :
    m_nTic(std::chrono::high_resolution_clock::now()),
    m_nToc(std::chrono::high_resolution_clock::now())
{
}

/// \brief Starts the timer
void Timer::Tic()
{
    std::chrono::high_resolution_clock::now();
    m_nToc=m_nTic=std::chrono::high_resolution_clock::now();
    m_fCumulative = 0.0;
}

/// \brief Stops the timer. Can be called multiple times, in this case the time from the most resent call to Tic() is measured.
/// \returns Number of elapsed tics since the timer started.
double Timer::Toc()
{
    m_nToc=std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration<double,std::milli>(m_nToc-m_nTic).count();
    m_fCumulative+=dt;

    return dt;
}
/// \brief Resets the last time measurement
void Timer::reset()
{
    m_fCumulative = 0.0;
}

/// \returns The elapsed time in seconds. This time is computed using the timer calibration.
double Timer::elapsedTime(kipl::profile::Timer::eTimerUnit tu)
{
    double dt=0.0;
    switch (tu) {
    case nanoSeconds  : dt = std::chrono::duration<double,std::nano>(m_nToc-m_nTic).count(); break;
    case microSeconds : dt = std::chrono::duration<double,std::micro>(m_nToc-m_nTic).count(); break;
    case milliSeconds : dt = std::chrono::duration<double,std::milli>(m_nToc-m_nTic).count(); break;
    case seconds      : dt = std::chrono::duration<double,std::ratio<1, 1>>(m_nToc-m_nTic).count(); break;
    case minutes      : dt = std::chrono::duration<double,std::ratio<60, 1>>(m_nToc-m_nTic).count(); break;
    case hours        : dt = std::chrono::duration<double,std::ratio<3600, 1>>(m_nToc-m_nTic).count(); break;
    }
    return dt;
}
double Timer::cumulativeTime()
{
    return m_fCumulative;
}

}}

std::ostream & operator<<(std::ostream &s, kipl::profile::Timer &t)
{
    s<<t.elapsedTime()<<"ms"<<std::flush;

    return s;
}
