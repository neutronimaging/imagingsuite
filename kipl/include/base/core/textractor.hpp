//<LICENCE>

#ifndef TEXTRACTOR_HPP_
#define TEXTRACTOR_HPP_

#include <iostream>

#include "../../logging/logger.h"
#include "../timage.h"
#include "../kiplenums.h"
#include "../KiplException.h"


namespace kipl { namespace base {

template<typename T>
kipl::base::TImage<T,2> ExtractSlice(kipl::base::TImage<T,3> &img, const size_t index, const kipl::base::eImagePlanes plane,size_t *roi)
{
	kipl::logging::Logger logger("ExtractSlice");
	size_t dims[2]={0,0};
	size_t offset[2]={0,0};
	kipl::base::TImage<T,2> dest;
	size_t sx=img.Size(0);
	size_t sxy=img.Size(0)*img.Size(1);

	switch (plane) {
	case kipl::base::ImagePlaneXY :
		if (img.Size(2)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXY.", __FILE__, __LINE__);
		if (roi==NULL) {
			dims[0]=img.Size(0); dims[1]=img.Size(1);
			offset[0]=0;
			offset[1]=0;
		}
		else {
			if (roi[2]<roi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
				swap(roi[2],roi[0]);
			}
			dims[0]=roi[2]-roi[0]+1;

			if (roi[3]<roi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
				swap(roi[3],roi[1]);
			}
			dims[1]=roi[3]-roi[1]+1;

			offset[0]=roi[0];
			offset[1]=roi[1];
		}

		dest.Resize(dims);
		for (size_t i=0; i<dims[1]; i++)
			memcpy(dest.GetLinePtr(i),img.GetLinePtr(i+offset[1],index)+offset[0],dest.Size(0)*sizeof(T));

		break;
	case kipl::base::ImagePlaneXZ : 
		if (img.Size(1)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneXZ.", __FILE__, __LINE__);

		if (roi==NULL) {
			dims[0]=img.Size(0); dims[1]=img.Size(2);
			offset[0]=0;
			offset[1]=0;
		}
		else {
			if (roi[2]<roi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
				swap(roi[2],roi[0]);
			}
			dims[0]=roi[2]-roi[0]+1;

			if (roi[3]<roi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
				swap(roi[3],roi[1]);
			}
			dims[1]=roi[3]-roi[1]+1;
			offset[0]=roi[0];
			offset[1]=roi[1];
		}
		
		dest.Resize(dims);
		
		for (size_t z=0; z<dims[1]; z++) {
			memcpy(dest.GetLinePtr(z),img.GetLinePtr(index,z+offset[1])+offset[0],dims[0]*sizeof(T));
		}

		break;
	case kipl::base::ImagePlaneYZ :

		if (img.Size(0)<=index)
			throw kipl::base::KiplException("Index exceeds image dimension for PlaneYZ.", __FILE__, __LINE__);

		if (roi==NULL) {
			dims[0]=img.Size(1); dims[1]=img.Size(2);
			offset[0]=0;
			offset[1]=0;

		}
		else {
			if (roi[2]<roi[0]) {
				logger(kipl::logging::Logger::LogWarning,"roi2<roi0, swapped");
				swap(roi[2],roi[0]);
			}
			dims[0]=roi[2]-roi[0]+1;

			if (roi[3]<roi[1]) {
				logger(kipl::logging::Logger::LogWarning,"roi3<roi1, swapped");
				swap(roi[3],roi[1]);
			}
			dims[1]=roi[3]-roi[1]+1;
			offset[0]=roi[0];
			offset[1]=roi[1];
		}
		
		dest.Resize(dims);
		
		for (size_t z=0; z<dims[1]; z++) {
			T * pDest=dest.GetLinePtr(z);
			T * pData=img.GetDataPtr()+index+(z+offset[1])*sxy+offset[0]*sx;
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

}}
#endif /*TEXTRACTOR_HPP_*/
