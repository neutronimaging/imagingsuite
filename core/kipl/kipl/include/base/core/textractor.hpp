

#ifndef TEXTRACTOR_HPP_
#define TEXTRACTOR_HPP_

#include <iostream>

#include "../../logging/logger.h"
#include "../timage.h"
#include "../kiplenums.h"
#include "../KiplException.h"
#include "../textractor.h"


namespace kipl { namespace base {

template<typename T,size_t N>
kipl::base::TImage<T,2> ExtractSlice(const kipl::base::TImage<T, N> &img,
                                     const size_t index,
                                     const kipl::base::eImagePlanes plane,
                                     const std::vector<size_t> &roi)
{
    if (N!=3)
        kipl::base::KiplException("ExtractSlice only works for N=3",__FILE__,__LINE__);

    auto tmproi = roi;
	kipl::logging::Logger logger("ExtractSlice");
    std::vector<size_t> dims={0,0};
	size_t offset[2]={0,0};
	kipl::base::TImage<T,2> dest;
	size_t sx=img.Size(0);
	size_t sxy=img.Size(0)*img.Size(1);

	switch (plane) {
	case kipl::base::ImagePlaneXY :
		if (img.Size(2)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXY.", __FILE__, __LINE__);
        if (roi.empty()) {
            dims = img.dims();
			offset[0]=0;
			offset[1]=0;
		}
		else {
            if (tmproi[2]<tmproi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
                swap(tmproi[2],tmproi[0]);
			}
            dims[0]=tmproi[2]-tmproi[0]+1;

            if (tmproi[3]<tmproi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
                swap(tmproi[3],tmproi[1]);
			}
            dims[1]=tmproi[3]-tmproi[1]+1;

            offset[0]=tmproi[0];
            offset[1]=tmproi[1];
		}

        dest.resize(dims);
		for (size_t i=0; i<dims[1]; i++)
			memcpy(dest.GetLinePtr(i),img.GetLinePtr(i+offset[1],index)+offset[0],dest.Size(0)*sizeof(T));

		break;
	case kipl::base::ImagePlaneXZ : 
		if (img.Size(1)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXZ.", __FILE__, __LINE__);

        if (roi.empty()) {
            dims= {img.Size(0),img.Size(2)};
			offset[0]=0;
			offset[1]=0;
		}
		else {
            if (tmproi[2]<tmproi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
                swap(tmproi[2],tmproi[0]);
			}
            dims[0]=tmproi[2]-tmproi[0]+1;

            if (tmproi[3]<tmproi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
                swap(tmproi[3],tmproi[1]);
			}
            dims[1]=tmproi[3]-tmproi[1]+1;
            offset[0]=tmproi[0];
            offset[1]=tmproi[1];
		}
		
        dest.resize(dims);
		
		for (size_t z=0; z<dims[1]; z++) {
			memcpy(dest.GetLinePtr(z),img.GetLinePtr(index,z+offset[1])+offset[0],dims[0]*sizeof(T));
		}

		break;
	case kipl::base::ImagePlaneYZ :

		if (img.Size(0)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneYZ.", __FILE__, __LINE__);

        if (roi.empty()) {
            dims = {img.Size(1), img.Size(2)};
			offset[0]=0;
			offset[1]=0;

		}
		else {
            if (tmproi[2]<tmproi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
                swap(tmproi[2],tmproi[0]);
			}
            dims[0]=tmproi[2]-tmproi[0]+1;

            if (tmproi[3]<tmproi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
                swap(tmproi[3],tmproi[1]);
			}
            dims[1]=tmproi[3]-tmproi[1]+1;
            offset[0]=tmproi[0];
            offset[1]=tmproi[1];
		}
		
        dest.resize(dims);
		
		for (size_t z=0; z<dims[1]; z++) {
			T * pDest=dest.GetLinePtr(z);
            auto pData=img.GetDataPtr()+index+(z+offset[1])*sxy+offset[0]*sx;
			for (size_t y=0; y<dims[0]; y++)
				pDest[y]=pData[y*sx];
		}

		break;
	default : 
		throw kipl::base::KiplException("Unknown image plane", __FILE__, __LINE__); 
		break;
	
	}
	
	dest.info=img.info;
	return dest;
}

template<typename T>
void InsertSlice(kipl::base::TImage<T,2> &slice, kipl::base::TImage<T,3> &volume, const size_t index, const kipl::base::eImagePlanes plane)
{
	switch (plane) {
	case kipl::base::ImagePlaneXY : 
		if (volume.Size(2)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXY.", __FILE__, __LINE__);
		if ((volume.Size(0)!=slice.Size(0)) || (volume.Size(1)!=slice.Size(1)))
			throw kipl::base::KiplException("Slice dimensions does not match the volume plane for PlaneXY.", __FILE__, __LINE__);
		memcpy(volume.GetLinePtr(0,index),slice.GetDataPtr(),sizeof(T)*slice.Size());
		break;
	case kipl::base::ImagePlaneXZ : 
		if (volume.Size(1)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXZ.", __FILE__, __LINE__);

		if ((volume.Size(0)!=slice.Size(0)) || (volume.Size(2)!=slice.Size(2)))
			throw kipl::base::KiplException("Slice dimensions does not match the volume plane for PlaneXZ.", __FILE__, __LINE__);
		
		throw kipl::base::KiplException("Extraction for PlaneXZ is not implemented",__FILE__,__LINE__);
		break;
	case kipl::base::ImagePlaneYZ : break;
		if (volume.Size(0)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneYZ.", __FILE__, __LINE__);

		if ((volume.Size(2)!=slice.Size(2)) || (volume.Size(1)!=slice.Size(1)))
			throw kipl::base::KiplException("Slice dimensions does not match the volume plane for PlaneYZ.", __FILE__, __LINE__);
		
		throw kipl::base::KiplException("Extraction for PlaneYZ is not implemented",__FILE__,__LINE__);
	default : 
		throw kipl::base::KiplException("Unknown image plane", __FILE__, __LINE__); 
		break;
	
	}
}

template<typename T>
kipl::base::TImage<float,2> getPatch(kipl::base::TImage<T,2> srcImg, size_t x,size_t y,size_t h,size_t w, size_t margin)
{
    std::vector<size_t> roi={x,y,x+w+margin,y+h+margin};

    if (x<margin)
        roi[0]=0UL;

    if (y<margin)
        roi[1]=0UL;

    if (srcImg.Size(0)<roi[2])
        roi[2]=srcImg.Size(0);

    if (srcImg.Size(1)<roi[3])
        roi[3]=srcImg.Size(1);

    kipl::base::TImage<T,2> resImg({roi[2]-roi[0],roi[3]-roi[1]});
    for (size_t i=0; i<roi[3]-roi[1]; ++i)
    {
        std::copy_n(srcImg.GetLinePtr(i+roi[1],resImg.Size(0),resImg.GetLinePtr(i)));
    }

    return resImg;
}

template<typename T>
void putPatch(kipl::base::TImage<T,2> patch, size_t x,size_t y, size_t margin,  kipl::base::TImage<T,2> dstImg)
{
    size_t margX = margin;
    size_t margY = margin;
    size_t w = patch.Size(0);
    size_t h = patch.Size(1);

    if (x<margin)
        margX=x;

    if (y<margin)
        margY=y;


    if (dstImg.Size(0)<x+patch.Size(0)-margin)
        w=(x+patch.Size(0)+margin) % (x+patch.Size(0));

    if (dstImg.Size(1)<y+patch.Size(1)-margin)
        h=dstImg.Size(1);

    for (size_t i=0; i<h; ++i)
    {
        std::copy_n(patch.GetLinePtr(i+margY)+margX,w,dstImg.GetLinePtr(i+y));
    }

    //return dstImg;
}

}}
#endif /*TEXTRACTOR_HPP_*/

