#ifndef __THISTOGRAM_H
#define __THISTOGRAM_H
#include "../kipl_global.h"
#include "timage.h"
#include <vector>
#include <map>
#include <fstream>
#include "../logging/logger.h"

namespace kipl { namespace base {
//int KIPLSHARED_EXPORT Histogram(float const * const data, size_t Ndata, size_t  * const hist, const size_t nBins, float lo=0.0f, float hi=0.0f, float  * const pAxis=NULL);
int KIPLSHARED_EXPORT Histogram(float * data, size_t Ndata, size_t  * hist, size_t nBins, float lo=0.0f, float hi=0.0f, float  * pAxis=NULL);

std::map<float, size_t> KIPLSHARED_EXPORT ExactHistogram(float const * const data, size_t Ndata);

int KIPLSHARED_EXPORT FindLimits(size_t const * const hist, size_t N, float percentage, size_t * lo, size_t * hi);

double KIPLSHARED_EXPORT Entropy(size_t const * const hist, size_t N);

template<typename T>
class THistogram
{
public:
    THistogram(size_t nbins, T low=static_cast<T>(0), T high=static_cast<T>(0));
    ~THistogram();
    void SetInterval(size_t length, T low=0, T high=0);
	template<size_t N>
    std::vector<pair<float,size_t> > operator()(TImage<T,N> &img) {
        ComputeHistogram(img.GetDataPtr(), img.Size());
        std::vector<pair<float,size_t> > hist;

        for (size_t i=0; i<m_nBins; i++)
            hist.push_back(std::make_pair(m_pAxis[i],m_pHistogram[i]));

        return hist;
    }

    template<size_t N>
    std::vector<pair<float,float> > cumHist(TImage<T,N> &img, bool bNorm=false) {
        ComputeHistogram(img.GetDataPtr(),img.Size());
        std::vector<pair<float,float> > hist;

        hist.push_back(std::make_pair(m_pAxis[0],m_pHistogram[0]));
        size_t sum=m_pHistogram[0];

        for (size_t i=1; i<m_nBins; i++) {
            sum+=m_pHistogram[i];
            hist.push_back(std::make_pair(m_pAxis[i],static_cast<float>(sum)));
        }
        if (bNorm) {
            std::vector<pair<float,float> >::iterator it;
            for (it=hist.begin(); it!=hist.end(); it++)
                it->second=it->second/static_cast<float>(sum);
        }

        return hist;
    }


private:
    void ComputeHistogram(T * pData, size_t nData);
    size_t m_nBins;
    size_t *m_pHistogram;
    float  *m_pAxis;
    float  m_fSlope;
    float  m_fIntercept;
    T      m_LowValue;
    T      m_HighValue;
};


class KIPLSHARED_EXPORT BivariateHistogram
{
    kipl::logging::Logger logger;
public:
    struct BinInfo {
        BinInfo(size_t c, float a, float b) :
            count(c), binA(a), binB(b) {}
        size_t count;
        float binA;
        float binB;
    };
    BivariateHistogram();
    ~BivariateHistogram();

    /// \brief Initialize the histogram using limits
    /// \param loA smallest accepted value for data set A
    /// \param hiA greatest accepted value for data set A
    /// \param binsA number of bins for data set A
    /// \param loB smallest accepted value for data set B
    /// \param hiB greatest accepted value for data set B
    /// \param binsB number of bins for data set B
    /// \test through a unit test
    void Initialize(float loA, float hiA, size_t binsA,
                    float loB, float hiB, size_t binsB);

    /// \brief Initialize the histogram using data
    /// \param pA reference to data set A
    /// \param binsA number of bins for data set A
    /// \param pB reference to data set B
    /// \param binsB number of bins for data set B
    /// \param N number of data elements in the data sets
    void Initialize(float *pA, size_t binsA,
                    float *pB, size_t binsB, size_t N);

    /// \brief Add single data pair to the histogram
    /// \param a value from data set A
    /// \param b value from data set B
    void AddData(float a, float b);

    /// \brief Add single data pair to the histogram
    /// \param a pointer to data set A
    /// \param b pointer to data set B
    /// \param N number of data points
    void AddData(float *a, float *b, size_t N);

    /// \brief Get counts at the bin closest to the coordinates
    /// \param a Coordinate in data set A
    /// \param b Coordinate in data set B
    /// \test Through a unit test
    BivariateHistogram::BinInfo GetBin(float a, float b);

    /// \brief Get axis ticks for data set A
    /// \returns the pointer to the axis ticks
    float const *  GetAxisA();

    /// \brief Get axis ticks for data set B
    /// \returns the pointer to the axis ticks
    float const *  GetAxisB();

    /// \brief An image reference to the histgram bins
    /// \note This a non-constant reference, i.e. the data can be modified. Be careful.
    /// \test Used during the GetBin tests
    kipl::base::TImage<size_t,2> & Bins();

    /// \brief The number of bins for each class in an array
    /// \test Tested in the initialization tests.
    const size_t *Dims();

    /// \brief Provides the limits for each class
    /// \param n Selects data set A or B using 0 or 1
    std::pair<float,float> GetLimits(int n);

protected:
    /// \brief Computes the index of the bin closest to a given value
    /// \param x the value
    /// \param scaling the scaling parameters needed to compute the index
    /// \param limits limits of the histogram used for boundary check
    /// \param nBins number of bins for the specified data set
    /// \test Indirectly tested through GetBin(a,b)
    int ComputePos(float x, std::pair<float,float> &scaling, std::pair<float,float> &limits, size_t nBins);
    kipl::base::TImage<size_t,2> m_bins;
    std::pair<float,float> m_limitsA;
    std::pair<float,float> m_limitsB;
    std::pair<float,float> m_scalingA;
    std::pair<float,float> m_scalingB;
    std::pair<size_t,size_t> m_nbins;

};
}} // Namespaces

#include "core/thistogram.hpp"
#endif
