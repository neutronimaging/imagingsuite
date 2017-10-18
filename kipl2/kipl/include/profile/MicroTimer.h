//<LICENCE>

#ifndef MICROTIMER_H_
#define MICROTIMER_H_

#include <sys/types.h>
#include <iostream>

#ifdef _MSC_VER
typedef unsigned long long u_int64_t; 
#endif

namespace kipl { namespace profile {
class MicroTimer {
public :
  /// \brief Default c'tor which calibrates all timers upon first call
	MicroTimer();
	/// \brief Calibration method. Measures the number of ticks for a specified elapsed time
	static void Calibrate();
	/// \brief Starts the timer
	void Tic();
	/// \brief Stops the timer. Can be called multiple times, in this case the time from the most resent call to Tic() is measured.
	/// \returns Number of elapsed tics since the timer started.
	u_int64_t Toc();
	/// \brief Resets the last time measurement
	void Reset() { m_nDiff=0; }
	/// \returns The last number of measured tics.
	u_int64_t ElapsedTics() {return m_nDiff;}
	/// \returns The elapsed time in seconds. This time is computed using the timer calibration.
	double ElapsedSeconds() {return m_nDiff*m_dCalibration;}
private :
  /// \returns The current current clock tic register reading
	static u_int64_t _rdtsc();
	/// \brief Computes the difference between two tic values 
	static u_int64_t _diff(u_int64_t a, u_int64_t b);
	
	/// \brief Memory for the first tic reading 
	///
	/// This member is only modified by Tic()
	u_int64_t m_nTic;
	/// \brief Memory for the most recent tic reading
	u_int64_t m_nToc;
	
	/// \brief Stores the measured time difference 
	u_int64_t m_nDiff;
	
	/// \brief Contains the time calibration. This value is required to compute the time in seconds from the number of clock tics. 
	static double m_dCalibration;
};

/// \breif Formatted output of the timer to an ostream.
std::ostream & operator<<(std::ostream &s, MicroTimer &t);

}}

#endif /*MICROTIMER_H_*/
