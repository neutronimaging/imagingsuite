//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef LUTCOLLECTION_H_
#define LUTCOLLECTION_H_
#include "LUTbase.h"
namespace kipl { namespace math {

class LogLUT : public kipl::math::LUTbase {
public:
	LogLUT(size_t len, float lo, float hi);
	virtual ~LogLUT();
protected:
	virtual void BuildLUT(size_t len, float lo, float hi);
};

class SigmoidLUT : public kipl::math::LUTbase {
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

class PowLUT : public kipl::math::LUTbase {
public:
	PowLUT(size_t len, float exponent, float lo, float hi);
	virtual ~PowLUT();
protected:
	virtual void BuildLUT(size_t len, float lo, float hi);
	float fExponent;
};

class GaussLUT : public kipl::math::LUTbase {
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
