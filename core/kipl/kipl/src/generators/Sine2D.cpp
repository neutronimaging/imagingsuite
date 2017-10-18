//<LICENCE>

#include <cmath>

#include "../../include/generators/Sine2D.h"
#include "../../include/base/timage.h"
#include "../../include/math/mathconstants.h"

using namespace std;

namespace kipl { namespace generators {

kipl::base::TImage<float,2> Sine2D::SineRings(size_t *dims, float fScale=1.0f)
{
	kipl::base::TImage<float,2> img(dims);
	size_t N=std::min(dims[0],dims[1]);
	const float cfHalf=N/2.0f;
	const float cfInvN=fScale*fPi/N;
	
	for (size_t y=0, i=0; y<dims[1]; y++) {
		float yy=static_cast<float>(y)-cfHalf;
		yy*=yy;
		for (size_t x=0; x<dims[0]; x++, i++) {
			float r2=static_cast<float>(x)-cfHalf;
			r2*=r2;
			r2+=yy;
			img[i]=sin(sqrt(r2)*cfInvN);
		}
	}
	return img;
}

kipl::base::TImage<float,2> Sine2D::JaehneRings(size_t N, float fScale=1.0f)
{
	size_t dims[]={N,N};
	kipl::base::TImage<float,2> img(dims);
	
	const float cfHalf=N/2.0f;
	const float cfInvN=fScale*fPi/N;
	
	for (size_t y=0, i=0; y<N; y++) {
		float yy=static_cast<float>(y)-cfHalf;
		yy*=yy;
		for (size_t x=0; x<N; x++, i++) {
			float r2=static_cast<float>(x)-cfHalf;
			r2*=r2;
			r2+=yy;
			img[i]=sin(sqrt(r2)*cfInvN);
		}
	}
	return img;
}

}}
