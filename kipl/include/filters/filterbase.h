#ifndef __FILTERBASE_H
#define __FILTERBASE_H
#include "../kipl_global.h"
#include <iostream>
#include "../base/timage.h"

namespace kipl { namespace UnitTests { namespace basefilters {
class testFilterBase;
}}}

namespace kipl { namespace filters {

class KIPLSHARED_EXPORT FilterBase {
public:
	enum EdgeProcessingStyle {
	EdgeZero=0,		// Pad the edge by zeros
	EdgeSame=1,		// Pad the edge with the same value as the edge
	EdgeMirror=2,	// Pad the edge with the mirrored edge
	EdgeValid=3		// Process the valid region only
	};

	virtual ~FilterBase();
protected:
	size_t PrepareIndex(size_t const * const imgDims, 
		size_t const * const nKernelDims, 
		size_t const nDims);

    ptrdiff_t * nKernelIndex;
	size_t nKernel;
};

template<class T, size_t nDims>
class TFilterBase : public FilterBase
{
	friend class kipl::UnitTests::basefilters::testFilterBase; 
public:
	TFilterBase(size_t const * const kDims);
	TFilterBase(T const * const kernel, size_t const * const kDims);
	~TFilterBase();
    virtual kipl::base::TImage<T,nDims> operator() (kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle edgeStyle);
protected:
	int ProcessCore(T const * const img,
		  size_t const * const imgDims,	
		  T *res, 
		  size_t const * const resDims);

	int ProcessEdge(T const * const img, 
		  T * res, 
		  size_t const * const imgDims, 
		  const FilterBase::EdgeProcessingStyle epStyle);

	int ProcessEdge1D(T const * const img, 
		  T * res, 
		  size_t const * const imgDims, 
		  const FilterBase::EdgeProcessingStyle epStyle);

	int ProcessEdge2D(T const * const img, 
		  T * res, 
		  size_t const * const imgDims, 
		  const FilterBase::EdgeProcessingStyle epStyle);

	int ProcessEdge3D(T const * const img, 
		  T * res, 
		  size_t const * const imgDims, 
		  const FilterBase::EdgeProcessingStyle epStyle);

	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest) = 0;
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N) = 0;
	T *pKernel;

    size_t nKernelDims[8];
	
};

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &str, FilterBase::EdgeProcessingStyle eps);
}}

#include "core/filterbase.hpp"

#endif
