//<LICENCE>

#ifndef TSUBIMAGE_HPP_
#define TSUBIMAGE_HPP_

#include "../timage.h"
#include "../KiplException.h"
#include "../tsubimage.h"

namespace kipl { namespace base {

template <typename T, size_t NDims>
TImage<T,NDims> TSubImage<T,NDims>::Get(TImage<T,NDims> const src, size_t const *const nStart, size_t const *const nLength)
{
	size_t const * const dims=src.Dims();
	
//	for (size_t i=0 ; i<NDims; i++)
//		if (dims[i]<nStart[i]+nLength[i])
	
	size_t startZ  = 0;
	size_t endZ    = 1;
	size_t startY  = 0;	
	size_t lengthY = 1;
	size_t startX  = nStart[0];	
	size_t sizeXY  = dims[0];
	size_t lengthX = nLength[0];
	
	switch (NDims) {
	case 2: 
		startY  = nStart[1];	
		lengthY = nLength[1];
				
		sizeXY  = dims[0]*dims[1];
		break;
	case 3:
		startZ  = nStart[2];	
		endZ    = startZ+nLength[2];
		startY  = nStart[1];	
		lengthY = nLength[1];
						
		sizeXY  = dims[0]*dims[1];
		break;
	}
		
	T * pSrc =const_cast<T*>( src.GetDataPtr());
	
	TImage<T,NDims> img(nLength);
	T* pImg = img.GetDataPtr();
	
	for (size_t z=startZ; z<endZ; z++) {
		pSrc+=z*sizeXY+startY*dims[0]+startX;
		for (size_t y=0; y<lengthY; y++) {
			pImg=img.GetLinePtr(y);
			memcpy(pImg,pSrc,lengthX*sizeof(T));
			pSrc+=dims[0];
		}
	}
			
	return img;
}

template <typename T, size_t NDims>
TImage<T,NDims> TSubImage<T,NDims>::Get(TImage<T,NDims> const src, size_t const * const roi, bool includeCoord)
{
    std::vector<size_t> dims(NDims);
    size_t plusOne=includeCoord ? 1 : 0;
    switch (NDims){
    case 1 :
        dims[0]=roi[1]-roi[0]+plusOne; break;
    case 2 :
        dims[0]=roi[2]-roi[0]+plusOne;
        dims[1]=roi[3]-roi[1]+plusOne;
        break;
    case 3 :
        dims[0]=roi[3]-roi[0]+plusOne;
        dims[1]=roi[4]-roi[1]+plusOne;
        dims[1]=roi[5]-roi[2]+plusOne;
        break;
    default:
        throw kipl::base::KiplException("nDim greater than 3 is not supported.",__FILE__,__LINE__);
        break;
    }

    kipl::base::TImage<T,NDims> img(dims);

    switch (NDims){
    case 1 :
        memcpy(img.GetDataPtr(),src.GetDataPtr()+roi[0],dims[0]*sizeof(T)); break;
    case 2 :
        for (size_t i=0; i<img.Size(1); i++ )
            memcpy(img.GetLinePtr(i),src.GetLinePtr(i+roi[1])+roi[0],sizeof(T)*img.Size(0));
        break;
    case 3 :
        for (size_t i=0; i<img.Size(2); i++ )
            for (size_t j=0; j<img.Size(1); j++ )
                memcpy(img.GetLinePtr(j,i),src.GetLinePtr(i+roi[1],j+roi[2])+roi[0],sizeof(T)*img.Size(0));
        break;
    default:
        throw kipl::base::KiplException("nDim greater than 3 is not supported.",__FILE__,__LINE__);
        break;
    }

    return img;
}

template <typename T, size_t NDims>
void TSubImage<T,NDims>::Put(const TImage<T,NDims> src, TImage<T, NDims> dest, size_t const *const nStart)
{
	size_t startZ  = 0;
	size_t endZ    = 1;
	size_t startY  = 0;	
	size_t lengthY = 1;
	size_t startX  = nStart[0];	
	size_t sizeXY  = dest.Size(0);
	size_t lengthX = src.Size(0);
	
	switch (NDims) {
	case 2: 
		startY  = nStart[1];	
		lengthY = src.Size(1);
				
		sizeXY  = dest.Size(0)*dest.Size(1);
		break;
	case 3:
		startZ  = nStart[2];	
		endZ    = startZ+src.Size(2);
		startY  = nStart[1];	
		lengthY = src.Size(1);
						
		sizeXY  = dest.Size(0)*dest.Size(1);
		break;
	}
		
	T * pSrc  = const_cast<T*>( src.GetDataPtr());
	
	for (size_t z=startZ; z<endZ; z++) {
		T * pDest = dest.GetLinePtr(0,z);
		pDest+=z*sizeXY+startY*dest.Size(0)+startX;
		
		for (size_t y=0; y<lengthY; y++) {
			memcpy(pDest, pSrc, lengthX*sizeof(T));
			pSrc+=src.Size(0);
			pDest+=dest.Size(0);
		}
	}
	
}

template <typename T>
TImage<T,2> ExtractSlice(TImage<T,3> volume, size_t idx)
{
	if (volume.Size(2)<=idx)
		throw kipl::base::KiplException("ExtractSlice: Index out of range",__FILE__,__LINE__);

	TImage<T,2> slice(volume.Dims());

	memcpy(slice.GetDataPtr(),volume.GetLinePtr(0,idx),slice.Size()*sizeof(T));

	return slice;
}

template <typename T>
void InsertSlice(TImage<T,3> volume, TImage<T,2> slice, size_t idx)
{
	if (volume.Size(2)<=idx)
		throw kipl::base::KiplException("InsertSlice: Index out of range",__FILE__,__LINE__);
	if ((volume.Size(0)!=slice.Size(0)) || (volume.Size(1)!=slice.Size(1)))
		throw kipl::base::KiplException("InsertSlice: Slice dims does not match volume xy-plane",__FILE__,__LINE__);
	
	memcpy(volume.GetLinePtr(0,idx),slice.GetDataPtr(),slice.Size()*sizeof(T));
}
}}

#endif /*TEXTRACTOR_HPP_*/
