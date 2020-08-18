//<LICENCE>

#ifndef LAPLACIANOFGAUSSIAN_HPP
#define LAPLACIANOFGAUSSIAN_HPP

#include "../filter.h"
#include "../filterbase.h"
#include "../../base/imagecast.h"

namespace kipl { namespace filters {
template <typename T>
kipl::base::TImage<float,2> LaplacianOfGaussian(kipl::base::TImage<T,2> & img, float sigma)
{
    // Full kernel
    // LoG(x,y)=1/s^2 (2-(x^2+y^2)/s^2) exp(-(x^2+y^2)/(2s^2))
    // Separable
    // h(x, y) = (f(x,y)*g_xx(x))*g(y) + (f(x,y)*g_yy(y))*g(x)
    // g(x) = Gauss x
    // g(y) = Gauss y
    // g_xx(x) = 1/s^2 (1-x/s^2) exp(-(x^2+y^2)/(2 s^2))
    kipl::base::TImage<float,2> res(img.dims());

    int N=ceil(sigma*2);
    int N2=2*N+1;
    std::vector<float> klog(N2*N2);
    float s2=1.0f/(sigma*sigma);

    for (int y=-N; y<=N; y++) {
        int yy=(y+N)*N2;
        float y2=y*y;
        for (int x=-N; x<=N; x++) {
            float x2=x*x;
            klog[x+N+yy]=s2 * (2-(x2+y2)*s2)*exp(-(x2+y2)*(0.5*s2));
        }
    }
//    for (int i=1; i<=N; i++) {
//        k[N+i]=exp(-i*i/(2*sigma*sigma));
//        k[N-i]=k[N+i];
//    }
    std::vector<float> d={-1,2,-1};
    std::vector<size_t> LoGDims={size_t(N2), size_t(N2)};
    kipl::filters::TFilter<float,2> LoG(klog,LoGDims);
    kipl::base::TImage<float,2> fimg;
    fimg=kipl::base::ImageCaster<T,float,2>::cast(img);
    res=LoG(fimg,kipl::filters::FilterBase::EdgeMirror);

    return res;
}

}}
#endif // LAPLACIANOFGAUSSIAN_HPP
