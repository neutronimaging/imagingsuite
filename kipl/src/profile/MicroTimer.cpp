#include "../../include/profile/MicroTimer.h"
#include <iostream>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif

namespace kipl { namespace profile {
double MicroTimer::m_dCalibration=0.0;
MicroTimer::MicroTimer() : m_nTic(0), m_nToc(0),m_nDiff(0)  
{
	if (MicroTimer::m_dCalibration==0.0) {
		MicroTimer::Calibrate();
	}
 }

void MicroTimer::Calibrate()
{
#ifdef _MSC_VER
#else
	const int usecs=100000;
	usleep(usecs);
	u_int64_t start=MicroTimer::_rdtsc();
	usleep(usecs);
	u_int64_t stop=MicroTimer::_rdtsc();
	u_int64_t diff=_diff(stop,start);
	MicroTimer::m_dCalibration=(usecs/1e6)/static_cast<double>(diff);
#endif
}

void MicroTimer::Tic()
{
	m_nToc=m_nTic=_rdtsc();
}

u_int64_t MicroTimer::Toc()
{
	m_nToc=_rdtsc();
	m_nDiff=_diff(m_nToc,m_nTic);
	
	return m_nDiff;
}

inline u_int64_t MicroTimer::_diff (u_int64_t v1, u_int64_t v2)
{
#ifdef _MSC_VER
	long long d = v1 - v2;
#else
	int64_t d = v1 - v2;
#endif
	if (d >= 0) 
		return d; 
	else 
		return -d;
}

inline u_int64_t MicroTimer::_rdtsc ()
{
	#ifdef _MSC_VER
	return 0;
#else
	u_int64_t d;
	// Instruction is volatile because we don't want it to move
	// over an adjacent gettimeofday. That would ruin the timing
	// calibrations.
	__asm__ __volatile__ ("rdtsc" : "=&A" (d));
	return d;
#endif
}

std::ostream & operator<<(std::ostream &s, MicroTimer &t)
{
	s<<t.ElapsedSeconds()<<"s ("<<t.ElapsedTics()<<" tics)"<<std::flush;
	
	return s;
}

}}
