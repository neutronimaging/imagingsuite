#ifndef SINE2D_H_
#define SINE2D_H_

#include "../base/timage.h"

namespace kipl { namespace generators { 

class Sine2D
{
private:
	Sine2D() {}
public:
	static kipl::base::TImage<float,2> SineRings(size_t *dims, float fScale);
	static kipl::base::TImage<float,2> JaehneRings(size_t N, float fScale);
};

}}

#endif /*SINE2D_H_*/
