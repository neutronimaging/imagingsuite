#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#include "../../include/filters/nonlocalmeans.h"
#include "../../include/filters/filter.h"
#include "../../include/base/thistogram.h"
#include "../../include/io/io_tiff.h"

namespace akipl {
NonLocalMeans::NonLocalMeans(int k, double h, size_t nBins) :
    logger("NonLocalMeans"),
    m_fWidth(1.0f/(h*h)),
    m_fWidthLimit(2.65f*h),
    m_nBoxSize(k),
    m_nHistSize(nBins),
    m_nHistogram(nullptr),
    m_fHistBins(nullptr)
{
    m_nHistogram = new size_t[m_nHistSize];
    m_fHistBins  = new float[m_nHistSize];
}

NonLocalMeans::~NonLocalMeans()
{
    delete [] m_nHistogram;
    delete [] m_fHistBins;

}

void NonLocalMeans::operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g)
{
    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,2> ff(f.Dims());
    size_t fdims[2]={static_cast<size_t>(m_nBoxSize), static_cast<size_t>(m_nBoxSize)};

    size_t nfilt=m_nBoxSize*m_nBoxSize;
    float *kernel=new float[nfilt];
    for (size_t i=0; i<nfilt; i++)
        kernel[i]=1.0f/static_cast<float>(nfilt);

    kipl::filters::TFilter<float,2> box(kernel,fdims);
    ff=box(f,kipl::filters::FilterBase::EdgeMirror);
    delete [] kernel;


    // NL loops
   // nlm_core(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size());
     nlm_core_hist(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size());
}

void NonLocalMeans::operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g)
{
    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,3> ff(f.Dims());
    size_t fdims[3]={static_cast<size_t>(m_nBoxSize), static_cast<size_t>(m_nBoxSize), static_cast<size_t>(m_nBoxSize)};

    size_t nfilt=m_nBoxSize*m_nBoxSize*m_nBoxSize;
    float *kernel=new float[nfilt];
    for (int i=0; i<nfilt; i++) {
       kernel[i]=1.0f;
    }
    kipl::filters::TFilter<float,3> box(kernel,fdims);
    ff=box(f,kipl::filters::FilterBase::EdgeValid);
    delete [] kernel;

    // NL loops
    nlm_core(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size());
}

void NonLocalMeans::nlm_core(float *f, float *ff, float *g, size_t N)
{
    float *w=new float[N];
    memset(w,0,sizeof(float)*N);

    float q;
    for (size_t i=0; i<N; i++) {
        float wi=w[i];
        float gg=g[i];
        float fi=f[i];
        for (size_t j=i; j<N; j++) {
            q=weight(ff[i],ff[j]);

            gg   += q*f[j];
            g[j] += q*fi;
            w[j] += q;
            wi+=q;
         }
         g[i]=gg/wi;
    }
    delete [] w;
}

vector<pair<float, size_t> > NonLocalMeans::ComputeHistogram(float *data, size_t N)
{
    memset(m_nHistogram,0, m_nHistSize*sizeof(size_t));
    memset(m_fHistBins,0, m_nHistSize*sizeof(float));

    kipl::base::Histogram(data,N,m_nHistogram,m_nHistSize,0.0f,0.0f,m_fHistBins);

    vector<pair<float, size_t> > hist;

    for (size_t i=0; i<m_nHistSize; i++) {
        if (m_nHistogram[i]!=0)
            hist.push_back(make_pair(m_fHistBins[i],m_nHistogram[i]));
    }

    return hist;
}

void NonLocalMeans::nlm_core_hist(float *f, float *ff, float *g, size_t N)
{
    std::ostringstream msg;
    vector<pair<float, size_t> > hist=ComputeHistogram(ff,N);

    msg.str(""); msg<<"Used histogram size = "<<hist.size();
    logger(logger.LogMessage,msg.str());

    float q;
    for (size_t i=0; i<N; i++) {
        float wi=0;
        float gg=0;

        for (auto it=hist.begin(); it!=hist.end(); it++) {
            auto  & bin = *it;
            q   = weight(f[i],bin.first) * static_cast<float>(bin.second);
            gg += q*bin.first;
            wi += q;
         }
         g[i]=gg/wi;
    }
}

float NonLocalMeans::weight(float a, float b)
{
    float diff=a-b;

    return (diff<m_fWidthLimit) ? exp(-diff*diff*m_fWidth) : 0.0f;
}

}
