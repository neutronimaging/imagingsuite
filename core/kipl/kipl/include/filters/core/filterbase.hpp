//<LICENCE>

#ifndef __FILTERBASE_HPP_
#define __FILTERBASE_HPP_
#include <limits>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../base/KiplException.h"
#include "../filterbase.h"

namespace kipl { namespace filters {

template<typename T, size_t nDims>
TFilterBase<T,nDims>::TFilterBase(const std::vector<T> & kernel, const std::vector<size_t> & kDims)
{
	if ((nDims<1) || (3<nDims))
		return;
		
	//	throw kipl::base::KiplException("TFilterBase was initialized with an unsupported number of dimension",__FILE__,__LINE__);
    nKernel = std::accumulate(kDims.begin(),kDims.end(),1,std::multiplies<size_t>());

	if (nKernel==0)
		throw kipl::base::KiplException("Zero sized kernel",__FILE__,__LINE__);

    pKernel = kernel;
    nKernelIndex.resize(nKernel);
    nKernelDims = kDims;

}

template<typename T, size_t nDims>
TFilterBase<T,nDims>::TFilterBase(const std::vector<size_t> & kDims)
{
	if ((nDims<1) || (3<nDims))
		return;
		
	//	throw kipl::base::KiplException("TFilterBase was initialized with an unsupported number of dimension",__FILE__,__LINE__);
    nKernel = std::accumulate(kDims.begin(),kDims.end(),1,std::multiplies<size_t>());

	if (nKernel==0)
		throw kipl::base::KiplException("Zero sized kernel",__FILE__,__LINE__);

    nKernelIndex.resize(nKernel);
    pKernel.resize(nKernel);
    nKernelDims = kDims;
}

template<typename T, size_t nDims>
TFilterBase<T,nDims>::~TFilterBase()
{
}

template<typename T, size_t nDims>
kipl::base::TImage<T,nDims> TFilterBase<T,nDims>::operator() (kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle epStyle)
{

    kipl::base::TImage<T,nDims> dest(src.dims());

	InitResultArray(src,dest);
    ProcessCore(src.GetDataPtr(), src.dims(), dest.GetDataPtr(), dest.dims());
    ProcessEdge(src.GetDataPtr(), dest.GetDataPtr(), src.dims(), epStyle);

	return dest;
}

template<typename T, size_t nDims>
int TFilterBase<T,nDims>::ProcessCore(T const * const img, 
    const std::vector<size_t> & imgDims,
	T *res,   
    const std::vector<size_t> & resDims)
{
	size_t nCenter=nKernelDims[0]/2;
	size_t nDimsProd=1;
	for (size_t i=1; i<nDims; i++)  {
		nDimsProd *= imgDims[i-1];
        nCenter+=nDimsProd*(nKernelDims[i]/2);
	}

	const size_t cnLineLength = imgDims[0]-nKernelDims[0]+1;

    PrepareIndex(imgDims, nKernelDims);

	T *pRes=res+nCenter;
	
	size_t nLines     = 0;
	size_t nSlices    = 0;
	size_t nSliceStep = 0;
	
	switch (nDims) {
	case 1 : 
		nLines     = 1; 
		nSlices    = 1; 
		nSliceStep = 0; 
		break;
	case 2 : 
		nLines     = imgDims[1] - nKernelDims[1] + 1 ; 
		nSlices    = 1 ;
		nSliceStep = 0 ; 
		break;
	case 3 : 
		nLines     = imgDims[1] - nKernelDims[1] + 1; 
		nSlices    = imgDims[2] - nKernelDims[2] ; 
		nSliceStep = nKernelDims[2];
		break;
	default : return -1;
	}

	size_t nImgLineIndex=0;
	size_t nResLineIndex=0;
	
	{
		for (size_t k=0 ; k<nSlices; k++) {
			for (size_t j=0 ; j< nLines ; j++) {
				for (size_t i=0; i<nKernel; i++) {
					T value=pKernel[i];
					if (value!=0) { 
						T const * const pImg=img+nKernelIndex[i]+nCenter;

						InnerLoop(pImg+nImgLineIndex,pRes+nResLineIndex, value,cnLineLength);
					}
				}
				nImgLineIndex+=imgDims[0];
				nResLineIndex+=resDims[0];
			}
			nImgLineIndex+=nSliceStep*imgDims[0];
			nResLineIndex+=nSliceStep*resDims[0];
		}
	}
    return static_cast<int>(nLines);
}

template<typename T, size_t nDims>
int TFilterBase<T,nDims>::ProcessEdge(T const * const img, 
	  T *res, 
      const std::vector<size_t> &  imgDims,
	  const FilterBase::EdgeProcessingStyle epStyle)
{
	switch (nDims) {
		//case 1: ProcessEdge1D(img,res,imgDims,epStyle); break;
		case 1: kipl::base::KiplException("Edges are not supported by 1D filter",__FILE__,__LINE__); break;
		case 2: ProcessEdge2D(img,res,imgDims,epStyle); break;
		case 3: ProcessEdge3D(img,res,imgDims,epStyle); break;
		//default: ProcessEdge1D(img,res,imgDims,epStyle); break;
		default: kipl::base::KiplException("Edge type not supported",__FILE__,__LINE__); break;
	}
	return 0;
}

template<typename T, size_t nDims>
int TFilterBase<T,nDims>::ProcessEdge1D(T const * const img, 
		  T * res, 
          const std::vector<size_t> &  imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle)
{
    const size_t cnCenter=nKernelDims[0]/2;
	if (epStyle==EdgeValid) {
		memset(res,0,cnCenter*sizeof(T));		
		memset(res+imgDims[0]-cnCenter,0,sizeof(T)*cnCenter);
		
		return 0;
	}
		
	const size_t cNEdgeBufferLen=nKernelDims[0]*2;
	if (cNEdgeBufferLen==0)
		return -1;
	
	T *edge=new T[cNEdgeBufferLen];

	// Left edge processing
	memset(edge,0,sizeof(T)*cNEdgeBufferLen);
	const size_t cNLeftEdge=nKernelDims[0]+cnCenter;
	
	memcpy(edge+cnCenter,img,(nKernelDims[0])*sizeof(T));
	float fEdgeValue=0.0f;
	switch (epStyle) {
		case EdgeZero : 
			memset(edge,0,cnCenter*sizeof(T)) ; 
			break;
		case EdgeSame : 
			fEdgeValue=edge[cnCenter];
			for (size_t i=0; i<cnCenter; i++) {
				edge[i]=fEdgeValue;
			}
			break;
		case EdgeMirror :
			for (int i=cnCenter-1, j=cnCenter+1; i>=0; i--, j++) {
				edge[i]=edge[j];
			}
			break;
		default:
			// This is a safety feature if casting from unsupported numeric value was done.
			throw kipl::base::KiplException("Unknown edge processing mode in ProcessEdge1D().",__FILE__,__LINE__);
	}
	size_t edgeDims[]={cNLeftEdge};
	
	ProcessCore(edge, edgeDims, res-cnCenter, imgDims);
	memset(edge,0,sizeof(T)*cNEdgeBufferLen);
	const size_t cNRightEdge=nKernelDims[0]+cnCenter;
	edgeDims[0]=cNRightEdge;

	size_t len=imgDims[0]-(cnCenter+1);
	memcpy(edge,img+len,(cnCenter+1)*sizeof(T));

	switch (epStyle) {
		case EdgeZero : 
			memset(edge+cnCenter+1,0,(nKernelDims[0]-cnCenter-1)*sizeof(T)) ;  
			break;
		case EdgeSame : 
			fEdgeValue=img[imgDims[0]-1];
			for (size_t i=cnCenter+1; i<nKernelDims[0]; i++) 
			{
				edge[i]=fEdgeValue;
			}
			break;
		case EdgeMirror :
			for (size_t i=imgDims[0]-nKernelDims[0]+cnCenter, j=cnCenter+1; 
				j<edgeDims[0]; 
				i--, j++) 	{	
				edge[j]=img[i];
			}
			break;
		default: 
			throw kipl::base::KiplException("Unknown edge processing mode in ProcessEdge1D().",__FILE__,__LINE__);
	}

	ProcessCore(edge,edgeDims, res+len, imgDims);

	if (edge!=NULL)
		delete [] edge;

	return 0;
}

template<typename T, size_t nDims>
int TFilterBase<T,nDims>::ProcessEdge2D(T const * const img, 
		  T * res, 
          const std::vector<size_t> &  imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle)
{
    const size_t cnCenter[2]={nKernelDims[0]/2, nKernelDims[1]/2};
	
	size_t nLen = imgDims[0]*(nKernelDims[1]-cnCenter[1]-1);
	const ptrdiff_t nLastLine=imgDims[1]-(nKernelDims[1]-cnCenter[1]-2);
	
	//ptrdiff_t i;
	if (epStyle == EdgeValid) {
		memset(res, 0, (imgDims[0]*cnCenter[1]+cnCenter[0])*sizeof(T));
		nLen=imgDims[0]-nKernelDims[0]+1;
		ptrdiff_t i=0;
		for (i=cnCenter[1]; i< nLastLine-1; ++i) 
		{
			memset(res+i*imgDims[0]+nLen+cnCenter[0],0,sizeof(T)*(nKernelDims[0]-1));
		}
		
		memset(res+i*imgDims[0],0,sizeof(T)*(nKernelDims[1]-cnCenter[1]-1)*imgDims[0]);		
	}
	else {
		
		kipl::base::TImage<T,2> edge;
        ptrdiff_t nLeftPixels  = nKernelDims[0]/2;
        size_t nRightPixels    = (nKernelDims[0]/2) - (1-nKernelDims[0]%2);
        ptrdiff_t nFrontLines  = nKernelDims[1]/2;
        ptrdiff_t nBackLines   = (nKernelDims[1]/2) - (1-nKernelDims[1]%2);
		
        std::vector<size_t> nEdgeDims={0,0};
		T *pEdge=NULL;
		
//		size_t nCenter=0;
		ptrdiff_t res_offset=0;

		switch (epStyle) {
			case EdgeZero   :		// Todo: TFilterBase, Pad 2D edge by zeros
				// Processing front pixels
				nEdgeDims[0] = imgDims[0]+nKernelDims[0]-1;
				nEdgeDims[1] = nFrontLines+nKernelDims[1]-1;
				
				if (0<nEdgeDims[0]*nEdgeDims[1]) 
				{
                    edge.resize(nEdgeDims);
					edge=static_cast<T>(0);
					
					for (ptrdiff_t i=0; i<(nFrontLines+nBackLines); i++) 
						memcpy(edge.GetLinePtr(nFrontLines+i)+nLeftPixels,img+imgDims[0]*i,sizeof(T)*imgDims[0]);
					
					res_offset=nFrontLines*nEdgeDims[0]+nLeftPixels;
					ProcessCore(edge.GetDataPtr(),nEdgeDims,res-res_offset,imgDims);
				}
				// Processing back pixels
				nEdgeDims[0] = imgDims[0]+nKernelDims[0]-1;
				nEdgeDims[1] = nBackLines+nKernelDims[1]-1;
				
				if (0<nEdgeDims[0]*nEdgeDims[1]) {
                    edge.resize(nEdgeDims);
					edge=static_cast<T>(0);
					for (ptrdiff_t i=0; i<(nFrontLines+nBackLines); i++) 
						memcpy(edge.GetLinePtr(i)+nLeftPixels,img+imgDims[0]*(i+imgDims[1]-nKernelDims[1]+1),sizeof(T)*imgDims[0]);
					res_offset=nFrontLines*nEdgeDims[0]+nLeftPixels;
					ProcessCore(edge.GetDataPtr(),nEdgeDims,res-res_offset+(imgDims[1]-nBackLines)*imgDims[0],imgDims);
				}
				// Processing back pixels
				nEdgeDims[0] = nLeftPixels+nKernelDims[0]-1;
				nEdgeDims[1] = imgDims[1];
				
				if (0<nEdgeDims[0]*nEdgeDims[1]) {
                    edge.resize(nEdgeDims);
					edge=static_cast<T>(0);
					for (ptrdiff_t i=0; i<static_cast<ptrdiff_t>(imgDims[1]-nKernelDims[1]+1); i++) 
						memcpy(edge.GetLinePtr(nFrontLines+i)+nLeftPixels,img+imgDims[0]*i,sizeof(T)*(nKernelDims[0]+nRightPixels));
					
					res_offset=-static_cast<ptrdiff_t>(nKernelDims[0]);//nFrontLines*nEdgeDims[0];
					ProcessCore(edge.GetDataPtr(),nEdgeDims,res+res_offset,imgDims);
				}
				
				break;
				
			case EdgeSame   :		// Todo: TFilterBase, Pad 2D edge with the same value as the edge

				throw kipl::base::KiplException("Same edge processing mode is not implemented in ProcessEdge2D().",__FILE__,__LINE__);
				break;
			case EdgeMirror :
				// Process top and bottom
				nEdgeDims[0]=imgDims[0]+nKernelDims[0]-1;
                nEdgeDims[1]=2*(nKernelDims[1]/2)+(nKernelDims[1]-(nKernelDims[1]/2)-1); // ???
                edge.resize(nEdgeDims);
				edge=static_cast<T>(0);
				// Process top edge
				pEdge=edge.GetDataPtr();
				for (size_t i=0; i<cnCenter[1]; ++i) 
				{
					memcpy(pEdge+nEdgeDims[0]*i+cnCenter[0],img+(cnCenter[1]-i)*imgDims[0],sizeof(T)*imgDims[0]);
				}
				for (size_t i=cnCenter[1]; i<(cnCenter[1]+nKernelDims[1]-1); ++i) 
				{
					memcpy(pEdge+nEdgeDims[0]*i+cnCenter[0],img+(i-cnCenter[1])*imgDims[0],sizeof(T)*imgDims[0]);
				}
				for (size_t i=0; i<(cnCenter[1]+nKernelDims[1]-1); ++i) 
				{
	//				T const * const pLine=img+i*imgDims[0];
					T val=pEdge[i*nEdgeDims[0]+cnCenter[0]];
					for (size_t j=0; j<cnCenter[0]; j++)
						pEdge[i*nEdgeDims[0]+j]=val;

					val=pEdge[i*nEdgeDims[0]+imgDims[0]+cnCenter[0]-1];
					for (size_t j=imgDims[0]+cnCenter[0]; j<nEdgeDims[0]; j++)
						pEdge[i*nEdgeDims[0]+j]=val;
				}
				
				res_offset=-static_cast<ptrdiff_t>(cnCenter[1]*nEdgeDims[0]+cnCenter[0]);//nFrontLines*nEdgeDims[0];
				ProcessCore(edge.GetDataPtr(),nEdgeDims,res+res_offset,imgDims);
				// Process bottom edge
				edge=static_cast<T>(0);
				for (size_t i=0; i<2*cnCenter[1]; i++) 
				{
					memcpy(pEdge+nEdgeDims[0]*i+cnCenter[0],img+(nLastLine-cnCenter[1]+i-1)*imgDims[0],sizeof(T)*imgDims[0]);
				}
				
				for (size_t i=2*cnCenter[1],j=imgDims[1]-2; i<nEdgeDims[1]; i++,j--) 
				{
				//	size_t idx=nEdgeDims[1]-2*cnCenter[1]-(i-2*cnCenter[1]);
					memcpy(pEdge+nEdgeDims[0]*(i)+cnCenter[0],img+(j)*imgDims[0],sizeof(T)*imgDims[0]);
				}

				for (size_t i=0; i<(cnCenter[1]+nKernelDims[1]-1); i++) {
					T val=pEdge[i*nEdgeDims[0]+cnCenter[0]];
					for (size_t j=0; j<cnCenter[0]; j++)
						pEdge[i*nEdgeDims[0]+j]=val;

					val=pEdge[i*nEdgeDims[0]+imgDims[0]+cnCenter[0]-1];
					for (size_t j=imgDims[0]+cnCenter[0]; j<nEdgeDims[0]; j++)
						pEdge[i*nEdgeDims[0]+j]=val;
				}

				res_offset=static_cast<ptrdiff_t>(imgDims[0]*imgDims[1] -(nKernelDims[1]-cnCenter[1]-1)*imgDims[0]-cnCenter[1]*nEdgeDims[0]-cnCenter[0]);//+nKernelDims[0]);
				ProcessCore(edge.GetDataPtr(),nEdgeDims,res+res_offset,imgDims);

				// Left-right processing
                nEdgeDims[0]=2*(nKernelDims[0]/2)+(nKernelDims[0]-(nKernelDims[0]/2)-1);
				nEdgeDims[1]=imgDims[1];
                edge.resize(nEdgeDims);
				// Left edge
				for (size_t y=0; y<imgDims[1]; y++) {
					pEdge=edge.GetLinePtr(y);
					T const * const pLine=img+y*imgDims[0];
					for (size_t x=0 ; x<cnCenter[0]; x++) {
						pEdge[x]=pLine[nKernelDims[0]-cnCenter[0]-1-x];
					}
					for (size_t x=cnCenter[0]; x<nEdgeDims[0]; x++)
						pEdge[x]=pLine[x-cnCenter[0]];
				}

				res_offset=imgDims[0]*cnCenter[1]-static_cast<ptrdiff_t>(cnCenter[1]*nEdgeDims[0]+cnCenter[0]);
				ProcessCore(edge.GetDataPtr(),nEdgeDims,res+res_offset,imgDims);

				// Right edge
				for (size_t y=0; y<imgDims[1]; y++) {
					pEdge=edge.GetLinePtr(y);
					T const * const pLine=img+y*imgDims[0];
					for (size_t x=0; x<2*cnCenter[0]; x++) {
						pEdge[x]=pLine[imgDims[0]-2*cnCenter[0]+x];
					}
					for (size_t x=2*cnCenter[0],j=imgDims[0]-2; x<nEdgeDims[0]; x++,j--) {
						pEdge[x]=pLine[j];
					}
				}

				res_offset=imgDims[0]*(cnCenter[1]+1)-nKernelDims[0]+1-static_cast<ptrdiff_t>(nEdgeDims[0]*cnCenter[1]);
				ProcessCore(edge.GetDataPtr(),nEdgeDims,res+res_offset,imgDims);
				break;
			default : 
				throw kipl::base::KiplException("Unknown edge processing mode in ProcessEdge2D().",__FILE__,__LINE__);
		}
	}
		
	return 0;
}

template<typename T, size_t nDims>
int TFilterBase<T,nDims>::ProcessEdge3D(T const * const img, 
		  T * res, 
          const std::vector<size_t> &  imgDims,
		  const FilterBase::EdgeProcessingStyle epStyle)
{
	size_t nSlice=imgDims[0]*imgDims[1];
	if (epStyle == EdgeValid) {
        size_t nTopSlices = nKernelDims[2]/2;
		memset(res, 0, nSlice*nTopSlices*sizeof(T));
		size_t nBottomSlices = nTopSlices - (1-nKernelDims[2]%2);
		size_t nLastSlice    = imgDims[2]-nBottomSlices; 
		memset(res+nSlice*nLastSlice, 0, nSlice*nBottomSlices*sizeof(T));
				
		for (size_t i=nTopSlices; i<nLastSlice; i++) {
			ProcessEdge2D(img+i*nSlice,res+i*nSlice,imgDims,epStyle);
		}
	}
	else {
		switch (epStyle) {
			case EdgeZero   :		// Todo: TFilterBase, Pad 3D edge by zeros
			case EdgeSame   :		// Todo: TFilterBase, Pad 3D edge with the same value as the edge
			case EdgeMirror :		// Todo: TFilterBase, Pad 3D edge with the mirrored edge
			default : 
				throw kipl::base::KiplException("Unknown edge processing mode in ProcessEdge3D().",__FILE__,__LINE__);
		}
	}
	return 0;
}

}}
#endif
