//<LICENCE>

#ifndef MEDIANFILTER_HPP_
#define MEDIANFILTER_HPP_

#include <iomanip>
#include <numeric>
#include <thread>
#include <vector>
#include <functional>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "../../base/timage.h"
#include "../../math/median.h"
#include "../../base/KiplException.h"
//#include "../medianfilter.h"

namespace kipl { namespace filters {

template <class T, size_t nDims>
TMedianFilter<T,nDims>::TMedianFilter(const std::vector<size_t> & dims) :
    kipl::filters::TFilterBase<T,nDims>(dims),
    logger("TMedianFilter"),
    medianAlgorithm(MedianAlg_HeapSortMedianSTL)
	// medianAlgorithm(MedianAlg_HeapSortMedian)
{
    if (nDims!=2)
        throw kipl::base::KiplException("Median filter is only supported for 2D", __FILE__, __LINE__);
    for (size_t i=0; i<nDims; i++)
        nHalfKernel[i]=dims[i]>>1;
}

/// Implements a median filter
template <class T, size_t nDims>
kipl::base::TImage<T,nDims> TMedianFilter<T,nDims>::operator() (const kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle edgeStyle)
{
    kipl::base::TImage<T,nDims> result(src.dims());
		
    switch (medianAlgorithm)
    {
//        case MedianAlg_BiLevel:           BilevelMedianFilter(src, result, edgeStyle);     break;
//        case MedianAlg_QuickMedian:       QuickMedianFilter(src, result, edgeStyle);       break;
        case MedianAlg_HeapSortMedian:    HeapSortMedianFilter(src, result, edgeStyle);    break;
        case MedianAlg_HeapSortMedianSTL: HeapSortMedianFilterSTL(src, result, edgeStyle); break;
//        case MedianAlg_STLSortMedian:     STLSortMedianFilter(src, result, edgeStyle);     break;
    default : HeapSortMedianFilter(src, result, edgeStyle);    break;
    }

	return result;
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::InitResultArray(const kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest) 
{
	std::ignore = src;
	std::ignore = dest;
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::InnerLoop(T const * const src, T *dest, T value, size_t N) 
{
	// Dummy
	std::ignore = src;
	std::ignore = dest;
	std::ignore = value;
	std::ignore = N;
} 


template <class T, size_t nDims>
int TMedianFilter<T,nDims>::ExtractNeighborhood(const kipl::base::TImage<T,nDims> &src, 
												const std::vector<size_t> & pos, 
												T * data, 
												const FilterBase::EdgeProcessingStyle /*edgeStyle*/)
{
	using namespace std;
	int sx   = src.Size(0);
	int sy   = src.Size(1);
	int posx = pos[0];
	int posy = pos[1];
	int k0   = this->nHalfKernel[0];
	int k1   = this->nHalfKernel[1];

	// int edgeinfo =   static_cast<int>( (posx<k0) 
	// 				+ ((sx-posx)<k0) * 2
	// 				+ (posy<k1)      * 4
	// 				+ ((sy-posy)<k1) * 8) ;

    


	int iPos0 = static_cast<int>(pos[0]);
	int iPos1 = static_cast<int>(pos[1]);
	int size0 = static_cast<int>(src.Size(0));
	int size1 = static_cast<int>(src.Size(1));
    int edgeinfo=static_cast<int> ((iPos0<nHalfKernel[0]) + (((size0-nHalfKernel[0]-1)<iPos0) * 2) +
            ((iPos1<nHalfKernel[1]) * 4)  + (((size1-nHalfKernel[1]-1)<iPos1) * 8)) ;


    // int startZ = 2 < nDims ? -nHalfKernel[2] : 0 ;
    // int endZ   = 2 < nDims ? this->nKernelDims[2]-nHalfKernel[2] : 1 ;
	int startY = 1 < nDims ? k1 : 1 ;
	int endY   = 1 < nDims ? this->nKernelDims[1]-k1 : 1 ;
	int startX = -k0;
	int endX   = this->nKernelDims[0]-k0;

	std::fill_n(data,this->nKernelDims[0]*this->nKernelDims[1],static_cast<T>(0));

	switch (edgeinfo) 
	{
		case 0: break;
		case 1: // left
			startX=-posx;
			break;
		case 2: // right
			endX=(sx-posx);
			break;
		case 3: // invalid
			throw kipl::base::KiplException("Invalid edge code (3) in median filter",__FILE__,__LINE__);
			break;
		case 4: // top
			startY=-posy;
			break;
		case 5: // top + left
			startX = -posx;
			startY = -posy;
			break;
		case 6: // top + right
			endX   =  sx-posx;
			startY = -posy;
			break;
		case 7: // invalid
			throw kipl::base::KiplException("Invalid edge code (7) in median filter",__FILE__,__LINE__);
			break;
		case 8: // bottom
			endY   = (sy-posy);
			break;
		case 9: // bottom + left
			startX = -posx;
			endY   = (sy-posy);
			break;
		case 10: // bottom + right
			endX   = (sx-posx);
			endY   = (sy-posy);
			break;
	}


	auto pImg=src.GetLinePtr(pos[1])+pos[0];
	int dpos = 0;
	for (int y = startY; y<endY; ++y) 
	{
		auto *d=pImg+y*src.Size(0);
		for (int x = startX; x<endX; ++x, ++dpos) 
		{
			data[dpos]=d[x];
		}
	}

	return dpos;
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::PrintMatrix(T *data,int nx, int ny)
{
	for (int y = 0, i=0; y<ny; y++) {
		cout.precision(2);
		for (int x = 0; x<nx; x++, i++) {
			cout<<setw(8)<<data[i]<<" ";
		}
		cout<<endl;
	}

}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::HeapSortMedianFilter(const kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle)
{
	using namespace std;
	const int startY = 0 ;
	const int endY   = 1 < nDims ? src.Size(1) : 1 ;
	const int startX = 0 ;
	const int endX   = src.Size(0);

	#pragma omp parallel
	{
		int N=0;
		T *kern=new T[this->nKernel];
		std::vector<size_t> pos={0,0};
		#pragma omp for
		for (int y=startY; y<endY; y++) {
			pos[1]=y;
			T * pLine=result.GetLinePtr(y);
			for (int x=startX; x<endX; x++) {
				pos[0]=x;
				N=ExtractNeighborhood(src,pos,kern,edgeStyle);
				kipl::math::median(kern,N, pLine+x);
				// pLine[x]=N;
			}
		}
		delete [] kern;
	}
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::HeapSortMedianFilterSTL(const kipl::base::TImage<T,nDims> &src,
                kipl::base::TImage<T,nDims> &result,
                const FilterBase::EdgeProcessingStyle edgeStyle)
{
	const int startY = 0 ;
    const int endY   = 1 < nDims ? src.Size(1) : 1 ;

    std::ostringstream msg;
    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();


    std::vector<std::thread> threads;
    const size_t nLines = endY-startY;

    size_t M=nLines/concurentThreadsSupported;

    size_t begin = 0;
    size_t end   = 0;
    ptrdiff_t rest  = nLines % concurentThreadsSupported ; // Take care of the rest slices

    for(size_t i = 0; i < concurentThreadsSupported; ++i,--rest)
    {
        if ( 0 < rest )
        {
            end = begin+M+1;
        }
        else
            end = begin + M;

        // spawn threads

        auto pSrc = &src;
        auto pRes = &result;

        threads.push_back(std::thread([=] { HeapSortInnerLoop(pSrc,
                                                              pRes,
                                                              begin,
                                                              end,
                                                              edgeStyle); }));
        begin = end;
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::HeapSortInnerLoop(const kipl::base::TImage<T,nDims> *src,
                kipl::base::TImage<T,nDims> *result,
                size_t begin,
                size_t end,
                const FilterBase::EdgeProcessingStyle edgeStyle)
{
    using namespace std;
    const int startX = 0 ;
    const int endX   = src->Size(0);

    T *kern=new T[this->nKernel];
    std::vector<size_t> pos={0,0};
	int N=0;

    for (size_t y=begin; y<end; ++y)
    {
        pos[1]=y;
        T * pLine=result->GetLinePtr(y);
        for (int x=startX; x<endX; ++x)
        {
            pos[0]=x;
            N=ExtractNeighborhood(*src,pos,kern,edgeStyle);
            kipl::math::median(kern,N, pLine+x);
			// pLine[x]=N;
        }
    }

    delete [] kern;
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::STLSortMedianFilter(const kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle)
{
	const int startY = 1 < nDims ? nHalfKernel[1] : 0 ;
	const int endY   = 1 < nDims ? src.Size(1) - (this->nKernelDims[1]-nHalfKernel[1]): 1 ;
	const int startX = nHalfKernel[0] ;
	const int endX   = src.Size(0) - (this->nKernelDims[0]-nHalfKernel[0]) ;
	
	std::vector<size_t> pos={0,0};
	for (int y=startY; y<endY; y++) {
		pos[1]=y-nHalfKernel[1];
		T * pLine=result.GetLinePtr(y);
		for (int x=startX; x<endX; x++) {
			pos[0]=x-nHalfKernel[0];
			ExtractNeighborhood(src,pos,this->pKernel,edgeStyle);
			kipl::math::median_STL(this->pKernel,this->nKernel, pLine+x);
		}
	}
	
	// Filling lazy edge
	for (int y=0; y<startY; y++) 
		memcpy(result.GetLinePtr(y),result.GetLinePtr(startY),result.Size(0)*sizeof(T));

	for (int y=endY; y<result.Size(1); y++) {
		memcpy(result.GetLinePtr(y),result.GetLinePtr(endY-1),result.Size(0)*sizeof(T));
	}

	for (int y=0; y<result.Size(1); y++) {
		T *pLine=result.GetLinePtr(y);
		T val=pLine[startX];
		for (int x=0; x<startX; x++)
			pLine[x]=val;
		val=pLine[endX-1];
		for (size_t x=endX; x<result.Size(0); x++)
			pLine[x]=val;
	}

}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::QuickMedianFilter(const kipl::base::TImage<T,nDims> &src, 
		kipl::base::TImage<T,nDims> &result, 
		const FilterBase::EdgeProcessingStyle edgeStyle)
{
	using namespace std;
	const int startY = 0 ;
	const int endY   = 1 < nDims ? src.Size(1) : 1 ;
	const int startX = 0 ;
	const int endX   = src.Size(0);

	std::vector<size_t> pos={0,0,0};
	int sum=0;

	for (int y=startY; y<endY; y++) {
		pos[1]=y;
		T * pLine=result.GetLinePtr(y);
		for (int x=startX; x<endX; x++) {
			pos[0]=x;
			sum+=ExtractNeighborhood(src,pos,this->pKernel,edgeStyle);
			kipl::math::median_quick_select(this->pKernel,this->nKernel, pLine+x);
		}
	}

}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::BilevelMedianFilter(const kipl::base::TImage<T,nDims> &/*src*/, 
		kipl::base::TImage<T,nDims> &/*result*/, 
		const FilterBase::EdgeProcessingStyle /*edgeStyle*/)
{
	// Convolution with uniform NxM kernel 
	// Compare number of elements with mid kernel
}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::RunningWindowLineInit( T const * const /*src*/, 
													T */*dest*/,
													size_t const * const /*dims*/)
{

}

template <class T, size_t nDims>
void TMedianFilter<T,nDims>::RunningWindowLine(T const * const /*src*/, T * /*dest*/, size_t)
{

}


template <class T, size_t nDims>
void TMedianFilter<T,nDims>::RunningWindowMedianFilter(kipl::base::TImage<T,nDims> &/*src*/, 
		kipl::base::TImage<T,nDims> &/*result*/, 
		const FilterBase::EdgeProcessingStyle /*edgeStyle*/)
{

	/*
	const size_t Nx=nKernelDims[0];
	const size_t Ny=nKernelDims[1];

	const size_t Nx2=Nx>>1;
	const size_t Ny2=Ny>>1;

	kipl::filters::RingBuffer<T> buffer;

	const size_t SizeX=src.Size(0);
	const size_t SizeY=src.Size(1);
	const size_t EndX=SizeX-nKernelDims[0];
	const size_t EndY=SizeY-nKernelDims[1];
	T *pSrc=src.GetDataPtr();
	T *pResult=result.GetDataPtr();
	for (size_t y=0; y<EndY; y++) {
		buffer.Reset();
		for (size_t x=0; x<EndX; x++) {
			for (size_t i=0; i<Ny; i++) {
				buffer.Push(pSrc[(y+i)*SizeX+x]);
			}	
			memcpy(this->pKernel,buffer.DataPtr(),buffer.Size()*sizeof(T));
			kipl::math::median(this->pKernel,this->nKernel, pResult+((y+Ny2)*SizeX+x+Nx2));
		}
	}
*/
}

template <typename T, size_t N>
TWeightedMedianFilter<T,N>::TWeightedMedianFilter(const std::vector<size_t> &dims, const std::vector<int> &weights) :
    TMedianFilter<T,N>(dims),
    m_nWeights(weights),
    m_nBufferLength(std::accumulate(weights.begin(),weights.end(),0))
{

}

template <typename T, size_t N>
TWeightedMedianFilter<T,N>::~TWeightedMedianFilter()
{
}

template <typename T, size_t N>
kipl::base::TImage<T,N> TWeightedMedianFilter<T,N>::operator() (kipl::base::TImage<T,N> &src,
			const FilterBase::EdgeProcessingStyle edgeStyle)
{
    kipl::base::TImage<T,N> result(src.dims());

	const int startY = 1 < N ? this->nHalfKernel[1] : 0 ;
	const int endY   = 1 < N ? src.Size(1) - (this->nKernelDims[1]-this->nHalfKernel[1]-1): 1 ;
	const int startX = this->nHalfKernel[0] ;
	const int endX   = src.Size(0) - (this->nKernelDims[0]-this->nHalfKernel[0]-1) ;

	#pragma omp parallel
	{
		T *kern  = new T[this->nKernel];
		T *kern2 = new T[this->m_nBufferLength];
		std::vector<size_t> pos={0,0};
		#pragma omp for
		for (int y=startY; y<endY; y++) {
			pos[1]=y-this->nHalfKernel[1];
			T * pLine=result.GetLinePtr(y);
			for (int x=startX; x<endX; x++) {
				pos[0]=x-this->nHalfKernel[0];
                TMedianFilter<T,N>::ExtractNeighborhood(src,pos,kern,edgeStyle);
				ExpandData(kern,kern2);
				kipl::math::median(kern2,m_nBufferLength, pLine+x);
			}
		}
		delete [] kern;
		delete [] kern2;
	}

	// Filling lazy edge
	for (int y=0; y<startY; y++)
		memcpy(result.GetLinePtr(y),result.GetLinePtr(startY),result.Size(0)*sizeof(T));

	for (size_t y=endY; y<result.Size(1); y++) {
		memcpy(result.GetLinePtr(y),result.GetLinePtr(endY-1),result.Size(0)*sizeof(T));
	}

	for (size_t y=0; y<result.Size(1); y++) {
		T *pLine=result.GetLinePtr(y);
		T val=pLine[startX];
		for (int x=0; x<startX; x++)
			pLine[x]=val;
		val=pLine[endX-1];
		for (size_t x=endX; x<result.Size(0); x++)
			pLine[x]=val;
	}

	return result;
}

template <typename T, size_t N>
void TWeightedMedianFilter<T,N>::ExpandData(T * src, T * buffer)
{
	for (size_t i=0, j=0; i<this->nKernel; ++i) 
	{
		for (int k=0; k<m_nWeights[i]; ++k, ++j)
			buffer[j]=src[i];
	}
}

}}

#endif /*MEDIANFILTER_HPP_*/
