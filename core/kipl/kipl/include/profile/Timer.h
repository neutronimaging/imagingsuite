//<LICENCE>

#ifndef TIMER_H_
#define TIMER_H_

#include "../kipl_global.h"

#include <sys/types.h>
#include <time.h>
#include <chrono>
#include <iostream>

namespace kipl { namespace profile {

class KIPLSHARED_EXPORT Timer {
public :
    enum eTimerUnit {
        nanoSeconds,
        microSeconds,
        milliSeconds,
        seconds,
        minutes,
        hours
    };
    /// \brief Default c'tor which calibrates all timers upon first call
    Timer();

    /// \brief Starts the timer
    void Tic();

    /// \brief Stops the timer. Can be called multiple times, in this case the time from the most resent call to Tic() is measured.
    /// \returns Number of elapsed tics since the timer started.
    double Toc();

    /// \brief Resets the last time measurement
    void Reset();

	/// \returns The elapsed time in seconds. This time is computed using the timer calibration.
    double elapsedTime(kipl::profile::Timer::eTimerUnit tu= kipl::profile::Timer::milliSeconds) ;

    double cumulativeElapsedTime() ;
private :

    std::chrono::steady_clock::time_point m_nTic; ///< \brief Memory for the most recent tic reading
    std::chrono::steady_clock::time_point m_nToc; ///< \brief Memory for the most recent tic reading
	
    double m_fCumulative;  ///< \brief Stores the cumulated time from several toc calls
};

}}

/// \breif Formatted output of the timer to an ostream.
//std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::profile::Timer &t);

#endif /*MICROTIMER_H_*/
