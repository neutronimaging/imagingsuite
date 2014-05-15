#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "filterbase.h"

namespace kipl { namespace filters {
template <typename T, size_t nDims>
class TFilter : public kipl::filters::TFilterBase<T,nDims>
{
public:
	TFilter(T const * const kernel, size_t const * const kDims);
	virtual ~TFilter(void);
protected:
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};

}}

#include "core/filter.hpp"

#endif /*CONVOLUTION_H_*/
