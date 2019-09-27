#ifndef MORPHGEO_HPP
#define MORPHGEO_HPP

#include <stddef.h>
#include <sstream>
#include <deque>
#include "../../base/kiplenums.h"
#include "../../base/imageoperators.h"
#include "../morphology.h"
#include "../morphfilters.h"
#include "../pixeliterator.h"

#include <QDebug>
using namespace std;

namespace kipl { namespace morphology {

/// \brief Performs a Self dual Geodesic Reconstruction of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	if \f$f\leq g\f$ will reconstruction by erosion \f$R^{\epsilon}_f\f$ be performed, if
///	\f$g\leq f\f$ will reconstruction by dilation \f$R^{\delta}_f\f$ be performed
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> SelfDualReconstruction(kipl::base::TImage<ImgType,NDim> &f,
        kipl::base::TImage<ImgType,NDim> &g,
        base::eConnectivity conn)
{
    kipl::base::TImage<ImgType,NDim> temp(f);
    deque<int> fifo;

    ptrdiff_t i;

    const size_t *pDimsF=f.Dims();
    const size_t *pDimsG=g.Dims();

    for (i=0; i<NDim; i++)
    {
        if (pDimsF[i]!=pDimsG[i])
        {
            throw kipl::base::KiplException("SelfDualReconstruction: Size(f) != Size(g)",__FILE__,__LINE__);
        }
    }

    ImgType *pF=temp.GetDataPtr();
    ImgType *pG=g.GetDataPtr();
    // Set up indexing arrays

    kipl::base::PixelIterator NG(f.Dims(), conn);
    ptrdiff_t ipos;
    ptrdiff_t pos;
    ImgType maxVal,minVal;
    const ptrdiff_t N=static_cast<ptrdiff_t>(f.Size());
    // Forward scan
    NG.setPosition(0L);
    for (pos=0; pos< N; ++pos, ++NG)
    {
        if (pF[pos]<pG[pos])
        {
            maxVal=pF[pos];
            // Compute max(f(q)) q in ng_backw(p) U p
            for (const auto & neighborPix : NG.backwardNeighborhood())
            {
                ipos = pos + neighborPix;
                if (pF[ipos]>maxVal) maxVal=pF[ipos];
            }
            pF[pos]=maxVal <pG[pos] ? maxVal : pG[pos];
        }
        else
        {
            minVal=pF[pos];
            // Compute min(f(q)) q in ng_backw(p) U p
            for (const auto & neighborPix : NG.backwardNeighborhood())
            {
                ipos = pos + neighborPix;
                if (pF[ipos]<minVal) minVal=pF[ipos];
            }
            pF[pos]=minVal >pG[pos] ? minVal : pG[pos];
        }
    }

    //Backward scan
    NG.setPosition(N-1);
    for (pos=N-1; pos>=0; --pos, --NG) {
        if (pF[pos]<pG[pos])
        {
            maxVal=pF[pos];

            for (const auto & neighborPix : NG.forwardNeighborhood())
            {
                ipos = pos + neighborPix;
                if (pF[ipos]>maxVal) maxVal=pF[ipos];
            }
            pF[pos]=maxVal <pG[pos] ? maxVal : pG[pos];
        }
        else {
            minVal=pF[pos];
            for (const auto & neighborPix : NG.forwardNeighborhood())
            {
                ipos = pos + neighborPix;
                if (pF[ipos]<minVal) minVal=pF[ipos];
            }
            pF[pos]=minVal >pG[pos] ? minVal : pG[pos];
        }

        for (const auto & neighborPix : NG.forwardNeighborhood())
        {
            ipos = pos + neighborPix;
            if ((pF[ipos]<pF[pos]) && (pF[ipos]<pG[ipos]))
            {
                fifo.push_back(pos);
                break;
            }
            else
            {
                if ((pF[ipos]>pF[pos]) && (pF[ipos]>pG[ipos]))
                {
                    fifo.push_back(pos);
                    break;
                }
            }
        }
    }

    while (!fifo.empty()) {
        pos=fifo.front();
        fifo.pop_front();
        NG.setPosition(pos);
        for (const auto & neighborPix : NG.neighborhood())
        {
            ipos = pos + neighborPix;
            if (pF[ipos]<pG[pos])
            {
                if ((pF[ipos]<pF[pos]) && (pG[ipos]!=pF[ipos]))
                {
                    pF[ipos]=std::min(pF[pos],pG[ipos]);
                    fifo.push_back(ipos);
                }
            }
            else
            {
                if ((pF[ipos]<pF[pos]) && (pG[ipos]!=pF[ipos]))
                {
                    pF[ipos]=std::max(pF[pos],pG[ipos]);
                    fifo.push_back(ipos);
                }
            }
        }
    }
    return temp;
}

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
        kipl::base::eConnectivity conn)
{
    std::ostringstream msg;

    kipl::base::TImage<ImgType,NDimG> temp;
    deque<ptrdiff_t> fifo;

    ptrdiff_t i;

    const size_t *pDimsF=f.Dims();
    const size_t *pDimsG=g.Dims();

    if (NDimG!=NDimF)
    {
        std::cerr<<"Dim(f)!=Dim(g)"<<endl;

        if ((NDimG!=3) || (NDimF!=2))
        {
            throw kipl::base::KiplException("RecByDilation: Dim(f)!=Dim(g) or Dim(g)!=2 and Dim(f)!=3",__FILE__,__LINE__);
        }
    }

    for (i=0; i<static_cast<ptrdiff_t>(NDimF); i++)
    {
        if (pDimsF[i]!=pDimsG[i])
        {
            throw kipl::base::KiplException("RecByDilation: Size(f) != Size(g)",__FILE__,__LINE__);
        }
    }


    temp.Resize(g.Dims());
    temp=static_cast<ImgType>(0);
    ImgType const * const pg=g.GetDataPtr();
    ImgType const * const pf=f.GetDataPtr();
    ImgType *ptemp=temp.GetDataPtr();
    size_t errcnt=0L;
    for (i=0; i<static_cast<ptrdiff_t>(f.Size()); i++)
    {
        if (pg[i]<pf[i]) {
            errcnt++;
        }
        ptemp[i]=pf[i];
    }
    if (errcnt)
    {
        msg.str("");
        msg<<"Error RecByDilation: f>g ("<<errcnt<<" times)";
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    // Set up indexing arrays
    ptrdiff_t ipos;
    ptrdiff_t pos;
    ImgType max;
    const ptrdiff_t N=g.Size();
    kipl::base::PixelIterator NG(f.Dims(),conn);
    // Forward scan
    NG.setPosition(0L);
    for (pos=0; pos< static_cast<ptrdiff_t>(f.Size()); ++pos, ++NG)
    {
        max=ptemp[pos];

        for (const auto & neighborPix : NG.backwardNeighborhood())
        {
            ipos = pos + neighborPix ;
            if (ptemp[ipos]>max) max=pf[ipos];
        }
        ptemp[pos]=max <pg[pos] ? max : pg[pos];
    }
    // Backward scan
    ImgType tmppos;
    NG.setPosition(f.Size()-1L);
    for (pos=static_cast<ptrdiff_t>(f.Size())-1; pos>=0; --pos,--NG)
    {
        max=ptemp[pos];
        tmppos=max;
        for (const auto & neighborPix : NG.forwardNeighborhood())
        {
            ipos = pos + neighborPix;
            if (ptemp[ipos]>max) max=ptemp[ipos];
        }

        ptemp[pos]=max<pg[pos] ? max : pg[pos];

        tmppos=ptemp[pos];
        for (const auto & neighborPix : NG.forwardNeighborhood())
        {
            ipos = pos + neighborPix;
            if ((ptemp[ipos]<tmppos) && (ptemp[ipos]<pg[ipos]))
            {
                fifo.push_front(pos);//fifo.push_back(pos);
                break;
            }
        }
    }

    // Fifo scan
    while (!fifo.empty())
    {
        pos=fifo.front();
        fifo.pop_front();
        tmppos=ptemp[pos];
        NG.setPosition(pos);
        for (const auto & neighborPix : NG.neighborhood())
        {
            ipos=pos+neighborPix;

            if (static_cast<size_t>(ipos)<N)
            {
                if ((ptemp[ipos]<tmppos) && (pg[ipos]!=ptemp[ipos]))
                {
                    ptemp[ipos]=tmppos<pg[ipos] ? tmppos : pg[ipos];
                    fifo.push_front(ipos);//fifo.push_back(ipos);
                }
            }
            else
            {
                if ((ptemp[ipos]<tmppos) && (ptemp[ipos])!=0)
                {
                    ptemp[ipos]=tmppos<0 ? tmppos : 0;
                    fifo.push_front(ipos);
                }
            }
        }
    }

    return temp;
}

/// \brief Performs a Reconstruction by erosion of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	\note \f$g \leq f \f$
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByErosion(const kipl::base::TImage<ImgType,NDimG> &g,
        const kipl::base::TImage<ImgType,NDimF> &f,
        kipl::base::eConnectivity conn)
{
    std::ostringstream msg;
    kipl::base::TImage<ImgType,NDimG> temp;
    std::deque<ptrdiff_t> fifo;

    size_t i;

    size_t const * const pDimsF=f.Dims();
    size_t const * const pDimsG=g.Dims();
    if (NDimG!=NDimF) {
        throw kipl::base::KiplException("RecByErosion: NDimG!=NDimF",__FILE__,__LINE__);

        if ((NDimG!=3 || (NDimF!=2))) {
            throw kipl::base::KiplException("RecByDilation: Dim(f)!=Dim(g) or Dim(g)!=2 and Dim(f)!=3",__FILE__,__LINE__);
        }
    }
    else
        temp.Clone(f);
        //f.ChangeDimension(temp);


    for (i=0; i<NDimF; i++) {
        if (pDimsF[i]!=pDimsG[i]) {
            throw kipl::base::KiplException("RecByErosion: size(f)!=size(g)",__FILE__,__LINE__);
        }
    }

    ImgType const * const pg=g.GetDataPtr();
    ImgType * pf=temp.GetDataPtr();

    size_t errcnt=0L;
    for (i=0; i<static_cast<ptrdiff_t>(f.Size()); i++)
    {
        if (pf[i]<pg[i])
        {
            errcnt++;
        }
    }

    if (errcnt)
    {
        msg.str("");
        msg<<"Error RecByErosion: g>f ("<<errcnt<<" times)";
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    // Set up indexing arrays
    ptrdiff_t ipos;
    ptrdiff_t pos;
    ImgType minVal;
    ptrdiff_t N=static_cast<ptrdiff_t>(g.Size());
    kipl::base::PixelIterator NG(f.Dims(),conn);

    // Forward scan
    NG.setPosition(0L);
    for (pos=0; pos< N; ++pos, ++NG)
    {
        minVal=pf[pos];

        for (const auto & neighborPix : NG.backwardNeighborhood()) //min(f(q)| q in NGp(p) u p)
        {
            ipos = pos + neighborPix;
            if (pf[ipos]<minVal) minVal=pf[ipos];
        }

        pf[pos]=minVal > pg[pos] ? minVal : pg[pos]; // max(min(f(NGp(p))),g(p))
    }


    // Backward scan
    ImgType tmppos;
    NG.setPosition(f.Size()-1L);
    for (pos=N-1; pos>=0; --pos, --NG) {
        minVal=pf[pos];
        tmppos=minVal;
        for (const auto & neighborPix : NG.forwardNeighborhood())
        {
        //for (i=1; i<cNGpm; i++) {// min(f(q)| q in NGp(p) u p)
            ipos =  pos + neighborPix;
            if (pf[ipos]<minVal) minVal=pf[ipos];
        }

        pf[pos]=minVal > pg[pos] ? minVal : pg[pos]; // max(min(f(NGp(p))),g(p))

        tmppos=pf[pos];

        for (const auto & neighborPix : NG.forwardNeighborhood())
        {
            ipos = pos + neighborPix;
            if ((pf[ipos]>tmppos) && (pf[ipos]>pg[ipos]))
            {
                fifo.push_front(pos);
                break;
            }

        }

    }

    while (!fifo.empty()) {
        pos=fifo.front();
        fifo.pop_front();
        tmppos=pf[pos];

        NG.setPosition(pos);
        for (const auto & neighborPix : NG.neighborhood())
        {
            ipos = pos + neighborPix;
            if ((pf[ipos]>tmppos) && (pg[ipos]!=pf[ipos]))
            {
                pf[ipos]=tmppos>pg[ipos] ? tmppos : pg[ipos];
                fifo.push_front(ipos);
            }
        }
    }

    return temp;
}

/// \brief Removes the objects that are connected to the edge of the image
///	\param img Input image
///	\param conn Connectivity of the reconstruction
///
///	Computes \f$img-R^{\delta}_{img}(fm)\f$ where the border of fm is equal
///	to the border of img while the rest of fm is equal to 0
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> RemoveEdgeObj(kipl::base::TImage<ImgType,NDim> &img,
        kipl::base::eConnectivity conn)
{
    size_t const *const dims=img.Dims();
    kipl::base::TImage<ImgType,NDim> edge(dims);
    edge=kipl::base::min(img);

    size_t y,z;
    ImgType *pLineEdge, *pLineImg;
    for (z=0; z<dims[2]; z++) {
        memcpy(edge.GetLinePtr(0,z),img.GetLinePtr(0,z),dims[0]*sizeof(ImgType));
        memcpy(edge.GetLinePtr(dims[1]-1,z),img.GetLinePtr(dims[1]-1,z),dims[0]*sizeof(ImgType));
        for (y=1; y<dims[1]-1; y++) {
            pLineEdge=edge.GetLinePtr(y,z);
            pLineImg=img.GetLinePtr(y,z);
            pLineEdge[0]=pLineImg[0];
            pLineEdge[dims[0]-1]=pLineImg[dims[0]-1];

        }
    }
    if (NDim==3) {
        memcpy(edge.GetDataPtr(),img.GetDataPtr(),dims[0]*dims[1]*sizeof(ImgType));
        memcpy(edge.GetLinePtr(0,dims[2]-1),img.GetLinePtr(0,dims[2]-1),dims[0]*dims[1]*sizeof(ImgType));
    }

    edge=img-kipl::morphology::RecByDilation(img,edge,conn);

    return edge;

}
/*
/// \brief Opens the image by reconstruction
///	\param img Input image
///	\param er The erosion operator
///	\param conn Connectivity selector
///
///	Computes \f$R^{\delta}_{img}(\epsilon(img))\f$
template <typename ImgType>
    kipl::base::TImage<ImgType,2> OpenByRec(kipl::base::TImage<ImgType,2> &img,
            CGrayErode<ImgType,2> &er,
            kipl::base::MorphConnect conn=kipl::morphology::conn4)
{
    kipl::base::TImage<ImgType,2> tmp;

    tmp=RecByDilation(img,er(img),conn);

    return tmp;
}
*/

/// \brief Closes the image by reconstruction
///	\param img Input image
///	\param di The dilation operator
///	\param conn Connectivity selector
///
///	Computes \f$R^{\epsilon}_{img}(\delta(img))\f$
template <typename ImgType,size_t NDim>
kipl::base::TImage<ImgType,NDim> CloseByRec(kipl::base::TImage<ImgType,NDim> &img,
            TDilate<ImgType,NDim> &di,
            kipl::morphology::MorphConnect conn=NDim==2 ? kipl::morphology::conn4 : kipl::morphology::conn6)
{
    kipl::base::TImage<ImgType,NDim> tmp;
    tmp=di(img);
    tmp=RecByErosion(img,tmp,conn);

    return tmp;
}
/*
/// \brief Closes the image by reconstruction
///	\param img Input image
///	\param di The dilation operator
///	\param conn Connectivity selector
///
///	Computes \f$R^{\epsilon}_{img}(\delta(img))\f$
template <typename ImgType, size_t NDim>
    kipl::base::TImage<ImgType,NDim> GrayCloseByRec(kipl::base::TImage<ImgType,NDim> &img,
            TGrayDilate<ImgType,NDim> &di,
            kipl::morphology::MorphConnect conn=NDim==2 ? kipl::morphology::conn4 : kipl::morphology::conn6)
{
    kipl::base::TImage<ImgType,NDim> tmp;
    tmp=di(img);
    tmp=RecByErosion(img,tmp,conn);

    return tmp;
}
*/

}} // end namespace morphology

namespace kipl { namespace morphology { namespace old {
/// \brief Performs a Self dual Geodesic Reconstruction of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	if \f$f\leq g\f$ will reconstruction by erosion \f$R^{\epsilon}_f\f$ be performed, if
///	\f$g\leq f\f$ will reconstruction by dilation \f$R^{\delta}_f\f$ be performed
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> SelfDualReconstruction(kipl::base::TImage<ImgType,NDim> &f,
        kipl::base::TImage<ImgType,NDim> &g,
        kipl::morphology::MorphConnect conn)
{
    kipl::base::TImage<ImgType,NDim> temp(f);
    deque<int> fifo;

    ptrdiff_t i;

    const size_t *pDimsF=f.Dims();
    const size_t *pDimsG=g.Dims();

    for (i=0; i<NDim; i++)
        if (pDimsF[i]!=pDimsG[i]) {
            throw kipl::base::KiplException("SelfDualReconstruction: Size(f) != Size(g)",__FILE__,__LINE__);
        }


    ImgType *pF=temp.GetDataPtr();
    ImgType *pG=g.GetDataPtr();
    // Set up indexing arrays

    CNeighborhood NG(g.Dims(), NDim, conn);
    const ptrdiff_t cNG=NG.N();
    const ptrdiff_t cNGpm=NG.Nfb();
    int ipos;
    ptrdiff_t pos;
    ImgType max,min;
    const ptrdiff_t N=f.Size();
    // Forward scan
    for (pos=0; pos< N; pos++) {
        if (pF[pos]<pG[pos]) {
            max=pF[pos];
            // Compute max(f(q)) q in ng_backw(p) U p
            for (i=1; i<cNGpm; i++) {
                if ((ipos=NG.backward(pos,i))!=-1)
                    if (pF[ipos]>max) max=pF[ipos];
            }
            pF[pos]=max <pG[pos] ? max : pG[pos];
        }
        else {
            min=pF[pos];
            // Compute min(f(q)) q in ng_backw(p) U p
            for (i=1; i<cNGpm; i++) {
                if ((ipos=NG.backward(pos,i))!=-1)
                    if (pF[ipos]<min) min=pF[ipos];
            }
            pF[pos]=min >pG[pos] ? min : pG[pos];
        }
    }

    //Backward scan
    for (pos=N-1; pos>=0; pos--) {
        if (pF[pos]<pG[pos]) {
            max=pF[pos];
            for (i=1; i<cNGpm; i++) {
                if ((ipos=NG.forward(pos,i))!=-1)
                    if (pF[ipos]>max) max=pF[ipos];
            }
            pF[pos]=max <pG[pos] ? max : pG[pos];
        }
        else {
            min=pF[pos];
            for (i=1; i<cNGpm; i++) {
                if ((ipos=NG.forward(pos,i))!=-1)
                    if (pF[ipos]<min) min=pF[ipos];
            }
            pF[pos]=min >pG[pos] ? min : pG[pos];
        }

        for (i=1; i<cNGpm; i++) {
            if ((ipos=NG.forward(pos,i)!=-1)) {
                if ((pF[ipos]<pF[pos]) && (pF[ipos]<pG[ipos])) {
                    fifo.push_back(pos);
                    break;
                }
                else {
                    if ((pF[ipos]>pF[pos]) && (pF[ipos]>pG[ipos])) {
                        fifo.push_back(pos);
                        break;
                    }
                }
            }
        }
    }
    //cout<<"Queue scan, queue size="<<fifo.size()<<endl;
    while (!fifo.empty()) {
        pos=fifo.front();
        fifo.pop_front();

        for (i=0; i<cNG; i++) {
            if ((ipos=NG.neighbor(pos,i))!=-1) {
                if (pF[ipos]<pG[pos]) {
                    if ((pF[ipos]<pF[pos]) && (pG[ipos]!=pF[ipos])) {
                        pF[ipos]=std::min(pF[pos],pG[ipos]);
                        fifo.push_back(ipos);
                    }
                }
                else {
                    if ((pF[ipos]<pF[pos]) && (pG[ipos]!=pF[ipos])) {
                        pF[ipos]=std::max(pF[pos],pG[ipos]);
                        fifo.push_back(ipos);
                    }
                }
            }
        }
    }
    return temp;
}

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

    ptrdiff_t i;

    const size_t *pDimsF=f.Dims();
    const size_t *pDimsG=g.Dims();

    for (i=0; i<static_cast<ptrdiff_t>(NDimF); i++) {
        if (pDimsF[i]!=pDimsG[i]) {
            throw kipl::base::KiplException("RecByDilation: Size(f) != Size(g)",__FILE__,__LINE__);
        }
    }

    if (NDimG!=NDimF) {
        cerr<<"Dim(f)!=Dim(g)"<<endl;

        if ((NDimG!=3) || (NDimF!=2)) {
            throw kipl::base::KiplException("RecByDilation: Dim(f)!=Dim(g) or Dim(g)!=2 and Dim(f)!=3",__FILE__,__LINE__);
        }
    }

    temp.Resize(g.Dims());
    temp=static_cast<ImgType>(0);

    ImgType const * const pg=g.GetDataPtr();
    ImgType const * const pf=f.GetDataPtr();
    ImgType *ptemp=temp.GetDataPtr();
    size_t errcnt=0L;
    for (i=0; i<static_cast<ptrdiff_t>(f.Size()); i++) {
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
    cNGpm=static_cast<ptrdiff_t>(NG.Nfb());
    cNG=static_cast<ptrdiff_t>(NG.N());
    for (pos=0; pos< static_cast<ptrdiff_t>(f.Size()); pos++) {
        max=ptemp[pos];
        for (i=1; i<cNGpm; i++) {
            if ((ipos=NG.backward(pos,i))!=-1)
                if (ptemp[ipos]>max) max=pf[ipos];
        }
        ptemp[pos]=max <pg[pos] ? max : pg[pos];
    }

    // Backward scan
    ImgType tmppos;
    for (pos=static_cast<ptrdiff_t>(f.Size())-1; pos>=0; pos--) {
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

/// \brief Performs a Reconstruction by erosion of the image
///	\param f mask image
///	\param g marker image
///	\param conn Selects connectivity
///
///	\note \f$g \leq f \f$
///
///	\note The algorithm is based on the hybrid algorithm described in L. Vincent, <em>Morphological Grayscale Reconstruction
///	in Image Analysis: Applications and Efficient Algorithms</em>, IEEE trans. on Image processing, 2(2), 1993
template <typename ImgType, size_t NDimG, size_t NDimF>
kipl::base::TImage<ImgType,NDimG> RecByErosion(const kipl::base::TImage<ImgType,NDimG> &g,
        const kipl::base::TImage<ImgType,NDimF> &f,
        kipl::morphology::MorphConnect conn)
{
    kipl::base::TImage<ImgType,NDimG> temp;
    deque<size_t> fifo;

    size_t i;

    size_t const * const pDimsF=f.Dims();
    size_t const * const pDimsG=g.Dims();
    if (NDimG!=NDimF) {
        throw kipl::base::KiplException("RecByErosion: NDimG!=NDimF",__FILE__,__LINE__);

        if ((NDimG!=3 || (NDimF!=2))) {
            throw kipl::base::KiplException("RecByDilation: Dim(f)!=Dim(g) or Dim(g)!=2 and Dim(f)!=3",__FILE__,__LINE__);
        }
    }
    else
        temp=f;
        //f.ChangeDimension(temp);


    for (i=0; i<NDimF; i++) {
        if (pDimsF[i]!=pDimsG[i]) {
            throw kipl::base::KiplException("RecByErosion: size(f)!=size(g)",__FILE__,__LINE__);
        }
    }

    ImgType const * const pg=g.GetDataPtr();
    ImgType * pf=temp.GetDataPtr();
    //int NGp[27],NGm[27],NG[27];
    // Set up indexing arrays
    ptrdiff_t ipos;
    ptrdiff_t pos;
    ImgType min;
    ptrdiff_t N=static_cast<ptrdiff_t>(g.Size());
    CNeighborhood NG(g.Dims(),NDimG,conn);
    // Forward scan
    const size_t cNGpm=NG.Nfb();
    const size_t cNG=NG.N();

    // Forward scan
    for (pos=0; pos< N; pos++) {
        min=pf[pos];
        for (i=1; i<cNGpm; i++) { // min(f(q)| q in NGp(p) u p)
            if ((ipos=NG.backward(pos,i))!=-1)
                if (pf[ipos]<min) min=pf[ipos];
        }
        pf[pos]=min > pg[pos] ? min : pg[pos]; // max(min(f(NGp(p))),g(p))
    }


    // Backward scan
    ImgType tmppos;

    for (pos=N-1; pos>=0; pos--) {
        min=pf[pos];
        tmppos=min;
        for (i=1; i<cNGpm; i++) {// min(f(q)| q in NGp(p) u p)
            if ((ipos=NG.forward(pos,i))!=-1){
                if (pf[ipos]<min) min=pf[ipos];
            }
        }
        pf[pos]=min > pg[pos] ? min : pg[pos]; // max(min(f(NGp(p))),g(p))
        tmppos=pf[pos];
        for (i=1; i<cNGpm; i++) {
            if ((ipos=NG.forward(pos,i))!=-1) {
                if ((pf[ipos]>tmppos) && (pf[ipos]>pg[ipos])) {
                    fifo.push_front(pos);
                    break;
                }
            }
        }

    }

    while (!fifo.empty()) {
        pos=fifo.front();
        fifo.pop_front();
        tmppos=pf[pos];

        for (i=0; i<cNG; i++) {
            if ((ipos=NG.neighbor(pos,i))!=-1) {
                if ((pf[ipos]>tmppos) && (pg[ipos]!=pf[ipos])) {
                    pf[ipos]=tmppos>pg[ipos] ? tmppos : pg[ipos];
                    fifo.push_front(ipos);
                }
            }
        }
    }

    return temp;
}

/// \brief Removes the objects that are connected to the edge of the image
///	\param img Input image
///	\param conn Connectivity of the reconstruction
///
///	Computes \f$img-R^{\delta}_{img}(fm)\f$ where the border of fm is equal
///	to the border of img while the rest of fm is equal to 0
template <typename ImgType, size_t NDim>
kipl::base::TImage<ImgType,NDim> RemoveEdgeObj(kipl::base::TImage<ImgType,NDim> &img,
        kipl::morphology::MorphConnect conn)
{
    size_t const *const dims=img.Dims();
    kipl::base::TImage<ImgType,NDim> edge(dims);
    edge=kipl::base::min(img);

    size_t y,z;
    ImgType *pLineEdge, *pLineImg;
    for (z=0; z<dims[2]; z++) {
        memcpy(edge.GetLinePtr(0,z),img.GetLinePtr(0,z),dims[0]*sizeof(ImgType));
        memcpy(edge.GetLinePtr(dims[1]-1,z),img.GetLinePtr(dims[1]-1,z),dims[0]*sizeof(ImgType));
        for (y=1; y<dims[1]-1; y++) {
            pLineEdge=edge.GetLinePtr(y,z);
            pLineImg=img.GetLinePtr(y,z);
            pLineEdge[0]=pLineImg[0];
            pLineEdge[dims[0]-1]=pLineImg[dims[0]-1];

        }
    }
    if (NDim==3) {
        memcpy(edge.GetDataPtr(),img.GetDataPtr(),dims[0]*dims[1]*sizeof(ImgType));
        memcpy(edge.GetLinePtr(0,dims[2]-1),img.GetLinePtr(0,dims[2]-1),dims[0]*dims[1]*sizeof(ImgType));
    }

    edge=img-kipl::morphology::old::RecByDilation(img,edge,conn);

    return edge;

}
}}}

#endif // MORPHGEO_HPP
