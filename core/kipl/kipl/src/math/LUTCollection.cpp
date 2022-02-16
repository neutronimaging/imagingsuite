//<LICENCE>

#include <cmath>
#include <iostream>

#include "../../include/math/LUTCollection.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace math {

LogLUT::LogLUT(size_t len, float lo, float hi) : LUTbase(len,lo,hi)
{
	BuildLUT(len,lo,hi);
}

LogLUT::~LogLUT() 
{}

void LogLUT::BuildLUT(size_t len, float lo, float hi)
{
    std::ignore = len;
    std::ignore = hi;

	float x=lo;
	float delta=1.0f/Slope();
	size_t i=0;
	if (lo<0.0f)
		throw kipl::base::KiplException("Negative values are not allowed in LogLUT",__FILE__,__LINE__);
	
	if (lo==0.0f) {
		i++;
		x=delta;
		fLUT[0]=log(delta);
	}
	const size_t N=size();	
	for ( ; i<N; i++, x+=delta) {
		fLUT[i]=log(x);
	}
}
//---------------------------------------------------------
// Sigmoid LUT
SigmoidLUT::SigmoidLUT(size_t len, float lambda, float sigma) :
LUTbase(len, LowerLimit(lambda,sigma), UpperLimit(lambda,sigma)),
fLambda(lambda),
fSigma(sigma)
{
BuildLUT(len,Low(),High());
}

SigmoidLUT::~SigmoidLUT()
{}

void SigmoidLUT::Setup(size_t len, float lambda, float sigma)
{
	fLambda=lambda;
	fSigma=sigma;
	Resize(len,LowerLimit(lambda,sigma),UpperLimit(lambda,sigma));
	
	BuildLUT(size(),Low(),High());
}

float SigmoidLUT::UpperLimit(float lambda, float sigma)
{
	const float epsilon=sigma*1e-3f;
	return lambda-sigma*log(1.0f/(1.0f-epsilon)-1.0f);
}

float SigmoidLUT::LowerLimit(float lambda, float sigma)
{
	const float epsilon=sigma*1e-3f;
	return lambda-sigma*log(1/epsilon);
}

void SigmoidLUT::BuildLUT(size_t len, float lo, float hi)
{
    std::ignore = hi;

	const float invwidth=1.0f/fSigma;
	
	const float delta=1.0f/Slope();
	float x=lo;
	
	for (size_t i=0; i<len; i++, x+=delta)
		fLUT[i]=1.0f/(1.0f+exp((fLambda-x)*invwidth));
	fLUT[0]=0.0f;
	fLUT[len-1]=1.0f;
}

PowLUT::PowLUT(size_t len, float exponent, float lo, float hi) :
LUTbase(len, lo, hi),
fExponent(exponent)
{
	BuildLUT(len,lo,hi);
}
PowLUT::~PowLUT()
{}

void PowLUT::BuildLUT(size_t len, float lo, float hi)
{
    std::ignore = hi;

	const float delta=1.0f/Slope();
	float x=lo;
	
	for (size_t i=0; i<len; i++, x+=delta) {
		fLUT[i]=pow(x,fExponent);
	}
	fLUT[len]=fLUT[len-1];
}
	

// Gauss LUT

GaussLUT::GaussLUT(size_t len, float m, float s, float lo, float hi) :
LUTbase(len, lo, hi),
m_fMean(m),
m_fSigma(s)
{
	BuildLUT(len,lo,hi);
}

GaussLUT::~GaussLUT()
{}

void GaussLUT::BuildLUT(size_t len, float lo, float hi)
{
    std::ignore = hi;

	const float delta=1.0f/Slope();
	float x=lo;
	

	for (size_t i=0; i<len; i++, x+=delta) {
		fLUT[i]=exp(-(x-m_fMean)*(x-m_fMean)/(2*m_fSigma*m_fSigma));
	}
	fLUT[len]=fLUT[len-1];
}
}}

