#ifndef _BASICPROJECTOR_HPP_
#define _BASICPROJECTOR_HPP_

#include "../../base/timage.h"
#include "../../base/kiplenums.h"
#include <iostream>
#include "../basicprojector.h"

namespace kipl { namespace math {

template<typename T>
kipl::base::TImage<T, 2> BasicProjector<T>::project(kipl::base::TImage<T, 3> img, const kipl::base::eImagePlanes plane)
{
	kipl::base::TImage<T,2> proj;

	switch (plane) {
		case kipl::base::ImagePlaneXY :
			return ProjectOnPlaneXY(img);
			break;
		case kipl::base::ImagePlaneXZ :
			return ProjectOnPlaneXZ(img);
			break;
		case kipl::base::ImagePlaneYZ :
			return ProjectOnPlaneYZ(img);
			break;
	}
	return proj;
}

template <typename T>
kipl::base::TImage<T, 2> BasicProjector<T>::ProjectOnPlaneXY(kipl::base::TImage<T, 3> img)
{
    kipl::base::TImage<T,2> proj(img.dims());
	T* pProj = proj.GetDataPtr();
	T* pImg  = img.GetDataPtr();
	for (size_t z=0, i=0; z<img.Size(2); z++) {
		for (size_t y=0, j=0; y<img.Size(1); y++) {
			for (size_t x=0; x<img.Size(0); x++,i++,j++) {
				pProj[j]+=pImg[i];
			}
		}
	}

	return proj;
}

template <typename T>
kipl::base::TImage<T, 2> BasicProjector<T>::ProjectOnPlaneXZ(kipl::base::TImage<T, 3> img)
{
    std::vector<size_t> dims={img.Size(0), img.Size(2)};
	kipl::base::TImage<T,2> proj(dims);
	T* pProj = proj.GetDataPtr();
	T* pImg  = img.GetDataPtr();
	for (size_t z=0, i=0; z<img.Size(2); z++) {
		for (size_t y=0; y<img.Size(1); y++) {
			for (size_t x=0; x<img.Size(0); x++,i++) {
				pProj[x+z*dims[0]]+=pImg[i];
			}
		}
	}

	return proj;
}

template <typename T>
kipl::base::TImage<T, 2> BasicProjector<T>::ProjectOnPlaneYZ(kipl::base::TImage<T, 3> img)
{
    std::vector<size_t> dims={img.Size(1), img.Size(2)};
	kipl::base::TImage<T,2> proj(dims);
	T* pProj = proj.GetDataPtr();
	T* pImg  = img.GetDataPtr();
	for (size_t z=0, i=0; z<img.Size(2); z++) {
		for (size_t y=0; y<img.Size(1); y++) {
			size_t index=y+z*dims[0];
			for (size_t x=0; x<img.Size(0); x++,i++) {
				pProj[index]+=pImg[i];
			}
		}
	}

	return proj;
}

}} 
#endif
