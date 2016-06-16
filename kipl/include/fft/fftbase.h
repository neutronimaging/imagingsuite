/***************************************************************************
 *   Copyright (C) 2008 by Anders Kaestner   *
 *   anders.kaestner@psi.ch   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
#ifndef _FFTBASE_H_
#define _FFTBASE_H_

#include "../kipl_global.h"
#include <complex>
#include <cstring>
#include <fftw3.h>

#include "../logging/logger.h"

namespace kipl {
/// \brief The math name space collects mathematical functions
namespace math {
/// \brief The FFT namespace collects classes related to the fft
namespace fft {

/// \brief Base class to provide an efficient interface to libFFTW
///
///	The class only computes the plan once and recycles it afterwards for all transforms.
///	For efficiency reasons are the data copied to local buffers of the same size as the 
///	input.
///	
///	Input and output data array must be allocated prior to use. FFTBase does not 
///	allocate any data to external pointers.
///
///@author Anders Kaestner
class KIPLSHARED_EXPORT FFTBase{
public:
	/// \brief Constructor that defines size and rank of the transform
	///	\param Dims array describing the size of the transform
	///	\param NDim the rank of the transform
    FFTBase(size_t const * const Dims, size_t const NDim);
    
    /// \brief Computes a complex Fourier transform
    ///	\param inCdata pointer to the input data
    ///	\param outCdata pointer to the result data
    ///	\param sign switch to decide the direction of the transform (forward sign=-1, inverse sign=1)
    ///	
    ///	\returns Number of elements in the processed array

    int operator() (std::complex<double> *inCdata, std::complex<double> *outCdata, int sign=1);
    
    /// \brief Computes a real to complex Fourier transform
    ///	\param inRdata pointer to the input data (Real valued)
    ///	\param outCdata pointer to the result data (Complex valued)
    ///	
    ///	\returns Number of elements in the processed array
    int operator() (double *inRdata, std::complex<double> *outCdata);
    
    /// \brief Computes a real to complex Fourier transform
    ///	\param inCdata pointer to the input data (Complex valued)
    ///	\param outRdata pointer to the result data (Real valued)
    ///	
    ///	\returns Number of elements in the processed array
    int operator() (std::complex<double> *inCdata, double *outRdata);
    
    /// \brief The destruct removes transform plans and deallocates buffer memory
    ~FFTBase();
protected:
	kipl::logging::Logger logger;
	bool have_r2cPlan;
	bool have_c2cPlan;
	bool have_c2rPlan;
	bool have_c2cPlanI;
	fftw_plan r2cPlan;
	fftw_plan c2rPlan;
	fftw_plan c2cPlan;
	fftw_plan c2cPlanI;

	std::complex<double> *cBufferA;
	std::complex<double> *cBufferB;
	double *rBuffer;
	
	long Ndata;
	int dims[8];
	int ndim;
};


class KIPLSHARED_EXPORT FFTBaseFloat{
public:
	/// \brief Constructor that defines size and rank of the transform
	///	\param Dims array describing the size of the transform
	///	\param NDim the rank of the transform
    FFTBaseFloat(size_t const * const Dims, size_t const NDim);
    
    /// \brief Computes a complex Fourier transform
    ///	\param inCdata pointer to the input data
    ///	\param outCdata pointer to the result data
    ///	\param sign switch to decide the direction of the transform (forward sign=-1, inverse sign=1)
    ///	
    ///	\returns Number of elements in the processed array
    int operator() (std::complex<float> *inCdata, std::complex<float> *outCdata, int sign=1);
    
    /// \brief Computes a real to complex Fourier transform
    ///	\param inRdata pointer to the input data (Real valued)
    ///	\param outCdata pointer to the result data (Complex valued)
    ///	
    ///	\returns Number of elements in the processed array
    int operator() (float *inRdata, std::complex<float> *outCdata);
    
    /// \brief Computes a real to complex Fourier transform
    ///	\param inCdata pointer to the input data (Complex valued)
    ///	\param outRdata pointer to the result data (Real valued)
    ///	
    ///	\returns Number of elements in the processed array
    int operator() (std::complex<float> *inCdata, float *outRdata);
    
    /// \brief The destruct removes transform plans and deallocates buffer memory
    ~FFTBaseFloat();
protected:
	kipl::logging::Logger logger;
	bool have_r2cPlan;
	bool have_c2cPlan;
	bool have_c2rPlan;
	bool have_c2cPlanI;
	fftwf_plan r2cPlan;
	fftwf_plan c2rPlan;
	fftwf_plan c2cPlan;
	fftwf_plan c2cPlanI;
	
	std::complex<float> *cBufferA;
	std::complex<float> *cBufferB;
	float *rBuffer;
	
	size_t Ndata;
	int dims[8];
	int ndim;
};


}}}

#endif
