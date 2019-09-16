//<LICENSE>
#ifndef MORPHEXTREMA_HPP
#define MORPHEXTREMA_HPP

#include "../../base/kiplenums.h"
#include "../pixeliterator.h"

/// Reconstruction based extrem operations
namespace kipl {
namespace morphology {
/// \brief Computes the regional minimum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector


template <typename ImgType,size_t N>
int RMin(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, kipl::base::eConnectivity conn, bool bilevel)
{
    const size_t *dims=img.Dims();

    ImgType maxVal=kipl::base::max(img);
    extremes.Clone(img);

    deque<ptrdiff_t> posQ;

    kipl::base::PixelIterator NG(dims,conn);

    ImgType *pExt=extremes.GetDataPtr();
    const ImgType *pImg=img.GetDataPtr();
    ImgType val;
    ptrdiff_t i,j,pos,p;

    NG.setPosition(0L);
    for (i=0; i<extremes.Size(); ++i) {
        NG.setPosition(i);
        if (pExt[i]!=maxVal) {
            for (const auto & neighborPix : NG.neighborhood())
            {
                pos = i + neighborPix;
                if (pImg[pos]<pImg[i]) {
                    posQ.push_front(i);
                    val=pImg[i];
                    pExt[i]=maxVal;
                    break;
                }
            }

            while (!posQ.empty()) {
                p=posQ.front();
                pExt[p]=maxVal;
                posQ.pop_front();
                NG.setPosition(p);
                for (const auto & neighborPix : NG.neighborhood())
                {
                    pos = p + neighborPix;
                    if (pExt[pos]==val)
                    {
                        posQ.push_front(pos);
                    }
                }
            }
        }
    }

    if (bilevel)
        for (i=0; i<extremes.Size(); i++)
            extremes[i]= (extremes[i]==maxVal) ? 0 : 1;

    return 0;
}

/// \brief Computes the regional maximum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int RMax(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, base::eConnectivity conn)
{
    const size_t *dims=img.Dims();

    ImgType min=kipl::base::min(img);
    extremes=img;

    deque<ptrdiff_t> posQ;
    kipl::base::PixelIterator NG(dims,conn);

    ImgType *pExt=extremes.GetDataPtr();
    const ImgType *pImg=img.GetDataPtr();
    ImgType val;
    ptrdiff_t i,j,pos,p;

    NG.setPosition(0L);
    for (i=0; i<extremes.Size(); ++i,++NG) {
        NG.setPosition(i);
        if (pExt[i]!=min) {
            for (const auto & neighborPix : NG.neighborhood())
            {
                pos = i + neighborPix;
                if (pImg[pos]>pImg[i]) {
                    posQ.push_front(i);
                    val=pImg[i];
                    pExt[i]=min;
                    break;
                }
            }

            while (!posQ.empty()) {
                p=posQ.front();
                pExt[p]=min;
                posQ.pop_front();
                NG.setPosition(p);
                for (const auto & neighborPix : NG.neighborhood())
                {
                    pos = p + neighborPix;
                    if (pExt[pos]==val)
                        posQ.push_front(pos);

                }
            }
        }
    }

    return 0;
}

///    \brief Computes the h maximum image
///	\param img Input image
///	\param result Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\delta}_{img}(img-h)\f$
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int hMax(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &result,ImgType h, kipl::base::eConnectivity conn)
{
    if (h<0) {
        cerr<<"hMax: h must be >0"<<endl;
        return -1;
    }

    result=img-h;

    result=RecByDilation(img,result, conn);

    return 0;
}

/// \brief Computes the extended maximum image
///	\param img Input image
///	\param res Resulting image
///	\param h Threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$RMAX(hMAX_h(img)\f$
///	\todo Fix edge processing
template <typename ImgType, size_t NDim>
int ExtendedMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &res, ImgType h, kipl::base::eConnectivity conn)
{
    if (h<static_cast<ImgType>(0)) {
        cerr<<"ExtendedMax: h must be >0"<<endl;
        return -1;
    }
    res=img-h;

    RMax(RecByDilation(img,res,conn),res,conn);

    return 0;
}



/// \brief Computes the h minimum image
///	\param img Input image
///	\param res Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\epsilon}_{img}(img+h)\f$
///	\todo Fix edge processing
template <class ImgType, size_t N>
int hMin(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &res, ImgType h, kipl::base::eConnectivity conn)
{
    if (h<0) {
        cerr<<"hMin: h must be >0"<<endl;
        return -1;
    }
    res=img+h;

    res=RecByErosion(img,res, conn);

    return 0;
}

/// \brief Computes the extended minimum image
///	\param img Input image
///	\param res Output image
///	\param h threshold level
///	\param conn Connectivity selector
///	\param bilevel obsolete parameter
///
///	The method computes \f$RMIN(hMIN_h(img)\f$
///	\todo Fix edge processing
template <typename T, size_t N>
int ExtendedMin(const kipl::base::TImage<T,N> &img,kipl::base::TImage<T,N> &res, T h, kipl::base::eConnectivity conn, bool bilevel)
{
    if (h<0) {
        cerr<<"ExtendedMin: h must be >0"<<endl;
        return -1;
    }

    res=img+h;

    RMin(RecByErosion(img,res,conn),res, conn);

    return 0;
}


//  \brief Imposes minimum according to a marker image
//	\param f Input image
//	\param fm Marker image fm=0 for min and =max{f} otherwise
//	\param res Resulting image
//	\param conn Connectivity of the reconstruction

//	Computes \f$R^{\epsilon}_{(f+1)\wedge f_m}(f_m)\f$
template <typename T, size_t N>
int MinImpose(const kipl::base::TImage<T,N> &f, const kipl::base::TImage<T,N> &fm, kipl::base::TImage<T,N> &res, kipl::base::eConnectivity conn)
{
    kipl::base::TImage<T,N> tmp(f.Dims());

    res=f;

    int i;

    //ImgType min=f.Min();
    T min=f.Min()<0 ? 10*f.Min(): -9*f.Min();
    //ImgType min=-numeric_limits<ImgType>::max();
    T max=f.Max();

    T *pF=f.getDataptr();
    T *pRes=res.getDataptr();
    T *pMark=fm.getDataptr();
    T *pTmp=tmp.getDataptr();

    for (i=0; i<f.N();i++) {
        //pTmp[i]=pMark[i] ? max : min;
        //pTmp[i]=pMark[i] ? numeric_limits<ImgType>::min() : max;
        pTmp[i]=pMark[i] ? min : max;
        //pRes[i]=pF[i]+1 < pTmp[i] ? pF[i]+1 : pTmp[i];
        pRes[i]=pF[i]+1 < pTmp[i] ? pF[i]+1 : pTmp[i];
    }

    res=RecByErosion(res,tmp,conn);

    return 0;

}

template <typename T>
kipl::base::TImage<T,2> FillHole(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn)
{
    std::stringstream msg;
    kipl::base::TImage<T,2> fm(img.Dims());

    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=maxval;

    std::copy_n(img.GetLinePtr(0),img.Size(0),fm.GetLinePtr(0));
    std::copy_n(img.GetLinePtr(img.Size(1)-1),img.Size(0),fm.GetLinePtr(img.Size(1)-1));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++)
    {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]    = pA[0];
        pB[last] = pA[last];
    }

    kipl::base::TImage<T,2> result;

    try
    {
        result=kipl::morphology::RecByErosion(img,fm,conn);
    }
    catch (kipl::base::KiplException & e) {
        msg<<"FillHoles failed with a kipl exception: "<<e.what();
        throw kipl::base::KiplException(msg.str());
    }

    return result;
}

template <typename T>
kipl::base::TImage<T,2> FillPeaks(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn)
{
    kipl::base::TImage<T,2> fm(img.Dims());
    std::stringstream msg;
    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=minval;
    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++) {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }

    kipl::base::TImage<T,2> tmp(img.Dims());
    try {
        tmp=kipl::morphology::RecByDilation(img,fm,conn);
    }
    catch (kipl::base::KiplException & e) {
        msg<<"FillPeaks failed with a kipl exception: "<<e.what();
        throw kipl::base::KiplException(msg.str());
    }
    return tmp;
}

template <typename T>
kipl::base::TImage<T,2> FillExtrema(kipl::base::TImage<T,2> &img)
{
    kipl::base::TImage<T,2> fm(img.Dims());

    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=maxval;
    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++) {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }

    std::cout<<"a";
    kipl::base::TImage<T,2> holes=kipl::morphology::RecByErosion(img,fm,kipl::morphology::conn8);
    std::cout<<"b";


    for (size_t i=0; i<holes.Size(); i++) {
        holes[i]=maxval-holes[i];
    }

    kipl::math::minmax(holes.GetDataPtr(),holes.Size(),&minval, & maxval);
    fm=maxval;

    memcpy(fm.GetLinePtr(0),holes.GetLinePtr(0),holes.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),holes.GetLinePtr(holes.Size(1)-1),holes.Size(0)*sizeof(T));
    for (size_t i=1; i<holes.Size(1)-1; i++) {
        T *pA=holes.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }
    std::cout<<"c";

    kipl::base::TImage<float,2> tmp=kipl::morphology::RecByErosion(holes,fm,kipl::morphology::conn8);

//    for (size_t i=0; i<holes.Size(); i++) {
//        tmp[i]=1-tmp[i];
//    }
    return tmp;
}

template <typename T>
kipl::base::TImage<T,2> FillHole2(kipl::base::TImage<T,2> &img, kipl::base::eConnectivity conn)
{
//    std::stringstream msg;
//    std::map<T,list<ptrdiff_t> > fifo;
//    kipl::base::TImage<T,2> fm(img.Dims());

//    T maxval;
//    T minval;
//    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);
//    // Initialize output image (borders=input, central region=max input) and queue
//    fm=maxval;
//    T *pA=img.GetDataPtr();
//    T *pB=fm.GetDataPtr();

//    ptrdiff_t p=0;
//    ptrdiff_t sx1=img.Size(0)-1;

//    for (int i=0; i<img.Size(0); i++, p++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//    }
//    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
//    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));

//    size_t last=img.Size(0)-1;
//    for (size_t i=1; i<img.Size(1)-1; i++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//        p+=sx1;
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//        p++
//    }

//    for (int i=0; i<img.Size(0); i++, p++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//    }

//    // Main loop


    return img;
}
}}

/// Reconstruction based extrem operations
namespace kipl {
namespace morphology {
namespace old {
/// \brief Computes the regional minimum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing

template <typename ImgType,size_t N>
int RMin(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, MorphConnect conn, bool bilevel)
{
    const size_t *dims=img.Dims();
    //Statistics stats=ComputeImageStats(img);
    //ImgType max=(ImgType)stats.Max();
    ImgType maxVal=kipl::base::max(img);
    extremes=img;

    deque<int> posQ;

    CNeighborhood NG(dims,N,conn);
    int Nng=NG.N();
    ImgType *pExt=extremes.GetDataPtr();
    const ImgType *pImg=img.GetDataPtr();
    ImgType val;
    int i,j,pos,p;

    for (i=0; i<extremes.Size(); i++) {
        if (pExt[i]!=maxVal) {
            for (j=0; j<Nng; j++) {
                if ((pos=NG.neighbor(i,j))!=-1)
                    if (pImg[pos]<pImg[i]) {
                        posQ.push_front(i);
                        val=pImg[i];
                        pExt[i]=maxVal;
                        break;
                    }
            }

            while (!posQ.empty()) {
                p=posQ.front();
                pExt[p]=maxVal;
                posQ.pop_front();
                for (j=0; j<Nng; j++) {
                    if ((pos=NG.neighbor(p,j))!=-1)
                        if (pExt[pos]==val) {
                            posQ.push_front(pos);
                        }
                }
            }
        }
    }

    if (bilevel)
        for (i=0; i<extremes.Size(); i++)
            extremes[i]= (extremes[i]==maxVal) ? 0 : 1;

    return 0;
}

/// \brief Computes the regional maximum image
///	\param img Input image
///	\param extremes Resulting image
///	\param conn Connectivity selector
///
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int RMax(const kipl::base::TImage<ImgType,N> &img, kipl::base::TImage<ImgType,N> &extremes, MorphConnect conn)
{
    const size_t *dims=img.Dims();

    ImgType minVal=kipl::base::min(img);
    extremes=img;

    deque<int> posQ;
    CNeighborhood NG(dims,N,conn);
    int Nng=NG.N();

    ImgType *pExt=extremes.GetDataPtr();
    const ImgType *pImg=img.GetDataPtr();
    ImgType val;
    long i,pos,p;
    int j;

    for (i=0; i<extremes.Size(); i++) {
        if (pExt[i]!=minVal) {
            for (j=0; j<Nng; j++) {
                if ((pos=NG.neighbor(i,j))!=-1) {
                    if (pImg[pos]>pImg[i]) {
                        posQ.push_front(i);
                        val=pImg[i];
                        pExt[i]=minVal;
                        break;
                    }
                }
            }

            while (!posQ.empty()) {
                p=posQ.front();
                pExt[p]=minVal;
                posQ.pop_front();
                for (j=0; j<Nng; j++) {
                    if ((pos=NG.neighbor(p,j))!=-1) {
                        if (pExt[pos]==val)
                            posQ.push_front(pos);
                    }
                }
            }
        }
    }

    return 0;
}

///    \brief Computes the h maximum image
///	\param img Input image
///	\param result Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\delta}_{img}(img-h)\f$
///	\todo Fix edge processing
template <typename ImgType,size_t N>
int hMax(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &result,ImgType h, MorphConnect conn)
{
    if (h<0) {
        cerr<<"hMax: h must be >0"<<endl;
        return -1;
    }

    result=img-h;

    result=RecByDilation(img,result, conn);

    return 0;
}

/// \brief Computes the extended maximum image
///	\param img Input image
///	\param res Resulting image
///	\param h Threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$RMAX(hMAX_h(img)\f$
///	\todo Fix edge processing
template <typename ImgType, size_t NDim>
int ExtendedMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &res, ImgType h, MorphConnect conn)
{
    if (h<0) {
        cerr<<"ExtendedMax: h must be >0"<<endl;
        return -1;
    }
    res=img-h;

    RMax(RecByDilation(img,res,conn),res,conn);

    return 0;
}



/// \brief Computes the h minimum image
///	\param img Input image
///	\param res Resulting image
///	\param h threshold level
///	\param conn Connectivity selector
///
///	The method computes \f$R^{\epsilon}_{img}(img+h)\f$
///	\todo Fix edge processing
template <class ImgType, size_t N>
int hMin(const kipl::base::TImage<ImgType,N> &img,kipl::base::TImage<ImgType,N> &res, ImgType h, MorphConnect conn)
{
    if (h<0) {
        cerr<<"hMin: h must be >0"<<endl;
        return -1;
    }
    res=img+h;

    res=RecByErosion(img,res, conn);

    return 0;
}

/// \brief Computes the extended minimum image
///	\param img Input image
///	\param res Output image
///	\param h threshold level
///	\param conn Connectivity selector
///	\param bilevel obsolete parameter
///
///	The method computes \f$RMIN(hMIN_h(img)\f$
///	\todo Fix edge processing
template <typename T, size_t N>
int ExtendedMin(const kipl::base::TImage<T,N> &img,kipl::base::TImage<T,N> &res, T h, MorphConnect conn, bool bilevel)
{
    if (h<0) {
        cerr<<"ExtendedMin: h must be >0"<<endl;
        return -1;
    }

    res=img+h;

    RMin(RecByErosion(img,res,conn),res, conn,bilevel);

    return 0;
}


//  \brief Imposes minimum according to a marker image
//	\param f Input image
//	\param fm Marker image fm=0 for min and =max{f} otherwise
//	\param res Resulting image
//	\param conn Connectivity of the reconstruction

//	Computes \f$R^{\epsilon}_{(f+1)\wedge f_m}(f_m)\f$
template <typename T, size_t N>
int MinImpose(const kipl::base::TImage<T,N> &f, const kipl::base::TImage<T,N> &fm, kipl::base::TImage<T,N> &res, MorphConnect conn)
{
    kipl::base::TImage<T,N> tmp(f.Dims());

    res=f;

    int i;

    //ImgType min=f.Min();
    T min=f.Min()<0 ? 10*f.Min(): -9*f.Min();
    //ImgType min=-numeric_limits<ImgType>::max();
    T max=f.Max();

    T *pF=f.getDataptr();
    T *pRes=res.getDataptr();
    T *pMark=fm.getDataptr();
    T *pTmp=tmp.getDataptr();

    for (i=0; i<f.N();i++) {
        //pTmp[i]=pMark[i] ? max : min;
        //pTmp[i]=pMark[i] ? numeric_limits<ImgType>::min() : max;
        pTmp[i]=pMark[i] ? min : max;
        //pRes[i]=pF[i]+1 < pTmp[i] ? pF[i]+1 : pTmp[i];
        pRes[i]=pF[i]+1 < pTmp[i] ? pF[i]+1 : pTmp[i];
    }

    res=RecByErosion(res,tmp,conn);

    return 0;

}

template <typename T>
kipl::base::TImage<T,2> FillHole(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn)
{
    std::stringstream msg;
    kipl::base::TImage<T,2> fm(img.Dims());

    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=maxval;
    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++) {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }
    kipl::base::TImage<T,2> tmp(img.Dims());
    try {
        tmp=kipl::morphology::old::RecByErosion(img,fm,conn);
    }
    catch (kipl::base::KiplException & e) {
        msg<<"FillHoles failed with a kipl exception: "<<e.what();
        throw kipl::base::KiplException(msg.str());
    }

    return tmp;
}

template <typename T>
kipl::base::TImage<T,2> FillPeaks(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn)
{
    kipl::base::TImage<T,2> fm(img.Dims());
    std::stringstream msg;
    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=minval;
    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++) {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }

    kipl::base::TImage<T,2> tmp(img.Dims());
    try {
        tmp=kipl::morphology::old::RecByDilation(img,fm,conn);
    }
    catch (kipl::base::KiplException & e) {
        msg<<"FillPeaks failed with a kipl exception: "<<e.what();
        throw kipl::base::KiplException(msg.str());
    }
    return tmp;
}

template <typename T>
kipl::base::TImage<T,2> FillExtrema(kipl::base::TImage<T,2> &img)
{
    kipl::base::TImage<T,2> fm(img.Dims());

    T maxval;
    T minval;
    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);

    fm=maxval;
    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));
    size_t last=img.Size(0)-1;
    for (size_t i=1; i<img.Size(1)-1; i++) {
        T *pA=img.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }

    std::cout<<"a";
    kipl::base::TImage<T,2> holes=kipl::morphology::RecByErosion(img,fm,kipl::morphology::conn8);
    std::cout<<"b";


    for (size_t i=0; i<holes.Size(); i++) {
        holes[i]=maxval-holes[i];
    }

    kipl::math::minmax(holes.GetDataPtr(),holes.Size(),&minval, & maxval);
    fm=maxval;

    memcpy(fm.GetLinePtr(0),holes.GetLinePtr(0),holes.Size(0)*sizeof(T));
    memcpy(fm.GetLinePtr(img.Size(1)-1),holes.GetLinePtr(holes.Size(1)-1),holes.Size(0)*sizeof(T));
    for (size_t i=1; i<holes.Size(1)-1; i++) {
        T *pA=holes.GetLinePtr(i);
        T *pB=fm.GetLinePtr(i);

        pB[0]=pA[0];
        pB[last]=pA[last];
    }
    std::cout<<"c";

    kipl::base::TImage<float,2> tmp=kipl::morphology::RecByErosion(holes,fm,kipl::morphology::conn8);

//    for (size_t i=0; i<holes.Size(); i++) {
//        tmp[i]=1-tmp[i];
//    }
    return tmp;
}

template <typename T>
kipl::base::TImage<T,2> FillHole2(kipl::base::TImage<T,2> &img, kipl::morphology::MorphConnect conn)
{
//    std::stringstream msg;
//    std::map<T,list<ptrdiff_t> > fifo;
//    kipl::base::TImage<T,2> fm(img.Dims());

//    T maxval;
//    T minval;
//    kipl::math::minmax(img.GetDataPtr(),img.Size(),&minval, & maxval);
//    // Initialize output image (borders=input, central region=max input) and queue
//    fm=maxval;
//    T *pA=img.GetDataPtr();
//    T *pB=fm.GetDataPtr();

//    ptrdiff_t p=0;
//    ptrdiff_t sx1=img.Size(0)-1;

//    for (int i=0; i<img.Size(0); i++, p++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//    }
//    memcpy(fm.GetLinePtr(0),img.GetLinePtr(0),img.Size(0)*sizeof(T));
//    memcpy(fm.GetLinePtr(img.Size(1)-1),img.GetLinePtr(img.Size(1)-1),img.Size(0)*sizeof(T));

//    size_t last=img.Size(0)-1;
//    for (size_t i=1; i<img.Size(1)-1; i++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//        p+=sx1;
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//        p++
//    }

//    for (int i=0; i<img.Size(0); i++, p++) {
//        pB[p]=pA[p];
//        fifo[pA[p]].push_back(p);
//    }

//    // Main loop


    return img;
}
}}}

#endif // MORPHEXTREMA_HPP
