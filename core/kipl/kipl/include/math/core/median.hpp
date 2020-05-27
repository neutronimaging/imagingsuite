#ifndef MEDIAN_HPP_
#define MEDIAN_HPP_

#include <algorithm>
#include <cstring>
#include "../../algorithms/sortalg.h"
#include "../median.h"
#include <vector>

namespace kipl { namespace math {

template<class T, class S>
void median(T *v,const size_t n, S * med)
{
        T *tmp=new T[n];
       memcpy(tmp,v,n*sizeof(T));
        heapsort(tmp,static_cast<int>(n));
        *med=0;
        if ((n & 1) == 1)
            *med=tmp[n/2];
        else
            *med=static_cast<T>(0.5*(tmp[(n/2)-1]+tmp[n/2]));

        delete [] tmp;
}

template<class T, class S>
void median(const std::vector<T> &v, S * med)
{
   std::vector<T> tmp(v.begin(),v.end());

   std::sort(tmp.begin(), tmp.end());

   size_t n=v.size();

   if ((n & 1) == 1)
        *med=tmp[n/2];
    else
        *med=static_cast<T>(0.5*(tmp[(n/2)-1]+tmp[n/2]));
}

template<class T, class S>
void median_STL(T *v,const size_t n, S * med)
{
        size_t mid=n/2;
        
        *med=0;
        if ((n & 1) == 1) {
        	std::partial_sort(v,v+mid+1,v+n);
        	*med=v[mid];
        }
        else {
        	std::partial_sort(v,v+mid+1,v+n);
        	*med=static_cast<T>(0.5*(v[mid-1]+v[mid]));
        }
}

#define PIX_SORT(a,b) { if ((a)>(b)) swap(a,b); }

template <class T>
void opt_med9(T * p, T *med)
{
	PIX_SORT(p[1], p[2]) ;
	PIX_SORT(p[0], p[1]) ;
	PIX_SORT(p[1], p[2]) ;
	
    PIX_SORT(p[4], p[5]) ; 
    PIX_SORT(p[3], p[4]) ;
    PIX_SORT(p[4], p[5]) ;
    
    PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[7], p[8]) ;
    
    PIX_SORT(p[0], p[3]) ; 
    PIX_SORT(p[3], p[6]) ;
    
    PIX_SORT(p[5], p[8]) ;
    PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[1], p[4]) ; 
    PIX_SORT(p[4], p[7]) ; 
    PIX_SORT(p[4], p[2]) ; 
    PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ; 
    *med=(p[4]) ;
}

#undef PIX_SORT

/// \brief Median computed using the quickselect algorithm
///
/// A median algorithm base on based on the quickselect algorithm described in
/// "Numerical recipes in C", Second Edition,  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
/// This code by Nicolas Devillard - 1998. Public domain.
/// Please note that the orignal data order is destroyed.
/// 
/// \param arr Array containing data
/// \param n number of elements in the array
/// \param med The median value of the array
template<class T, class S>
void median_quick_select(T *arr, const size_t n, S *med)
{
	if (n==9) {
		opt_med9(arr,med);
		return;
	}
		
	int low, high ;
	int median;
	int middle, ll, hh; low = 0 ;
	high = n-1 ;
	median = (low + high) >> 1;
	middle=median;

    T lowval  = arr[low];
    T midval  = arr[middle];
    T highval = arr[high];
    T tmpval  = 0;
	for (;;) {
        if (high <= low) {// One element only
        		*med=static_cast<S>(arr[median]);
                return; 
        }
        if (high == low + 1) { // Two elements only
        	if (arr[low] > arr[high]) 
            	swap(arr[low], arr[high]) ;
        	*med= static_cast<S>(arr[median]);
            return; 
        }
        // Find median of low, middle and high items; swap into position low
        middle = (low + high) >> 1;

        lowval  = arr[low];
        midval  = arr[middle];
        highval = arr[high];

        if (midval > highval) swap(midval, highval) ; 
        if (lowval > highval) swap(lowval, highval) ;
        if (midval > lowval)  swap(midval, lowval) ;   // Swap low item
                                                       // (now in position middle) into position (low+1)
        arr[low]=lowval;
        arr[middle]=arr[low+1];
        arr[low+1]=midval;
        arr[high]=highval;        
        									    // Nibble from each end towards middle,
                                                // swapping items when stuck
        ll = low + 1;
        hh = high;


        for (;;) {
            do ll++;
            while (arr[low] > arr[ll]) ;
            tmpval=arr[ll];
            do hh--;
            while (arr[hh] > arr[low]) ; 
            if (hh < ll)
            	break;
            arr[hh]=arr[ll];
            arr[ll]=tmpval;
        }
        
        // Swap middle item (in position low) back into correct position
        swap(arr[low], arr[hh]) ; // Re-set active partition
        if (hh <= median) low = ll;
        if (hh >= median) high = hh - 1;
	}
}


}}

#endif /*MEDIAN_HPP_*/
