//<LICENCE>

#include "../../../include/kipl_global.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <thread>
#include <mutex>          // std::mutex, std::lock_guard
#ifdef _OPENMP
#include <omp.h>
#endif

#include "../../../include/math/sums.h"
#include "../../../include/math/image_statistics.h"
#include "../../../include/base/thistogram.h"
#include "../../../include/utilities/threadpool.h"
#include "../../../include/logging/logger.h"
#ifndef NO_TIFF
#include "../../../include/io/io_tiff.h"
#endif

namespace kipl { namespace base {
//int Histogram(float const * const data, size_t nData, size_t * const hist, const size_t nBins, float lo, float hi, float * const pAxis)

std::mutex histmtx;

int KIPLSHARED_EXPORT Histogram(float const * const data, size_t nData, size_t * hist, size_t nBins, float lo, float hi, float * pAxis, bool avoidZeros)
{

    float start=0;
    float stop=1.0f;

    if (lo==hi)
    {
        kipl::math::minmax(data,nData,&start,&stop,true);
    }
    else
    {
		start=std::min(lo,hi);
		stop=std::max(lo,hi);
    }

    std::fill_n(hist,nBins,0UL);
    
	float scale=(nBins)/(stop-start);
    #pragma omp parallel firstprivate(nData,start,scale)
    {
		int index;
		long long int i=0;	
        std::vector<size_t> temp_hist(nBins);
        //size_t *temp_hist=new size_t[nBins];
        std::fill(temp_hist.begin(),temp_hist.end(),0UL);

		const ptrdiff_t snData=static_cast<ptrdiff_t>(nData);
		const ptrdiff_t snBins=static_cast<ptrdiff_t>(nBins);
        if (avoidZeros)
        {
            #pragma omp for
            for (i=0; i<snData; i++) {
                if (data[i]!=0.0f)
                {
                    index=static_cast<int>((data[i]-start)*scale);
                    if ((index<snBins) && (0<=index))
                        temp_hist[index]++;
                }
            }
        }
        else
        {
            #pragma omp for
            for (i=0; i<snData; i++) {
                index=static_cast<int>((data[i]-start)*scale);
                if ((index<snBins) && (0<=index))
                    temp_hist[index]++;
            }
        }
		#pragma omp critical
		{
			for (i=0; i<snBins; i++)
				hist[i]+=temp_hist[i];
		}
        //delete [] temp_hist;
    }
    scale=(stop-start)/nBins;
    if (pAxis!=nullptr) {
        pAxis[0]=start+scale/2;
        for (size_t i=1; i<nBins; i++)
            pAxis[i]=pAxis[i-1]+scale;
    }

	return 0;
}

// int KIPLSHARED_EXPORT Histogram(float const * const data, size_t nData, size_t nBins, std::vector<size_t> & hist, std::vector<float> &axis, float lo, float hi, bool avoidZeros)
// {

//     float start=0;
//     float stop=1.0f;

//     if (lo==hi)
//     {
//         kipl::math::minmax(data,nData,&start,&stop,true);

//     }
//     else
//     {
//         start=std::min(lo,hi);
//         stop=std::max(lo,hi);
//     }

//     hist.resize(nBins);
//     axis.resize(nBins);

//     std::fill(hist.begin(),hist.end(),0UL);

//     float scale=(nBins)/(stop-start);
//     #pragma omp parallel firstprivate(nData,start,scale)
//     {
//         int index;
//         long long int i=0;
//         std::vector<size_t> temp_hist(nBins);
//         std::fill(temp_hist.begin(),temp_hist.end(),0UL);

//         const ptrdiff_t snData=static_cast<ptrdiff_t>(nData);
//         const ptrdiff_t snBins=static_cast<ptrdiff_t>(nBins);
//         if (avoidZeros)
//         {
//             #pragma omp for
//             for (i=0; i<snData; i++) {
//                 if ( (data[i]!=0.0f) && std::isfinite(data[i]))
//                 {
//                     index=static_cast<int>((data[i]-start)*scale);
//                     if ((index<snBins) && (0<=index))
//                         temp_hist[index]++;
//                 }
//             }
//         }
//         else
//         {
//             #pragma omp for
//             for (i=0; i<snData; i++) {
//                 index=static_cast<int>((data[i]-start)*scale);
//                 if ((index<snBins) && (0<=index))
//                     temp_hist[index]++;
//             }
//         }
//         #pragma omp critical
//         {
//             for (i=0; i<static_cast<int>(hist.size()); i++)
//                 hist[i]+=temp_hist[i];
//         }
//     }

//     float binIncrement = (stop-start)/nBins;
//     float binVal       = start+binIncrement/2;

//     for (auto &bin : axis)
//     {
//         bin     = binVal;
//         binVal += binIncrement;
//     }

//     return 0;
// }

int KIPLSHARED_EXPORT Histogram(float const * const data, size_t nData, size_t nBins, std::vector<size_t> & hist, std::vector<float> &axis, float lo, float hi, bool avoidZeros)
{

    kipl::logging::Logger logger("Histogram");

    float start=0;
    float stop=1.0f;

    if (lo==hi)
    {
        kipl::math::minmax(data,nData,&start,&stop,true);

    }
    else
    {
        start=std::min(lo,hi);
        stop=std::max(lo,hi);
    }

    hist.resize(nBins);
    axis.resize(nBins);

    std::fill(hist.begin(),hist.end(),0UL);

    float scale=(nBins-1)/(stop-start);

    const auto nThreads = std::thread::hardware_concurrency();
    kipl::utilities::ThreadPool pool(nThreads);
    logger.debug("Number of threads: " + std::to_string(nThreads));

    size_t nBlockSize = 8192; //nData/nThreads;
    size_t nBlocks    = nData/nBlockSize;
    size_t nRem       = nData % nBlockSize; //nData%nThreads;
    auto pBlock       = data;

    size_t sum = 0;

    for (size_t j=0; j<nBlocks; ++j)
    {
        size_t nBlockLen = nBlockSize;
        if (j==0)
            nBlockLen += nRem;
        std::ostringstream oss;
        oss << "Block " << j << " has length " << nBlockLen ;
        logger.debug(oss.str());

        pool.enqueue([pBlock, nBlockLen, start, scale, nBins, avoidZeros, &hist] {
            kipl::logging::Logger tlogger("Histogram thread");
            int index;

            std::vector<size_t> temp_hist(nBins,0UL);
            
            if (avoidZeros)
            {
                for (auto i=0; i<nBlockLen; i++)
                {
                    if ( (pBlock[i]!=0.0f) && std::isfinite(pBlock[i]))
                    {
                        index=static_cast<int>((pBlock[i]-start)*scale);
                        if ((index<nBins) && (0<=index))
                            temp_hist[index]++;
                        // else {
                        //     std::ostringstream msg;
                        //     msg << "Index out of range: " << index;  
                        //     tlogger.debug(msg.str());
                        // }
                    }
                }
            }
            else
            {
                for (auto i=0; i<nBlockLen; i++) 
                {
                    index=static_cast<int>((pBlock[i]-start)*scale);
                    if ((index<nBins) && (0<=index))
                        temp_hist[index]++;
                    // else {
                    //     std::ostringstream msg;
                    //     msg << "Index out of range: " << index;  
                    //     tlogger.debug(msg.str());
                    // }
                        
                }
            }

            std::lock_guard<std::mutex> lock(histmtx);
            {
                // for (auto i=0; i<static_cast<int>(hist.size()); ++i)
                //     hist[i]+=temp_hist[i];
                std::transform(hist.begin(), hist.end(), temp_hist.begin(), hist.begin(), std::plus<size_t>());
            }
        });

        pBlock += nBlockLen;
        sum+=nBlockLen;
    }

    pool.barrier();

    float binIncrement = (stop-start)/nBins;
    float binVal       = start+binIncrement/2;

    for (auto &bin : axis)
    {
        bin     = binVal;
        binVal += binIncrement;
    }

    std::cout << "Sum of all blocks: " << sum << std::endl;
    return 0;
}

void highEntropyHistogram(float const * const data,
                          size_t nData,
                          size_t nBins,
                          std::vector<size_t> & hist,
                          std::vector<float> &axis,
                          float loLevel,
                          float hiLevel,
                          bool avoidZeros)
{
   size_t lo=0;
   size_t hi=1;
   float ll = loLevel;
   float hl = hiLevel;

   while (2 < nBins/static_cast<float>(hi-lo))
   {
       Histogram(data,nData,nBins,hist,axis,ll,hl,avoidZeros);
       FindLimits(hist, 99.0f, lo, hi);
       if (lo==hi)
       {
            if (0<lo)
                lo--;
            else
                hi++;
       }
       ll = axis[lo];
       hl = axis[hi];
   } ;
}

std::map<float, size_t> ExactHistogram(float const * const data, size_t Ndata)
{
	std::map<float, size_t> hist;

    for (size_t i=0; i<Ndata; i++)
    {
		hist[data[i]]++;
	}

	return hist;
}


double  KIPLSHARED_EXPORT Entropy(size_t const * const hist, size_t N)
{
	double p=0.0;
	double entropy=0.0;

	size_t histsum=kipl::math::sum(hist,N);

    for (size_t i=0; i<N; i++)
    {
        if (hist[i]!=0)
        {
			p=static_cast<double>(hist[i])/static_cast<double>(histsum);
			entropy-=p*std::log10(p);
		}
	}

	return entropy;
}

std::vector<size_t> cumulativeHistogram(std::vector<size_t> &hist)
{
    std::vector<size_t> cumulated(hist.size(),0);

    cumulated[0]=hist[0];

    for (size_t i=1; i<cumulated.size(); ++i)
        cumulated[i] = cumulated[i-1]+hist[i];

    return cumulated;

}

int  KIPLSHARED_EXPORT FindLimits(size_t const * const hist, size_t N, float percentage, size_t * lo, size_t * hi)
{
	ptrdiff_t *cumulated=new ptrdiff_t[N];
	memset(cumulated,0,sizeof(ptrdiff_t)*N);

	cumulated[0]=hist[0];
	for (size_t i=1; i<N; i++) {
		cumulated[i]=cumulated[i-1]+hist[i];
	}

    if ((lo!=nullptr) && (hi!=nullptr)) {
		*lo=0;
		*hi=0;

		float fraction=(100.0f-percentage)/200.0f;
		ptrdiff_t lowlevel  = static_cast<ptrdiff_t>(cumulated[N-1]*fraction);
		ptrdiff_t highlevel = static_cast<ptrdiff_t>(cumulated[N-1]*(1-fraction));

		ptrdiff_t lowdiff  = cumulated[N-1];
		ptrdiff_t highdiff = cumulated[N-1];

        //ptrdiff_t diff=cumulated[N-1];
		for (size_t i=0; i<N; i++){
            ptrdiff_t diff=static_cast<ptrdiff_t>(std::abs(static_cast<double>(cumulated[i]-lowlevel)));
			if (diff<lowdiff) {
				lowdiff=diff;
				*lo=i;
			}

			diff=static_cast<ptrdiff_t>(std::abs(static_cast<double>(cumulated[i]-highlevel)));
			if (diff<highdiff) {
				highdiff=diff;
				*hi=i;
			}
		}
	}
	return 0;
}

int  KIPLSHARED_EXPORT FindLimits(std::vector<size_t> &hist, float percentage, size_t & lo, size_t & hi)
{
    std::vector<size_t> cumulated=cumulativeHistogram(hist);

    lo=0;
    hi=0;

    float fraction=(100.0f-percentage)/200.0f;
    ptrdiff_t lowlevel  = static_cast<ptrdiff_t>(cumulated.back()*fraction);
    ptrdiff_t highlevel = static_cast<ptrdiff_t>(cumulated.back()*(1-fraction));

    lo = std::distance(cumulated.begin(), std::lower_bound(cumulated.begin(), cumulated.end(), lowlevel));
    hi = std::distance(cumulated.begin(), std::lower_bound(cumulated.begin(), cumulated.end(), highlevel));

    return 0;
}

int  KIPLSHARED_EXPORT FindLimit(std::vector<size_t> &hist, float fraction, size_t & lo)
{
    std::vector<size_t> cumulated=cumulativeHistogram(hist);

    lo=0;

    ptrdiff_t lowlevel  = static_cast<ptrdiff_t>(cumulated.back()*fraction);
    lo = std::distance(cumulated.begin(), std::lower_bound(cumulated.begin(), cumulated.end(), lowlevel));

    return 0;
}

//------------------------------------------------------------------
// Bivariate histogram class
BivariateHistogram::BivariateHistogram() :
    logger("BivariateHistogram"),
    m_limitsA(0.0f,1.0f),
    m_limitsB(0.0f,1.0f),
    m_scalingA(1.0f,0.0f),
    m_scalingB(1.0f,0.0f)
{

}

BivariateHistogram::~BivariateHistogram()
{

}

/// \brief Initialize the histogram using limits
/// \param loA smallest accepted value for data set A
/// \param hiA greatest accepted value for data set A
/// \param binsA number of bins for data set A
/// \param loB smallest accepted value for data set B
/// \param hiB greatest accepted value for data set B
/// \param binsB number of bins for data set B
void BivariateHistogram::Initialize(float loA, float hiA, size_t binsA,
                float loB, float hiB, size_t binsB)
{
    m_limitsA.first   = min(loA,hiA);
    m_limitsA.second  = max(loA,hiA);

    m_limitsB.first   = min(loB,hiB);
    m_limitsB.second  = max(loB,hiB);

    m_nbins.first     = binsA;
    m_nbins.second    = binsB;

    m_scalingA.first  = static_cast<float>(m_nbins.first) / (m_limitsA.second-m_limitsA.first);
    m_scalingA.second = m_limitsA.first;

    m_scalingB.first  = static_cast<float>(m_nbins.second) / (m_limitsB.second-m_limitsB.first);
    m_scalingB.second = m_limitsB.first;

    std::vector<size_t> dims={m_nbins.first,m_nbins.second};
    m_bins.resize(dims);
}

/// \brief Initialize the histogram using data
/// \param pA reference to data set A
/// \param binsA number of bins for data set A
/// \param pB reference to data set B
/// \param binsB number of bins for data set B
/// \param N number of data elements in the data sets
void BivariateHistogram::Initialize(float *pA, size_t binsA, float *pB, size_t binsB, size_t N)
{
    m_limitsA.first = *std::min_element(pA,pA+N);
    m_limitsA.second = *std::max_element(pA,pA+N);

    m_limitsB.first = *std::min_element(pB,pB+N);
    m_limitsB.second = *std::max_element(pB,pB+N);

    Initialize(m_limitsA.first,m_limitsA.second, binsA,
               m_limitsB.first,m_limitsB.second, binsB);
}

/// \brief Add single data pair to the histogram
/// \param a value from data set A
/// \param b value from data set B
void BivariateHistogram::AddData(float a, float b)
{
    m_bins(ComputePos(a,m_scalingA,m_limitsA,m_nbins.first),
           ComputePos(b,m_scalingB,m_limitsB,m_nbins.second))++;
}

/// \brief Add single data pair to the histogram
/// \param a pointer to data set A
/// \param b pointer to data set B
/// \param N number of data points
void BivariateHistogram::AddData(float *a, float *b, size_t N)
{
    for (size_t i=0; i<N; i++) {
        m_bins(ComputePos(a[i],m_scalingA,m_limitsA,m_nbins.first),
               ComputePos(b[i],m_scalingB,m_limitsB,m_nbins.second))++;
    }
}

/// \brief Get counts at the bin closest to the coordinates
/// \param a Coordinate in data set A
/// \param b Coordinate in data set B
BivariateHistogram::BinInfo BivariateHistogram::GetBin(float a, float b)
{
    int posA=ComputePos(a,m_scalingA,m_limitsA,m_nbins.first);
    int posB=ComputePos(b,m_scalingB,m_limitsB,m_nbins.second);


    return BinInfo(m_bins(posA,posB),
                   posA/m_scalingA.first+m_scalingA.second+m_scalingA.first*0.5f,
                   posB/m_scalingB.first+m_scalingB.second+m_scalingB.first*0.5f,
                   posA,posB);

}

/// \brief Get counts at the bin closest to the coordinates
/// \param a Coordinate in data set A
/// \param b Coordinate in data set B
BivariateHistogram::BinInfo BivariateHistogram::GetBin(int posA, int posB)
{
    return BinInfo(m_bins(posA,posB),
                   posA/m_scalingA.first+m_scalingA.second+m_scalingA.first*0.5f,
                   posB/m_scalingB.first+m_scalingB.second+m_scalingB.first*0.5f,
                   posA,posB);
}

/// \brief Get axis ticks for data set A
/// \returns the pointer to the axis ticks
const float * BivariateHistogram::GetAxisA()
{
    return nullptr;
}

/// \brief Get axis ticks for data set B
/// \returns the pointer to the axis ticks
float const * BivariateHistogram::GetAxisB()
{
    return nullptr;
}

kipl::base::TImage<size_t,2> & BivariateHistogram::Bins()
{
    return m_bins;
}

const std::vector<size_t> & BivariateHistogram::Dims()
{
    return m_bins.dims();
}

/// \brief Compute index to a histogram bin. This is the generic version.
/// \param x
/// \param scaling
/// \param limits
/// \param nBins
inline int BivariateHistogram::ComputePos(float x, std::pair<float, float> &scaling, std::pair<float, float> &limits, size_t nBins)
{
    int selector=(x<limits.first)+2*(limits.second<=x);

    switch (selector) {
        case 0: return static_cast<int>((x-scaling.second)*scaling.first); break;
        case 1: return 0;
        case 2: return nBins-1;
        default: logger(logger.LogWarning, "Strange selector value in ComputePos"); break;
    }

    return 0;
}

std::pair<float,float> BivariateHistogram::GetLimits(int n)
{
    switch (n) {
    case 0: return m_limitsA; break;
    case 1: return m_limitsB; break;
    default : throw kipl::base::KiplException("No existing axis selected in GetLimits",__FILE__,__LINE__);
    }

    return m_limitsA;
}

std::map<float, map<float,size_t> > BivariateHistogram::CompressedHistogram(kipl::base::eImageAxes mainaxis, size_t threshold)
{
    std::ignore = threshold;

    std::map<float, std::map<float,size_t> > hist;

    size_t *pLine;
    switch (mainaxis) {
    case kipl::base::ImageAxisX:
        for (size_t i=0; i<m_bins.Size(1); i++)
        {
            pLine=m_bins.GetLinePtr(i);
// todo: fix the code
//            std::map<float,size_t> listline;
//            for (size_t j=0; j<m_Bins.Size(0); j++)
//            {
//                if (threshold<=pLine[j])
//                {
//                 //   listline.insert(make_pair())
//                }
//            }
//            hist.insert(make_pair(binval,listline));
        }

        break;
    case kipl::base::ImageAxisY:
    case kipl::base::ImageAxisZ:
    default:
        throw kipl::base::KiplException("Unsupported axis was selected in compressedhistogram.",__FILE__,__LINE__);
        break;
    }

    return hist;
}

void BivariateHistogram::Write(string fname)
{
#ifndef NO_TIFF
    kipl::base::TImage<float,2> img(m_bins.dims());

   // float cnt=static_cast<float>(kipl::math::sum(m_bins.GetDataPtr(),m_bins.Size()));

    for (size_t i=0; i<m_bins.Size(); i++) {
        img[i]=static_cast<float>(m_bins[i]);
    }

    kipl::io::WriteTIFF(img,fname,kipl::base::Float32);
#endif
}

}}
