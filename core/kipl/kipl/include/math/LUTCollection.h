

#ifndef LUTCOLLECTION_H_
#define LUTCOLLECTION_H_

#include "../kipl_global.h"

#include "LUTbase.h"

namespace kipl { namespace math {

class KIPLSHARED_EXPORT LogLUT : public kipl::math::LUTbase {
public:
	LogLUT(size_t len, float lo, float hi);
	virtual ~LogLUT();
protected:
	virtual void BuildLUT(size_t len, float lo, float hi);
};

class KIPLSHARED_EXPORT SigmoidLUT : public kipl::math::LUTbase {
public:
	SigmoidLUT(size_t len, float lambda, float sigma);
	virtual ~SigmoidLUT();
	void Setup(size_t len, float lambda, float sigma);
protected:
	float UpperLimit(float lambda, float sigma);
	float LowerLimit(float lambda, float sigma);
	virtual void BuildLUT(size_t len, float lo, float hi);
	float fLambda;
	float fSigma;
};

class KIPLSHARED_EXPORT PowLUT : public kipl::math::LUTbase {
public:
	PowLUT(size_t len, float exponent, float lo, float hi);
	virtual ~PowLUT();
protected:
	virtual void BuildLUT(size_t len, float lo, float hi);
	float fExponent;
};

class KIPLSHARED_EXPORT GaussLUT : public kipl::math::LUTbase {
public:
	GaussLUT(size_t len, float m, float s, float lo, float hi);
	virtual ~GaussLUT();
protected:
	virtual void BuildLUT(size_t len, float lo, float hi);
	float m_fMean;
	float m_fSigma;
};
}}

#endif /*LUTCOLLECTION_H_*/
