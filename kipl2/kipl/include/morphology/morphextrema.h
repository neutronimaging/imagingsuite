//<LICENCE>

#ifndef __MORPHEXTREMA_H
#define __MORPHEXTREMA_H

#include <deque>
#include <iostream>

#include "../base/KiplException.h"
#include "../base/timage.h"
#include "../math/image_statistics.h"
#include "morphology.h"
#include "morphgeo.h"


/** Reconstruction based extrem operations */
namespace kipl {
namespace morphology {
/** \brief Computes the regional minimum image
	\param img Input image
	\param extremes Resulting image
	\param conn Connectivity selector

	\todo Fix edge processing
*/
template <typename ImgType,size_t NDim>
int RMin(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &extremes, MorphConnect &conn, bool bilevel=true)
{
    const size_t *dims=img.Dims();
	//Statistics stats=ComputeImageStats(img);
	//ImgType max=(ImgType)stats.Max();
	ImgType max=img.Max();
	extremes=img;
	
	deque<int> posQ;

	CNeighborhood NG(dims,NDim,conn);
	int Nng=NG.N();
	ImgType *pExt=extremes.getDataptr();
	ImgType *pImg=img.getDataptr();
	ImgType val;
	int i,j,pos,p;
	
	for (i=0; i<extremes.N(); i++) {
		if (pExt[i]!=max) {
			for (j=0; j<Nng; j++) {
				if ((pos=NG.neighbor(i,j))!=-1)
					if (pImg[pos]<pImg[i]) {
						posQ.push_front(i);
						val=pImg[i];
						pExt[i]=max;
						break;
					}
			}
			
			while (!posQ.empty()) {
				p=posQ.front();
				pExt[p]=max;
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
		for (i=0; i<extremes.N(); i++)
			extremes[i]= (extremes[i]==max) ? 0 : 1;
	
	return 0;
}

/** \brief Computes the regional maximum image
	\param img Input image
	\param extremes Resulting image
	\param conn Connectivity selector

	\todo Fix edge processing
*/
template <typename ImgType,size_t NDim>
int RMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &extremes, MorphConnect &conn)
{
    const size_t *dims=img.Dims();
	
	ImgType min=img.Min();
	extremes=img;
	
	deque<int> posQ;
	CNeighborhood NG(dims,NDim,conn);
	int Nng=NG.N();
	
	ImgType *pExt=extremes.getDataptr();
	ImgType *pImg=img.getDataptr();
	ImgType val;
	long i,pos,p;
	int j;
	
	for (i=0; i<extremes.N(); i++) {
		if (pExt[i]!=min) {
			for (j=0; j<Nng; j++) {
				if ((pos=NG.neighbor(i,j))!=-1) {
					if (pImg[pos]>pImg[i]) {
						posQ.push_front(i);
						val=pImg[i];
						pExt[i]=min;
						break;
					}
				}
			}
			
			while (!posQ.empty()) {
				p=posQ.front();
				pExt[p]=min;
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

/** \brief Computes the h maximum image
	\param img Input image
	\param result Resulting image
	\param h threshold level
	\param conn Connectivity selector

	The method computes \f$R^{\delta}_{img}(img-h)\f$
	\todo Fix edge processing
*/
template <typename ImgType,int NDim>
int hMax(const kipl::base::TImage<ImgType,NDim> &img,kipl::base::TImage<ImgType,NDim> &result,ImgType h, MorphConnect conn=conn4)
{
	if (h<0) {
		cerr<<"hMax: h must be >0"<<endl;
		return -1;
	}

	result=img-h;

	result=RecByDilation(img,result, conn);

	return 0;
}

/** \brief Computes the extended maximum image
	\param img Input image
	\param res Resulting image
	\param h Threshold level
	\param conn Connectivity selector
	
	The method computes \f$RMAX(hMAX_h(img)\f$
	\todo Fix edge processing
*/
template <typename ImgType, int NDim>
int ExtendedMax(const kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &res, ImgType h, MorphConnect conn=conn4)
{
	if (h<0) {
		cerr<<"ExtendedMax: h must be >0"<<endl;
		return -1;
	}
	res=img-h;

	RMax(RecByDilation(img,res,conn),res,conn);

	return 0;
}



/** \brief Computes the h minimum image
	\param img Input image
	\param res Resulting image
	\param h threshold level
	\param conn Connectivity selector

	The method computes \f$R^{\epsilon}_{img}(img+h)\f$
	\todo Fix edge processing
*/
template <class ImgType, int NDim>
int hMin(const kipl::base::TImage<ImgType,NDim> &img,kipl::base::TImage<ImgType,NDim> &res, ImgType h, MorphConnect conn=conn4)
{
	if (h<0) {
		cerr<<"hMin: h must be >0"<<endl;
		return -1;
	}
	res=img+h;

	res=RecByErosion(img,res, conn);

	return 0;
}

/** \brief Computes the extended minimum image
	\param img Input image
	\param res Output image
	\param h threshold level
	\param conn Connectivity selector
	\param bilevel obsolete parameter

	The method computes \f$RMIN(hMIN_h(img)\f$
	\todo Fix edge processing
*/
template <typename T, size_t NDim>
int ExtendedMin(const kipl::base::TImage<T,NDim> &img,kipl::base::TImage<T,NDim> &res, T h, MorphConnect conn=conn4, bool bilevel=true)
{
	if (h<0) {
		cerr<<"ExtendedMin: h must be >0"<<endl;
		return -1;
	}

	res=img+h;

	RMin(RecByErosion(img,res,conn),res, conn);

	return 0;
}


/** \brief Imposes minimum according to a marker image
	\param f Input image
	\param fm Marker image fm=0 for min and =max{f} otherwise
	\param res Resulting image
	\param conn Connectivity of the reconstruction

	Computes \f$R^{\epsilon}_{(f+1)\wedge f_m}(f_m)\f$
  */
template <typename T, size_t NDim>
int MinImpose(const kipl::base::TImage<T,NDim> &f, const kipl::base::TImage<T,NDim> &fm, kipl::base::TImage<T,NDim> &res, MorphConnect conn=conn4)
{
    kipl::base::TImage<T,NDim> tmp(f.Dims());
	
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
        tmp=kipl::morphology::RecByErosion(img,fm,conn);
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
}}
#endif
