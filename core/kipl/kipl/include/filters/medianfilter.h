//<LICENCE>

#ifndef _MEDIANFILTER_H_
#define _MEDIANFILTER_H_

#include <iostream>
#include <vector>

#include "../base/timage.h"
#include "../logging/logger.h"
#include "filterbase.h"


namespace kipl { namespace filters {
    enum eMedianAlgorithm
    {
        MedianAlg_BiLevel,
        MedianAlg_QuickMedian,
        MedianAlg_HeapSortMedian,
        MedianAlg_HeapSortMedianSTL,
		MedianAlg_HeapSortMedianPool,
        MedianAlg_STLSortMedian
    };

	/// Implements a median filter
	template <class T, size_t nDims>
	class TMedianFilter : public kipl::filters::TFilterBase<T,nDims>
	{
        kipl::logging::Logger logger;
	public:
		// /// \brief Median filters an image
		// /// \param img The source image and result
		// /// \param edgeStyle Processing style for the image edges
		// ///
		// /// \note Edge processing solved by assigning outside locations to edge value
        kipl::base::TImage<T,nDims> operator() (const kipl::base::TImage<T,nDims> &src, const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero) override;

		/// \brief Creates a median filter 
		/// \param Dims Array constaining the dimensions of the filter
        TMedianFilter(const std::vector<size_t> & dims) ;
		/// Speed-up switch for bilevel images (majority counting)
		bool bilevel;
		/// Switch to select a quick median implementation
        eMedianAlgorithm medianAlgorithm;
		
		/// Empty destructor
		virtual ~TMedianFilter() {}
		
	protected: 
		// /// \brief Filter core method that process most of the image
		// /// \param img Pointer to the image to filter
		// /// \param imgDims image dimensions
		// /// \param res pointer to the filtered image
		// /// \param resDims dimensions of the filtered image
		// int ProcessCore(const kipl::base::TImage<T,nDims> & img,
		// 				kipl::base::TImage<T,nDims>  & res, 
		// 				const FilterBase::EdgeProcessingStyle edgeStyle);

		void PrintMatrix(T *data,int nx, int ny);
		void HeapSortMedianFilter(const kipl::base::TImage<T,nDims>    &src, 
										kipl::base::TImage<T,nDims>    &result, 
								  const FilterBase::EdgeProcessingStyle edgeStyle);

        void HeapSortMedianFilterSTL(const kipl::base::TImage<T,nDims>    &src,
										   kipl::base::TImage<T,nDims>    &result,
									 const FilterBase::EdgeProcessingStyle edgeStyle);
									 
		void HeapSortMedianFilterPool( const kipl::base::TImage<T,nDims> &src,
                							kipl::base::TImage<T,nDims> &result,
                						const FilterBase::EdgeProcessingStyle edgeStyle);

        void HeapSortInnerLoop(const kipl::base::TImage<T,nDims> *src,
									 kipl::base::TImage<T,nDims> *result,
									 size_t begin,
									 size_t end,
							   const FilterBase::EdgeProcessingStyle edgeStyle);

        void STLSortMedianFilter(const kipl::base::TImage<T,nDims> &src,
								kipl::base::TImage<T,nDims> &result, 
								const FilterBase::EdgeProcessingStyle edgeStyle);

		void QuickMedianFilter(const kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle);
				
		void BilevelMedianFilter(const kipl::base::TImage<T,nDims> &src, 
				kipl::base::TImage<T,nDims> &result, 
				const FilterBase::EdgeProcessingStyle edgeStyle);
		
		int ExtractNeighborhood(const kipl::base::TImage<T,nDims> &src, const std::vector<size_t> & pos, T * data, const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero);
		
        void InitResultArray(const kipl::base::TImage<T,nDims> &src, kipl::base::TImage<T,nDims> &dest) override;
        void InnerLoop(T const * const src, T *dest, T value, size_t N) override;
	
		void RunningWindowLineInit(T const * const src, T *dest,size_t const * const dims);
		void RunningWindowLine(T const * const src, T *dest, size_t);
		void RunningWindowMedianFilter(kipl::base::TImage<T,nDims> &src, 
		kipl::base::TImage<T,nDims> &result, 
		const FilterBase::EdgeProcessingStyle edgeStyle);

		// // Edge processing
		// /// \brief Specific edge filtering method that handles the edge processing for 2D data
    	// /// \param img Pointer to the image to filter
    	// /// \param res pointer to the filtered image
    	// /// \param imgDims image dimensions
    	// /// \param epStyle Select how to process the edges
		// int ProcessEdge(T const * const img, 
		// 				T       *       res, 
		// 				const std::vector<size_t> & imgDims,
		// 				const FilterBase::EdgeProcessingStyle epStyle) override;

		// /// \brief Specific edge filtering method that handles the edge processing for 2D data
    	// /// \param img Pointer to the image to filter
    	// /// \param res pointer to the filtered image
    	// /// \param imgDims image dimensions
    	// /// \param epStyle Select how to process the edges
    	// int ProcessEdge2D(T const * const img,
		//   T * res, 
        //   const std::vector<size_t> & imgDims,
		//   const FilterBase::EdgeProcessingStyle epStyle) override;	
		
        int nHalfKernel[nDims];
	};

    /// \brief Implements a weighted median filter.
    ///
    /// The filter repeats the neigborhood pixels the number of times the filter weight indicates. This Filter can be used as a outlier suppression filter.
	template <typename T, size_t N>
	class TWeightedMedianFilter : public TMedianFilter<T,N>
	{
	public:
        TWeightedMedianFilter(const std::vector<size_t> & dims, const std::vector<int> &weights);
		virtual ~TWeightedMedianFilter();

		virtual kipl::base::TImage<T,N> operator() (kipl::base::TImage<T,N> &src,
				const FilterBase::EdgeProcessingStyle edgeStyle=FilterBase::EdgeZero);

	protected:
		void ExpandData(T * src, T * buffer);
        std::vector<int> m_nWeights;
		int m_nBufferLength;
	};



}}

#include "core/medianfilter.hpp"

#endif
