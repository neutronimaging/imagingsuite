//<LICENCE>

#ifndef _MEDIANFILTER_H_
#define _MEDIANFILTER_H_

#include <iostream>

#include "../base/timage.h"
#include "filterbase.h"


namespace kipl { namespace filters {

	/// Implements a median filter
	template <class T, size_t nDims>
	class TMedianFilter : public kipl::filters::TFilterBase<T,nDims>
	{
	public:
		/// \brief Median filters an image
		/// \param img The source image and result
		/// \param edgeStyle Processing style for the image edges
		///
		/// \note Edge processing solved by assigning outside locations to edge value
		virtual kipl::base::TImage<T,nDims> operator() (kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero);

		/// \brief Creates a median filter 
		/// \param Dims Array constaining the dimensions of the filter
		TMedianFilter(size_t const * const Dims) ;
		/// Speed-up switch for bilevel images (majority counting)
		bool bilevel;
		/// Switch to select a quick median implementation
		bool quick_median;
		
		/// Empty destructor
		virtual ~TMedianFilter() {}
		
	protected: 
		void PrintMatrix(T *data,int nx, int ny);
		void HeapSortMedianFilter(kipl::base::TImage<T,nDims> &src, 
						kipl::base::TImage<T,nDims> &result, 
						const FilterBase::EdgeProcessingStyle edgeStyle);
		void STLSortMedianFilter(kipl::base::TImage<T,nDims> &src, 
								kipl::base::TImage<T,nDims> &result, 
								const FilterBase::EdgeProcessingStyle edgeStyle);
		void QuickMedianFilter(kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle);
		void BilevelMedianFilter(kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle);
		
		int ExtractNeighborhood(kipl::base::TImage<T,nDims> &src, size_t const * const pos, T * data, const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero);
		
        virtual void InitResultArray(kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest) {}
        virtual void InnerLoop(T const * const src, T *dest, T value, size_t N) {} // Dummy
	
		void RunningWindowLineInit(T const * const src, T *dest,size_t const * const dims);
		void RunningWindowLine(T const * const src, T *dest, size_t);
		void RunningWindowMedianFilter(kipl::base::TImage<T,nDims> &src, 
		kipl::base::TImage<T,nDims> &result, 
		const FilterBase::EdgeProcessingStyle edgeStyle);
		
		int nHalfKernel[nDims];
	};

    /// \brief Implements a weighted median filter.
    ///
    /// The filter repeats the neigborhood pixels the number of times the filter weight indicates. This Filter can be used as a outlier suppression filter.
	template <typename T, size_t N>
	class TWeightedMedianFilter : public TMedianFilter<T,N>
	{
	public:
		TWeightedMedianFilter(size_t const * const Dims, int *weights);
		virtual ~TWeightedMedianFilter();

		virtual kipl::base::TImage<T,N> operator() (kipl::base::TImage<T,N> &src,
				const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero);

	protected:
		void ExpandData(T * src, T * buffer);
		int *m_nWeights;
		int m_nBufferLength;
	};



}}

#include "core/medianfilter.hpp"

#endif
