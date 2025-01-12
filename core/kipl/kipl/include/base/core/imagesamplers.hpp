//<LICENCE>

#ifndef IMAGESAMPLERS_HPP
#define IMAGESAMPLERS_HPP

#include <vector>
#include "../KiplException.h"
#include "../timage.h"

namespace kipl { namespace base {

template <class ImgType, size_t NDim>
int ReBin(const kipl::base::TImage<ImgType,NDim> &img, 
		kipl::base::TImage<ImgType,NDim> &result, 
		size_t const * const nbin) {

    auto imgDim=img.dims();
	int bins[8]={1,1,1,1,1,1,1,1};
    std::vector<size_t> dims(NDim,1);

	double scale=1.0;
    for (size_t i=0; i<NDim; i++)
    {
		bins[i]=nbin[i];
		dims[i]=imgDim[i]/nbin[i];
		if (dims[i]==0)
			throw kipl::base::KiplException("Binning result in zero-size image");
		scale*=nbin[i];
	}
	
	kipl::base::TImage<double,NDim> ws(dims);
	ws=0.0;

	ImgType *pRes=NULL;
	double *pWs=NULL;

    for (size_t z=0 ; z<dims[2]; z++)
    {
        for (int zi=0; zi<bins[2]; zi++)
        {
            for (size_t y=0; y<dims[1]; y++)
            {
				pWs=ws.GetLinePtr(y,z);
                for (int yi=0; yi<bins[1]; yi++)
                {
					ImgType const * pImg=img.GetLinePtr(y*bins[1]+yi,z*bins[2]+zi);
                    for (size_t x=0; x<dims[0]; x++)
                    {
						pWs[x]=0.0;
						size_t bx=x*bins[0];
                        for (size_t xi=0; xi<static_cast<size_t>(bins[0]); xi++)
                        {
							pWs[x]+=pImg[bx++];
						}
					}
				}
			}
		}
	}
	
    try
    {
        result.resize(dims);
		result=static_cast<ImgType>(0);
	}
    catch (kipl::base::KiplException & E)
    {
		throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
	}

	pRes=result.GetDataPtr();
	pWs=ws.GetDataPtr();

	scale=1.0f/scale;
    for (size_t i=0; i<result.Size(); i++)
    {
		pRes[i]=static_cast<ImgType>(scale*pWs[i]);
	}

	return 1;
}

template <class ImgType, int NDim>
int GaussianSampler(	const kipl::base::TImage<ImgType,NDim> &img, 
						kipl::base::TImage<ImgType,NDim> &result, 
						int scale, 
						double sigma) 
{
    std::ignore = sigma;
	const unsigned int *imgDim=img.getDimsptr();
	unsigned int dims[3];
	int i;
		
	for (i=0; i<NDim; i++) {
		dims[i]=imgDim[i]/scale;
	}
	
	try {
		result.Resize(dims);
	}
    catch (kipl::base::KiplException & E) {
		throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
	}
	
    int x,y,z, nx,ny,nz;
	float *pImg;
	ImgType *pRes=result.GetDataPtr();
	kipl::base::TImage<float,NDim> filteredImage;
	filteredImage=img.toFloat();
	throw kipl::base::KiplException("The Gaussian filter is not activated in the down sampler", __FILE__, __LINE__);
//	Filter::CGaussianFilter<float,NDim> G(sigma);
//	G(filteredImage);
	
    for (z=0; z<static_cast<int>(imgDim[2]); z+=scale)
    {
		nz=z/scale; 
        if (nz>=static_cast<int>(dims[2]))
            nz--;
        for (y=0; y<static_cast<int>(imgDim[1]); y+=scale)
        {
			ny=y/scale; 
            if (ny>=static_cast<int>(dims[1]))
                ny--;
			
			pImg=filteredImage.getLineptr(y,z);
			
			if (NDim==3) 
				pRes=result.getLineptr(ny,nz);
			else 
				pRes=result.getLineptr(ny);
				
            for (x=0; x<static_cast<int>(imgDim[0]); x+=scale)
            {
				nx=x/scale; 
                if (nx>=static_cast<int>(dims[0]))
                    nx--;

				pRes[nx]=ImgType(pImg[x]);
			}
		}
	}
	result.touch();

	return 1;
}
/*
template <class ImgType, int NDim>
int ReBlock(CImage<ImgType,NDim> &img, CImage<ImgType,NDim> &result, int *nblock) {
	const unsigned int *imgDim=img.getDimsptr();
	unsigned int dims[3];
	int i, img_x;
	for (i=0; i<NDim; i++) dims[i]=imgDim[i]*nblock[i];
	
	try {
		result.resize(dims);
	}
	catch (const std::bad_alloc & E) {
		cerr<<"Allocation error in ReBlock(CImage<>,CImage<>,int *))"<<endl;
		throw E;
	}
	int x,y,z;
	const int sxy=dims[0]*dims[1];
	
	ImgType *pImg=img.getDataptr();
	ImgType *pRes, *pFirstRes;
	for (z=0,img_x=0; z<dims[2]; z+=nblock[2]) {
		for (y=0; y<dims[1]; y+=nblock[1]) {
			pFirstRes=result.getLineptr(y,z); // First line of a block
			pRes=pFirstRes;
			for (x=0; x<dims[0]; x+=nblock[0], img_x++) 
				for (i=0; i<nblock[0]; i++) pRes[x+i]=pImg[img_x];
			for (i=1; i<nblock[1]; i++) { // Copy the line to expand in z direction
				pRes=result.getLineptr(y+i,z);
				memcpy(pRes,pFirstRes,dims[0]*sizeof(ImgType));
			}
		}
		pFirstRes=result.getLineptr(0,z);
		for (i=1; i<nblock[2]; i++) {
			pRes=result.getLineptr(0,z+i);
			memcpy(pRes,pFirstRes,sxy*sizeof(ImgType));
		}
	}
	
	result.touch();

	return 1;
}

template <class ImgType, int NDim>
int ReverseAxis(CImage<ImgType,NDim> &img, CImage<ImgType,NDim> &result, AxisType axis) 
{
	int i;
	const int *dims=img.getDimsptr();
	result.resize(dims);
	CImage<ImgType,2> tmpimg;
	
	switch (axis) {
	case axis_X :
		for (i=0; i<dims[0]; i++) {
			 img.ExtractSlice(tmpimg, plane_YZ, i);
			 result.InsertSlice(tmpimg, plane_YZ, dims[0]-1-i);
		}
		break;
	case axis_Y :
		for (i=0; i<dims[1]; i++) {
			 img.ExtractSlice(tmpimg, plane_XZ, i);
			 result.InsertSlice(tmpimg, plane_XZ, dims[1]-1-i);
		}
		break;
	case axis_Z :
		for (i=0; i<dims[2]; i++) {
			 img.ExtractSlice(tmpimg, plane_XY, i);
			 result.InsertSlice(tmpimg, plane_XY, dims[2]-1-i);
		}
		break;
	}
}

template <class ImgType, int NDim>
int PermuteAxis(CImage<ImgType,NDim> &img, CImage<ImgType,NDim> &result,int *perm)
{
	int i;
	const int *dims=img.getDimsptr();
	int newdims[8]={0,0,0,0,0,0,0,0};
	
	// Tests
	for (i=0; i<NDim; i++) {
		if (perm[i]>=NDim) {
			cerr<<"PermuteAxis: wrong axis index in permute array"<<endl;
			return -1;
		}
		if (newdims[perm[i]]) {
			cerr<<"PermuteAxis: Crowded axis"<<endl;
			return -2;
		}
		newdims[perm[i]]=dims[i];
	}

	result.resize(newdims);
	
	ImgType *pImg=img.getDataptr();
	ImgType *pRes=result.getDataptr();
	int pos[3];
	
	for (pos[2]=0; pos[2]<dims[2]; pos[2]++)
		for (pos[1]=0; pos[1]<dims[1]; pos[1]++)
			for (pos[0]=0; pos[0]<dims[0]; pos[0]++)
				result(pos[perm[0]],pos[perm[1]],pos[perm[2]])=img(pos[0],pos[1],pos[2]);
	return 0;	
}
*/
}}

#endif
