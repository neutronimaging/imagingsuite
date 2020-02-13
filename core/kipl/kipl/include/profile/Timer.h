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
  /// \brief Default c'tor which calibrates all timers upon first call
	Timer();
	/// \brief Starts the timer
    void Tic();
//	/// \brief Stops the timer. Can be called multiple times, in this case the time from the most resent call to Tic() is measured.
//	/// \returns Number of elapsed tics since the timer started.
    double Toc();
//	/// \brief Resets the last time measurement
    void Reset() {
  //      m_nTic = std::chrono::duration<double, std::milli>(0);
        m_nDiff       = std::chrono::duration<double, std::milli>(0);
        m_nCumulative = std::chrono::duration<double, std::milli>(0);
    }
	/// \returns The last number of measured tics.
//	clock_t ElapsedTics() {return m_nDiff;}
	
    double CumulativeElapsedTics() {return m_nCumulative.count();}
	/// \returns The elapsed time in seconds. This time is computed using the timer calibration.
    double elapsedTime() {return m_nDiff.count();}
    double cumulativeElapsedTime() {return m_nCumulative.count();}
private :

    std::chrono::steady_clock::time_point m_nTic;
/// \brief Memory for the most recent tic reading
    std::chrono::steady_clock::time_point m_nToc;
	
//	/// \brief Stores the measured time difference
    std::chrono::duration<double, std::milli> m_nDiff;
    std::chrono::duration<double, std::milli> m_nCumulative;
};

}}

/// \breif Formatted output of the timer to an ostream.
//std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::profile::Timer &t);

#endif /*MICROTIMER_H_*/
