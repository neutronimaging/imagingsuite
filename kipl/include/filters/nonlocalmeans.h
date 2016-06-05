#ifndef NONLOCALMEANS_H
#define NONLOCALMEANS_H

#include <vector>
#include <iostream>
#include <string>

#include "../base/timage.h"
#include "../logging/logger.h"
#include "filterbase.h"
#include "../base/thistogram.h"


namespace akipl {

class KIPLSHARED_EXPORT HistogramBin {
public:
    HistogramBin();
    HistogramBin(const HistogramBin &h);
    HistogramBin & operator=(const HistogramBin &h);

    size_t cnt;
    double sum;
    double local_avg;
    double local_avg2;
    double bin;
};

class KIPLSHARED_EXPORT NonLocalMeans {
protected:
    kipl::logging::Logger logger;
    public:
        enum NLMalgorithms {
            NLM_Naive,              // Reference code to original algorithm
            NLM_HistogramOriginal,  // Reference code to histgram based algorithm
            NLM_HistogramSum,
            NLM_HistogramSumParallel, // c++11 threaded version of NLM_HistogramSum, speedup close to N threads
            NLM_Bivariate,
            NLM_BivariateParallel   // c++11 threaded version of NLM_HistogramSum, speedup close to N threads
        };

        enum NLMwindows {
            NLM_window_sum,
            NLM_window_avg,
            NLM_window_gauss,
            NLM_window_buades
        };
        /// \brief C'tor
        /// \param k Size of the box filter
        /// \param h width of the weighting function
        /// \param nBin number of histogram bins
        NonLocalMeans(int k, double h, size_t nBins=2048, NLMwindows window=NonLocalMeans::NLMwindows::NLM_window_sum,NLMalgorithms algorithm=NonLocalMeans::NLMalgorithms::NLM_HistogramSum);
        ~NonLocalMeans();

        void operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g);
        void operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g);

    protected:
        void NeighborhoodSums(kipl::base::TImage<float,2> &f,
                              kipl::base::TImage<float,2> &ff,
                              kipl::base::TImage<float,2> &ff2);

        size_t GetNeighborhood(float *img, float *pNeighborhood, ptrdiff_t pos, ptrdiff_t nLine, ptrdiff_t N);

        float L2Norm(float *a, float *b, size_t N);

        /// \brief Computes distance weight between a and b
        /// \param first value
        float weight(float a);

        /// \brief Computes distance weight between a and b
        /// \param first value
        /// \param second value
        float weight(float a,float b);

        /// \brief Computes a new histogram
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

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream & s, akipl::NonLocalMeans::NLMalgorithms a);
std::string  KIPLSHARED_EXPORT enum2string(akipl::NonLocalMeans::NLMalgorithms a);
void  KIPLSHARED_EXPORT string2enum(std::string s, akipl::NonLocalMeans::NLMalgorithms &a);

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream & s, akipl::NonLocalMeans::NLMwindows w);
std::string  KIPLSHARED_EXPORT enum2string(akipl::NonLocalMeans::NLMwindows w);
void  KIPLSHARED_EXPORT string2enum(std::string s, akipl::NonLocalMeans::NLMwindows &w);

//#include "core/nonlocalmeans.hpp"
#endif // NONLOCALMEANS_H

