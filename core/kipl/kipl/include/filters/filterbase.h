

#ifndef __FILTERBASE_H
#define __FILTERBASE_H
#include "../kipl_global.h"

#include <iostream>

#include "../base/timage.h"

namespace kipl { namespace UnitTests { namespace basefilters {
class testFilterBase;
}}}

namespace kipl {
/// \brief Name space that contains different filters
namespace filters {

/// \brief Selects if a filter can be implemented with separable kernels
enum class KernelType {
    Full,   ///< The kernel must be fully defined
    Separable   ///< The kernel is separable and can be computed much faster
};

/// \brief Base class for all filters. The class does not support any datatype.
class KIPLSHARED_EXPORT FilterBase {
public:
    /// \brief Selector for the edge processing style
	enum EdgeProcessingStyle {
    EdgeZero=0,		///< Pad the edge by zeros
    EdgeSame=1,		///< Pad the edge with the same value as the edge
    EdgeMirror=2,	///< Pad the edge with the mirrored edge
    EdgeValid=3		///< Process the valid region only
	};

	virtual ~FilterBase();
protected:
    /// \brief Prepares an array with neighborhood indices of a kernel
    /// \param imgDims The size of the image to be filtered
    /// \param nKernelDims Size of the filter kernel
    /// \param nDims Number of image dimensions
    size_t PrepareIndex(const std::vector<size_t> & imgDims,
                        const std::vector<size_t> &  nKernelDims);

    std::vector<ptrdiff_t> nKernelIndex;
	size_t nKernel;
};

/// \brief Template base class for filter implementations.
template<class T, size_t nDims>
class TFilterBase : public FilterBase
{
	friend class kipl::UnitTests::basefilters::testFilterBase; 
public:
    /// \brief C'tor to initialize the filter dimensions.
    /// \param kDims Array contatining the kernel size per axis. There shall be nDims entries in the array.
    TFilterBase(const std::vector<size_t> & kDims);
    /// \brief C'tor to initialize the filter dimensions and to provide kernel weights.
    /// \param kernel Array containing the filter weights. There shall be $\prod^{nDims}_i kDims_i$ entries in the array
    /// \param kDims Array contatining the kernel size per axis. There shall be nDims entries in the array.
    TFilterBase(const std::vector<T> & kernel, const std::vector<size_t> & kDims);
	~TFilterBase();
    virtual kipl::base::TImage<T,nDims> operator() (kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle edgeStyle);
protected:
    /// \brief Filter core method that process most of the image
    /// \param img Pointer to the image to filter
    /// \param imgDims image dimensions
    /// \param res pointer to the filtered image
    /// \param resDims dimensions of the filtered image
	int ProcessCore(T const * const img,
          const std::vector<size_t> & imgDims,
		  T *res, 
          const std::vector<size_t> & resDims);

    /// \brief Edge filtering method that handles the edge processing
    /// \param img Pointer to the image to filter
    /// \param res pointer to the filtered image
    /// \param imgDims image dimensions
    /// \param epStyle Select how to process the edges
	int ProcessEdge(T const * const img, 
		  T * res, 
          const std::vector<size_t> & imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle);

    /// \brief Specific edge filtering method that handles the edge processing for 1D data
    /// \param img Pointer to the image to filter
    /// \param res pointer to the filtered image
    /// \param imgDims image dimensions
    /// \param epStyle Select how to process the edges
	int ProcessEdge1D(T const * const img, 
		  T * res, 
          const std::vector<size_t> & imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle);

    /// \brief Specific edge filtering method that handles the edge processing for 2D data
    /// \param img Pointer to the image to filter
    /// \param res pointer to the filtered image
    /// \param imgDims image dimensions
    /// \param epStyle Select how to process the edges
    int ProcessEdge2D(T const * const img,
		  T * res, 
          const std::vector<size_t> & imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle);

    /// \brief Specific edge filtering method that handles the edge processing for 3D data
    /// \param img Pointer to the image to filter
    /// \param res pointer to the filtered image
    /// \param imgDims image dimensions
    /// \param epStyle Select how to process the edges
	int ProcessEdge3D(T const * const img, 
		  T * res, 
          const std::vector<size_t> & imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle);

	virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest) = 0;
    /// \brief The inner loop of the filter operation that performs that actual calculations between filter weights and pixels
	virtual void InnerLoop(T const * const src, T *dest, T value, size_t N) = 0;
    std::vector<T> pKernel; ///< Contains the filter weights

    std::vector<size_t> nKernelDims; ///< Contains the filter dimensions
};

}}

#include "core/filterbase.hpp"
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &str, kipl::filters::FilterBase::EdgeProcessingStyle eps);
std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &str, kipl::filters::KernelType kt);
#endif
