#ifndef IMAGE_STATISTICS_HPP_
#define IMAGE_STATISTICS_HPP_
#include <cmath>
#include <emmintrin.h>
#include <iostream>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <limits>

namespace kipl { namespace math {

template <typename T, size_t N>
double RegionMean(kipl::base::TImage<T,N> img, size_t const * const region)
{
	double sum=0.0;
	T *pImg=NULL;
	size_t startX=0, startY=0, startZ=0;
	size_t endX=0, endY=0, endZ=0;
	switch (N) {
	case 1: 
		pImg=img.GetDataPtr();
		startX = std::min(img.Size(0)-1,region[0]);
		endX=std::min(img.Size(0),region[1]);
		for (size_t i=startX; i<=endX; i++)
			sum+=pImg[i];
		return sum/(endX-startX+1);
	case 2:
		startX = std::min(img.Size(0)-1,region[0]);
		startY = std::min(img.Size(1)-1,region[1]);
		endX   = std::min(img.Size(0)-1,region[2]);
		endY   = std::min(img.Size(1)-1,region[3]);

		for (size_t y=startY; y<=endY; y++) {
			pImg=img.GetLinePtr(y);
			for (size_t x=startY; x<=endX; x++)
					sum+=pImg[x];
		}
			
		return sum/((endX-startX+1)*(endY-startX+1));
		
	case 3:
		startX = std::min(img.Size(0)-1,region[0]);
		startY = std::min(img.Size(1)-1,region[1]);
		startZ = std::min(img.Size(2)-1,region[2]);
		endX   = std::min(img.Size(0)-1,region[3]);
		endY   = std::min(img.Size(1)-1,region[4]);
		endZ   = std::min(img.Size(2)-1,region[5]);
		for (size_t z=startZ; z<=endZ; z++) {
			for (size_t y=startY; y<=endY; y++) {
				pImg=img.GetLinePtr(y);
				for (size_t x=startX; x<=endX; x++)
						sum+=pImg[x];
			}
		}
		return sum/((endX-startX+1)*(endY-startY+1)*(endZ-startZ+1));
	}

	return 0.0;
	 
}

template <typename T>
double RegionMean(T const * const data, size_t width, size_t height, size_t stride)
{
	double sum=0.0;
	size_t pos=0;
	for (size_t y=0; y<height; y++) {
		pos=y*stride;
		for (size_t x=0; x<width; x++) {
			sum+=static_cast<double>(data[pos+x]);
		}
	}

	return sum/static_cast<double>(width*height);
}


template <typename T>
T sum(T * const begin, T const * const end)
{
	T val=static_cast<T>(0);
	for  (T * const it=begin; it<end; it++) {
		val+=*it;
	}
	
	return val;	
}

template <typename T>
double sum2(T * const begin, T const * const end)
{
	double val=0.0;
	double x;
	for  (T * it=begin; it<end; it++) {
		x=static_cast<double>(*it);
		val+=x*x;
	}

	return val;
}
template <typename T>
double diffsum(T * const beginA, T const * const endA, T * const beginB)
{
//	if (typeid(T)==typeid(float)) {
//	
//	}
	T * itA=beginA;
	T * itB=beginB;
	double sum=0.0;
	for (; itA<endA; itA++,itB++) {
		sum+=static_cast<double>(*itA-*itB);
	}

	return sum;
}

template <typename T>
double diffsum2(T * const beginA, T const * const endA, T * const beginB)
{
	T * itA=beginA;
	T * itB=beginB;
	double sum=0.0;
	double diff=0.0;

	for (; itA<endA; itA++,itB++) {
		diff=static_cast<double>(*itA-*itB);
		sum+=diff*diff;
	}

	return sum;

}

template <typename T>
double diffsum(T * const beginA, T const * const endA, const T val)
{
	T * itA=beginA;
	double sum=0.0;
	for (; itA<endA; itA++) {
		sum+=static_cast<double>(*itA-val);
	}

	return sum;
}

template <typename T>
double diffsum2(T * const beginA, T const * const endA, const T val)
{
	T * itA=beginA;
	double sum=0.0;
	double diff=0.0;

	for (; itA<endA; itA++) {
		diff=static_cast<double>(*itA-val);
		sum+=diff*diff;
	}

	return sum;
}

template <typename T>
void minmax(T const * const data, const size_t N, T *minval, T *maxval)
{
//	*minval=std::numeric_limits<T>::max();
//	*maxval=-std::numeric_limits<T>::max();

    std::pair<const T*, const T*> mm = minmax_element(data, data+N);

    *minval=*(mm.first);
    *maxval=*(mm.second);
//	#pragma omp parallel
//	{
//		T mi=*minval;
//		T ma=*maxval;

//		ptrdiff_t NN=static_cast<ptrdiff_t>(N);

//		#pragma omp for
//		for (ptrdiff_t i=0; i<NN; i++) {
//			if (data[i]==data[i]) { // Exclude NaNs from the search
//				if (ma<data[i])
//					ma=data[i];
//				else if (data[i]<mi)
//					mi=data[i];
//			}
//		}
//		#pragma omp critical
//		{
//			*minval=     mi < *minval ? mi : *minval;
//			*maxval=*maxval <  ma     ? ma : *maxval ;
//		}
//	}
}

template <typename T>
std::pair<double,double> statistics(T const * const x, const size_t N)
{
	ptrdiff_t i=0;
	ptrdiff_t uN=static_cast<ptrdiff_t>(N);
	double mean=0.0;
	double var=0.0;
	double delta=0.0;
	double value=0.0;

	for (i=0; i<uN; )  // this version is more robust for large N
	{
		value=x[i];
		delta = value - mean;
		i++;
		mean += delta/i;
		var  += delta*(value-mean);
	}

    var=var/(N);

	return std::pair<double,double>(mean,std::sqrt(static_cast<double>(var)));
}

template <typename T>
void statistics(T const * const x,double *m, double *s, const size_t *dims, size_t nDims, bool bAllocate)
{
    ptrdiff_t nSlices=static_cast<ptrdiff_t>(dims[nDims-1]);
    size_t N=dims[0];

    for (size_t i=1; i<nDims-1; i++) {
        N*=dims[i];
    }

    if (bAllocate) {
        if (m!=NULL)
            delete [] m;

        m=new double[nSlices];

        if (s!=NULL)
            delete [] s;

        s=new double[nSlices];
    }


    ptrdiff_t uN=static_cast<ptrdiff_t>(N);
    #pragma omp parallel for
    for (ptrdiff_t slice=0; slice<nSlices; slice++) {
        ptrdiff_t i=0;
        double mean=0.0;
        double var=0.0;
        double delta=0.0;
        double value=0.0;

        for (i=0; i<uN; )  // this version is more robust for large N
        {
            value=x[i];
            delta = value - mean;
            i++;
            mean += delta/i;
            var  += delta*(value-mean);
        }

        s[slice]=std::sqrt(var/static_cast<double>(N-1));
        m[slice]=mean;
    }
}

}}

#endif /*STATISTICS_HPP_*/
