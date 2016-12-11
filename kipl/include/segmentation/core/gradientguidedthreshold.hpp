#ifndef GRADIENTGUIDEDTHRESHOLD_HPP
#define GRADIENTGUIDEDTHRESHOLD_HPP

#include "../thresholds.h"
#include "../../filters/filter.h"
#include "../../morphology/pixeliterator.h"

namespace kipl { namespace segmentation {

template <typename T0, typename T1,size_t N>
gradientGuidedThreshold<T0,T1,N>::gradientGuidedThreshold() :
    SegmentationBase<T0,T1,N>("GradientGuidedThreshold"),
    m_edgeThreshold(0.1f),
    m_filterWidth(1.0f)
{}

template <typename T0, typename T1,size_t N>
gradientGuidedThreshold<T0,T1,N>::~gradientGuidedThreshold()
{}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(T0 val)
{
    m_threshold.insert(val);
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(std::set<T0> val)
{
    m_threshold.clear();
    m_threshold=val;
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(std::vector<T0> val)
{
    m_threshold.clear();
    std::copy(val.begin(),val.end(),val.begin());
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(T0 *val, size_t Nelements)
{
    m_threshold.clear();

    for (size_t i=0; i<N; ++i)
        m_threshold.insert(val[i]);
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setEdgeThreshold(float val)
{
    m_edgeThreshold=val;
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setFilterWidth(int w)
{
    m_filterWidth=w;
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setParameters(std::map<std::string,std::string> parameters)
{

}

template <typename T0, typename T1,size_t N>
int gradientGuidedThreshold<T0,T1,N>::operator()(kipl::base::TImage<T0,N> & img, kipl::base::TImage<T1,N> &seg)
{
    seg.Resize(img.Dims());

    T0 *th=new T0[m_threshold.size()];

    copy(m_threshold.begin(),m_threshold.end(),th);
    kipl::segmentation::MultiThreshold(img,seg,th,m_threshold.size());
    delete [] th;

    size_t dims[2];
    dims[0]=dims[1]=m_filterWidth;
    int Nf=dims[0]*dims[1];
    float *k=new T0[Nf];
    float w=1.0f/Nf;

    for (int i=0; i<Nf; ++i) {
        k[i]=w;
    }

    kipl::filters::TFilter<float,N> box(k,dims);
    size_t gdims[2]={3,3};

    float gx[9]={-3, 0, 3, -10, 0, 10, -3, 0, 3 };
    float gy[9]={-3, -10, -3, 0, 0, 0, 3, 10, 3 };
    kipl::filters::TFilter<float,N> fgx(gx,gdims);
    kipl::filters::TFilter<float,N> fgy(gy,gdims);

    kipl::base::TImage<float,2> smooth=box(img,kipl::filters::FilterBase::EdgeMirror);
    kipl::base::TImage<float,2> dx=fgx(smooth,kipl::filters::FilterBase::EdgeMirror);
    kipl::base::TImage<float,2> dy=fgy(smooth,kipl::filters::FilterBase::EdgeMirror);

    std::set<ptrdiff_t> edgelist0,edgelist1;

    kipl::base::PixelIterator pit(img.Dims(), kipl::base::conn8);

    T1 edgeLabel = m_threshold.size()+1;

    ptrdiff_t pos=0;
    for (size_t i=0; i<img.Size(); i++)
    {
        float absgrad=sqrt(dx[i]*dx[i]+dy[i]*dy[i]);
        if (m_edgeThreshold<absgrad)
            seg[i]=edgeLabel;
    }

    // Locate the edge pixels
    for (size_t i=0; i<seg.Size(); ++i)
    {
        if (seg[i]==edgeLabel)
        {
            pit.setPosition(i);

            for (int j=0; j<pit.neighborhoodSize(); ++j)
            {
                pos=pit.neighborhood(j);
                if (seg[pos]!=edgeLabel) {
                    edgelist0.insert(pos);
                    break;
                }
            }
        }
    }

    // Grow from the edge
    std::set<ptrdiff_t> &edge0=edgelist0;
    std::set<ptrdiff_t> &edge1=edgelist1;
    std::set<ptrdiff_t> &edgetmp=edgelist0;
  //  cout<<"Entering with "<<edge0.size()<<endl;
    while (!edge0.empty())
    {
        edge1.clear();
        for (auto it=edge0.begin(); it!=edge0.end(); ++it)
        {
            pit.setPosition(*it);

            for (int j=0; j<pit.neighborhoodSize(); ++j)
            {
                pos=pit.neighborhood(j);
                if (seg[pos]!=edgeLabel) { // todo Make this a majority voter
                    seg[*it]=seg[pos];
                }
                else {
                    edge1.insert(pos);
                }
            }
        }
//            cout<<"Iterated with e0="<<edge0.size()<<", e1="<<edge1.size()<<endl;
        std::swap(edge0,edge1);


    }
}

}}

#endif // GRADIENTGUIDEDTHRESHOLD_HPP
