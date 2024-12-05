

#ifndef MORPHFILTERS_H
#define MORPHFILTERS_H

#include "../filters/filterbase.h"

namespace kipl { namespace morphology {
template <typename T, size_t nDims>
class TErode : public kipl::filters::TFilterBase<T,nDims>
{
public:
    TErode(const std::vector<T> & kernel, const std::vector<size_t> & kDims);
	virtual ~TErode(void);
protected:
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};

template <typename T, size_t nDims>
class TDilate : public kipl::filters::TFilterBase<T,nDims>
{
public:
    TDilate(const std::vector<T> & kernel, const std::vector<size_t> & kDims);
	virtual ~TDilate(void);
protected:
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};
}}

#include "core/morphfilters.hpp"
#endif
