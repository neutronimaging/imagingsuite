//<LICENCE>


#include <cstring>
#include <complex>
#include <iostream>
#include <cmath>
#include <algorithm>

#include <fftw3.h>

#include "../../include/fft/fftbase.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace math { namespace fft {
using namespace std;



FFTBaseFloat::FFTBaseFloat(size_t const * const Dims, size_t const NDim) : 	
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
	Ndata(Dims[0])
{
	for (size_t i=1; i<NDim; i++)
		Ndata*=Dims[i];
		
	for (size_t i=0; i<NDim; i++)
        dims[i]=static_cast<int>(Dims[i]);

	for (size_t i=NDim; i<8; i++)
		dims[i]=1;
		
	ndim=static_cast<int>(NDim);

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

int FFTBaseFloat::operator() ( complex<float> *inCdata,  complex<float> *outCdata, int sign)
{
	if (!cBufferA)
		cBufferA=new  complex<float>[Ndata];
	
	if (!cBufferB)
		cBufferB=new  complex<float>[Ndata];

//    std::fill_n(cBufferA,0,Ndata);
//    std::fill_n(cBufferB,0,Ndata);

	if (sign<0) {
		if (!have_c2cPlan) {
			switch (ndim) {
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
					cerr<<"ndim="<<ndim<<" is not supported"<<endl;
					return -1;
			}
							
			have_c2cPlan=true;
		}
	
		memcpy(cBufferA,inCdata,sizeof( complex<float>)*Ndata);
		fftwf_execute(c2cPlan);
	}
	else {
		if (!have_c2cPlanI) {
			ostringstream str;
			switch (ndim) {
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
					return -1;
					
			}
							
			have_c2cPlanI=true;
		}
			
		memcpy(cBufferA,inCdata,sizeof( complex<float>)*Ndata);
		fftwf_execute(c2cPlanI);
	}

	memcpy(outCdata,cBufferB,sizeof( complex<float>)*Ndata);
	
	return static_cast<int>(Ndata);
}

int FFTBaseFloat::operator() (float *inRdata,  complex<float> *outCdata)
{
	if (!cBufferA)
		cBufferA=new  complex<float>[Ndata];
	
	if (!rBuffer)
		rBuffer=new float[2*Ndata];

    std::fill_n(cBufferA,0,Ndata);
    std::fill_n(rBuffer,0,2*Ndata);
		
	if (!have_r2cPlan) {
		ostringstream str;
		switch (ndim) {
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
				return -1;
		}
	
		have_r2cPlan=true;
	}
	
	memcpy(rBuffer,inRdata,sizeof(float)*Ndata);		
	
	fftwf_execute(r2cPlan);

	memcpy(outCdata,cBufferA,sizeof( complex<float>)*Ndata);

	return static_cast<int>(Ndata);
}


int FFTBaseFloat::operator() ( complex<float> *inCdata, float *outRdata)
{
	if (!cBufferA)
		//cBufferA=reinterpret_cast<complex<float> *>(fftw_malloc(sizeof(fftwf_complex) * Ndata));
		cBufferA=new  complex<float>[Ndata];
	
	if (!rBuffer)
		rBuffer=new float[2*Ndata];

    std::fill_n(cBufferA,0,Ndata);
    std::fill_n(rBuffer,0,2*Ndata);

	if (!have_c2rPlan) {
		ostringstream str;
		switch (ndim) {
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
				return -1;
		}
		
		have_c2rPlan=true;
	}
	
	memcpy(cBufferA,inCdata,sizeof(complex<float>)*Ndata);
	fftwf_execute(c2rPlan);

	memcpy(outRdata,rBuffer,sizeof(float)*Ndata);

    return static_cast<int>(Ndata);
}

int FFTBaseFloat::size(int idx)
{
    if (ndim<=idx)
        throw kipl::base::KiplException("Attempt to access axis beyond the transform dimensions",__FILE__,__LINE__);

    return dims[idx];
}
}}}

