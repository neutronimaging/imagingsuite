//<LICENCE>

#include <iostream>
#include <limits>
#include <algorithm>
#ifdef __aarch64__
    #pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wcast-align"
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-W#warnings"
		#include <sse2neon/sse2neon.h>
	#pragma clang diagnostic pop
#else
   	#include <xmmintrin.h>
	#include <emmintrin.h>
#endif


#include "../../include/math/median.h"
#include "../../include/base/core/quad.h"

namespace kipl { namespace math {

inline __m128 SortQuad(__m128 data)
{ 
	__m128 a;
	__m128 b=_mm_shuffle_ps(data,data,177);
	__m128 minval=_mm_min_ps(data,b);
	__m128 maxval=_mm_max_ps(data,b);

	a=_mm_movehl_ps(maxval,minval);
	b=_mm_movelh_ps(maxval,minval);
	minval=_mm_min_ps(a,b);
	maxval=_mm_max_ps(a,b);

	a=_mm_movehl_ps(maxval,minval);
	b=_mm_movelh_ps(maxval,minval);
	minval=_mm_min_ps(a,b);
	maxval=_mm_max_ps(a,b);
	
	a=_mm_shuffle_ps(minval,maxval,34);
	b=_mm_shuffle_ps(minval,maxval,136);
	minval=_mm_min_ps(a,b);
	maxval=_mm_max_ps(a,b);
	
	a=_mm_shuffle_ps(minval,maxval,136);
	b=_mm_shuffle_ps(a,a,216);
	
	return b;
}

inline __m128 SortQuad2(__m128 data)
{ 
	__m128 b=_mm_shuffle_ps(data,data,_MM_SHUFFLE(2,1,0,3));
	__m128 c=_mm_shuffle_ps(data,data,_MM_SHUFFLE(1,0,3,2));
	__m128 d=_mm_shuffle_ps(data,data,_MM_SHUFFLE(0,3,2,1));
	__m128 e=_mm_min_ps(data,b);
	__m128 f=_mm_max_ps(data,b);

	__m128 a=_mm_min_ps(c,d);
	b=_mm_max_ps(c,d);

	c=_mm_min_ps(a,e);
	d=_mm_max_ps(b,f);

	__m128 g=_mm_min_ps(b,f);
	__m128 h=_mm_max_ps(e,a);
	a=_mm_min_ps(g,h);
	b=_mm_max_ps(g,h);
	e=_mm_movehl_ps(a,c);
	f=_mm_movelh_ps(b,d);
	b=_mm_shuffle_ps(e,f,_MM_SHUFFLE(2,0,2,0));
	return b;
}

//inline void SortQuad2(__m128 *data) {
//__m128 a=_mm_min_ps(data[0],data[1]);
//__m128 b=_mm_max_ps(data[0],data[1]);
//data[0]=_mm_movehl(a,b);
//data[1]=_mm_movelh(a,b);
//a=_mm_min_ps(data[0],data[1]);
//b=_mm_max_ps(data[0],data[1]);
//data[0]=_mm_shuffle(a,b,_MM_SHUFFLE(2,0,2,0));
//data[1]=_mm_shuffle(a,b,_MM_SHUFFLE(3,1,3,1));
//a=_mm_min_ps(data[0],data[1]);
//b=_mm_max_ps(data[0],data[1]);
//std::cout<<a<<", "<<b<<std::endl;
//}


void median_quick_select_sse(float *arr, const size_t n, float *med)
{
	kipl::base::uFQuad data;
	kipl::base::uFQuad sorted;
	data.q.d=std::numeric_limits<float>::max();
	__m128 d;
	int low=0;
	int high=n-1 ;
	int middle, ll, hh;

	int median = (low + high) >> 1;
	for (;;) {
        if (high <= low) {// One element only
        		*med=arr[median];
                return; 
        }
		ll=low+1;
		if (high == ll) {
        	if (arr[low] > arr[high]) 
				std::swap(arr[low], arr[high]) ;

			*med= arr[median];
            return; 
        }
        // Find median of low, middle and high items; swap into position low
        middle = (low + high) >> 1;
  
		//data.q.a=arr[low]; data.q.b=arr[middle]; data.q.c=arr[high];
		d=_mm_set_ps(arr[low], arr[middle],arr[high],std::numeric_limits<float>::max());
		sorted.xmm=SortQuad(d);//data.xmm);

		arr[low]=sorted.q.b;
		arr[high]=sorted.q.c;
		arr[middle]=arr[ll];
		arr[ll]=sorted.q.a;

        hh = high;
        for (;;) {
                do ll++;
                while (arr[low] > arr[ll]) ;
                do hh--;
                while (arr[hh] > arr[low]) ; 
                if (hh < ll)
                	break;
				std::swap(arr[ll], arr[hh]) ;
        }
        // Swap middle item (in position low) back into correct position
		std::swap(arr[low], arr[hh]) ; // Re-set active partition
        if (hh <= median) low = ll;
        if (hh >= median) high = hh - 1;
	}
}

void median_quick_select_sse2(float *arr, const size_t n, float *med)
{
	kipl::base::uFQuad data,sorted;
	data.q.d=std::numeric_limits<float>::max();
	int low=0;
	int high=n-1 ;
	int middle, ll, hh;

	int median = (low + high) >> 1;
	for (;;) {
        if (high <= low) {// One element only
        		*med=arr[median];
                return; 
        }
		ll=low+1;
		if (high == ll) {
        	if (arr[low] > arr[high]) 
				std::swap(arr[low], arr[high]) ;

			*med= arr[median];
            return; 
        }
        // Find median of low, middle and high items; swap into position low
        middle = (low + high) >> 1;
  
		data.q.a=arr[low]; data.q.b=arr[middle]; data.q.c=arr[high];
		
		sorted.xmm=SortQuad2(data.xmm);

		arr[low]=sorted.q.b;
		arr[high]=sorted.q.c;
		arr[middle]=arr[ll];
		arr[ll]=sorted.q.a;

        hh = high;
        for (;;) {
                do ll++;
                while (arr[low] > arr[ll]) ;
                do hh--;
                while (arr[hh] > arr[low]) ; 
                if (hh < ll)
                	break;
				std::swap(arr[ll], arr[hh]) ;
        }
        // Swap middle item (in position low) back into correct position
		std::swap(arr[low], arr[hh]) ; // Re-set active partition
        if (hh <= median) low = ll;
        if (hh >= median) high = hh - 1;
	}
}

}}
