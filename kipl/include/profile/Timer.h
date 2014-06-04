#ifndef TIMER_H_
#define TIMER_H_

#include "../kipl_global.h"
#include <sys/types.h>
#include <time.h>
#include <iostream>

namespace kipl { namespace profile {
class KIPLSHARED_EXPORT Timer {
public :
  /// \brief Default c'tor which calibrates all timers upon first call
	Timer();
	/// \brief Starts the timer
	void Tic();
	/// \brief Stops the timer. Can be called multiple times, in this case the time from the most resent call to Tic() is measured.
	/// \returns Number of elapsed tics since the timer started.
	clock_t Toc();
	/// \brief Resets the last time measurement
	void Reset() { 
		m_nTic=m_nToc=m_nDiff=m_nCumulative=0; 
		m_fTic=m_fToc=m_fDiff=m_fCumulative=0.0;
	}
	/// \returns The last number of measured tics.
	clock_t ElapsedTics() {return m_nDiff;}
	
	clock_t CumulativeElapsedTics() {return m_nCumulative;}
	/// \returns The elapsed time in seconds. This time is computed using the timer calibration.
	double ElapsedSeconds() {return static_cast<double>(m_nDiff)/static_cast<double>(CLOCKS_PER_SEC);}
	double WallTime() {return m_fDiff;}
	double CumulativeElapsedSeconds() {return static_cast<double>(m_nCumulative)/static_cast<double>(CLOCKS_PER_SEC);}
	double CumulativeWallTime() {return m_fCumulative;}
private :

	clock_t m_nTic;
	/// \brief Memory for the most recent tic reading
	clock_t m_nToc;

	double m_fTic;
	double m_fToc;
	
	/// \brief Stores the measured time difference 
	clock_t m_nDiff;
	clock_t m_nCumulative;
	double m_fDiff;
	double m_fCumulative;
	
	inline clock_t _diff (clock_t v1, clock_t v2);
};

}}

/// \breif Formatted output of the timer to an ostream.
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::profile::Timer &t);

#endif /*MICROTIMER_H_*/
