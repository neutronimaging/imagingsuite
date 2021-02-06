//<LICENCE>

#include <iostream>
#include <complex>
#include <vector>
#include <numeric>
#include <fftw3.h>

#include "../../include/base/KiplException.h"
#include "../../include/fft/fftbase.h"

namespace kipl { namespace math { namespace fft {

FFTBase::FFTBase(const std::vector<size_t> &_dims) :
	logger("kipl::math::fft::FFTBase", std::clog),
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


FFTBase::~FFTBase()
{
	if (r2cPlan)
		fftw_destroy_plan(r2cPlan);	
		
	if (c2cPlan)
		fftw_destroy_plan(c2cPlan);
		
	if (c2rPlan)
		fftw_destroy_plan(c2rPlan);
			
	if (c2cPlanI)
		fftw_destroy_plan(c2cPlanI);
				
	if (cBufferA)
		delete [] cBufferA;
		
	if (cBufferB)
		delete [] cBufferB;
		
	if (rBuffer)
		delete [] rBuffer;
}

int FFTBase::operator() ( std::complex<double> *inCdata,  std::complex<double> *outCdata, int sign)
{
	if (!cBufferA)
        cBufferA=new  std::complex<double>[Ndata];
	
	if (!cBufferB)
        cBufferB=new  std::complex<double>[Ndata];
	

	if (sign<0) {
        if (!have_c2cPlan)
        {
            switch (ndim)
            {
				case 1:
					c2cPlan=fftw_plan_dft_1d(dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				case 2:
					c2cPlan=fftw_plan_dft_2d(dims[1],dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				case 3:
					c2cPlan=fftw_plan_dft_3d(dims[2],dims[1], dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),-1,
							FFTW_ESTIMATE);
					break;
				default:
                    throw kipl::base::KiplException("using unexpected number of dimensions",__FILE__,__LINE__);
			}
							
			have_c2cPlan=true;
		}
	
        memcpy(cBufferA,inCdata,sizeof( std::complex<double>)*Ndata);
		fftw_execute(c2cPlan);
	}
    else
    {
        if (!have_c2cPlanI)
        {
            std::ostringstream str;
            switch (ndim)
            {
				case 1:
					c2cPlanI=fftw_plan_dft_1d(dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				case 2:
					c2cPlanI=fftw_plan_dft_2d(dims[1],dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				case 3:
					c2cPlanI=fftw_plan_dft_3d(dims[2],dims[1], dims[0],
                           reinterpret_cast<fftw_complex*>(cBufferA), 
							reinterpret_cast<fftw_complex*>(cBufferB),1,
							FFTW_ESTIMATE);
					break;
				default:
					str.str("");
					str<<"ndim="<<ndim<<" is not supported";
					logger(kipl::logging::Logger::LogError,str.str());
					return -1;
					
			}
							
			have_c2cPlanI=true;
		}
			
        memcpy(cBufferA,inCdata,sizeof( std::complex<double>)*Ndata);
		fftw_execute(c2cPlanI);
	}

    memcpy(outCdata,cBufferB,sizeof( std::complex<double>)*Ndata);
	
	return Ndata;
}

int FFTBase::operator() (double *inRdata,  std::complex<double> *outCdata)
{
	if (!cBufferA)
        cBufferA=new  std::complex<double>[Ndata];
	
	if (!rBuffer)
		rBuffer=new double[Ndata];
		
    if (!have_r2cPlan)
    {
        std::ostringstream str;
        switch (ndim)
        {
			case 1:
				r2cPlan=fftw_plan_dft_r2c_1d(dims[0],
						rBuffer,reinterpret_cast<fftw_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			case 2:
				r2cPlan=fftw_plan_dft_r2c_2d(dims[0],dims[1],
						rBuffer,reinterpret_cast<fftw_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			case 3:
				r2cPlan=fftw_plan_dft_r2c_3d(dims[0],dims[1], dims[2],
						rBuffer, reinterpret_cast<fftw_complex*>(cBufferA),
						FFTW_ESTIMATE);
				break;
			default:
				str.str("");
				str<<"ndim="<<ndim<<" is not supported";
				logger(kipl::logging::Logger::LogError,str.str());
				return -1;
		}
	
		have_r2cPlan=true;
	}
	
	memcpy(rBuffer,inRdata,sizeof(double)*Ndata);		
	
	fftw_execute(r2cPlan);

    memcpy(outCdata,cBufferA,sizeof( std::complex<double>)*Ndata);

	return Ndata;
}

int FFTBase::operator() ( std::complex<double> *inCdata, double *outRdata)
{
	if (!cBufferA)
        cBufferA=new  std::complex<double>[Ndata];
	
	if (!rBuffer)
		rBuffer=new double[Ndata];
		
    if (!have_c2rPlan)
    {
        std::ostringstream str;
        switch (ndim)
        {
			case 1:
				c2rPlan=fftw_plan_dft_c2r_1d(dims[0],
						reinterpret_cast<fftw_complex*>(cBufferA),rBuffer,
						FFTW_ESTIMATE);
				break;
			case 2:
				c2rPlan=fftw_plan_dft_c2r_2d(dims[0],dims[1],
						reinterpret_cast<fftw_complex*>(cBufferA),rBuffer,
						FFTW_ESTIMATE);
				break;
			case 3:
				c2rPlan=fftw_plan_dft_c2r_3d(dims[0],dims[1], dims[2],
						reinterpret_cast<fftw_complex*>(cBufferA),rBuffer, 
						FFTW_ESTIMATE);
				break;
			default:
				str.str("");
				str<<"ndim="<<ndim<<" is not supported";
				logger(kipl::logging::Logger::LogError,str.str());
				return -1;
		}
		
		have_c2rPlan=true;
	}
	
    memcpy(cBufferA,inCdata,sizeof(std::complex<double>)*Ndata);
	fftw_execute(c2rPlan);

	memcpy(outRdata,rBuffer,sizeof(double)*Ndata);

	return Ndata;
}

}}}

