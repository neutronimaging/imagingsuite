/*
 * boxmueller.h
 *
 *  Created on: Mar 27, 2011
 *      Author: anders
 */

#ifndef BOXMULLER_H_
#define BOXMULLER_H_

namespace kipl { namespace math {


float Gaussian(float m, float s);

void GaussianNoise(float *pData, size_t N, float m=0.0f, float s=1.0f);

void AddGaussianNoise(float *pData, size_t N, float m=0.0f, float s=1.0f);

}}


#endif /* BOXMUELLER_H_ */
