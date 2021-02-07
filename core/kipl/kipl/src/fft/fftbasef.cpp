//<LICENCE>

#include <cstring>
#include <complex>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>
#include <thread>
#include <mutex>

#include <fftw3.h>

#include "../../include/fft/fftbase.h"
#include "../../include/base/KiplException.h"

namespace kipl
{
namespace math
{
namespace fft
{

std::mutex fftMutex;

FFTBaseFloat::FFTBaseFloat(const std::vector<size_t> _dims) :
	logger("kipl::math::fft::FFTBaseFloat", std::clog),
	have_r2cPlan(false),
	have_c2cPlan(false),
	have_c2rPlan(false),
	have_c2cPlanI(false),
    r2cPlan(nullptr),
    c2rPlan(nullptr),
    c2cPlan(nullptr),
    c2cPlanI(nullptr),
    cBufferA(nullptr),
    cBufferB(nullptr),
    rBuffer(nullptr),
    dims(_dims),
    Ndata(std::accumulate(_dims.begin(), _dims.end(), 1, std::multiplies<size_t>()))
{
    ndim=dims.size();
}

FFTBaseFloat::~FFTBaseFloat()
{
	if (r2cPlan)
		fftwf_destroy_plan(r2cPlan);	
		
	if (c2cPlan)
		fftwf_destroy_plan(c2cPlan);
		
	if (c2rPlan)
		fftwf_destroy_plan(c2rPlan);
			
	if (c2cPlanI)
		fftwf_destroy_plan(c2cPlanI);
				
	if (cBufferA)
		delete [] cBufferA;
		
	if (cBufferB)
		delete [] cBufferB;
		
	if (rBuffer)
		delete [] rBuffer;
}

int FFTBaseFloat::operator() ( std::complex<float> *inCdata,  std::complex<float> *outCdata, int sign)
{
	if (!cBufferA)
        cBufferA = new std::complex<float>[Ndata];
	
	if (!cBufferB)
        cBufferB = new std::complex<float>[Ndata];

    std::fill_n(cBufferA,0,Ndata);
    std::fill_n(cBufferB,0,Ndata);

    if (sign<0)
    {
        if (!have_c2cPlan)
        {
            std::lock_guard<std::mutex> lock(fftMutex);
            switch (ndim)
            {
				case 1:
					c2cPlan=fftwf_plan_dft_1d(dims[0],
                           reinterpret_cast<fftwf_complex*>(cBufferA), 
							reinterpret_cast<fftwf_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				case 2:
					c2cPlan=fftwf_plan_dft_2d(dims[1],dims[0],
                           reinterpret_cast<fftwf_complex*>(cBufferA), 
							reinterpret_cast<fftwf_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				case 3:
					c2cPlan=fftwf_plan_dft_3d(dims[2],dims[1], dims[0],
                           reinterpret_cast<fftwf_complex*>(cBufferA), 
							reinterpret_cast<fftwf_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				default:
                    throw kipl::base::KiplException("Number of dimensions are not supported",__FILE__,__LINE__);
			}
							
			have_c2cPlan=true;
		}
	
        std::copy_n(inCdata,Ndata,cBufferA);
		fftwf_execute(c2cPlan);
	}
    else
    {
        if (!have_c2cPlanI)
        {
            std::lock_guard<std::mutex> lock(fftMutex);
            std::ostringstream str;
            switch (ndim)
            {
				case 1:
					c2cPlanI=fftwf_plan_dft_1d(dims[0],
                            reinterpret_cast<fftwf_complex*>(cBufferA),
							reinterpret_cast<fftwf_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				case 2:
					c2cPlanI=fftwf_plan_dft_2d(dims[1],dims[0],
                           reinterpret_cast<fftwf_complex*>(cBufferA), 
							reinterpret_cast<fftwf_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				case 3:
					c2cPlanI=fftwf_plan_dft_3d(dims[2],dims[1], dims[0],
                           reinterpret_cast<fftwf_complex*>(cBufferA), 
							reinterpret_cast<fftwf_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				default:
					str.str("");
					str<<"ndim="<<ndim<<" is not supported";
					logger(kipl::logging::Logger::LogError,str.str());
                    throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
					
			}
							
			have_c2cPlanI=true;
		}
			
        std::copy_n(inCdata,Ndata,cBufferA);
		fftwf_execute(c2cPlanI);
	}

    std::copy_n(cBufferB,Ndata,outCdata);

	return static_cast<int>(Ndata);
}

int FFTBaseFloat::operator() (float *inRdata,  std::complex<float> *outCdata)
{
	if (!cBufferA)
        cBufferA = new std::complex<float>[Ndata];
	
	if (!rBuffer)
        rBuffer = new float[2*Ndata];

    std::fill_n(cBufferA,0,Ndata);
    std::fill_n(rBuffer,0,2*Ndata);
		
    if (!have_r2cPlan)
    {
        std::lock_guard<std::mutex> lock(fftMutex);
        std::ostringstream str;
        switch (ndim)
        {
			case 1:
				r2cPlan=fftwf_plan_dft_r2c_1d(dims[0],
						rBuffer,reinterpret_cast<fftwf_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			case 2:
				r2cPlan=fftwf_plan_dft_r2c_2d(dims[0],dims[1],
						rBuffer,reinterpret_cast<fftwf_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			case 3:
				r2cPlan=fftwf_plan_dft_r2c_3d(dims[0],dims[1], dims[2],
						rBuffer, reinterpret_cast<fftwf_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			default:
				str.str("");
				str<<"ndim="<<ndim<<" is not supported";
				logger(kipl::logging::Logger::LogError,str.str());
                kipl::base::KiplException(str.str(),__FILE__,__LINE__);
		}
	
		have_r2cPlan=true;
	}
	
    std::copy_n(inRdata,Ndata,rBuffer);
	
	fftwf_execute(r2cPlan);

    std::copy_n(cBufferA,Ndata,outCdata);

	return static_cast<int>(Ndata);
}


int FFTBaseFloat::operator() ( std::complex<float> *inCdata, float *outRdata)
{
	if (!cBufferA)
		//cBufferA=reinterpret_cast<complex<float> *>(fftw_malloc(sizeof(fftwf_complex) * Ndata));
        cBufferA=new  std::complex<float>[Ndata];
	
	if (!rBuffer)
		rBuffer=new float[2*Ndata];

    std::fill_n(cBufferA, 0,   Ndata);
    std::fill_n(rBuffer,  0, 2*Ndata);

    if (!have_c2rPlan)
    {
        std::lock_guard<std::mutex> lock(fftMutex);
        std::ostringstream str;
        switch (ndim)
        {
			case 1:
				c2rPlan=fftwf_plan_dft_c2r_1d(dims[0],
						reinterpret_cast<fftwf_complex*>(cBufferA),rBuffer,
						FFTW_ESTIMATE);
				break;
			case 2:
				c2rPlan=fftwf_plan_dft_c2r_2d(dims[0],dims[1],
						reinterpret_cast<fftwf_complex*>(cBufferA),rBuffer,
						FFTW_ESTIMATE);
				break;
			case 3:
				c2rPlan=fftwf_plan_dft_c2r_3d(dims[0],dims[1], dims[2],
						reinterpret_cast<fftwf_complex*>(cBufferA),rBuffer, 
						FFTW_ESTIMATE);
				break;
			default:
				str.str("");
				str<<"ndim="<<ndim<<" is not supported";
				logger(kipl::logging::Logger::LogError,str.str());
                throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
		}
		
		have_c2rPlan=true;
	}
	
    std::copy_n(inCdata,Ndata,cBufferA);
	fftwf_execute(c2rPlan);

    std::copy_n(rBuffer, Ndata, outRdata);

    return static_cast<int>(Ndata);
}

int FFTBaseFloat::size(int idx)
{
    if (ndim<=idx)
        throw kipl::base::KiplException("Attempt to access axis beyond the transform dimensions",__FILE__,__LINE__);

    return dims[idx];
}
}}}

