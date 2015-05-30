#include "../../include/filters/nonlocalmeans.h"
#include "../../include/filters/filter.h"

namespace akipl {
NonLocalMeans::NonLocalMeans(int k, double h) :
    m_fWidth(h*h),
    m_fWidthLimit(2.65f*h),
    m_nBoxSize(k)
{}

void NonLocalMeans::operator()(kipl::base::TImage<float,2> &f, kipl::base::TImage<float,2> &g)
{
    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,2> ff(f.Dims());
    size_t fdims[2]={m_nBoxSize, m_nBoxSize};

    size_t nfilt=m_nBoxSize*m_nBoxSize;
    float *kernel=new float[nfilt];
    kipl::filters::TFilter<float,2> box(kernel,fdims);
    ff=box(f,kipl::filters::FilterBase::EdgeMirror);
    delete [] kernel;

    // NL loops
    nlm_core(f.GetDataPtr(),ff.GetDataPtr(),g.GetDataPtr(),f.Size());
}

void NonLocalMeans::operator()(kipl::base::TImage<float,3> &f, kipl::base::TImage<float,3> &g)
{
    g.Resize(f.Dims());
    g=0.0f;

    // Box filter
    kipl::base::TImage<float,3> ff(f.Dims());
    size_t fdims[3]={m_nBoxSize, m_nBoxSize, m_nBoxSize};

    size_t nfilt=m_nBoxSize*m_nBoxSize*m_nBoxSize;
    float *kernel=new float[nfilt];
    kipl::filters::TFilter<float,3> box(kernel,fdims);
    ff=box(f,kipl::filters::FilterBase::EdgeMirror);
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
        float gg=0.0f;
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

float NonLocalMeans::weight(float a, float b)
{
    float diff=a-b;

    return (diff<m_fWidthLimit) ? exp(-diff*diff/m_fWidth) : 0.0f;
}

}
