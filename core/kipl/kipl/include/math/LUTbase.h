//<LICENCE>

#ifndef LUTBASE_H_
#define LUTBASE_H_
#include "../kipl_global.h"

#include <iostream>
#include <string>

namespace kipl { namespace math {
class KIPLSHARED_EXPORT LUTbase
{
public:
	LUTbase(size_t len, float lo, float hi);
	virtual ~LUTbase();
	void InPlace(float *data, const size_t N);
	float operator()(const float val);
	size_t index(float x) { return static_cast<size_t>(x*fSlope+fIntercept);}
	size_t size() { return nLUT; }
	float Low() {return fLow;}
	float High() {return fHigh;}
	float Slope() {return fSlope;}
	float Intercept() {return fIntercept;}
	virtual std::string Print();
protected:
	float *fLUT;
	
private:
	size_t nLUT;
	float fLow;
	float fHigh;
	float fSlope;
	float fIntercept;
	
protected:
	virtual void BuildLUT(size_t len, float lo, float hi) = 0;
	void Resize(size_t len, float lo, float hi);
	
	void BasicLUT(float *data, const size_t N);
	void SSELUT(float *data, const size_t N);
};
}}

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::math::LUTbase &lut);
#endif /*LUTBASE_H_*/
