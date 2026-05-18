#if !defined(LISTMORPHOLOGY_HPP)
#define LISTMORPHOLOGY_HPP

#include <vector>
#include <algorithm>
#include <base/kiplenums.h>
#include <morphology/pixeliterator.h>

#include "../unique.h"


namespace kipl {
namespace algorithms {
    
template<class T>
std::vector<T> dilate_points(std::vector<T> &v, const std::vector<size_t>& dims, kipl::base::eConnectivity connectivity)
{
    ptrdiff_t imgSize = 1L;
    for (const auto & d : dims)
        imgSize*=static_cast<ptrdiff_t>(d);

    kipl::base::PixelIterator neighborhood(dims, connectivity);

    std::vector<T> result{};
    result.reserve(v.size()*(neighborhood.neighborhoodSize()+1));

    for (const auto &pp : v) 
    {
        ptrdiff_t pixPos = static_cast<ptrdiff_t>(pp);

        result.push_back(pp);

        neighborhood=pixPos;
        for (const auto &neighborPix : neighborhood.neighborhood())
        {
            ptrdiff_t pixel = pixPos + neighborPix;

            if ((0<=pixel) && (pixel<imgSize))
            {
                result.push_back(static_cast<T>(pixel));
            }

        }
    }

    return kipl::algorithms::unique_values(result);
    // return result;
}

}}
#endif