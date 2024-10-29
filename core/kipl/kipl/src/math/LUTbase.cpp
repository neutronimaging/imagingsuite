//<LICENCE>

#include <iostream>
#include <sstream>

#ifdef __aarch64__
    #pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wcast-align"
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-W#warnings"
		#include <sse2neon/sse2neon.h>
	#pragma clang diagnostic pop
#else
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

#include "../../include/math/LUTbase.h"
#include "../../include/base/core/quad.h"
#include "../../include/base/KiplException.h"

using namespace std;

namespace kipl { namespace math {
LUTbase::LUTbase(size_t len, float lo, float hi) : 
    fLUT(nullptr),
	nLUT(len), 
	fLow(lo), 
	fHigh(hi),
	fSlope((nLUT-1)/(fHigh-fLow)),
	fIntercept(-fSlope*fLow+0.5f)
{
	fLUT=new float[nLUT+1];
}

LUTbase::~LUTbase()
{
    if (fLUT!=nullptr)
		delete [] fLUT;
}

void LUTbase::Resize(size_t len, float lo, float hi) 
{
    if (fLUT!=nullptr)
		delete [] fLUT;
	nLUT=len;
	fLow=lo;
	fHigh=hi;
	fSlope=((nLUT-1)/(fHigh-fLow));
	fIntercept=(-fSlope*fLow+0.5f);

	fLUT=new float[nLUT];
}

void LUTbase::InPlace(float *data, const size_t N)
{
	BasicLUT(data,N);

	//if ((pos & 3)==0) {
	//	SSELUT(data,N);
	//}
	//else {
	//	BasicLUT(data,N);
	//}
	
}

void LUTbase::BasicLUT(float *data, const size_t N)
{
	size_t index=0;

    for (size_t i=0; i<N; i++)
    {
		float val=data[i];
        if (val<fLow)
        {
			data[i]=fLUT[0];
			continue;
		}
        else if (fHigh<val)
        {
			data[i]=fLUT[nLUT-1];
			continue;
		} 
		
		index=static_cast<size_t>(val*fSlope+fIntercept);
		
		data[i]=fLUT[index];
	}
}

void LUTbase::SSELUT(float *data, const size_t N)
{
	__m128 hi128=_mm_set_ps(fHigh,fHigh,fHigh,fHigh);
	__m128 lo128=_mm_set_ps(fLow,fLow,fLow,fLow);
	__m128 slope128= _mm_set_ps(fSlope,fSlope,fSlope,fSlope);
	__m128 intercept128=_mm_set_ps(fIntercept,fIntercept,fIntercept,fIntercept);
	kipl::base::uFQuad val;
	kipl::base::uIQuad index;
	
	const size_t N4=N>>2;
	
	float *pData=data;
    for (size_t i=0; i<N4; i++, pData+=4)
    {
		val.xmm=_mm_load_ps(pData);
		val.xmm=_mm_max_ps(val.xmm,lo128);
		_mm_store_ps(pData,val.xmm);
	}
	
    for (size_t i=0; i<N4; i++, pData+=4)
    {
		val.xmm=_mm_load_ps(pData);
		val.xmm=_mm_min_ps(val.xmm,hi128);
		_mm_store_ps(pData,val.xmm);
	}
	
	pData=data;
    for (size_t i=0; i<N4; i++, pData+=4)
    {
		val.xmm=_mm_load_ps(pData);
		val.xmm=_mm_add_ps(_mm_mul_ps(val.xmm,slope128), intercept128);
		index.xmm= _mm_cvtps_epi32(val.xmm);
		
		val.q.a=fLUT[index.q.a];
		val.q.b=fLUT[index.q.b];
		val.q.c=fLUT[index.q.c];
		val.q.d=fLUT[index.q.d];	
		
		_mm_stream_ps(pData,val.xmm);
	}
}

float LUTbase::operator()(const float val)
{
    if (val<fLow)
    {
		return fLUT[0];
	}
    else if (fHigh<val)
    {
		return fLUT[nLUT-1];
	} 

	float fIndex  = val*fSlope+fIntercept;
	size_t nIndex = static_cast<size_t>(fIndex);
	float fWeight = fIndex-nIndex;
	
	return (1.0f-fWeight)*fLUT[nIndex]+fWeight*fLUT[nIndex+1];

}

std::string LUTbase::Print()
{
	std::ostringstream s;
	
	s<<"LUT size="<<size()
	<<", slope="<<Slope()
	<<", intercept="
	<<Intercept()
	<<", low="<<Low()
	<<", high="<<High();
	
	return s.str();
}


}}

std::ostream & operator<<(std::ostream &s, kipl::math::LUTbase &lut)
{
	s<<lut.Print();
	
	return s;
}

