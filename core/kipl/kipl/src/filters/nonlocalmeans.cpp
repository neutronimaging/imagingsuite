//<LICENCE>

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
#ifndef NO_TIFF
#include "../../include/io/io_tiff.h"
#endif
namespace akipl {
//===========================================================
// Implementation of helper class Histogram bin
HistogramBin::HistogramBin() :
    cnt(0L),
    sum(0.0),
    local_avg(0.0),
    local_avg2(0.0),
    bin(0.0)
{

}

HistogramBin::HistogramBin(const HistogramBin &h) :
    cnt(h.cnt),
    sum(h.sum),
    local_avg(h.local_avg),
    local_avg2(h.local_avg2),
    bin(h.bin)
{

}

HistogramBin & HistogramBin::operator=(const HistogramBin &h)
{
    cnt=h.cnt;
    sum=h.sum;
    bin=h.bin;
    local_avg=h.local_avg;
    local_avg2=h.local_avg2;

    return *this;
}

//===========================================================
// Implementation of Non local means filter
NonLocalMeans::NonLocalMeans(int k, double h, size_t nBins, NLMwindows window, NLMalgorithms algorithm) :
    logger("NonLocalMeans"),
    m_bSaveDebugData(true),
    m_fWidth(1.0f/(h*h)),
    m_fWidthLimit(2.65f*h),
    m_nBoxSize(k),
    m_nHistSize(nBins),
    m_eAlgorithm(algorithm),
    m_eWindow(window),
    m_Kerneltype(kipl::filters::KernelType::Full),
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

void NonLocalMeans::SaveCurrentHistogram2()
{
    if (m_bSaveDebugData) {
        std::ofstream histfile("histogram2.csv");

        for (auto it=m_Histogram.begin(); it!=m_Histogram.end(); it++) {
            histfile<< (it->bin) <<", ";
            histfile<< (it->cnt) <<", ";
            histfile<< (it->sum) <<", ";
            histfile<< (it->local_avg) <<std::endl;
        }
    }
}

void NonLocalMeans::SaveDebugImage(kipl::base::TImage<float,2> & img, std::string fname)
{
    if (m_bSaveDebugData) {
#ifndef NO_TIFF
        kipl::io::WriteTIFF(img,fname,kipl::base::Float32);
#endif
    }
}

void NonLocalMeans::operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g)
{
    // Initialize result image
    g.resize(f.dims());

    kipl::base::TImage<float,2> ff(f.dims());
    kipl::base::TImage<float,2> ff2(f.dims());

    NeighborhoodSums(f,ff,ff2);

    // Select non-local means algorithm, most are development versions
    switch (m_eAlgorithm) {
        case NonLocalMeans::NLMalgorithms::NLM_Naive :
            nlm_naive(f.GetDataPtr(), g.GetDataPtr(),f.Size(0), f.Size());
            break;
        case NonLocalMeans::NLMalgorithms::NLM_HistogramOriginal      :
            nlm_hist_original(f.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size());
            break;
        case NonLocalMeans::NLMalgorithms::NLM_HistogramSum           :
            nlm_hist_sum_single(f.GetDataPtr(),ff.GetDataPtr(), ff2.GetDataPtr(), g.GetDataPtr(),f.Size());
            break;
        case NonLocalMeans::NLMalgorithms::NLM_HistogramSumParallel   :
            nlm_hist_sum_threaded(f.GetDataPtr(),ff.GetDataPtr(),ff2.GetDataPtr(), g.GetDataPtr(),f.Size());
            break;
        case NonLocalMeans::NLMalgorithms::NLM_Bivariate :
            nlm_bivariate(f.GetDataPtr(),ff.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size());
            break;
        case NonLocalMeans::NLMalgorithms::NLM_BivariateParallel :
            nlm_bivariate_threaded(f.GetDataPtr(),ff.GetDataPtr(),ff2.GetDataPtr(),g.GetDataPtr(),f.Size());
            break;
    }

}

void NonLocalMeans::operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g)
{
    // The 3D version is not yet implemented
    throw kipl::base::KiplException("Non-local means for 3D data is not implemented",__FILE__,__LINE__);
}

void NonLocalMeans::NeighborhoodSums(kipl::base::TImage<float,2> &f,
                                     kipl::base::TImage<float,2> &ff,
                                     kipl::base::TImage<float,2> &ff2)
{
    std::ostringstream msg;
    // Box filter
    ff.resize(f.dims());
    ff2.resize(f.dims());
    kipl::base::TImage<float,2> g;

    for (size_t i=0; i<f.Size(); i++) { // Compute the squared pixel values of f as preparation for the L2 norm
        ff2[i]=f[i]*f[i];
    }

    // Preparing filter kernels
    std::vector<size_t> fdims_x={static_cast<size_t>(m_nBoxSize), 1};
    std::vector<size_t> fdims_y={1, static_cast<size_t>(m_nBoxSize)};

    std::vector<float> kernel;
    switch (m_eWindow) {
    case NonLocalMeans::NLMwindows::NLM_window_sum :
        m_Kerneltype=kipl::filters::KernelType::Separable;
        kernel=std::vector<float>(m_nBoxSize,1.0f);
        break;
    case NonLocalMeans::NLMwindows::NLM_window_avg :
        m_Kerneltype=kipl::filters::KernelType::Separable;
        kernel=std::vector<float>(m_nBoxSize,1.0f/static_cast<float>(m_nBoxSize));
        break;
    case NonLocalMeans::NLMwindows::NLM_window_gauss: {
        m_Kerneltype=kipl::filters::KernelType::Separable;
        kernel=std::vector<float>(m_nBoxSize+1);

        int mid=m_nBoxSize/2;
        const float sfactor=sqrt(-log(0.05));
        float sigma=mid/sfactor;

        float sum=0.0f;
        for (int i=0; i<=mid; i++) {
             kernel[mid+i]=exp(-i*i/(2*sigma*sigma));
             sum+=2*kernel[mid+i];
        }
        for (int i=0; i<=mid; i++)
             kernel[mid-i]=kernel[mid+i]=kernel[mid+i]/sum;
        }
        break;
    case NonLocalMeans::NLMwindows::NLM_window_buades : {
            fdims_x[0]=fdims_x[1]=static_cast<size_t>(2*m_nBoxSize+1);
            size_t N=fdims_x[0]*fdims_x[1];
            kernel=std::vector<float>(m_nBoxSize+1);

            int mid=m_nBoxSize;

            float sum=0.0f;
            int idx=0;
            for (int i=-mid; i<=mid; i++) {
                for (int j=-mid; j<=mid; j++, idx++) {
                 if (!j && !i)
                     kernel[idx]=kernel[idx-1];
                 else
                    kernel[idx]=min(mid-abs(i),mid-abs(j));

                 sum+=kernel[idx];
                }
            }
            for (size_t i=0; i<N; i++)
                 kernel[i]/=sum;

            m_Kerneltype=kipl::filters::KernelType::Full;
        }
        break;
    }

//    msg<<"Filter="<<m_eWindow<<", KernelType="<<m_Kerneltype;
//    logger(logger.LogMessage,msg.str());
    if (m_Kerneltype==kipl::filters::KernelType::Full) {
        kipl::filters::TFilter<float,2> filt(kernel,fdims_x);

        // Filter the f image in two steps using g as intermediate storage
       ff=filt(f,kipl::filters::FilterBase::EdgeMirror);

        // Filter the f^2 image in two steps using g as intermediate storage
        g=filt(ff2,kipl::filters::FilterBase::EdgeMirror);
        ff2=g;
    }
    else {
        kipl::filters::TFilter<float,2> box_x(kernel,fdims_x);
        kipl::filters::TFilter<float,2> box_y(kernel,fdims_y);

        // Filter the f^2 image in two steps using g as intermediate storage
        g=box_x(f,kipl::filters::FilterBase::EdgeMirror);
        ff=box_y(g,kipl::filters::FilterBase::EdgeMirror);

        // Filter the f^2 image in two steps using g as intermediate storage
        g=box_x(ff2,kipl::filters::FilterBase::EdgeMirror);
        ff2=box_y(g,kipl::filters::FilterBase::EdgeMirror);
    }

    SaveDebugImage(ff,"ff.tif");
    SaveDebugImage(ff2,"ff2.tif");
}

size_t NonLocalMeans::GetNeighborhood(float *img, float *pNeighborhood, ptrdiff_t pos, ptrdiff_t nLine, ptrdiff_t N)
{
    ptrdiff_t box2=m_nBoxSize/2;
    ptrdiff_t c=0;
    size_t idx=0;
    ptrdiff_t cc=0;
    for (ptrdiff_t y=-box2; y<=box2; y++) {
        c=pos+nLine*y;

        for (ptrdiff_t x=-box2; x<=box2; x++) {
            cc=c+x;
            if ((0<=cc) && (cc<N)) {
                pNeighborhood[idx]=img[cc];
                idx++;
            }

        }
    }

    return idx;
}

float NonLocalMeans::L2Norm(float *a, float *b, size_t N)
{
    float sum=0.0f;
    float diff=0.0f;

    for (int i=0; i<N; i++) {
        diff=a[i]-b[i];
        sum+=diff*diff;
    }

    return sum;
}

void NonLocalMeans::nlm_naive(float *f, float *g, size_t nLine, size_t N)
{
    float *NGi=new float[m_nBoxSize*m_nBoxSize];
    float *NGj=new float[m_nBoxSize*m_nBoxSize];

    float *w=new float[N];
    memset(w,0,sizeof(float)*N);

    float q;
    size_t nNeighborhood=0UL;
    for (size_t i=0; i<N; i++) {
        nNeighborhood=GetNeighborhood(f,NGi,i,nLine,N);
        float wi=w[i];
        float gg=g[i];
        float fi=f[i];
        for (size_t j=i; j<N; j++) {
            GetNeighborhood(f,NGj,j,nLine,N);
            q=weight(L2Norm(NGi,NGj,nNeighborhood)); // I forgot the neighborhood...

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


// ------------------------------------------------------------------------
// Second version using the sum of all pixels contributing to a bin

void NonLocalMeans::ComputeHistogramSum(float *f, float *ff, float *ff2, size_t N)
{
    std::ostringstream msg;
    // Reset histogram arrays
    m_Histogram.clear();
    HistogramBin bin;
    m_Histogram.reserve(m_nHistSize);
    m_Histogram.resize(m_nHistSize);

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
            m_Histogram[idx].local_avg2+=ff2[i];
        }
    }

    double iScale=1.0/scale;
    m_Histogram[0].bin=start+iScale/2.0;
    m_Histogram[0].local_avg=m_Histogram[0].local_avg/m_Histogram[0].cnt;
    m_Histogram[0].local_avg2=m_Histogram[0].local_avg2/m_Histogram[0].cnt;

    for (size_t i=1; i<m_nHistSize; i++) {// Compute the bin values
        m_Histogram[i].bin=m_Histogram[i-1].bin+iScale;
        if (m_Histogram[i].cnt!=0) {
            m_Histogram[i].local_avg=m_Histogram[i].local_avg/m_Histogram[i].cnt;
            m_Histogram[i].local_avg2=m_Histogram[i].local_avg2/m_Histogram[i].cnt;
        }
        else {
            m_Histogram[i].local_avg=0.0;
            m_Histogram[i].local_avg2=0.0;
        }
    }

}

/// \brief Implementation with histogram patching
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_hist_sum_single(float *f, float *ff, float *ff2, float *g, size_t N)
{
    ComputeHistogramSum(f,ff,ff2,N);

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

    throw kipl::base::KiplException("threaded histogram is not implemented",__FILE__,__LINE__);
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
        size_t rest=(i==concurentThreadsSupported-1)*(N % concurentThreadsSupported);
        threads.push_back(std::thread([=] {nlm_core_hist_sum(f+i*M,
                                                             ff+i*M,
                                                             ff2+i*M,
                                                             g+i*M,
                                                             M + rest); }));
    }

    // call join() on each thread in turn this doesn't compile on ubuntu 18.04
//    for_each(threads.begin(), threads.end(),
//        std::mem_fn(&std::thread::join));
}

void NonLocalMeans::nlm_core_hist_sum(float *f, float *ff, float *ff2, float *g, size_t N)
{
    std::ostringstream msg;

    float q;
    float qMax=0.0;
    for (size_t i=0; i<N; i++) {
        double wi=0.0f;
        double gg=0.0f;
        qMax=0.0;
        for (size_t j=0; j<m_nHistSize; j++) {
            if (m_Histogram[j].cnt) {
                q   = weight(ff2[i]-2*ff[i]*m_Histogram[j].local_avg+m_Histogram[j].local_avg2); // Using distance from local average
                gg += q * m_Histogram[j].sum;
                wi += q * m_Histogram[j].cnt ;

                if (qMax<q) {
                    qMax=q;
                }
             }
        }
        gg+=f[i]*qMax;
        wi+=qMax;
        if (wi!=0.0)
            g[i]=static_cast<float>(gg/wi);
        else
            g[i]=f[i];

    }
}

/// \brief Implementation with histogram patching parallelized by c++11 threads
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_bivariate(float *f, float *ff, float *ff2, float *g, size_t N)
{

    m_BivariateHistogram.Initialize(ff,m_nHistSize,ff2,m_nHistSize,N);
    m_BivariateHistogram.AddData(ff,ff2,N);
    m_BivariateHistogram.Write("nl_bivarhist.tif");
    nlm_core_bivariate(f,ff,ff2,g,N);
}

/// \brief Implementation with histogram patching parallelized by c++11 threads
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_bivariate_threaded(float *f, float *ff, float *ff2, float *g, size_t N)
{
    throw kipl::base::KiplException("The threaded algorithm NLM-bivariate is not implemented");
}

/// \brief Implementation with histogram patching, core algorithm
/// \param f pointer to the original image
/// \param ff pointer to the filtered image
/// \param g pointer to the result image
/// \param N number of pixels
void NonLocalMeans::nlm_core_bivariate(float *f, float *ff, float *ff2, float *g, size_t N)
{
    std::ostringstream msg;

    float q;
    float qMax=0.0;
    kipl::base::BivariateHistogram::BinInfo bi,bi_lower,bi_upper;

    for (size_t i=0; i<N; i++) {
        double wi=0.0f;
        double gg=0.0f;
        qMax=0.0;
        bi_lower=m_BivariateHistogram.GetBin(ff[i]-m_fWidthLimit,ff2[i]-m_fWidthLimit*m_fWidthLimit);
        bi_upper=m_BivariateHistogram.GetBin(ff[i]+m_fWidthLimit,ff2[i]+m_fWidthLimit*m_fWidthLimit);

        for (int j=0; j<static_cast<int>(m_nHistSize); j++) { // scan the histogram
            for (int k=bi_lower.idxA; k<bi_upper.idxA; k++) {
                bi=m_BivariateHistogram.GetBin(k,j); // get each hist bin
                if (bi.count!=0L) {
                    q   = weight(ff2[i]-2*ff[i]*bi.binA+bi.binB); // Using distance from local average
                    gg += q * bi.count*bi.binA; // This should be the sum for the bin
                    wi += q * bi.count;

                    if (qMax<q) {
                        qMax=q;
                    }
                }
            }
        }
        gg+=f[i]*qMax;
        wi+=qMax;
        if (wi!=0.0)
            g[i]=static_cast<float>(gg/wi);
        else
            g[i]=f[i];

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
    case akipl::NonLocalMeans::NLMalgorithms::NLM_Naive                : s="NLM_Naive"; break;
    case akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramOriginal    : s="NLM_HistogramOriginal"; break;
    case akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramSum         : s="NLM_HistogramSum"; break;
    case akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramSumParallel : s="NLM_HistogramSumParallel"; break;
    case akipl::NonLocalMeans::NLMalgorithms::NLM_Bivariate            : s="NLM_Bivariate"; break;
    case akipl::NonLocalMeans::NLMalgorithms::NLM_BivariateParallel    : s="NLM_BivariateParallel"; break;
    default: throw kipl::base::KiplException("Failed to convert NLMalgorithm enum value to string.", __FILE__, __LINE__);
    }
    return s;
}

void string2enum(std::string s, akipl::NonLocalMeans::NLMalgorithms &a)
{
    std::map<std::string, akipl::NonLocalMeans::NLMalgorithms> values;
    values["NLM_Naive"]                     = akipl::NonLocalMeans::NLMalgorithms::NLM_Naive;
    values["NLM_HistogramOriginal"]         = akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramOriginal;
    values["NLM_HistogramSum"]              = akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramSum;
    values["NLM_HistogramSumParallel"]      = akipl::NonLocalMeans::NLMalgorithms::NLM_HistogramSumParallel;
    values["NLM_Bivariate"]              = akipl::NonLocalMeans::NLMalgorithms::NLM_Bivariate;
    values["NLM_BivariateParallel"]              = akipl::NonLocalMeans::NLMalgorithms::NLM_BivariateParallel;

    auto it=values.find(s);

    if (it!=values.end())
        a=it->second;
    else
      throw kipl::base::KiplException("string2enum could not convert string to NLMAlgorithms",__FILE__,__LINE__);
}


std::ostream & operator<<(std::ostream & s, akipl::NonLocalMeans::NLMwindows w)
{
    s<<enum2string(w);

    return s;
}

std::string enum2string(akipl::NonLocalMeans::NLMwindows w)
{
    std::string s;

    switch (w) {
    case akipl::NonLocalMeans::NLMwindows::NLM_window_sum           : s="NLM_window_sum"; break;
    case akipl::NonLocalMeans::NLMwindows::NLM_window_avg           : s="NLM_window_avg"; break;
    case akipl::NonLocalMeans::NLMwindows::NLM_window_gauss         : s="NLM_window_gauss"; break;
    case akipl::NonLocalMeans::NLMwindows::NLM_window_buades        : s="NLM_window_buades"; break;

    default: throw kipl::base::KiplException("Failed to convert NLMwindows enum value to string.", __FILE__, __LINE__);
    }
    return s;
}

void string2enum(std::string s, akipl::NonLocalMeans::NLMwindows &w)
{
    std::map<std::string, akipl::NonLocalMeans::NLMwindows> values;
    values["NLM_window_sum"]    = akipl::NonLocalMeans::NLMwindows::NLM_window_sum;
    values["NLM_window_avg"]    = akipl::NonLocalMeans::NLMwindows::NLM_window_avg;
    values["NLM_window_gauss"]  = akipl::NonLocalMeans::NLMwindows::NLM_window_gauss;
    values["NLM_window_buades"]  = akipl::NonLocalMeans::NLMwindows::NLM_window_buades;

    auto it=values.find(s);

    if (it!=values.end())
        w=it->second;
    else
      throw kipl::base::KiplException("string2enum could not convert string to NLMWindows",__FILE__,__LINE__);
}
