#ifndef THISTOGRAM_HPP
#define THISTOGRAM_HPP

namespace kipl { namespace base {
template<typename T>
THistogram<T>::THistogram(size_t nbins, T low, T high) :
    m_nBins(nbins),
    m_pHistogram(NULL),
    m_pAxis(NULL),
    m_fSlope(1),
    m_fIntercept(0),
    m_LowValue(low),
    m_HighValue(high)
{
    SetInterval(nbins,low,high);
}

template<typename T>
THistogram<T>::~THistogram()
{
    if (m_pHistogram!=NULL)
        delete [] m_pHistogram;

    if (m_pAxis!=NULL)
        delete [] m_pAxis;
}

template<typename T>
void THistogram<T>::SetInterval(size_t length, T low, T high)
{
    m_nBins=length;
    if (m_pHistogram!=NULL)
        delete [] m_pHistogram;

    m_pHistogram = new size_t[m_nBins];

    if (m_pAxis!=NULL)
        delete [] m_pAxis;

    m_pAxis = new float[m_nBins];

    if (high<low)
        swap(low,high);

    m_fSlope    = (high-low)/(length-1);
    m_fIntercept = low;
    m_LowValue  = low;
    m_HighValue = high;
}

template<typename T>
void THistogram<T>::ComputeHistogram(T * pData, size_t nData)
{
    memset(m_pHistogram,0,sizeof(size_t)*m_nBins);

    float scale=1.0f/m_fSlope;
    #pragma omp parallel firstprivate(nData,scale)
    {
        int index;
        long long int i=0;
        size_t *temp_hist=new size_t[m_nBins];
        memset(temp_hist,0,m_nBins*sizeof(size_t));
        const ptrdiff_t snData=static_cast<ptrdiff_t>(nData);
        const ptrdiff_t snBins=static_cast<ptrdiff_t>(m_nBins);

        #pragma omp for
        for (i=0; i<snData; i++) {
            index=static_cast<int>((pData[i]-m_fIntercept)*scale);
            if ((index<snBins) && (0<=index))
                temp_hist[index]++;
        }
        #pragma omp critical
        {
            for (i=0; i<snBins; i++)
                m_pHistogram[i]+=temp_hist[i];
        }
        delete [] temp_hist;
    }
    scale=(m_HighValue-m_LowValue)/m_nBins;
    if (m_pAxis!=NULL) {
        m_pAxis[0]=m_LowValue+scale/2;
        for (size_t i=1; i<m_nBins; i++)
            m_pAxis[i]=m_pAxis[i-1]+scale;
    }
}

}}
#endif // THISTOGRAM_HPP
