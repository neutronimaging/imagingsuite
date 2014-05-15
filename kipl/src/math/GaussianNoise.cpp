#include <math.h>
#include <cstdlib>

namespace kipl { namespace math {

// boxmuller.c           Implements the Polar form of the Box-Muller
//                         Transformation
//
//                      (c) Copyright 1994, Everett F. Carter Jr.
//                          Permission is granted by the author to use
//			  this software for any application provided this
//			  copyright notice is preserved.
float Gaussian(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	const float scale=2.0f/static_cast<float>(RAND_MAX);

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = scale * std::rand() - 1.0f;
			x2 = scale * std::rand() - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0f );

		w = sqrt( (-2.0f * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}


void GaussianNoise(float *pData, size_t N, float m, float s)
{
	for (size_t i=0; i<N; i++) {
		pData[i]=Gaussian(m,s);
	}
}

void AddGaussianNoise(float *pData, size_t N, float m, float s)
{
	for (size_t i=0; i<N; i++) {
		pData[i]+=Gaussian(m,s);
	}
}

}}


