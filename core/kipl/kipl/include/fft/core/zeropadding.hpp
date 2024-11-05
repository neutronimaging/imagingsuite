

#ifndef ZEROPADDING_HPP_
#define ZEROPADDING_HPP_

#include "../../base/KiplException.h"

namespace kipl { namespace math { namespace fft {

template <typename T>
void ZeroPad(kipl::base::TImage<T,2> &img, 
			 kipl::base::TImage<T,2> &padded, 
			 size_t const * const dims, 
			 size_t * const insert, 
			 ePaddingPosition position)
{
	if ((dims[0]<img.Size(0)) || (dims[1]<img.Size(1)))
        throw kipl::base::KiplException("Requested dimensions of the padded image are smaller than the original image.",__FILE__,__LINE__);
	
	padded.Resize(dims);

	std::cout<<"doing zp "<<img<<" "<<padded<<std::endl;


	switch (position) {
		case PadCorner:
			insert[0]=0;
			insert[1]=1;
			break;
		case PadCenter:
			insert[0]=(dims[0]-img.Size(0))>>1;
			insert[1]=(dims[1]-img.Size(1))>>1;
			break;
	}

	padded=static_cast<T>(0);

	std::cout<<"insert at"<<insert[0]<<" "<< insert[1]<<" "<<padded<<std::endl;

	for (size_t y=0; y<img.Size(1); y++) {
		memcpy(padded.GetLinePtr(y+insert[1])+insert[0],img.GetLinePtr(y),sizeof(T)*img.Size(0));
	}


	std::cout<<" y"<<std::endl;
}

template <typename T>
void BaseTwoZeroPad(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &padded, size_t * const insert, size_t addexp)
{
	size_t dims[2]={NextPower2(img.Size(0))<<addexp , NextPower2(img.Size(1))<<addexp};
	ZeroPad(img, padded, dims, insert, PadCenter);
}

template <typename T>
void RemoveZeroPad(kipl::base::TImage<T,2> &padded, kipl::base::TImage<T,2> &img, size_t *insert, size_t const * dims)
{
	img.Resize(dims);

	for (size_t i=0; i<img.Size(1); i++)
		memcpy(img.GetLinePtr(i),padded.GetLinePtr(i+insert[1])+insert[0],img.Size(0)*sizeof(T));
}

}}}

#endif /* ZEROPADDING_H_ */
