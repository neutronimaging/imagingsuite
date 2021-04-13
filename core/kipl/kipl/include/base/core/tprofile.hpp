//<LICENCE>

#ifndef TPROFILE_HPP
#define TPROFILE_HPP

#include "../tprofile.h"
#include <algorithm>
#include "../KiplException.h"


namespace kipl { namespace base {

template <typename T, typename S>
void VerticalProjection2D(const T *pData, const std::vector<size_t> &dims, S *pProfile, bool bMeanProjection)
{

    memset(pProfile,0,dims[1]*sizeof(S));
    for (size_t y=0; y<dims[1]; y++) {
        const T* d=pData+y*dims[0];
        for (size_t x=0; x<dims[0]; x++) {
            pProfile[y]+=d[x];
        }
    }

    if (bMeanProjection) {
        for (size_t y=0; y<dims[1]; y++)
        pProfile[y]=pProfile[y]/dims[0];
    }
}

template <typename T>
std::vector<T> projection2D(const T *pData, const std::vector<size_t> &dims, int axis, bool bMeanProjection, const std::vector<size_t> &slice)
{
    if (!slice.empty() && slice.size()!=2)
        throw kipl::base::KiplException("The slice vector has the wrong size",__FILE__,__LINE__);
    std::vector<T> profile;

    size_t N=0UL;

    switch (axis)
    {
    case 0 :
        {
            N=dims[0];
            profile = std::vector<float>(dims[1],static_cast<T>(0));
            size_t begin = slice.empty() ? 0       : slice[0];
            size_t end   = slice.empty() ? dims[0] : slice[1];
            for (size_t y=0; y<dims[1] ; ++y)
            {
                const T* d=pData+y*dims[0];

                T sum=0;

                for (size_t x=begin; x<end; ++x)
                    sum+=d[x];

                profile[y]=sum;
            }
        }
        break;
    case 1 :
        {
            N=dims[1];
            profile = std::vector<float>(dims[0],static_cast<T>(0));
            size_t begin = slice.empty() ? 0       : slice[0];
            size_t end   = slice.empty() ? dims[1] : slice[1];

            for (size_t y=begin; y<end; ++y)
            {
                const T* d=pData+y*dims[0];

                for (size_t x=0; x<dims[0]; ++x)
                    profile[x]+=d[x];
            }
        }
        break;
    default:
        throw kipl::base::KiplException("Axis can only be 0 or 1 for a 2D image",__FILE__,__LINE__);
    }

    if (bMeanProjection)
    {
        std::transform(profile.begin(), profile.end(), profile.begin(),
                       std::bind1st(std::divides<T>(), N));
    }



}

template <typename T, typename S>
void HorizontalProjection2D(const T *pData, const std::vector<size_t> &dims, S *pProfile, bool bMeanProjection)
{
    memset(pProfile,0,dims[0]*sizeof(S));
    for (size_t y=0; y<dims[1]; y++) {
        const T* d=pData+y*dims[0];
        for (size_t x=0; x<dims[0]; x++) {
            pProfile[x]+=d[x];
        }
    }

    if (bMeanProjection) {
        for (size_t x=0; x<dims[0]; x++)
        pProfile[x]=pProfile[x]/dims[1];
    }
}


}}

#endif // TPROFILE_HPP
