

#ifndef TROTATE_HPP_
#define TROTATE_HPP_
#include "../trotate.h"
#include "../timage.h"
#include "../KiplException.h"

namespace kipl{ namespace base {
template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::Rotate(kipl::base::TImage<T,2> &img, double angle)
{
    (void)img;

 if (fmod(angle,90.0)==0) {


 }

 return img;

}

template <typename T>
void TRotate<T>::HorizontalSkew (T *pSrc,
	                size_t nSrc,
	                T *pDst,
	                size_t sDst,
	                size_t uRow,
	                int iOffset,
	                double dWeight,
	                T blackvalue)
{
    (void)pSrc;
    (void)nSrc;
    (void)pDst;
    (void)sDst;
    (void)uRow;
    (void)iOffset;
    (void)dWeight;
    (void)blackvalue;
}

template <typename T>
void TRotate<T>::VerticalSkew (T *pSrc,
		                size_t nSrc,
		                T *pDst,
		                size_t sDst,
		                size_t uRow,
		                int iOffset,
		                double dWeight,
		                T blackvalue)
{
    (void)pSrc;
    (void)nSrc;
    (void)pDst;
    (void)sDst;
    (void)uRow;
    (void)iOffset;
    (void)dWeight;
    (void)blackvalue;
}


template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::Rotate90  ( kipl::base::TImage<T,2> & img)
{
    std::vector<size_t> dims={img.Size(1), img.Size(0)};
    dst.resize(dims);

	size_t sx=dst.Size(0);
	T *pDst=dst.GetDataPtr();
	T *pImg=img.GetDataPtr();
	for (size_t i=0; i<img.Size(1); i++) {
		pImg=img.GetLinePtr(i);
		pDst=dst.GetDataPtr()+(dst.Size(0)-1-i);
		for (size_t j=0; j<img.Size(0); j++) {
			pDst[j*sx]=pImg[j];
		}
	}


	return dst;
}

template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::Rotate180 (kipl::base::TImage<T,2> & img)
{
    dst=img;

	dst.Clone();

	size_t sy=dst.Size(1)-1;
	size_t Nx=sizeof(T)*img.Size(0);

	for (size_t i=0; i<dst.Size(1); i++) {
		memcpy(dst.GetLinePtr(i),img.GetLinePtr(sy-i),Nx);
	}

	size_t sx=dst.Size(0)-1;
	for (size_t i=0; i<dst.Size(1); i++) {
		T* pDst=dst.GetLinePtr(i);
		for (size_t j=0; j<(dst.Size(0)>>1); j++) {
			swap(pDst[j],pDst[sx-j]);
		}
	}

	return dst;
}

template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::Rotate270 (kipl::base::TImage<T,2> &img)
{
    std::vector<size_t> dims={img.Size(1), img.Size(0)};
    dst.resize(dims);

	size_t sx=dst.Size(0);
	size_t sy1=(dst.Size(1)-1);
	T *pDst=dst.GetDataPtr();
	T *pImg=img.GetDataPtr();
	for (size_t i=0; i<img.Size(1); i++) {
		pImg=img.GetLinePtr(i);
		pDst=dst.GetDataPtr()+i;
		for (size_t j=0; j<img.Size(0); j++) {
			pDst[(sy1-j)*sx]=pImg[j];
		}
	}

	return dst;
}

template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::Rotate45 (kipl::base::TImage<T,2> &img, double angle)
{
    (void)angle;
	throw kipl::base::KiplException("Rotate45 is not implemented",__FILE__,__LINE__);

	size_t dims[2]={img.Size(1), img.Size(0)};
    dst.Resize(dims);

	return dst;
}

template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::MirrorHorizontal  (kipl::base::TImage<T,2> &img)
{
    dst=img;

	dst.Clone();

	size_t sx=dst.Size(0)-1;
	for (size_t i=0; i<dst.Size(1); i++) {
		T* pDst=dst.GetLinePtr(i);
		for (size_t j=0; j<(dst.Size(0)>>1); j++) {
			swap(pDst[j],pDst[sx-j]);
		}
	}

	return dst;
}

template <typename T>
kipl::base::TImage<T,2> & TRotate<T>::MirrorVertical  (kipl::base::TImage<T,2> & img)
{
    dst.resize(img.dims());


	size_t sy=dst.Size(1)-1;
	size_t Nx=sizeof(T)*img.Size(0);

	for (size_t i=0; i<dst.Size(1); i++) {
		memcpy(dst.GetLinePtr(i),img.GetLinePtr(sy-i),Nx);
	}

	return dst;
}

template <typename T>
kipl::base::TImage<float,2> TRotate<T>::Rotate(kipl::base::TImage<float,2> & img,
        kipl::base::eImageFlip flip,
        kipl::base::eImageRotate rotate)
{

    kipl::base::TImage<float,2> res=img;

    res.Clone();

    switch (rotate) {
        case kipl::base::ImageRotateDefault : break;
        case kipl::base::ImageRotateNone : break;
        case kipl::base::ImageRotate90   : res=Rotate90(img);break;
        case kipl::base::ImageRotate180  : res=Rotate180(img);break;
        case kipl::base::ImageRotate270  : res=Rotate270(img);break;
    }

    switch (flip) {
        case kipl::base::ImageFlipDefault             : break;
        case kipl::base::ImageFlipNone                : break;
        case kipl::base::ImageFlipHorizontal          : res=MirrorHorizontal(res); break;
        case kipl::base::ImageFlipVertical            : res=MirrorVertical(res); break;
        case kipl::base::ImageFlipHorizontalVertical  :
            res=MirrorHorizontal(res);
            res=MirrorVertical(res);
            break;
    }

    return res;
}
}}

#endif /* TROTATE_HPP_ */
