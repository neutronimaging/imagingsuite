//<LICENCE>

#ifndef ISSFILTERQ3DP_HPP_
#define ISSFILTERQ3DP_HPP_

#include <iomanip>
#include <omp.h>
#include <sstream>
#include <iostream>
#include <emmintrin.h>

#include "../../base/timage.h"
#include "../../base/tsubimage.h"

#include "../../math/mathfunctions.h"
#include "../../base/core/quad.h"

#include "../../io/io_tiff.h"
#include "../../strings/filenames.h"


namespace akipl { namespace scalespace {

template <typename T>
int ISSfilterQ3Dp<T>::Process(kipl::base::TImage<T,3> &img, double dTau, double dLambda, double dAlpha, int nN, bool saveiterations, std::string itpath)
{
    if (error!=NULL)
        delete [] error;
    
    error=new T[nN];
	switch (eInitialImage) {
		case InitialImageOriginal: m_f=img; m_f.Clone();break;
		case InitialImageZero: m_f.Resize(img.Dims()); m_f=static_cast<T>(0); break;
	}

	

	m_dTau=dTau; 
	m_dLambda=dLambda;
	m_dAlpha=dAlpha;
	m_v.Resize(img.Dims());
	m_v=static_cast<T>(0);

	std::ostringstream msg;
	
	//img=0.0f;
	for (int i=0; i<nN	; i++) {	
		msg.str("");
		msg<<"Processing iteration "<<i+1;
		logger(kipl::logging::Logger::LogMessage,msg.str());

		error[i]=_SolveIterationSSE(img);

		if (saveiterations) {
			std::string fname, mask,ext;
			kipl::strings::filenames::MakeFileName("iteration_####.tif",i, fname, ext, '#', '0');
			std::string p=itpath;
			kipl::strings::filenames::CheckPathSlashes(p,true);
			fname=p+fname;
			kipl::base::TImage<T,2> slice=ExtractSlice(img, img.Size(2)/2, kipl::base::ImagePlaneXY);
            kipl::io::WriteTIFF(slice,fname.c_str(), kipl::base::Float32);

		}

	}

	return nN;
}

template <typename T>
T ISSfilterQ3Dp<T>::_SolveIterationSSE(kipl::base::TImage<T,3> &img)
{
	kipl::base::TImage<T,3> p;

	_P(img,p);

    ptrdiff_t i;

    double sum2=0.0f;
    const T lambda=static_cast<T>(m_dLambda);

	float fTau=static_cast<T>(m_dTau);
    float fTauAlpha=static_cast<T>(m_dTau*m_dAlpha);

	#pragma omp parallel reduction(+:sum2) num_threads(m_nThreads)
	{
        ptrdiff_t N=(img.Size()+3)/4;
        __m128 tempFU;
		__m128 xTau      = _mm_set1_ps(fTau);
		__m128 xTauAlpha = _mm_set1_ps(fTauAlpha);
		__m128 xLambda   = _mm_set1_ps(lambda);

        __m128 *pU=reinterpret_cast<__m128 *>(img.GetDataPtr());
        __m128 *pV=reinterpret_cast<__m128 *>(m_v.GetDataPtr());
        __m128 *pP=reinterpret_cast<__m128 *>(p.GetDataPtr());
        __m128 *pF=reinterpret_cast<__m128 *>(m_f.GetDataPtr());

		kipl::base::uFQuad qLocalSum;
		qLocalSum.xmm=_mm_set1_ps(0.0f);

		__m128 * ru=pU;
		__m128 * rv=pV;
		#pragma omp for
		for (i=0; i<N ; i++) {
			// tempFU=pF[i]-pU[i];
			ru=pU+i;
			tempFU=_mm_sub_ps(pF[i],*ru);
			// localsum+=diff*diff;
			qLocalSum.xmm=_mm_add_ps(qLocalSum.xmm,
							  _mm_mul_ps(tempFU,tempFU));

			// pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
			rv=pV+i;
			*ru=_mm_add_ps(*ru,
					_mm_mul_ps(xTau,
						_mm_add_ps(pP[i],
							_mm_mul_ps(xLambda,
								_mm_add_ps(tempFU,*rv)))));

			// pV[i]+=dTauAlpha*tempFU;
			*rv=_mm_add_ps(*rv,_mm_mul_ps(tempFU,xTauAlpha));
		}

//        #pragma omp for
//        for (i=0; i<N ; i++) {
//			// tempFU=pF[i]-pU[i];
//
//        	tempFU=_mm_sub_ps(pF[i],pU[i]);
//			// pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
//			pU[i]=_mm_add_ps(pU[i],
//					_mm_mul_ps(xTau,
//						_mm_add_ps(pP[i],
//							_mm_mul_ps(xLambda,
//								_mm_add_ps(tempFU,pV[i])))));
//
//			// pV[i]+=dTauAlpha*tempFU;
//			pV[i]=_mm_add_ps(pV[i],_mm_mul_ps(tempFU,xTauAlpha));
//
//			// diff=static_cast<double>(pU[i]-pF[i]);
//			//xDiff=_mm_sub_ps(pU[i],pF[i]);
//			// localsum+=diff*diff;
//			//qLocalSum.xmm=_mm_add_ps(qLocalSum.xmm,_mm_mul_ps(xDiff,xDiff));
//			qLocalSum.xmm=_mm_add_ps(qLocalSum.xmm,_mm_mul_ps(tempFU,tempFU));
//        }
		 
        sum2+=qLocalSum.q.a+qLocalSum.q.b+qLocalSum.q.c+qLocalSum.q.d;
    }

	return static_cast<T>(sum2);
}

template <typename T>
T ISSfilterQ3Dp<T>::_SolveIteration(kipl::base::TImage<T,3> &img)
{
	kipl::base::TImage<T,3> p;

	_P(img,p);
    ptrdiff_t i;

    double sum2=0.0f;
    const T lambda=static_cast<T>(m_dLambda);

    #pragma omp parallel reduction(+:sum2) num_threads(m_nThreads)
    {
    	double localsum=0.0;
    	int TID=omp_get_thread_num();
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
        for (i=0; i<N ; i++) {
            tempFU=pF[i]-pU[i];
            pU[i]+=fTau*(pP[i]+lambda*(tempFU+pV[i]));
            pV[i]+=dTauAlpha*tempFU;
			diff=static_cast<double>(pU[i]-pF[i]);
            localsum+=diff*diff;
        }
        sum2+=localsum;
    }

	return static_cast<T>(sum2);
}
	

template <typename T>
int ISSfilterQ3Dp<T>::_P(kipl::base::TImage<T,3> &img, kipl::base::TImage<T,3> &res)
{
	const int N=static_cast<int>(img.Size(2));
	res.Resize(img.Dims());

	int nthreads=omp_get_num_threads();
	int block=static_cast<int>(ceil(static_cast<float>(N)/static_cast<float>(m_nThreads)));


	std::map<int, kipl::base::TImage<T,2> > dx_boundary;
	std::map<int, kipl::base::TImage<T,2> > dy_boundary;
	std::map<int, kipl::base::TImage<T,2> > dz_boundary;
	#pragma omp parallel num_threads(m_nThreads)
	{
		size_t sxy=img.Size(0)*img.Size(1);
		T* pImg=img.GetDataPtr();
		T* pRes=res.GetDataPtr();

		int TID=omp_get_thread_num();

		kipl::base::TImage<T,2> prev_dx;
		kipl::base::TImage<T,2> prev_dy;
		kipl::base::TImage<T,2> prev_dz;

		#pragma omp single
		{
			nthreads=omp_get_num_threads();
			block=static_cast<int>(ceil(static_cast<float>(N)/static_cast<float>(nthreads)));
		}

		int first=TID*block;
		int last=(TID+1)*block;

		if (img.Size(2)<last) last=img.Size(2);

		std::list<kipl::base::TImage<T,2> > img_queue;
		std::list<kipl::base::TImage<T,2> > dx_queue;
		std::list<kipl::base::TImage<T,2> > dy_queue;
		std::list<kipl::base::TImage<T,2> > dz_queue;

		kipl::base::TImage<T,2> dx,dy,dz, d2x, d2y,d2z;

		kipl::base::TImage<T,2> resslice(res.Dims());

		kipl::base::TImage<T,2> slice(img.Dims());
		memcpy(slice.GetDataPtr(),pImg+abs(first-1)*sxy,sxy*sizeof(T));

		img_queue.push_back(slice);		
		
		for (int i=first; i<last;i++) {
			memcpy(slice.GetDataPtr(),pImg+i*sxy,sxy*sizeof(T));
			slice.Clone();
			img_queue.push_back(slice);

			_FirstDerivateSSE2(img_queue,dx_queue,dy_queue,dz_queue); // Lag difference
			img_queue.pop_front();

			if (1<dx_queue.size()) {
				_CurvatureSSE(dx_queue,dy_queue,dz_queue,resslice); // Lead difference
				prev_dx=dx_queue.front(); dx_queue.pop_front();
				prev_dy=dy_queue.front(); dy_queue.pop_front();
				prev_dz=dz_queue.front(); dz_queue.pop_front();

				memcpy(pRes+(i-1)*sxy,resslice.GetDataPtr(),sxy*sizeof(T));
			}
			else {
					dx_boundary[TID]=dx_queue.front(); dx_boundary[TID].Clone();
					dy_boundary[TID]=dy_queue.front(); dy_boundary[TID].Clone();
					dz_boundary[TID]=dz_queue.front(); dz_boundary[TID].Clone();
			}
		}

	//	#pragma omp barrier // Ensure that all threads are ready to process the last slice
		if (TID<nthreads-1) {
			dx_queue.push_back(dx_boundary[TID+1]);
			dy_queue.push_back(dy_boundary[TID+1]);
			dz_queue.push_back(dz_boundary[TID+1]);
		}
		else {
			dx_queue.push_back(prev_dx);
			dy_queue.push_back(prev_dy);
			dz_queue.push_back(prev_dz);
		}
		_CurvatureSSE(dx_queue,dy_queue,dz_queue,resslice); // Lead difference
		dx_queue.pop_front();
		dy_queue.pop_front();
		dz_queue.pop_front();

		memcpy(pRes+(last-1)*sxy,resslice.GetDataPtr(),sxy*sizeof(T));
	}
	return 0;
}

template <typename T>
int ISSfilterQ3Dp<T>::_FirstDerivate(std::list<kipl::base::TImage<T,2> > &img_queue,
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

    #pragma omp parallel for num_threads(m_nThreads)
	for (int y=0; y<sy; y++) {
		kipl::base::uFQuad d;

		T* pImg0=img1.GetLinePtr(y);
		T* pImgX=img1.GetLinePtr(y)-1;
		T* pImgY=(y!=0) ? img1.GetLinePtr(y-1) : img1.GetLinePtr(1);
		T* pImgZ=img0.GetLinePtr(y);

		T* pDx=dx.GetLinePtr(y);
		T* pDy=dy.GetLinePtr(y);
		T* pDz=dz.GetLinePtr(y);

		T sum2  = static_cast<T>(0);
		T denom = static_cast<T>(0);

		for (int x=0; x<sx; x++) {
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
            if (sum2!=0) {
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
int ISSfilterQ3Dp<T>::_FirstDerivateSSE(std::list<kipl::base::TImage<T,2> > &img_queue,
									std::list<kipl::base::TImage<T,2> > &dx_queue,
									std::list<kipl::base::TImage<T,2> > &dy_queue,
									std::list<kipl::base::TImage<T,2> > &dz_queue) // Lag difference
{
	kipl::base::TImage<T,2> dx(img_queue.front().Dims());
	kipl::base::TImage<T,2> dy(img_queue.front().Dims());
	kipl::base::TImage<T,2> dz(img_queue.front().Dims());
	dx[0]=0.0f;
	dy[0]=0.0f;
	dz[0]=0.0f;

	kipl::base::TImage<T,2> &img1=img_queue.back();
	kipl::base::TImage<T,2> &img0=img_queue.front();
    stringstream msg;

	int sx=static_cast<int>(img1.Size(0));
	int sy=static_cast<int>(img1.Size(1));


//	memset(dy.GetLinePtr(0),0,sx*sizeof(float));
	float *px=dx.GetLinePtr(0);
	float *py=dy.GetLinePtr(0);
	float *pz=dz.GetLinePtr(0);
	float *p0=img0.GetLinePtr(0);
	float *p1=img1.GetLinePtr(0);
	// First line procesing
	for (int x=1; x<sx; x++) {
		if (x!=0) {
			px[x]=p1[x]-p1[x-1];
			py[x]=p1[x]-p1[x+sx];
			pz[x]=p1[x]-p0[x];
		}
		else {
			px[0]=p1[0]-p1[1];
			py[0]=p1[0]-p1[sx];
			pz[0]=p1[0]-p0[0];
		}
		float sum2=sqrt(px[x]*px[x]+py[x]*py[x]+pz[x]*pz[x]);
		if (sum2!=0) {
			sum2=1.0f/sum2;
			px[x]*=sum2;
			py[x]*=sum2;
			pz[x]*=sum2;
		}
	}

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
		for (int x=0; x<sx4 ; x++) {
			__m128 tmpX = _mm_sub_ps(pImg0[x], pImgX[x]);
			__m128 tmpY = _mm_sub_ps(pImg0[x], pImgY[x]);
			__m128 tmpZ = _mm_sub_ps(pImg0[x], pImgZ[x]);

			Sum2 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(tmpX,tmpX),_mm_mul_ps(tmpY,tmpY)),_mm_mul_ps(tmpZ,tmpZ));
			Sum2 = _mm_and_ps(_mm_rsqrt_ps(Sum2),_mm_cmpgt_ps(Sum2,zero));

			int pos=4*x;
			_mm_storeu_ps(pfDx+pos,_mm_mul_ps(tmpX, Sum2));
			_mm_storeu_ps(pfDy+pos,_mm_mul_ps(tmpY, Sum2));
			_mm_storeu_ps(pfDz+pos,_mm_mul_ps(tmpZ, Sum2));
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
	dx_queue.push_back(dx);
	dy_queue.push_back(dy);
	dz_queue.push_back(dz);

	return 0;
}



template <typename T>
int ISSfilterQ3Dp<T>::_FirstDerivateSSE2(std::list<kipl::base::TImage<T,2> > &img_queue,
									std::list<kipl::base::TImage<T,2> > &dx_queue,
									std::list<kipl::base::TImage<T,2> > &dy_queue,
									std::list<kipl::base::TImage<T,2> > &dz_queue) // Lag difference
{
	kipl::base::TImage<T,2> dx(img_queue.front().Dims());
	kipl::base::TImage<T,2> dy(img_queue.front().Dims());
	kipl::base::TImage<T,2> dz(img_queue.front().Dims());
	dx[0]=0.0f;
	dy[0]=0.0f;
	dz[0]=0.0f;

	kipl::base::TImage<T,2> &img1=img_queue.back();
	kipl::base::TImage<T,2> &img0=img_queue.front();
    stringstream msg;

	int sx=static_cast<int>(img1.Size(0));

	float *px=dx.GetDataPtr();
	float *py=dy.GetDataPtr();
	float *pz=dz.GetDataPtr();
	float *p0=img0.GetDataPtr();
	float *p1=img1.GetDataPtr();
	// First line procesing
	for (int x=1; x<sx; x++) {
		if (x!=0) {
			px[x]=p1[x]-p1[x-1];
			py[x]=p1[x]-p1[x+sx];
			pz[x]=p1[x]-p0[x];
		}
		else {
			px[0]=p1[0]-p1[1];
			py[0]=p1[0]-p1[sx];
			pz[0]=p1[0]-p0[0];
		}
		float sum2=sqrt(px[x]*px[x]+py[x]*py[x]+pz[x]*pz[x]);
		if (sum2!=0) {
			sum2=1.0f/sum2;
			px[x]*=sum2;
			py[x]*=sum2;
			pz[x]*=sum2;
		}
	}
		
	__m128 Sum2 , zero={0.0f,0.0f,0.0f,0.0f};
	__m128 orig, tmpX, tmpY, tmpZ;
	int N=img0.Size();
	float *pImgX=p1-1;
	float *pImgY=p1-sx;
	float *pImgZ=p0;

	for (int i=sx; i<N; i+=4) {
		// Division by two is omitted since the result is normed later
			orig=_mm_loadu_ps(p1+i);
			tmpX = _mm_sub_ps(orig, _mm_loadu_ps(pImgX+i));
			tmpY = _mm_sub_ps(orig, _mm_loadu_ps(pImgY+i));
			tmpZ = _mm_sub_ps(orig, _mm_loadu_ps(pImgZ+i));

			Sum2 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(tmpX,tmpX),_mm_mul_ps(tmpY,tmpY)),_mm_mul_ps(tmpZ,tmpZ));
			Sum2 = _mm_and_ps(_mm_rsqrt_ps(Sum2),_mm_cmpgt_ps(Sum2,zero));

			_mm_storeu_ps(px+i,_mm_mul_ps(tmpX, Sum2));
			_mm_storeu_ps(py+i,_mm_mul_ps(tmpY, Sum2));
			_mm_storeu_ps(pz+i,_mm_mul_ps(tmpZ, Sum2));
	}

	dx_queue.push_back(dx);
	dy_queue.push_back(dy);
	dz_queue.push_back(dz);

	return 0;
}


template <typename T>
int ISSfilterQ3Dp<T>::_Curvature(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	T d2x = static_cast<T>(0);
	T d2y = static_cast<T>(0);
	T d2z = static_cast<T>(0);

	const int sx = dx.Size(0);
	const int sy = dx.Size(1);
	int y=0;
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
			pResult[x]=0.5f*(d2x+d2y+d2z);
		}
	}

	return 0;
}

template <typename T>
int ISSfilterQ3Dp<T>::_CurvatureSSE(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result)
{
	kipl::base::TImage<T,2> & dx  = dx_queue.front();
	kipl::base::TImage<T,2> & dy  = dy_queue.front();
	kipl::base::TImage<T,2> & dz0 = dz_queue.front();
	kipl::base::TImage<T,2> & dz1 = dz_queue.back();

	const int sx = dx.Size(0);
	const int sy = dx.Size(1);

	int N=dx.Size()-dx.Size(0);

	T* pDx0=dx.GetDataPtr();
	T* pDx1=pDx0+1;

	T* pDy0=dy.GetDataPtr();
	T* pDy1=dy.GetLinePtr(1) ;

	T* pDz0=dz0.GetDataPtr();
	T* pDz1=dz1.GetDataPtr();
	T* pRes=result.GetDataPtr();
	int i=0;
	__m128 res;
	for (i=0; i<N; i+=4) {
		//pRes[i]=0.5f*(pDx1[i]-pDx0[i]+pDy1[i]-pDy0[i]+pDz1[i]-pDz0[i]);	
		//res=_mm_sub_ps(_mm_loadu_ps(pDx1+i),_mm_load_ps(pDx0+i));
		res=_mm_add_ps(_mm_add_ps(
				           _mm_sub_ps(_mm_loadu_ps(pDx1+i),_mm_load_ps(pDx0+i)),
				           _mm_sub_ps(_mm_loadu_ps(pDy1+i),_mm_load_ps(pDy0+i))),
				_mm_sub_ps(_mm_load_ps(pDz1+i),_mm_load_ps(pDz0+i)));
		_mm_store_ps(pRes+i,res);
	}

	pDx0=dx.GetLinePtr(sy-1);
	pDx1=pDx0+1;

	pDy0=dy.GetLinePtr(sy-1);
	pDy1=dy.GetLinePtr(sy-2) ;

	pDz0=dz0.GetLinePtr(sy-1);
	pDz1=dz1.GetLinePtr(sy-1);
	pRes=result.GetLinePtr(sy-1);

	for (i=0; i<sx-1; i++)
		pRes[i]=0.5f*(pDx1[i]-pDx0[i]+pDy1[i]-pDy0[i]+pDz1[i]-pDz0[i]);	


	i=sx-1;
	pRes[i]=0.5f*(pDx0[i-1]-pDx0[i]+pDy1[i]-pDy0[i]+pDz1[i]-pDz0[i]);
	return 0;
}


template <typename T>
int ISSfilterQ3Dp<T>::_LeadRegularize(kipl::base::TImage<T,3> *img, Direction dir)
{
	T *pA=NULL;
	T *pB=NULL;
	
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
int ISSfilterQ3Dp<T>::_LagRegularize(kipl::base::TImage<T, 3> *img, Direction dir)
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


}}

#endif /*ISSFILTER_HPP_*/
