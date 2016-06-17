/*
 * NoiseImage.cpp
 *
 *  Created on: Mar 28, 2011
 *      Author: kaestner
 */
#include "../../include/base/timage.h"
#include "../../include/filters/filter.h"
#include "../../include/math/GaussianNoise.h"


namespace kipl { namespace generators {

void AddTexturedNoise(kipl::base::TImage<float,2> &img, float variance, size_t width)
{
    size_t const * const dims=img.Dims();
	kipl::base::TImage<float,2> noise(dims);
	size_t fdims[]={width,width};
	float kernel[1024];
	float w=1.0f/static_cast<float>(width*width);
	for (size_t i=0; i<width*width; i++) {
		kernel[i]=w;
	}
	kipl::filters::TFilter<float,2> box_filter(kernel,fdims);

	kipl::math::GaussianNoise(noise.GetDataPtr(),noise.Size(),0.0f,1.0f);
	kipl::base::TImage<float,2> texture=box_filter(noise,kipl::filters::FilterBase::EdgeMirror);

	float * pImg=img.GetDataPtr();
	float * pTexture= texture.GetDataPtr();
	for (size_t j=0; j<texture.Size(); j++) {
		pImg[j]+=variance*pTexture[j];
	}
}

void AddTexturedNoise(kipl::base::TImage<float,3> &img, float variance, size_t width)
{
	size_t const * const dims=img->Dims();
	kipl::base::TImage<float,2> noise(dims);
	size_t fdims[]={width,width};
	float kernel[1024];
	float w=1.0f/static_cast<float>(width*width);
	for (size_t i=0; i<width*width; i++) {
		kernel[i]=w;
	}
	kipl::filters::TFilter<float,2> box_filter(kernel,fdims);

	for (size_t i=0; i<img->Size(2); i++) {
		kipl::math::GaussianNoise(noise.GetDataPtr(),noise.Size(),0.0f,1.0f);
		kipl::base::TImage<float,2> texture=box_filter(noise,kipl::filters::FilterBase::EdgeMirror);

		float * pImg=img->GetLinePtr(0,i);
		float * pTexture= texture.GetDataPtr();
		for (size_t j=0; j<texture.Size(); j++) {
			pImg[j]+=variance*pTexture[j];
		}

	}
}

}}
