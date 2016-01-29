#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <thread>

#include "../../include/filters/nonlocalmeans.h"
#include "../../include/filters/filter.h"
#include "../../include/base/thistogram.h"
#include "../../include/io/io_tiff.h"

namespace akipl {
//===========================================================
// Implementation of helper class Histogram bin
HistogramBin::HistogramBin() :
    cnt(0L),
    sum(0.0),
    bin(0.0)
{

}

HistogramBin::HistogramBin(const HistogramBin &h) :
    cnt(h.cnt),
    sum(h.sum),
    bin(h.bin)
{

}

HistogramBin & HistogramBin::operator=(const HistogramBin &h)
{
    cnt=h.cnt;
    sum=h.sum;
    bin=h.bin;
}

//===========================================================
// Implementation of Non local means filter
NonLocalMeans::NonLocalMeans(int k, double h, size_t nBins, NLMalgorithms algorithm) :
    logger("NonLocalMeans"),
    m_bSaveDebugData(true),
    m_fWidth(1.0f/(h*h)),
    m_fWidthLimit(2.65f*h),
    m_nBoxSize(k),
    m_nHistSize(nBins),
    m_eAlgorithm(algorithm),
    m_nHistogram(nullptr),
    m_fHistBins(nullptr),
    m_fSums(nullptr)
{
    m_nHistogram = new size_t[m_nHistSize];
    m_fHistBins  = new double[m_nHistSize];
    m_fSums      = new double[m_nHistSize];
}

NonLocalMeans::~NonLocalMeans()
{
    delete [] m_nHistogram;
    delete [] m_fHistBins;
    delete [] m_fSums;
}

void NonLocalMeans::SaveCurrentHistogram()
{
    if (m_bSaveDebugData) {
        std::ofstream histfile("histogram.csv");

        for (size_t i=0; i<m_nHistSize; i++) {
            histfile<<m_fHistBins[i]<<", ";
            histfile<<m_nHistogram[i]<<", ";
            histfile<<m_fSums[i]<<std::endl;
        }
    }
}

void NonLocalMeans::SaveDebugImage(kipl::base::TImage<float,2> & img, std::string fname)
{
    if (m_bSaveDebugData) {
        kipl::io::WriteTIFF32(img,fname.c_str());
    }
}

void NonLocalMeans::operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g)
{
    // Initialize result image
    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,2> ff(f.Dims());
    kipl::base::TImage<float,2> ff2(f.Dims());

    double sum=0.0;
    double sum2=0.0;

    const size_t N=f.Size();
    for (size_t i=0; i<N; i++) { // Compute the squared pixel values of f as preparation for the L2 norm
        sum+=f[i];
        sum2+=f[i]*f[i];
    }

    double m=sum/static_cast<double>(N);
    double s=sqrt(1.0/(N-1.0)*(sum2-sum*sum/static_cast<double>(N)));
    double datamin=std::numeric_limits<double>::max();
    for (size_t i=0; i<f.Size(); i++) { // Compute the squared pixel values of f as preparation for the L2 norm
        double v=(f[i]-m)/s;
        ff[i]=v;
        datamin= ff[i]<datamin ? ff[i] : datamin;
    }

    for (size_t i=0; i<f.Size(); i++) { // Compute the squared pixel values of f as preparation for the L2 norm
        double v=ff[i];
        ff2[i]=v*v;
    }

    SaveDebugImage(ff2,"normed_sq.tif");

    // Preparing filter kernels
    size_t fdims_x[2]={static_cast<size_t>(m_nBoxSize), 1};
    size_t fdims_y[2]={1, static_cast<size_t>(m_nBoxSize)};

    float *kernel=new float[m_nBoxSize];
    for (int i=0; i<m_nBoxSize; i++)
        kernel[i]=1.0f/static_cast<float>(m_nBoxSize);

    kipl::filters::TFilter<float,2> box_x(kernel,fdims_x);
    kipl::filters::TFilter<float,2> box_y(kernel,fdims_y);

    // Filter the f^2 image in two steps using g as intermediate storage
    g=box_x(f,kipl::filters::FilterBase::EdgeMirror);
    ff=box_y(g,kipl::filters::FilterBase::EdgeMirror);

    // Filter the f^2 image in two steps using g as intermediate storage
    g=box_x(ff2,kipl::filters::FilterBase::EdgeMirror);
    ff2=box_y(g,kipl::filters::FilterBase::EdgeMirror);

    SaveDebugImage(ff,"normed_sq_filtered.tif");
    delete [] kernel;
    g=0.0f;

    // Select non-local means algorithm, most are development versions
    switch (m_eAlgorithm) {
        case NLM_Naive                  : nlm_naive(f.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramOriginal      : nlm_hist_original(f.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_ReducedHistogram       : nlm_hist_single(f.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramParallel      : nlm_hist_threaded(f.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramSum           : nlm_hist_sum_single(f.GetDataPtr(),ff.GetDataPtr(), ff2.GetDataPtr(), g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramSumParallel   : nlm_hist_sum_threaded(f.GetDataPtr(),ff.GetDataPtr(),ff2.GetDataPtr(), g.GetDataPtr(),f.Size()); break;
    }

}

void NonLocalMeans::operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g)
{
    // The 3D version is not yet fully implemented

    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,3> ff(f.Dims());
    size_t fdims[3]={static_cast<size_t>(m_nBoxSize), static_cast<size_t>(m_nBoxSize), static_cast<size_t>(m_nBoxSize)};

    size_t nfilt=m_nBoxSize*m_nBoxSize*m_nBoxSize; // Split into separated kernels
    float *kernel=new float[nfilt];
    for (size_t i=0; i<nfilt; i++) {
       kernel[i]=1.0f;
    }
    kipl::filters::TFilter<float,3> box(kernel,fdims);
    ff=box(f,kipl::filters::FilterBase::EdgeValid);
    delete [] kernel;

    // NL loops
    switch (m_eAlgorithm) {
        case NLM_Naive              : nlm_naive(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramOriginal  : nlm_hist_original(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_ReducedHistogram   : nlm_hist_single(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
        case NLM_HistogramParallel  : nlm_hist_threaded(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
   //     case NLM_HistogramSum       : nlm_hist_sum_single(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
   // case NLM_HistogramSumParallel : nlm_hist_sum_threaded(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size()); break;
    }
}

void NonLocalMeans::nlm_naive(float *f, float *ff, float *g, size_t N)
{
    float *w=new float[N];
    memset(w,0,sizeof(float)*N);

    float q;
    for (size_t i=0; i<N; i++) {
        float wi=w[i];
        float gg=g[i];
        float fi=f[i];
        for (size_t j=i; j<N; j++) {
            q=weight(ff[i],ff[j]); // I forgot the neighborhood...

            gg   += q*f[j];
            g[j] += q*fi;
            w[j] += q;
            wi+=q;
         }
         g[i]=gg/wi;
    }
    delete [] w;
}

vector<pair<double, size_t> > NonLocalMeans::ComputeHistogram(float *data, size_t N)
{
    memset(m_nHistogram,0, m_nHistSize*sizeof(size_t));
    memset(m_fHistBins,0, m_nHistSize*sizeof(double));

    float *hb=new float[m_nHistSize];

    kipl::base::Histogram(data,N,m_nHistogram,m_nHistSize,0.0f,0.0f,hb);

    for (size_t i=0; i<m_nHistSize; i++) {
        m_fHistBins[i]=static_cast<double>(hb[i]);
    }

    vector<pair<double, size_t> > hist;

    // Reduce the histogram length by removing zero bins
    for (size_t i=0; i<m_nHistSize; i++) {
        if (m_nHistogram[i]!=0)
            hist.push_back(make_pair(m_fHistBins[i],m_nHistogram[i]));
    }

    return hist;
}

/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_original(float *f, float *ff, float *g, size_t N)
{
    std::ostringstream msg;

    std::vector<pair<double, size_t> > hist=ComputeHistogram(ff,N);

    float q;
    for (size_t i=0; i<N; i++) {
        float wi=0;
        float gg=0;

        for (size_t j=0; j<m_nHistSize; j++) {
            q   = weight(ff[i],m_fHistBins[j]) * static_cast<float>(m_nHistogram[j]);
            gg += q*m_fHistBins[j];
            wi += q;
         }
         g[i]=gg/wi;
    }
}

/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_single(float *f, float *ff, float *g, size_t N)
{
    m_hist=ComputeHistogram(ff,N);

    nlm_core_hist(f, ff, g, N);
}

/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_threaded(float *f, float *ff, float *g, size_t N)
{
    std::ostringstream msg;

    m_hist=ComputeHistogram(ff,N);

    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();
    msg.str("");
    msg<<"Number of threads: "<<concurentThreadsSupported;
    logger(logger.LogMessage,msg.str());

    std::vector<std::thread> threads;

    size_t M=N/concurentThreadsSupported;
    for(size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        // spawn threads
        threads.push_back(std::thread([=] {nlm_core_hist(f+i*M,ff+i*M,g+i*M,M + (i==concurentThreadsSupported-1)*(N % concurentThreadsSupported)); }));
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

}

void NonLocalMeans::nlm_core_hist(float *f, float *ff, float *g, size_t N)
{
    float q;
    for (size_t i=0; i<N; i++) {
        float wi=0;
        float gg=0;

        for (auto it=m_hist.begin(); it!=m_hist.end(); it++) {
            auto  & bin = *it; // first - bin value, second - bin count
            //q   = weight(f[i],bin.first) * static_cast<float>(bin.second); // Using distance from pixel value, not working...
            q   = weight(ff[i],bin.first) * static_cast<float>(bin.second); // Using distance from local average
            gg += q*bin.first;
            wi += q;
         }
         g[i]=gg/wi;
    }

}

// ------------------------------------------------------------------------
// Second version using the sum of all pixels contributing to a bin

void NonLocalMeans::ComputeHistogramSum(float *f, float *ff, float *ff2, size_t N)
{
    // Reset histogram arrays
    memset(m_nHistogram,0, m_nHistSize*sizeof(size_t));
    memset(m_fHistBins,0, m_nHistSize*sizeof(double));
    memset(m_fSums,0, m_nHistSize*sizeof(double));

    double start = *std::min_element(ff2,ff2+N);
    double stop  = *std::max_element(ff2,ff2+N);
    double scale=(m_nHistSize)/(stop-start);
    size_t idx=0;

    for (size_t i=0; i<N; i++) { // Compute histogram and average value in each bin
        idx=static_cast<size_t>((ff2[i]-start)*scale);
        if (idx<m_nHistSize) {
            m_nHistogram[idx]++;
            m_fSums[idx]+=f[i];
        }
    }

    double iScale=1.0/scale;
    m_fHistBins[0]=start+iScale/2.0;
    for (size_t i=1; i<m_nHistSize; i++) // Compute the bin values
        m_fHistBins[i]=m_fHistBins[i-1]+iScale;

    SaveCurrentHistogram();
}

void NonLocalMeans::ComputeHistogramSum2(float *f, float *ff, float *ff2, size_t N)
{
    // Reset histogram arrays
    m_Histogram.clear();
    HistogramBin bin;
    m_Histogram.resize(m_nHistSize,bin);

    double start = *std::min_element(ff2,ff2+N);
    double stop  = *std::max_element(ff2,ff2+N);
    double scale=(m_nHistSize)/(stop-start);
    size_t idx=0;

    for (size_t i=0; i<N; i++) { // Compute histogram and average value in each bin
        idx=static_cast<size_t>((ff2[i]-start)*scale);
        if (idx<m_nHistSize) {
            m_Histogram[idx].cnt++;
            m_Histogram[idx].sum+=f[i];
            m_Histogram[idx].local_avg+=ff[i];
        }
    }

    double iScale=1.0/scale;
    m_Histogram[0].bin=start+iScale/2.0;
    for (size_t i=1; i<m_nHistSize; i++) {// Compute the bin values
        m_Histogram[i].bin=m_Histogram[i-1].bin+iScale;
        m_Histogram[i].local_avg=m_Histogram[i].local_avg/m_Histogram[i].cnt;
    }

    SaveCurrentHistogram();
}
/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_sum_single(float *f, float *ff, float *ff2, float *g, size_t N)
{
    ComputeHistogramSum(f,ff,ff2,N);
  //  std::cout<<"histogram interval ["<<m_fHistBins[0]<<", "<<m_fHistBins[m_nHistSize-1]<<"] step="<<m_fHistBins[1]-m_fHistBins[0]<<std::endl;

    try {
        nlm_core_hist_sum(f, ff, ff2, g, N);
    }
    catch (kipl::base::KiplException & e) {
        std::cout<<e.what()<<std::endl;
    }
}

/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_sum_threaded(float *f, float *ff, float *ff2, float *g, size_t N)
{
    std::ostringstream msg;

    ComputeHistogramSum(f,ff,ff2,N);

    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();
    msg.str("");
    msg<<"Number of threads: "<<concurentThreadsSupported;
    logger(logger.LogMessage,msg.str());

    std::vector<std::thread> threads;

    size_t M=N/concurentThreadsSupported;
    for(size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        // spawn threads
        threads.push_back(std::thread([=] {nlm_core_hist_sum(f+i*M,ff+i*M,ff2+i*M,g+i*M,M + (i==concurentThreadsSupported-1)*(N % concurentThreadsSupported)); }));
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));
}

void NonLocalMeans::nlm_core_hist_sum(float *f, float *ff, float *ff2, float *g, size_t N)
{
    std::ostringstream msg;

    float q;

    for (size_t i=0; i<N; i++) {
        double wi=0.0f;
        double gg=0.0f;

        for (size_t j=0; j<m_nHistSize; j++) {
            if (m_Histogram[j].bin) {
                q   = weight(ff2[i]-2*ff[i]*m_Histogram[j].local_avg+m_Histogram[j].bin); // Using distance from local average
                gg += q * m_Histogram[j].sum;
                wi += q * m_Histogram[j].cnt ;
             }
        }

        g[i]=static_cast<float>(gg/wi);
        if (isnan(g[i])) {
            msg.str(""); msg<<"g[i] is NaN at i="<<i<<" gg="<<gg<<", wi="<<wi;
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }
    }
}

inline float NonLocalMeans::weight(float a)
{
    return exp(-a*m_fWidth); // m_fWidth is precalculated as the reciprocal
}

inline float NonLocalMeans::weight(float a, float b)
{
    float diff=a+b-2*sqrt(a*b); // corresponds to L2 norm sum((x-y)^2)=sum(x^2)+sum(y^2)-2*sum(x*y)
                              // The cross correlation term is approximated by 2*sum(x*y)
                              // a and b are neighborhood sums of x^2 and y^2

    return exp(-diff*m_fWidth); // m_fWidth is precalculated as the reciprocal
}

}


// Helpers for the enums
std::ostream & operator<<(std::ostream & s, akipl::NonLocalMeans::NLMalgorithms a)
{
    s<<enum2string(a);

    return s;
}

std::string enum2string(akipl::NonLocalMeans::NLMalgorithms a)
{
    std::string s;

    switch (a) {
    case akipl::NonLocalMeans::NLM_Naive :             s="NLM_Naive"; break;
    case akipl::NonLocalMeans::NLM_HistogramOriginal : s="NLM_HistogramOriginal"; break;
    case akipl::NonLocalMeans::NLM_ReducedHistogram  : s="NLM_ReducedHistogram"; break;
    case akipl::NonLocalMeans::NLM_HistogramParallel : s="NLM_HistogramParallel"; break;
    case akipl::NonLocalMeans::NLM_HistogramSum      : s="NLM_HistogramSum"; break;
    default: throw kipl::base::KiplException("Failed to convert NLMalgorithm enum value to string.", __FILE__, __LINE__);
    }
    return s;
}

void string2enum(std::string s, akipl::NonLocalMeans::NLMalgorithms &a)
{
    std::map<std::string, akipl::NonLocalMeans::NLMalgorithms> values;
    values["NLM_Naive"]             = akipl::NonLocalMeans::NLM_Naive;
    values["NLM_HistogramOriginal"] = akipl::NonLocalMeans::NLM_HistogramOriginal;
    values["NLM_ReducedHistogram"]  = akipl::NonLocalMeans::NLM_ReducedHistogram;
    values["NLM_HistogramParallel"] = akipl::NonLocalMeans::NLM_HistogramParallel;
    values["NLM_HistogramSum"]      = akipl::NonLocalMeans::NLM_HistogramSum;

    auto it=values.find(s);

    if (it!=values.end())
        a=it->second;
    else
      throw kipl::base::KiplException("string2enum could not convert string to NLMAlgorithms",__FILE__,__LINE__);
}
