//<LICENSE>

#ifndef ISSFILTERQ3D_HPP_
#define ISSFILTERQ3D_HPP_

#include "ISSfilterQ3D.h"

#include <base/timage.h>
#include <base/tsubimage.h>
#include <base/thistogram.h>
#include <base/timagetests.h>
#include <math/mathfunctions.h>
#include <base/core/quad.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>
#include <interactors/interactionbase.h>

#include <iomanip>
#include <sstream>

#ifdef __aarch64__
    #include <sse2neon/sse2neon.h>
#else
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "filterenums.h"

namespace advancedfilters
{

inline __m128 sign(__m128 m)
{
    return _mm_and_ps(_mm_or_ps(_mm_and_ps(m, _mm_set1_ps(-0.0f)), // is -0.0f correct?
                _mm_set1_ps(1.0f)),
              _mm_cmpneq_ps(m, _mm_setzero_ps()));
}

template <typename T>
ISSfilterQ3D<T>::ISSfilterQ3D(kipl::interactors::InteractionBase *interactor) :
	logger("ISSfilterQ3D"),
    m_Interactor(interactor),
    m_eInitialImage(InitialImageOriginal),
    m_eRegularization(RegularizationTV2)
{
	m_dEpsilon=1e-7;
}

template <typename T>
ISSfilterQ3D<T>::~ISSfilterQ3D()
{

}

template <typename T>
eInitialImageType ISSfilterQ3D<T>::initialImageType() const
{
    return m_eInitialImage;
}
template <typename T>
void ISSfilterQ3D<T>::setInitialImageType(const eInitialImageType &eInitialImage)
{
    m_eInitialImage = eInitialImage;
}

template <typename T>
eRegularizationType ISSfilterQ3D<T>::regularizationType() const
{
    return m_eRegularization;
}

template <typename T>
void ISSfilterQ3D<T>::setRegularizationType(const eRegularizationType &eRegularization)
{
    m_eRegularization = eRegularization;
}

template <typename T>
int ISSfilterQ3D<T>::process(kipl::base::TImage<T,3> &img, double dTau, double dLambda, double dAlpha, int nN, bool saveiterations, std::string itpath)
{
    m_error.resize(nN);
    m_entropy.resize(nN);

    switch (m_eInitialImage)
    {
        case InitialImageOriginal: m_f=img; m_f.Clone(); break;
        case InitialImageZero:     m_f.resize(img.dims()); m_f=static_cast<T>(0); break;
	}

    m_dTau    = dTau;
    m_dLambda = dLambda;
    m_dAlpha  = dAlpha;

    m_v.resize(img.dims());
	m_v=static_cast<T>(0);

	std::ostringstream msg;
    switch (m_eRegularization)
    {
        case advancedfilters::RegularizationTV1 :
            logger(kipl::logging::Logger::LogMessage, "Running ISS with TV1");
            break;
        case advancedfilters::RegularizationTV2 :
            logger(kipl::logging::Logger::LogMessage, "Running ISS with TV2");
            break;
    }

    for (int i=0; (i<nN) && (updateStatus(float(i)/nN,"ISS 3D filter iteration")==false)	; ++i)
    {
		msg.str("");
		msg<<"Processing iteration "<<i+1;
		logger(kipl::logging::Logger::LogMessage,msg.str());
        m_error[i]=_SolveIterationSSE(img)/img.Size();
	//	entropy[i]=_ComputeEntropy(img);

        if (saveiterations)
        {
			std::string fname, mask,ext;
			kipl::strings::filenames::MakeFileName("iteration_####.tif",i, fname, ext, '#', '0');
			std::string p=itpath;
			kipl::strings::filenames::CheckPathSlashes(p,true);
			fname=p+fname;
			kipl::base::TImage<T,2> slice=kipl::base::ExtractSlice(img, img.Size(2)/2);
            kipl::io::WriteTIFF(slice,fname.c_str(),kipl::base::Float32);
		}
	}

	return nN;
}

template <typename T>
T ISSfilterQ3D<T>::_SolveIterationSSE(kipl::base::TImage<T,3> &img)
{
	kipl::base::TImage<T,3> p;

	_P(img,p);
	if (kipl::base::hasNan(p))
		logger(kipl::logging::Logger::LogWarning,"p has nan");
    ptrdiff_t i;

    double sum2=0.0;
    const T lambda=static_cast<T>(m_dLambda);

	float fTau=static_cast<T>(m_dTau);
    float fTauAlpha=static_cast<T>(m_dTau*m_dAlpha);

    if (m_eRegularization == advancedfilters::RegularizationTV1)
    {
        #pragma omp parallel reduction(+:sum2)
        {
        //	int TID=omp_get_thread_num();
            ptrdiff_t N=(img.Size()+3)/4;
            __m128 tempFU;
            __m128 xTau      = _mm_set1_ps(fTau);
            __m128 xTauAlpha = _mm_set1_ps(fTauAlpha);
            __m128 xLambda   = _mm_set1_ps(lambda);

            __m128 *pU=reinterpret_cast<__m128 *>(img.GetDataPtr());
            __m128 *pV=reinterpret_cast<__m128 *>(m_v.GetDataPtr());
            __m128 *pP=reinterpret_cast<__m128 *>(p.GetDataPtr());
            __m128 *pF=reinterpret_cast<__m128 *>(m_f.GetDataPtr());
            __m128 xDiff;

            kipl::base::uFQuad qLocalSum;
            qLocalSum.xmm=_mm_set1_ps(0.0f);

            #pragma omp for
            for (i=0; i<N ; i++) {
                // tempFU=pF[i]-pU[i];
                tempFU=sign(_mm_sub_ps(pF[i],pU[i]));
                // pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
                pU[i]=_mm_add_ps(pU[i],_mm_mul_ps(xTau,_mm_add_ps(pP[i],_mm_mul_ps(xLambda,_mm_add_ps(tempFU,pV[i])))));

                // pV[i]+=dTauAlpha*tempFU;
                pV[i]=_mm_add_ps(pV[i],_mm_mul_ps(xTauAlpha,tempFU));

                // diff=static_cast<double>(pU[i]-pF[i]);
                xDiff=_mm_sub_ps(pU[i],pF[i]);
                // localsum+=diff*diff;
                qLocalSum.xmm=_mm_add_ps(qLocalSum.xmm,_mm_mul_ps(xDiff,xDiff));
            }

            sum2+=qLocalSum.q.a+qLocalSum.q.b+qLocalSum.q.c+qLocalSum.q.d;
        }
    }

    if (m_eRegularization == advancedfilters::RegularizationTV2) {
        #pragma omp parallel reduction(+:sum2)
        {
        //	int TID=omp_get_thread_num();
            ptrdiff_t N=(img.Size()+3)/4;
            __m128 tempFU;
            __m128 xTau      = _mm_set1_ps(fTau);
            __m128 xTauAlpha = _mm_set1_ps(fTauAlpha);
            __m128 xLambda   = _mm_set1_ps(lambda);

            __m128 *pU=reinterpret_cast<__m128 *>(img.GetDataPtr());
            __m128 *pV=reinterpret_cast<__m128 *>(m_v.GetDataPtr());
            __m128 *pP=reinterpret_cast<__m128 *>(p.GetDataPtr());
            __m128 *pF=reinterpret_cast<__m128 *>(m_f.GetDataPtr());
            __m128 xDiff;

            kipl::base::uFQuad qLocalSum;
            qLocalSum.xmm=_mm_set1_ps(0.0f);

            #pragma omp for
            for (i=0; i<N ; i++) {
                // tempFU=pF[i]-pU[i];
                tempFU=_mm_sub_ps(pF[i],pU[i]);
                // pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
                pU[i]=_mm_add_ps(pU[i],_mm_mul_ps(xTau,_mm_add_ps(pP[i],_mm_mul_ps(xLambda,_mm_add_ps(tempFU,pV[i])))));

                // pV[i]+=dTauAlpha*tempFU;
                pV[i]=_mm_add_ps(pV[i],_mm_mul_ps(xTauAlpha,tempFU));

                // diff=static_cast<double>(pU[i]-pF[i]);
                xDiff=_mm_sub_ps(pU[i],pF[i]);
                // localsum+=diff*diff;
                qLocalSum.xmm=_mm_add_ps(qLocalSum.xmm,_mm_mul_ps(xDiff,xDiff));
            }

            sum2+=qLocalSum.q.a+qLocalSum.q.b+qLocalSum.q.c+qLocalSum.q.d;
        }
    }

	return static_cast<T>(sum2);
}

template <typename T>
T ISSfilterQ3D<T>::_SolveIteration(kipl::base::TImage<T,3> &img)
{
	kipl::base::TImage<T,3> p;

	_P(img,p);
    ptrdiff_t i;

    double sum2=0.0;
    const T lambda=static_cast<T>(m_dLambda);

    if (m_eRegularization == advancedfilters::RegularizationTV1) {
        logger(kipl::logging::Logger::LogMessage, "Running ISS with TV1");
        #pragma omp parallel reduction(+:sum2)
        {
            double localsum=0.0;

            long long int N=img.Size();
            T tempFU;
            T fTau=static_cast<T>(m_dTau);
            T dTauAlpha=static_cast<T>(m_dTau*m_dAlpha);
            T *pU=img.GetDataPtr();
            T *pV=m_v.GetDataPtr();
            T *pP=p.GetDataPtr();
            T *pF=m_f.GetDataPtr();
            double diff=0.0;

            #pragma omp for
            for (i=0; i<N ; i++)
            {
                tempFU=sign(pF[i]-pU[i]);
                pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
                pV[i]+=dTauAlpha*tempFU;
                diff=static_cast<double>(pU[i]-pF[i]);
                localsum+=diff*diff;
            }
            sum2+=localsum;
        }
    }


    if (m_eRegularization == advancedfilters::RegularizationTV2) {
        logger(kipl::logging::Logger::LogMessage, "Running ISS with TV2");
        #pragma omp parallel reduction(+:sum2)
        {
            double localsum=0.0;
            // int TID=omp_get_thread_num();
            long long int N=img.Size();
            T tempFU;
            T fTau=static_cast<T>(m_dTau);
            T dTauAlpha=static_cast<T>(m_dTau*m_dAlpha);
            T *pU=img.GetDataPtr();
            T *pV=m_v.GetDataPtr();
            T *pP=p.GetDataPtr();
            T *pF=m_f.GetDataPtr();
            double diff=0.0;

            #pragma omp for
            for (i=0; i<N ; i++)
            {
                tempFU=pF[i]-pU[i];
                pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
                pV[i]+=dTauAlpha*tempFU;
                diff=static_cast<double>(pU[i]-pF[i]);
                localsum+=diff*diff;
            }
            sum2+=localsum;
        }
    }

	return static_cast<T>(sum2);
}
	
template <typename T>
int ISSfilterQ3D<T>::_P(kipl::base::TImage<T,3> &img, kipl::base::TImage<T,3> &res)
{
	std::ostringstream msg;
	size_t nMargin=2;
	std::list<kipl::base::TImage<T,2> > img_queue;
	std::list<kipl::base::TImage<T,2> > dx_queue;
	std::list<kipl::base::TImage<T,2> > dy_queue;
	std::list<kipl::base::TImage<T,2> > dz_queue;

    kipl::base::TImage<T,2> dx,dy,dz, d2x, d2y,d2z;

    res.resize(img.dims());
    kipl::base::TImage<T,2> resslice(res.dims());
	kipl::base::TImage<T,2> prev_dx;
	kipl::base::TImage<T,2> prev_dy;
	kipl::base::TImage<T,2> prev_dz;

    const int N=static_cast<int>(img.Size(2));
    for (int i=0; i<N;i++)
    {
        if (img_queue.empty())
        {
		//	kipl::base::TImage<T,2> slice=kipl::base::ExtractSlice(img,1);
			kipl::base::TImage<T,2> slice=GetPaddedSlice(img,1,nMargin);
			img_queue.push_back(slice);
            resslice.resize(slice.dims());
		}

		//kipl::base::TImage<T,2> slice=kipl::base::ExtractSlice(img,i);
		kipl::base::TImage<T,2> slice=GetPaddedSlice(img,i,nMargin);
		img_queue.push_back(slice);


		_FirstDerivateSSE(img_queue,dx_queue,dy_queue,dz_queue); // Lag difference


		img_queue.pop_front();		
		
        if (1<dx_queue.size())
        {
			_Curvature(dx_queue,dy_queue,dz_queue,resslice); // Lead difference
			
			prev_dx=dx_queue.front(); dx_queue.pop_front();
			prev_dy=dy_queue.front(); dy_queue.pop_front();
			prev_dz=dz_queue.front(); dz_queue.pop_front();
            //kipl::base::InsertSlice(res,resslice,i-1);
			InsertPaddedSlice(resslice,res,i-1,nMargin);
		}
	}

	dx_queue.push_back(prev_dx);
	dy_queue.push_back(prev_dy);
	dz_queue.push_back(prev_dz);

	_Curvature(dx_queue,dy_queue,dz_queue,resslice); // Lead difference
	dx_queue.pop_front();
	dy_queue.pop_front();
	dz_queue.pop_front();
            
    //kipl::base::InsertSlice(res,resslice,N-1);
	InsertPaddedSlice(resslice,res,N-1, nMargin);
  
	return 0;
}

template <typename T>
int ISSfilterQ3D<T>::_FirstDerivate(std::list<kipl::base::TImage<T,2> > &img_queue,
									std::list<kipl::base::TImage<T,2> > &dx_queue,
									std::list<kipl::base::TImage<T,2> > &dy_queue,
									std::list<kipl::base::TImage<T,2> > &dz_queue) // Lag difference
{
	kipl::base::TImage<T,2> dx(img_queue.front().Dims());
	kipl::base::TImage<T,2> dy(img_queue.front().Dims());
	kipl::base::TImage<T,2> dz(img_queue.front().Dims());

	kipl::base::TImage<T,2> &img1=img_queue.back();
	kipl::base::TImage<T,2> &img0=img_queue.front();
    stringstream msg;
    
	int sx=img1.Size(0);
	int sy=img1.Size(1);

    #pragma omp parallel for
    for (int y=0; y<sy; y++)
    {
		// kipl::base::uFQuad d;

		T* pImg0=img1.GetLinePtr(y);
		T* pImgX=img1.GetLinePtr(y)-1;
		T* pImgY=(y!=0) ? img1.GetLinePtr(y-1) : img1.GetLinePtr(1);
		T* pImgZ=img0.GetLinePtr(y);

		T* pDx=dx.GetLinePtr(y);
		T* pDy=dy.GetLinePtr(y);
		T* pDz=dz.GetLinePtr(y);

		T sum2  = static_cast<T>(0);
		T denom = static_cast<T>(0);

        for (int x=0; x<sx; x++)
        {
            // Division by two is omitted since the result is normed later
			if (x==0)
				pDx[0]=pImg0[1]-pImg0[0];
			else
				pDx[x]=pImg0[x]-pImgX[x];

			sum2=pDx[x]*pDx[x];

			if (y==0)
				pDy[x]=pImgY[x]-pImg0[x];
			else
				pDy[x]=pImg0[x]-pImgY[x];

			sum2+=pDy[x]*pDy[x];

			pDz[x]=pImg0[x]-pImgZ[x];

			sum2+=pDz[x]*pDz[x];
            if (sum2!=0)
            {
                denom=static_cast<T>(1)/sqrt(sum2); // Hot spot!!!

                pDx[x]*=denom;
                pDy[x]*=denom;
                pDz[x]*=denom;
            }
		}
	}

	dx_queue.push_back(dx);
	dy_queue.push_back(dy);
	dz_queue.push_back(dz);

	return 0;
}

template <typename T>
kipl::base::TImage<T,2> ISSfilterQ3D<T>::GetPaddedSlice(kipl::base::TImage<T,3> &vol, size_t nIdx, size_t nMargin)
{
    std::vector<size_t> dims={vol.Size(0)+nMargin*2, vol.Size(1)+nMargin*2};
	kipl::base::TImage<T,2> slice(dims);

	slice=static_cast<T>(0);
    for (size_t i=0; i<vol.Size(1); i++)
    {
		memcpy(slice.GetLinePtr(i+nMargin)+nMargin,vol.GetLinePtr(i,nIdx),sizeof(T)*vol.Size(0));
	}

	return slice;
}

template <typename T>
void ISSfilterQ3D<T>::InsertPaddedSlice(kipl::base::TImage<T,2> & slice, kipl::base::TImage<T,3> &vol, size_t nIdx, size_t nMargin)
{
    for (size_t i=0; i<vol.Size(1); i++)
    {
        std::copy_n(slice.GetLinePtr(i+nMargin)+nMargin,vol.Size(0),vol.GetLinePtr(i,nIdx));
	}
}

#ifdef LASTEST_VERSION
template <typename T>
int ISSfilterQ3D<T>::_FirstDerivateSSE(std::list<kipl::base::TImage<T,2> > &img_queue,
									std::list<kipl::base::TImage<T,2> > &dx_queue,
									std::list<kipl::base::TImage<T,2> > &dy_queue,
									std::list<kipl::base::TImage<T,2> > &dz_queue) // Lag difference
{
	kipl::base::TImage<T,2> dx(img_queue.front().Dims());
	kipl::base::TImage<T,2> dy(img_queue.front().Dims());
	kipl::base::TImage<T,2> dz(img_queue.front().Dims());

	kipl::base::TImage<T,2> &img1=img_queue.back();
	kipl::base::TImage<T,2> &img0=img_queue.front();
    stringstream msg;

	int sx=static_cast<int>(img1.Size(0));
	int sy=static_cast<int>(img1.Size(1));


	memset(dy.GetLinePtr(0),0,sx*sizeof(float));
	float *px=dx.GetLinePtr(0);
	float *py=dy.GetLinePtr(0);
	float *pz=dz.GetLinePtr(0);
	float *p0=img0.GetLinePtr(0);
	float *p1=img1.GetLinePtr(0);
	// First line procesing
	px[0]=p1[0]-p1[1];
	py[0]=p1[0]-p1[sx];
	pz[0]=p1[0]-p0[0];

	for (int x=1; x<sx; x++) {
		px[x]=p1[x]-p1[x-1];
		py[x]=p1[x]-p1[x+sx];
		pz[x]=p1[x]-p0[x];


		float sum2=sqrt(px[x]*px[x]+py[x]*py[x]+pz[x]*pz[x]);
		if (sum2!=0.0f) {
			sum2=1.0f/sum2;
			px[x]*=sum2;
			py[x]*=sum2;
			pz[x]*=sum2;
		}
	}


	#pragma omp parallel
	{
		float *pfDx;
		float *pfDy;
		float *pfDz;

		int sx4=(sx+3)/4;
		
#ifndef _MSC_VER
		__m128 * pImg0 = new __m128[sx];
		__m128 * pImgX = new __m128[sx];
		__m128 * pImgY = new __m128[sx];
		__m128 * pImgZ = new __m128[sx];
#else
		__m128 * pImg0 = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgX = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgY = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgZ = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
#endif

		__m128 Sum2 , zero={0.0f,0.0f,0.0f,0.0f};

		// Code for y=0 missing

		#pragma omp for
		for (int y=1; y<sy; y++) {
			memcpy(pImg0,img1.GetLinePtr(y),sx*sizeof(float));
			memcpy(pImgX,img1.GetLinePtr(y)-1,sx*sizeof(float));
			memcpy(pImgY,img1.GetLinePtr(y-1), sx*sizeof(float));
			memcpy(pImgZ,img0.GetLinePtr(y), sx*sizeof(float));

			pfDx = dx.GetLinePtr(y);
			pfDy = dy.GetLinePtr(y);
			pfDz = dz.GetLinePtr(y);

			kipl::base::uFQuad *q=reinterpret_cast<kipl::base::uFQuad *>(pImgX);
			q->q.a=q->q.c;
			// Division by two is omitted since the result is normed later
		    for (int x=0; x<sx4; x++) {
				__m128 tmpX = _mm_sub_ps(pImg0[x], pImgX[x]);
				__m128 tmpY = _mm_sub_ps(pImg0[x], pImgY[x]);
				__m128 tmpZ = _mm_sub_ps(pImg0[x], pImgZ[x]);

//				Sum2 = _mm_sqrt_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(tmpX,tmpX),_mm_mul_ps(tmpY,tmpY)),_mm_mul_ps(tmpZ,tmpZ)));
//				Sum2 = _mm_and_ps(_mm_rcp_ps(Sum2),_mm_cmpgt_ps(Sum2,zero));
				Sum2 = (_mm_add_ps(_mm_add_ps(_mm_mul_ps(tmpX,tmpX),_mm_mul_ps(tmpY,tmpY)),_mm_mul_ps(tmpZ,tmpZ)));
				Sum2 = _mm_and_ps(_mm_rsqrt_ps(Sum2),_mm_cmpgt_ps(Sum2,zero));

				_mm_storeu_ps(pfDx+4*x,_mm_mul_ps(tmpX, Sum2));
				_mm_storeu_ps(pfDy+4*x,_mm_mul_ps(tmpY, Sum2));
				_mm_storeu_ps(pfDz+4*x,_mm_mul_ps(tmpZ, Sum2));
			}

		    for (int x=4*sx4+1; x<sx; x++) {
		    	pfDx[x]=0.0f;
		    	pfDy[x]=0.0f;
		    	pfDz[x]=0.0f;

		    }
		}

#ifndef _MSC_VER
		delete [] pImg0;
		delete [] pImgX;
		delete [] pImgY;
		delete [] pImgZ;
#else
		_aligned_free(pImg0);
		_aligned_free(pImgX);
		_aligned_free(pImgY);
		_aligned_free(pImgZ);
#endif
	}

	if (kipl::base::hasNan(dx))
		logger(kipl::logging::Logger::LogWarning,"dx has nan");
	if (kipl::base::hasNan(dy))
			logger(kipl::logging::Logger::LogWarning,"dy has nan");
	if (kipl::base::hasNan(dz))
			logger(kipl::logging::Logger::LogWarning,"dz has nan");

	dx_queue.push_back(dx);
	dy_queue.push_back(dy);
	dz_queue.push_back(dz);

	return 0;
}


template <typename T>
int ISSfilterQ3D<T>::_Curvature(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	#pragma omp parallel
	{
		T d2x = static_cast<T>(0);
		T d2y = static_cast<T>(0);
		T d2z = static_cast<T>(0);

		const int sx = dx.Size(0);
		const int sy = dx.Size(1);
		int y=0;
		#pragma omp for
		for (y=0; y<sy; y++) {

			T* pDx0=dx.GetLinePtr(y);
			T* pDx1=pDx0+1;

			T* pDy0=(y<sy-1) ? dy.GetLinePtr(y)  : dy.GetLinePtr(y-1);
			T* pDy1=(y<sy-1) ? dy.GetLinePtr(y+1): dy.GetLinePtr(y) ;

			T* pDz0=dz0.GetLinePtr(y);
			T* pDz1=dz1.GetLinePtr(y);

			T* pResult=result.GetLinePtr(y);
			for (int x=0; x<sx; x++) {
				if (x==sx-1)
					d2x=pDx1[x]-pDx0[x-1];
				else
					d2x=pDx1[x]-pDx0[x];

				d2y=pDy1[x]-pDy0[x];
				d2z=pDz1[x]-pDz0[x];
				pResult[x]=static_cast<T>(0.5*(d2x+d2y+d2z));
			}
		}
	}

	if (kipl::base::hasNan(result))
		logger(kipl::logging::Logger::LogWarning,"curvature has NaN");
	return 0;
}

template <typename T>
int ISSfilterQ3D<T>::_CurvatureSSE(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	#pragma omp parallel
	{
		T d2x = static_cast<T>(0);
		T d2y = static_cast<T>(0);
		T d2z = static_cast<T>(0);

		const int sx = dx.Size(0);
		const int sy = dx.Size(1);

		int i=0;
		const int Nx= static_cast<int>(dx.Size())-1;
		T *pResult=result.GetDataPtr();
		T* pDx0=dx.GetDataPtr();
		T* pDx1=pDx0+1;
		#pragma omp for
		for (i=0; i<Nx; i++) {
			pResult[i]=pDx1[i]-pDx0[i];
		}

		const int Ny= static_cast<int>(dx.Size()-dx.Size(0));
		T* pDy0=dy.GetDataPtr();
		T* pDy1=pDy0 + dy.Size(0) ;
		#pragma omp for
		for (i=0; i<Nx; i++) {
			pResult[i]+=pDy1[i]-pDy0[i];
		}
	}

	const int Nz= static_cast<int>(dz0.Size())/4;
	__m128 * pDz0  = reinterpret_cast<__m128 *>(dz0.GetDataPtr());
	__m128 * pDz1  = reinterpret_cast<__m128 *>(dz1.GetDataPtr());
	__m128 * pR128 = reinterpret_cast<__m128 *>(result.GetDataPtr());
	__m128 half={0.5f,0.5f,0.5f,0.5f};

	int i;
	#pragma omp parallel for
	for (i=0; i<Nz; i++) {
		__m128 res=_mm_add_ps(pR128[i],_mm_sub_ps(pDz1[i],pDz0[i]));
		pR128[i]=_mm_mul_ps(res,half);
	}

	return 0;
}

#else
template <typename T>
int ISSfilterQ3D<T>::_FirstDerivateSSE(std::list<kipl::base::TImage<T,2> > &img_queue,
									std::list<kipl::base::TImage<T,2> > &dx_queue,
									std::list<kipl::base::TImage<T,2> > &dy_queue,
									std::list<kipl::base::TImage<T,2> > &dz_queue) // Lag difference
{
    kipl::base::TImage<T,2> dx(img_queue.front().dims());
    kipl::base::TImage<T,2> dy(img_queue.front().dims());
    kipl::base::TImage<T,2> dz(img_queue.front().dims());

	kipl::base::TImage<T,2> &img1=img_queue.back();
	kipl::base::TImage<T,2> &img0=img_queue.front();
    stringstream msg;

	int sx=static_cast<int>(img1.Size(0));
	int sy=static_cast<int>(img1.Size(1));


	memset(dy.GetLinePtr(0),0,sx*sizeof(float));
    float *px  = dx.GetLinePtr(0);
    float *py  = dy.GetLinePtr(0);
    float *pz  = dz.GetLinePtr(0);
    float *p0  = img0.GetLinePtr(0);
    float *p1  = img1.GetLinePtr(0);
	// First line processing
    for (int x=1; x<sx; x++)
    {
        if (x!=0)
        {
			px[x]=p1[x]-p1[x-1];
			py[x]=p1[x]-p1[x+sx];
			pz[x]=p1[x]-p0[x];
		}
        else
        {
			px[0]=p1[0]-p1[1];
			py[0]=p1[0]-p1[sx];
			pz[0]=p1[0]-p0[0];
		}

        float sum2=sqrt(px[x]*px[x]+py[x]*py[x]+pz[x]*pz[x]);
        if (sum2!=0)
        {
			sum2=1.0f/sum2;
			px[x]*=sum2;
			py[x]*=sum2;
			pz[x]*=sum2;
		}


	}


	#pragma omp parallel
	{
		float *pfDx;
		float *pfDy;
		float *pfDz;

		int sx4=(sx+3)/4;

#ifndef _MSC_VER
		__m128 * pImg0 = new __m128[sx];
		__m128 * pImgX = new __m128[sx];
		__m128 * pImgY = new __m128[sx];
		__m128 * pImgZ = new __m128[sx];
#else
		__m128 * pImg0 = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgX = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgY = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
		__m128 * pImgZ = reinterpret_cast<__m128 *>(_aligned_malloc(sx*sizeof(float),16));
#endif

		__m128 Sum2 , zero={0.0f,0.0f,0.0f,0.0f};

		// Code for y=0 missing


		#pragma omp for
        for (int y=1; y<sy; y++)
        {
			memcpy(pImg0,img1.GetLinePtr(y),sx*sizeof(float));
			memcpy(pImgX,img1.GetLinePtr(y)-1,sx*sizeof(float));
			memcpy(pImgY,img1.GetLinePtr(y-1), sx*sizeof(float));
			memcpy(pImgZ,img0.GetLinePtr(y), sx*sizeof(float));

			pfDx = dx.GetLinePtr(y);
			pfDy = dy.GetLinePtr(y);
			pfDz = dz.GetLinePtr(y);

			kipl::base::uFQuad *q=reinterpret_cast<kipl::base::uFQuad *>(pImgX);
			q->q.a=q->q.c;
			// Division by two is omitted since the result is normed later
		//	for (int x=sx4-1; 0<=x ; x--) {
            for (int x=0; x<sx4; x++)
            {
				__m128 tmpX = _mm_sub_ps(pImg0[x], pImgX[x]);
				__m128 tmpY = _mm_sub_ps(pImg0[x], pImgY[x]);
				__m128 tmpZ = _mm_sub_ps(pImg0[x], pImgZ[x]);

				Sum2 = _mm_sqrt_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(tmpX,tmpX),_mm_mul_ps(tmpY,tmpY)),_mm_mul_ps(tmpZ,tmpZ)));
				Sum2 = _mm_and_ps(_mm_rcp_ps(Sum2),_mm_cmpgt_ps(Sum2,zero));

				_mm_storeu_ps(pfDx+4*x,_mm_mul_ps(tmpX, Sum2));
				_mm_storeu_ps(pfDy+4*x,_mm_mul_ps(tmpY, Sum2));
				_mm_storeu_ps(pfDz+4*x,_mm_mul_ps(tmpZ, Sum2));
			}
		}

#ifndef _MSC_VER
		delete [] pImg0;
		delete [] pImgX;
		delete [] pImgY;
		delete [] pImgZ;
#else
		_aligned_free(pImg0);
		_aligned_free(pImgX);
		_aligned_free(pImgY);
		_aligned_free(pImgZ);
#endif
	}
	dx_queue.push_back(dx);
	dy_queue.push_back(dy);
	dz_queue.push_back(dz);

	return 0;
}


template <typename T>
int ISSfilterQ3D<T>::_Curvature(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	#pragma omp parallel
	{
		T d2x = static_cast<T>(0);
		T d2y = static_cast<T>(0);
		T d2z = static_cast<T>(0);

		const int sx = dx.Size(0);
		const int sy = dx.Size(1);
		int y=0;
		#pragma omp for
		for (y=0; y<sy; y++) {

			T* pDx0=dx.GetLinePtr(y);
			T* pDx1=pDx0+1;

			T* pDy0=(y<sy-1) ? dy.GetLinePtr(y)  : dy.GetLinePtr(y-1);
			T* pDy1=(y<sy-1) ? dy.GetLinePtr(y+1): dy.GetLinePtr(y) ;

			T* pDz0=dz0.GetLinePtr(y);
			T* pDz1=dz1.GetLinePtr(y);

			T* pResult=result.GetLinePtr(y);
			for (int x=0; x<sx; x++) {
				if (x==sx-1)
					d2x=pDx1[x]-pDx0[x-1];
				else
					d2x=pDx1[x]-pDx0[x];

				d2y=pDy1[x]-pDy0[x];
				d2z=pDz1[x]-pDz0[x];
				pResult[x]=static_cast<T>(0.5*(d2x+d2y+d2z));
			}
		}
	}

	return 0;
}

template <typename T>
int ISSfilterQ3D<T>::_CurvatureSSE(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	#pragma omp parallel
	{
		T d2x = static_cast<T>(0);
		T d2y = static_cast<T>(0);
		T d2z = static_cast<T>(0);

		// const int sx = dx.Size(0);
		// const int sy = dx.Size(1);

		int i=0;
		const int Nx= static_cast<int>(dx.Size())-1;
		T *pResult=result.GetDataPtr();
		T* pDx0=dx.GetDataPtr();
		T* pDx1=pDx0+1;
		#pragma omp for
		for (i=0; i<Nx; ++i) 
		{
			pResult[i]=pDx1[i]-pDx0[i];
		}

		const int Ny= static_cast<int>(dx.Size()-dx.Size(0));
		T* pDy0=dy.GetDataPtr();
		T* pDy1=pDy0 + dy.Size(0) ;
		#pragma omp for
		for (i=0; i<Ny; ++i) // Replaced Nx by Ny was it a typo originally?
		{
			pResult[i]+=pDy1[i]-pDy0[i];
		}


	}

	const int Nz= static_cast<int>(dz0.Size())/4;
	__m128 * pDz0  = reinterpret_cast<__m128 *>(dz0.GetDataPtr());
	__m128 * pDz1  = reinterpret_cast<__m128 *>(dz1.GetDataPtr());
	__m128 * pR128 = reinterpret_cast<__m128 *>(result.GetDataPtr());
	__m128 half={0.5f,0.5f,0.5f,0.5f};

	int i;
	#pragma omp parallel for firstprivate(half,pDz0,pDz1,pR128)
	for (i=0; i<Nz; i++) {
		__m128 res=_mm_add_ps(pR128[i],_mm_sub_ps(pDz1[i],pDz0[i]));
		pR128[i]=_mm_mul_ps(res,half);
	}

	return 0;
}
#endif
template <typename T>
int ISSfilterQ3D<T>::_LeadRegularize(kipl::base::TImage<T,3> *img, Direction dir)
{
    T *pA = nullptr;
    T *pB = nullptr;
	
	size_t const * const dims=img->Dims();
	size_t sxy=dims[0]*dims[1];
	
	switch (dir) {
	case dirX :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<dims[1]; y++) {
				pA=img->GetLinePtr(y,z);
				
				for (size_t x=0; x<(dims[0]-1); x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pA[x+1]));
				}
			}
		}
		break;
	case dirY :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<(dims[1]-1); y++) {
				pA=img->GetLinePtr(y,z);
				pB=img->GetLinePtr(y+1,z);
				for (size_t x=0; x<dims[0]; x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pB[x]));
				}
			}
		}
		break;
	case dirZ :
		
		for (size_t z=0; z<(dims[2]-1); z++) {
			pA=img->GetLinePtr(0,z);
			pB=img->GetLinePtr(0,z+1);
			
			for (size_t i=0; i<sxy; i++) {
					pA[i]=kipl::math::sign(pA[i])*min(std::abs(pA[i]),std::abs(pB[i]));
				}
			}
		
		break;
	}
	
	return 0;
}


template <typename T>
int ISSfilterQ3D<T>::_LagRegularize(kipl::base::TImage<T, 3> *img, Direction dir)
{
	T *pA=NULL;
	T *pB=NULL;
	
	size_t const * const dims=img->Dims();
	size_t sxy=dims[0]*dims[1];
	
	switch (dir) {
	case dirX :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<dims[1]; y++) {
				pA=img->GetLinePtr(y,z)+1;
				
				for (size_t x=0; x<(dims[0]-1); x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pA[x-1]));
				}
			}
		}
		break;
	case dirY :
		for (size_t z=0; z<dims[2]; z++) {
			for (size_t y=0; y<(dims[1]-1); y++) {
				pA=img->GetLinePtr(y+1,z);
				pB=img->GetLinePtr(y,z);
				for (size_t x=0; x<dims[0]; x++) {
					pA[x]=kipl::math::sign(pA[x])*min(std::abs(pA[x]),std::abs(pB[x]));
				}
			}
		}
		break;
	case dirZ :
		
		for (size_t z=0; z<(dims[2]-1); z++) {
			pA=img->GetLinePtr(0,z+1);
			pB=img->GetLinePtr(0,z);
			
			for (size_t i=0; i<sxy; i++) {
				pA[i]=kipl::math::sign(pA[i])*min(std::abs(pA[i]),std::abs(pB[i]));
			}
		}
		break;
	}
	
	return 0;
}



template <typename T>
double ISSfilterQ3D<T>::_ComputeEntropy(kipl::base::TImage<T,3> &img)
{
	kipl::base::Histogram(img.GetDataPtr(),img.Size(),this->hist,512);

	double e=kipl::base::Entropy(this->hist,512);
	std::ostringstream msg;

	msg<<"Entropy = "<<e;
	logger(kipl::logging::Logger::LogMessage,msg.str());
	return e;
}

template <typename T>
bool ISSfilterQ3D<T>::updateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }

    return false;
}

}

#endif /*ISSFILTER_HPP_*/
