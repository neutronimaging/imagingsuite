//<LICENCE>

#ifndef GRADIENTGUIDEDTHRESHOLD_HPP
#define GRADIENTGUIDEDTHRESHOLD_HPP

#include "../thresholds.h"
#include "../../filters/filter.h"
#include "../../morphology/pixeliterator.h"
#include "../gradientguidedthreshold.h"

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
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(const std::set<T0> & val)
{
    m_threshold.clear();
    m_threshold(val.begin(),val.end());
    std::sort(m_threshold.begin(),m_threshold.end());
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(const std::vector<T0> &val)
{
    m_threshold=val;
    std::sort(m_threshold.begin(),m_threshold.end());
}

template <typename T0, typename T1,size_t N>
void gradientGuidedThreshold<T0,T1,N>::setDataThreshold(T0 *val, size_t /*Nelements*/)
{
    m_threshold.clear();

    for (size_t i=0; i<N; ++i)
        m_threshold.push_back(val[i]);

    std::sort(m_threshold.begin(),m_threshold.end());
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
void gradientGuidedThreshold<T0,T1,N>::setParameters(std::map<std::string,std::string> /*parameters*/)
{

}

template <typename T0, typename T1,size_t N>
int gradientGuidedThreshold<T0,T1,N>::operator()(kipl::base::TImage<T0,N> & img, kipl::base::TImage<T1,N> &seg)
{
    seg.resize(img.dims());

    kipl::segmentation::MultiThreshold(img,seg,m_threshold);

    std::vector<size_t> dims(2,m_filterWidth);

    int Nf=dims[0]*dims[1];

    float w=1.0f/Nf;

    std::vector<T0> k(Nf,w);

    kipl::filters::TFilter<float,N> box(k,dims);
    std::vector<size_t> gdims={3,3};

    std::vector<float> gx={-3.0f, 0.0f, 3.0f, -10.0f, 0.0f, 10.0f, -3.0f, 0.0f, 3.0f };
    std::vector<float> gy={-3.0f, -10.0f, -3.0f, 0.0f, 0.0f, 0.0f, 3.0f, 10.0f, 3.0f };
    kipl::filters::TFilter<float,N> fgx(gx,gdims);
    kipl::filters::TFilter<float,N> fgy(gy,gdims);

    kipl::base::TImage<float,2> smooth=box(img,kipl::filters::FilterBase::EdgeMirror);
    kipl::base::TImage<float,2> dx=fgx(smooth,kipl::filters::FilterBase::EdgeMirror);
    kipl::base::TImage<float,2> dy=fgy(smooth,kipl::filters::FilterBase::EdgeMirror);

    std::set<ptrdiff_t> edgelist0,edgelist1;

    kipl::base::PixelIterator pit(img.dims(), kipl::base::conn8);

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

            for (size_t j=0; j<pit.neighborhoodSize(); ++j)
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

    while (!edge0.empty())
    {
        edge1.clear();
        for (auto it=edge0.begin(); it!=edge0.end(); ++it)
        {
            pit.setPosition(*it);

            for (size_t j=0; j<pit.neighborhoodSize(); ++j)
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

        std::swap(edge0,edge1);
    }
    return 0;
}

}}

#endif // GRADIENTGUIDEDTHRESHOLD_HPP
