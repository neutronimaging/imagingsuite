#ifndef MORPHGEO2_H
#define MORPHGEO2_H
#include "morphfilters.h"

namespace kipl { namespace morphology2 {

/// \brief Performs a Reconstruction by dilation of the image (\f$R^{\delta}_g(f) \f$)
///	\param g mask image
///	\param f marker image
///	\param conn Selects connectivity
///
///	\note \f$f \leq g \f$
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
///	\todo Fix edge processing 3D
template <typename ImgType,size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByDilation(const kipl::base::TImage<ImgType,NDimG> &g,
        const kipl::base::TImage<ImgType,NDimF> &f,
        kipl::morphology::MorphConnect conn)
{
    std::ostringstream msg;

    kipl::base::TImage<ImgType,NDimG> temp;
    deque<int> fifo;

    size_t i;

    const size_t *pDimsF=f.Dims();
    const size_t *pDimsG=g.Dims();

    if (NDimF!=2)
        throw kipl::base::KiplException("RecByDilation: Only 2D is supported for the moment",__FILE__,__LINE__);

    if (NDimG!=NDimF) {
        cerr<<"Dim(f)!=Dim(g)"<<endl;

        if ((NDimG!=3) || (NDimF!=2)) {
            throw kipl::base::KiplException("RecByDilation: Dim(f)!=Dim(g) or Dim(g)!=2 and Dim(f)!=3",__FILE__,__LINE__);
        }
    }

    for (i=0; i<NDimF; i++) {
        if (pDimsF[i]!=pDimsG[i]) {
            throw kipl::base::KiplException("RecByDilation: Size(f) != Size(g)",__FILE__,__LINE__);
        }
    }

    temp.Resize(g.Dims());
    temp=static_cast<ImgType>(0);

    ImgType const * const pg=g.GetDataPtr();
    ImgType const * const pf=f.GetDataPtr();
    ImgType *ptemp=temp.GetDataPtr();
    size_t errcnt=0L;
    for (i=0; i<f.Size(); i++) {
        if (pg[i]<pf[i]) {
            errcnt++;
        }
        ptemp[i]=pf[i];
    }
    if (errcnt) {
        msg.str("");
        msg<<"Error RecByDilation: f>g ("<<errcnt<<" times)";
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    // Set up indexing arrays
    ptrdiff_t cNGpm,cNG;

    long ipos;
    ptrdiff_t pos;
    ImgType max;
    const ptrdiff_t N=g.Size();
    CNeighborhood NG(g.Dims(),NDimG,conn);
    // Forward scan
    cNGpm=NG.Nfb();
    cNG=NG.N();
    for (pos=0; pos< f.Size(); pos++) {
        max=ptemp[pos];
        for (i=1; i<cNGpm; i++) {
            if ((ipos=NG.backward(pos,i))!=-1)
                if (ptemp[ipos]>max) max=pf[ipos];
        }
        ptemp[pos]=max <pg[pos] ? max : pg[pos];
    }

    // Backward scan
    ImgType tmppos;
    for (pos=f.Size()-1; pos>=0; pos--) {
        max=ptemp[pos];
        tmppos=max;
        for (i=1; i<cNGpm; i++) {
            if ((ipos=NG.forward(pos,i))!=-1)
                if (ptemp[ipos]>max) max=ptemp[ipos];
        }
        ptemp[pos]=max<pg[pos] ? max : pg[pos];

        tmppos=ptemp[pos];
        for (i=1; i<cNGpm; i++) {
            if((ipos=NG.forward(pos,i))!=-1) {
                if ((ptemp[ipos]<tmppos) && (ptemp[ipos]<pg[ipos])) {
                    fifo.push_front(pos);//fifo.push_back(pos);
                    break;
                }
            }
        }
    }

    // Fifo scan
    while (!fifo.empty()) {
        pos=fifo.front();
        fifo.pop_front();
        tmppos=ptemp[pos];

        for (i=0; i<cNG; i++) {
            if ((ipos=NG.neighbor(pos,i))!=-1) {
                if (static_cast<size_t>(ipos)<N) {
                    if ((ptemp[ipos]<tmppos) && (pg[ipos]!=ptemp[ipos])) {
                        ptemp[ipos]=tmppos<pg[ipos] ? tmppos : pg[ipos];
                        fifo.push_front(ipos);//fifo.push_back(ipos);
                    }
                }
                else {
                    if ((ptemp[ipos]<tmppos) && (ptemp[ipos])!=0) {
                        ptemp[ipos]=tmppos<0 ? tmppos : 0;
                        fifo.push_front(ipos);
                    }
                }
            }
        }
    }

    return temp;
}
}
#endif // MORPHGEO2_H
