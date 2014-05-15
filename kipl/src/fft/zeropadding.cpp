/*
 * zeropadding.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: anders
 */

#include <cstdlib>
#include <cmath>

namespace kipl { namespace math { namespace fft {

size_t NextPower2(size_t N)
{
	const double logtwo=log(2.0);

	return static_cast<size_t>(floor(exp(logtwo*ceil(log(static_cast<double>(N))/logtwo))+0.5));
}

}}}
