

#include <vector>
#include <cmath>
#include <tuple>

#include "../../include/generators/SignalGenerator.h"

namespace kipl { namespace generators {

void SignalGenerator::Sine(size_t N, float start, float stop, float A, float w, float q, std::vector<float> &axis, std::vector<float> &signal)
{
    std::ignore = A;
    std::ignore = w;
    std::ignore = q;

	axis.resize(N);
	signal.resize(N);
	
	std::vector<float>::iterator ait=axis.begin();
	std::vector<float>::iterator sit=signal.begin();
	
	float step=(stop-start)/(N-1);
		
    for (float x=start; ait!=axis.end(); ait++, sit++, x+=step)
    {
		*ait=x;
		*sit=std::sin(x);
	}	
}

void SignalGenerator::Sine(size_t N, float start, float stop, float A, float w, float q, float *axis, float *signal)
{
    std::ignore = A;
    std::ignore = w;
    std::ignore = q;

	float step=(stop-start)/(N-1);
	size_t i=0;
	float x=start;
    for (x=start, i=0; i<N; i++, x+=step)
    {
        if (axis!=nullptr)
			axis[i]=x;
		signal[i]=std::sin(x);
	}	
}
}}

