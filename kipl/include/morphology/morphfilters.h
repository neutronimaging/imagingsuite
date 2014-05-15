#ifndef __morphfilters_h_
#define __morphfilters_h_
#include "../filters/filterbase.h"

namespace kipl { namespace morphology {
template <typename T, size_t nDims>
class TErode : public kipl::filters::TFilterBase<T,nDims>
{
public:
	TErode(T const * const kernel, size_t const * const kDims);
	virtual ~TErode(void);
protected:
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};

template <typename T, size_t nDims>
class TDilate : public kipl::filters::TFilterBase<T,nDims>
{
public:
	TDilate(T const * const kernel, size_t const * const kDims);
	virtual ~TDilate(void);
protected:
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};
}}

#include "core/morphfilters.hpp"
#endif
