//<LICENCE>

#ifndef _TPERMUTEIMAGE_HPP_
#define _TPERMUTEIMAGE_HPP_

#include "../timage.h"
#include "../kiplenums.h"
#include "../KiplException.h"
namespace kipl { namespace base {
 
template <typename T, size_t N>
TImage<T,N> Mirror(TImage<T,N> img, eImageAxes axis)
{
	
	if (N!=2)
		throw KiplException("Mirror is only supported for 2D images", __FILE__, __LINE__);
	
	const size_t SizeX=img.Size(0);
	const size_t SizeY=img.Size(1);
//	const size_t SizeZ=N<3 ? 1 : img.Size(2);
	
	TImage<T,N> result(img.Dims());
	
	if (axis==kipl::base::ImageAxisX) {
		const size_t SizeXm1=SizeX-1; 
		for (size_t y=0; y<SizeY; y++) {
			T *pImg=img.GetLinePtr(y);
			T *pRes=result.GetLinePtr(y);
			
			for (size_t x=0; x<SizeX; x++) {
				pRes[SizeXm1-x]=pImg[x];				
			} 
		}	
	}
	
	if (axis==kipl::base::ImageAxisY) {
		const size_t LastX=SizeX-1;
		for (size_t y=0; y<SizeY; y++) {
			T * pSrcLine=img.GetLinePtr(y);
			T * pDestLine=result.GetLinePtr(y);
			
			for (size_t x=0; x<SizeX; x++)
				pDestLine[LastX-x]=pSrcLine[x];	
		}
	}
	
	if (axis==kipl::base::ImageAxisZ) {
		throw KiplException("Mirror does not support ImageAxisZ", __FILE__, __LINE__);		
	}

	return result;
}

//-------------------------------------------------------------------

template <typename T>
kipl::base::TImage<T,3> PermuteAxes<T>::operator() (kipl::base::TImage<T,3> src, const kipl::base::eAxisPermutations permutation)
{
	size_t dims[3]={0,0,0};
	
	const size_t Nx=src.Size(0);
	const size_t Ny=src.Size(1);
	const size_t Nz=src.Size(2);
	size_t Cx=0;
	size_t Cy=0;
	size_t Cz=0;
	switch (permutation) {
		case PermuteXYZ : break;
		case PermuteXZY : 
			dims[0] = Nx; 
			dims[1] = Nz;    
			dims[2] = Ny;
			Cx = 1;  
			Cy = dims[0]*dims[1]; 
			Cz = dims[0];
			break;
		case PermuteYXZ : 
			dims[0] = Ny; 
			dims[1] = Nx; 
			dims[2] = Nz;
			Cx = dims[0]; 
			Cy = 1;  
			Cz = dims[0]*dims[1];
			break;
		case PermuteYZX : 
			dims[0] = Ny; 
			dims[1] = Nz; 
			dims[2] = Nx;
			Cx = dims[0]*dims[1]; 
			Cy = 1;  
			Cz = dims[0];
			break;
		case PermuteZXY : 
			dims[0] = Nz; 
			dims[1] = Nx; 
			dims[2] = Ny;
			Cx = dims[0]; 
			Cy = dims[0]*dims[1];  
			Cz = 1;
			break;

		case PermuteZYX : 
			dims[0] = Ny; 
			dims[1] = Nz; 
			dims[2] = Nx;
			Cx = dims[0]*dims[1]; 
			Cy = dims[0];  
			Cz = 1;
			break;
	}

//	std::cout<<"Creating destination for permute"<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<"\n";
	kipl::base::TImage<T,3> dest(dims);

	T *pSrc=src.GetDataPtr();
	T *pDest=dest.GetDataPtr();
	size_t index=0;
	for (size_t z=0, i=0; z<Nz; z++) {
		for (size_t y=0; y<Ny; y++) {
			index=y*Cy+z*Cz;
			for (size_t x=0; x<Nx; x++,i++) {
				pDest[x*Cx+index]=pSrc[i];
			}
		}
	}

	return dest;
}

}}
#endif
