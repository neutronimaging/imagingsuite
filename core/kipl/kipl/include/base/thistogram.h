//<LICENCE>

#ifndef __THISTOGRAM_H
#define __THISTOGRAM_H
#include "../kipl_global.h"

#include <vector>
#include <map>
#include <fstream>

#include "timage.h"
#include "../logging/logger.h"
#include "kiplenums.h"

namespace kipl { namespace base {
//int KIPLSHARED_EXPORT Histogram(float const * const data, size_t Ndata, size_t  * const hist, const size_t nBins, float lo=0.0f, float hi=0.0f, float  * const pAxis=NULL);

/// \brief Computes a histogram from the data present in an array
/// \param data The data array
/// \param Ndata number of data points in the array
/// \param hist the histogram counts, array must be preallocated
/// \param nBins Number of bins in the histogram
/// \param lo Lower bound of the bins. Values less than this bound are counted in the first bin
/// \param hi Upper bound of the bins. Values greater than this bound are counted in the last bin
/// \note If lo==hi the histogram will be using min and max intensity as interval.
/// \param pAxis optional bin axis value array, must be preallocated. Not considered if NULL
/// \returns Always 0
int KIPLSHARED_EXPORT Histogram(float * data, size_t Ndata, size_t  * hist, size_t nBins, float lo=0.0f, float hi=0.0f, float  * pAxis=NULL);

/// \brief Computes an exact histogram from the data present in an array
/// \param data The data array
/// \param Ndata number of data points in the array
std::map<float, size_t> KIPLSHARED_EXPORT ExactHistogram(float const * const data, size_t Ndata);

/// \brief Identifies an intensity interval using a histogram based on the percentage of pixels
/// \param hist The histogram
/// \param N number of bins in the histogram
/// \param percentage the percentage of the quantile to consider.
/// \param lo resulting lower bound
/// \param hi resulting upper bound
int KIPLSHARED_EXPORT FindLimits(size_t const * const hist, size_t N, float percentage, size_t * lo, size_t * hi);

/// \brief Computes the entropy of the histogram
/// \param hist The histogram
/// \param N number of bins in the histogram
/// \returns the entropy value
double KIPLSHARED_EXPORT Entropy(size_t const * const hist, size_t N);

/// \brief A generic implementation of a histogram. The next generation of the histogram handling
template<typename T>
class THistogram
{
public:
    /// \brief Initializes the histogram
    /// \param nbin Number of bins
    /// \param low Lower bound of the histogram
    /// \param high Upper bound of the histogram
    /// \note If lo==hi the histogram will be using min and max intensity as interval.
    THistogram(size_t nbins=256L, T low=static_cast<T>(0), T high=static_cast<T>(0));
    ~THistogram();

    /// \brief Change the interval and number of bins of the histogram
    /// \param length Number of bins
    /// \param low Lower bound of the histogram
    /// \param high Upper bound of the histogram
    /// \note If lo==hi the histogram will be using min and max intensity as interval.
    void SetInterval(size_t length, T low=0, T high=0);

    /// \brief Computes a histogram of an image
    /// \param img The image with te data
    /// \return a vector with bin value and count pairs
	template<size_t N>
    std::vector<pair<float,size_t> > operator()(TImage<T,N> &img) {
        ComputeHistogram(img.GetDataPtr(), img.Size());
        std::vector<pair<float,size_t> > hist;

        for (size_t i=0; i<m_nBins; i++)
            hist.push_back(std::make_pair(m_pAxis[i],m_pHistogram[i]));

        return hist;
    }

    /// \brief Computes a cumulative histogram of an image
    /// \param img The image with the data
    /// \param bNorm Selects if the cumulative histogram is normalized to one for the last bin
    /// \return a vector with bin value and sum/fraction pairs
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

/// \brief Computes a bivariate histogram from two data sets and provides access methods
/// \test TkiplbasetestTest::testBivariateHistogramInitialize() in tkiplbasetest.pro
class KIPLSHARED_EXPORT BivariateHistogram
{
    kipl::logging::Logger logger;
public:
    /// \brief Bin information struct
    struct KIPLSHARED_EXPORT BinInfo {
        /// \brief Basic initialization
        BinInfo() : count(0L), binA(0.0f), binB(0.0f), idxA(0), idxB(0) {}

        /// \brief Initialization
        /// \param c Bin count
        /// \param a bin axis position on data set A
        /// \param b bin axis position on data set B
        /// \param ia index for bin A
        /// \param ib index for bin B
        BinInfo(size_t c, float a, float b, int ia, int ib) :
            count(c), binA(a), binB(b), idxA(ia), idxB(ib) {}

        /// \brief Assignment operator
        /// \param bi the info to copy
        const BinInfo & operator=(BinInfo const &bi) {
            count=bi.count;
            binA=bi.binA;
            binB=bi.binB;
            idxA=bi.idxA;
            idxB=bi.idxB;

            return *this;
        }
        size_t count;
        float binA;
        float binB;
        int idxA;
        int idxB;
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

    /// \brief Get counts at the bin defined by the indices
    /// \param posA Coordinate in data set A
    /// \param posB Coordinate in data set B
    /// \test Through a unit test (not implemented)
    BivariateHistogram::BinInfo GetBin(int posA, int posB);

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

    /// \brief Makes a list of all non-zero bins in the histogram
    ///
    /// \param axis Selects the dominant axis in the list
    /// \param threshold selects the smallest count to include in the list
    ///
    /// \returns A list of all bins and their counts
    std::map<float, std::map<float,size_t> > CompressedHistogram(kipl::base::eImageAxes mainaxis, size_t threshold=1);

    /// \brief Writes the histogram as normalized image
    /// \param fname File name of the destination tiff file
    void Write(std::string fname);

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
