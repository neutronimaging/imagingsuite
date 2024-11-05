

#ifndef _SignalGenerator_H_
#define _SignalGenerator_H_

#include <cstddef>
#include <vector>

namespace kipl { namespace generators {

class SignalGenerator {
public:
	static void Sine(size_t N, float start, float stop, float A, float w, float q, std::vector<float> &axis, std::vector<float> &signal);	
	static void Sine(size_t N, float start, float stop, float A, float w, float q, float *axis, float *signal);
};

}}

#endif
