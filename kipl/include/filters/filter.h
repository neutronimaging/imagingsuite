#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "filterbase.h"

namespace kipl {
namespace filters {

/// \brief A convolution filter
///
/// Example with a 2D image
/// \code
/// size_t dims[]={100,100};
/// kipl::base::TImage<float,2> img(dims),res;
///
/// float kernel[]={1,1,1, 1,1,1, 1,1,1};
/// size_t fdims[]={3,3};
///
/// kipl::filters::TFilter<float,2> box(kernel,fdims);
/// res=box(img);
/// \endcode
template <typename T, size_t nDims>
class TFilter : public kipl::filters::TFilterBase<T,nDims>
{
public:
    /// \brief Initializes the filter with weights and filter dimensions
    /// \param kernel Array containing the kernel weights
    /// \param kDims array with the kernel dimensions
	TFilter(T const * const kernel, size_t const * const kDims);
	virtual ~TFilter(void);
protected:
    /// \brief implements the inner convloution loop
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N);
	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest);
};


}}

#include "core/filter.hpp"

#endif /*CONVOLUTION_H_*/
