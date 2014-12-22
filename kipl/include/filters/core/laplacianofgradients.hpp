#ifndef LAPLACIANOFGRADIENTS_HPP
#define LAPLACIANOFGRADIENTS_HPP

#include "../filter.h"

namespace kipl { namespace filters {
template <typename T>
kipl::base::TImage<T,2> LaplacianOfGradients(kipl::base::TImage<T,2> & img, float sigma)
{
    // Full kernel
    // LoG(x,y)=1/s^2 (2-(x^2+y^2)/s^2) exp(-(x^2+y^2)/(2s^2))
    // Separable
    // h(x, y) = (f(x,y)*g_xx(x))*g(y) + (f(x,y)*g_yy(y))*g(x)
    // g(x) = Gauss x
    // g(y) = Gauss y
    // g_xx(x) = 1/s^2 (1-x/s^2) exp(-(x^2+y^2)/(2 s^2))
    kipl::base::TImage<T,2> res(img.Dims());

    int N=ceil(sigma*2);
    int N2=2*N+1;
    float *klog=new float[N2*N2];
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
    const float d[3]={-1,2,-1};
    size_t LoGDims[2]={N2, N2};
    kipl::filters::TFilter<T,2> LoG(klog,LoGDims);

    res=LoG(img);
    delete [] klog;
    return res;
}

}}
#endif // LAPLACIANOFGRADIENTS_HPP
