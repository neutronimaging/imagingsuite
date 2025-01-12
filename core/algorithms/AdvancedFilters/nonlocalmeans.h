//<LICENCE>

#ifndef NONLOCALMEANS_H
#define NONLOCALMEANS_H

#include "advancedfilters_global.h"
#include <vector>
#include <iostream>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>
#include <filters/filterbase.h>
#include <base/thistogram.h>


namespace advancedfilters {

/// \brief A container for histogram bins that manages counts, sums, averages.
///
/// These bins are created as support for the NonLocalMeans filter with histogram implementation.
class ADVANCEDFILTERSSHARED_EXPORT HistogramBin {
public:
    /// \brief Initializes the bin to 0
    HistogramBin();

    /// \brief Copy C'tor
    /// \param h The bin instance to copy
    HistogramBin(const HistogramBin &h);

    /// \brief Copy C'tor
    /// \param h The bin instance to copy.
    HistogramBin & operator=(const HistogramBin &h);

    size_t cnt;         ///< Pixel counts for the specific bin
    double sum;         ///< Sum of all pixel values that the were put in the bin
    double local_avg;   ///< The local average sum
    double local_avg2;  ///< The sum of local average using squared pixel values
    double bin;         ///< The central value of the bin
};

class ADVANCEDFILTERSSHARED_EXPORT NonLocalMeans
{
protected:
    kipl::logging::Logger logger;
    public:
        /// \brief Enum to select the nonlocal means algorithm to use for the filtering
        enum NLMalgorithms
        {
            NLM_Naive,                  ///< Reference code to original algorithm
            NLM_HistogramOriginal,      ///< Reference code to histgram based algorithm
            NLM_HistogramSum,           ///<
            NLM_HistogramSumParallel,   ///< c++11 threaded version of NLM_HistogramSum, speedup close to N threads
            NLM_Bivariate,              ///< Nonlocal means filter using a bivariate histogram to store the counts
            NLM_BivariateParallel       ///< c++11 threaded version of NLM_HistogramSum, speedup close to N threads
        };

        /// \brief Enum to select the type of window weights to sum up the neighborhood values.
        enum NLMwindows
        {
            NLM_window_sum,     ///< Use the sum of the neighborhod pixels
            NLM_window_avg,     ///< Use the average of the neighborhood pixels
            NLM_window_gauss,   ///< Apply Gaussian weighting to the neighborhood pixels before summing
            NLM_window_buades   ///< Apply the Weighting defined by Buades in the original filter to the neighborhood pixels before summing
        };

        /// \brief C'tor to initialize the filter
        /// \param k Size of the box filter
        /// \param h width of the weighting function
        /// \param nBin number of histogram bins
        /// \param window Select neighborhood window
        /// \param algorithm Select non-local algorithm
        NonLocalMeans(int k, double h, size_t nBins=2048, NLMwindows window=NonLocalMeans::NLMwindows::NLM_window_sum,NLMalgorithms algorithm=NonLocalMeans::NLMalgorithms::NLM_HistogramSum);
        ~NonLocalMeans();

        /// \brief Applies the non-local means filter to a 2D image
        /// \param f The image to filter
        /// \param g The filtered image
        void operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g);

        /// \brief Applies the non-local means filter to a 3D image
        /// \param f The image to filter
        /// \param g The filtered image
        void operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g);

    protected:
        void NeighborhoodSums(kipl::base::TImage<float,2> &f,
                              kipl::base::TImage<float,2> &ff,
                              kipl::base::TImage<float,2> &ff2);

        size_t GetNeighborhood(float *img, float *pNeighborhood, ptrdiff_t pos, ptrdiff_t nLine, ptrdiff_t N);

        /// \brief Compute the squared difference sum
        /// \param a data set a
        /// \param b data set b
        /// \param N Number of elements in the arrays
        /// \returns the L2 norm of the arrays
        float L2Norm(float *a, float *b, size_t N);

        /// \brief Computes distance weight for the distance a
        /// \param a The value to compute the weight for
        /// \returns the distance weight
        float weight(float a);

        /// \brief Computes distance weight between a and b
        /// \param first value
        /// \param second value
        /// \returns the distance weight
        float weight(float a,float b);

        /// \brief Computes a new histogram
        /// \param data The data to fill the historam
        /// \param N number of elements in the data set.
        vector<pair<double, size_t> > ComputeHistogram(float *data, size_t N);

        /// \brief Computes a histogram and the average intensity in each bin
        /// \param f Original image
        /// \param ff mean image
        /// \param ff2 mean squared image
        /// \param N number of pixels
        void ComputeHistogramSum(float *f, float *ff, float *ff2, size_t N);

        /// \brief Naive implementation of the non-local means algorithm. Very slow due to N^2 complexity.
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_naive(float *f, float *g, size_t nLine, size_t N);

        /// \brief Implementation with histogram patching
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_hist_original(float *f, float *ff, float *g, size_t N);

        /// \brief Implementation with histogram patching
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_hist_single(float *f, float *ff, float *g, size_t N);

        /// \brief Implementation with histogram patching
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_hist_sum_single(float *f, float *ff, float *ff2, float *g, size_t N);

        /// \brief Implementation with bivariate histogram patching
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_bivariate(float *f, float *ff, float *ff2, float *g, size_t N);

        /// \brief Implementation with histogram patching summing a contributions per bin parallelized by c++11 threads
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_hist_sum_threaded(float *f, float *ff, float *ff2, float *g, size_t N);

        /// \brief Implementation with histogram patching parallelized by c++11 threads
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_hist_threaded(float *f, float *ff, float *g, size_t N);

        /// \brief Implementation with bivariate histogram patching parallelized by c++11 threads
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_bivariate_threaded(float *f, float *ff, float *ff2, float *g, size_t N);

        /// \brief Implementation with histogram patching, core algorithm
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_core_hist(float *f, float *ff, float *g, size_t N);

        /// \brief Implementation with histogram patching, core algorithm
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_core_hist_sum(float *f, float *ff, float *ff2, float *g, size_t N);

        /// \brief Implementation with histogram patching, core algorithm
        /// \param f pointer to the original image
        /// \param ff pointer to the filtered image
        /// \param g pointer to the result image
        /// \param N number of pixels
        void nlm_core_bivariate(float *f, float *ff, float *ff2, float *g, size_t N);

        void SaveCurrentHistogram();
        void SaveCurrentHistogram2();
        void SaveDebugImage(kipl::base::TImage<float,2> &img, std::string fname);

        bool m_bSaveDebugData;

        float m_fWidth;
        float m_fWidthLimit;
        int m_nBoxSize;

        size_t m_nHistSize;
        NLMalgorithms m_eAlgorithm;
        NLMwindows m_eWindow;
        kipl::filters::KernelType m_Kerneltype;
        size_t *m_nHistogram;
        double *m_fHistBins;
        double *m_fSums;
        std::vector<pair<double, size_t> > m_hist;
        std::vector<HistogramBin> m_Histogram;
        kipl::base::BivariateHistogram m_BivariateHistogram;
};

}

/// \brief Writes the algorithm enum value to a string
/// \param s The target stream
/// \param a The enum value
/// \returns the target stream
std::ostream ADVANCEDFILTERSSHARED_EXPORT & operator<<(std::ostream & s, advancedfilters::NonLocalMeans::NLMalgorithms a);

/// \brief Converts a enum value to its string name
/// \param a The enum value to convert
/// \returns the string name of the enum value
std::string  ADVANCEDFILTERSSHARED_EXPORT enum2string(advancedfilters::NonLocalMeans::NLMalgorithms a);

/// \brief Converts an enum name to its enum value
/// \param s The name string
/// \param a The algorithm enum as converted from the string
void  ADVANCEDFILTERSSHARED_EXPORT string2enum(std::string s, advancedfilters::NonLocalMeans::NLMalgorithms &a);

/// \brief Writes the window enum value to a string
/// \param s The target stream
/// \param a The enum value
/// \returns the target stream
std::ostream ADVANCEDFILTERSSHARED_EXPORT & operator<<(std::ostream & s, advancedfilters::NonLocalMeans::NLMwindows w);

/// \brief Converts a enum value to its string name
/// \param a The enum value to convert
/// \returns the string name of the enum value
std::string  ADVANCEDFILTERSSHARED_EXPORT enum2string(advancedfilters::NonLocalMeans::NLMwindows w);

/// \brief Converts an enum name to its enum value
/// \param s The name string
/// \param a The window enum as converted from the string
void  ADVANCEDFILTERSSHARED_EXPORT string2enum(std::string s, advancedfilters::NonLocalMeans::NLMwindows &w);

//#include "core/nonlocalmeans.hpp"
#endif // NONLOCALMEANS_H

