

#ifndef STDDEVFILTER_H
#define STDDEVFILTER_H

#include "../kipl_global.h"

#include "../base/timage.h"
#include "filterbase.h"

namespace kipl { namespace filters {

/// \brief A filter to compute the local standard deviation of a 2D image
class KIPLSHARED_EXPORT StdDevFilter
{
public:
    /// \brief Enum to select which type of Variance to compute
    ///
    /// This enum is currently not in use but it would make sense to implement it.
    enum VarianceType {
        StandardVariance, ///< The traditional summed variance
        MADVariance       ///< Variance computed using median average deviation
    };

    /// \brief Initializes the filter size and processing style
    /// \param size The size of the filter neighborhood
    /// \param eps Selects which type of edge processing to use
    StdDevFilter(size_t size=5UL, FilterBase::EdgeProcessingStyle eps=FilterBase::EdgeMirror);


    ~StdDevFilter();

    /// \brief Applies the std dev filter to an image
    /// \param img The image to filter
    ///
    /// \returns A filtered image
    kipl::base::TImage<float,2> operator() (kipl::base::TImage<float,2> img);
protected:
    kipl::base::TImage<float,2> BoxFilter(kipl::base::TImage<float,2> img);

    size_t m_FilterSize;    ///< Size of the neighborhood to sum
    FilterBase::EdgeProcessingStyle m_eEdgeProcessingStyle; ///< Selects how the edges are to be processed
};

}}

#endif // STDDEVFILTER_H
