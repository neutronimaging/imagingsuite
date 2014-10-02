#ifndef TPROFILE_HPP
#define TPROFILE_HPP

namespace kipl { namespace base {

template <typename T, typename S>
void verticalprofile2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProfile)
{

    memset(pProfile,0,dims[1]*sizeof(S));
    for (size_t y=0; y<dims[1]; y++) {
        const T* d=pData+y*dims[0];
        for (size_t x=0; x<dims[0]; x++) {
            pProfile[y]+=d[x];
        }
    }

    if (bMeanProfile) {
        for (size_t y=0; y<dims[1]; y++)
        pProfile[y]=pProfile[y]/dims[0];
    }
}

template <typename T, typename S>
void horizontalprofile2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProfile)
{
    memset(pProfile,0,dims[0]*sizeof(S));
    for (size_t y=0; y<dims[1]; y++) {
        T* d=pData+y*dims[0];
        for (size_t x=0; x<dims[0]; x++) {
            pProfile[x]+=d[x];
        }
    }

    if (bMeanProfile) {
        for (size_t x=0; x<dims[0]; x++)
        pProfile[x]=pProfile[x]/dims[1];
    }
}


}}

#endif // TPROFILE_HPP
