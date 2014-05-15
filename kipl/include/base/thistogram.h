#ifndef __THISTOGRAM_H
#define __THISTOGRAM_H
#include "timage.h"
#include <vector>
#include <map>
#include <fstream>

namespace kipl { namespace base {
int Histogram(float const * const data, size_t Ndata, size_t  * const hist, size_t nBins, float lo=0.0f, float hi=0.0f, float  * const pAxis=NULL);

std::map<float, size_t> ExactHistogram(float const * const data, size_t Ndata);

int FindLimits(size_t const * const hist, size_t N, float percentage, size_t * lo, size_t * hi);

double Entropy(size_t const * const hist, size_t N);

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

}} // Namespaces

#include "core/thistogram.hpp"
#endif
