#include "../../../include/kipl_global.h"
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <map>
#include "../../../include/math/sums.h"

namespace kipl { namespace base {
//int Histogram(float const * const data, size_t nData, size_t * const hist, const size_t nBins, float lo, float hi, float * const pAxis)

int KIPLSHARED_EXPORT Histogram(float * data, size_t nData, size_t * hist, size_t nBins, float lo, float hi, float * pAxis)
{

    float start=0;
    float stop=1.0f;
   
    if (lo==hi) {
        start = *std::min_element(data,data+nData);
        stop  = *std::max_element(data,data+nData);
    }
    else {
		start=std::min(lo,hi);
		stop=std::max(lo,hi);
    }

    memset(hist,0,sizeof(size_t)*nBins);
    
	float scale=(nBins)/(stop-start);
    #pragma omp parallel firstprivate(nData,start,scale)
    {
		int index;
		long long int i=0;	
		size_t *temp_hist=new size_t[nBins];
		memset(temp_hist,0,nBins*sizeof(size_t));
		const ptrdiff_t snData=static_cast<ptrdiff_t>(nData);
		const ptrdiff_t snBins=static_cast<ptrdiff_t>(nBins);

		#pragma omp for
		for (i=0; i<snData; i++) {
			index=static_cast<int>((data[i]-start)*scale);
			if ((index<snBins) && (0<=index))
				temp_hist[index]++;
		}
		#pragma omp critical
		{
			for (i=0; i<snBins; i++)
				hist[i]+=temp_hist[i];
		}
		delete [] temp_hist;
    }
    scale=(stop-start)/nBins;
    if (pAxis!=NULL) {
        pAxis[0]=start+scale/2;
        for (size_t i=1; i<nBins; i++)
            pAxis[i]=pAxis[i-1]+scale;
    }

	return 0;
}

std::map<float, size_t> ExactHistogram(float const * const data, size_t Ndata)
{
	std::map<float, size_t> hist;

	for (size_t i=0; i<Ndata; i++) {
		hist[data[i]]++;
	}

	return hist;
}


double  KIPLSHARED_EXPORT Entropy(size_t const * const hist, size_t N)
{
	double p=0.0;
	double entropy=0.0;

	size_t histsum=kipl::math::sum(hist,N);

	for (size_t i=0; i<N; i++) {
		if (hist[i]!=0) {
			p=static_cast<double>(hist[i])/static_cast<double>(histsum);
			entropy-=p*std::log10(p);
		}
	}

	return entropy;
}

int  KIPLSHARED_EXPORT FindLimits(size_t const * const hist, size_t N, float percentage, size_t * lo, size_t * hi)
{
	ptrdiff_t *cumulated=new ptrdiff_t[N];
	memset(cumulated,0,sizeof(ptrdiff_t)*N);

	cumulated[0]=hist[0];
	for (size_t i=1; i<N; i++) {
		cumulated[i]=cumulated[i-1]+hist[i];
	}

	if ((lo!=NULL) && (hi!=NULL)) {
		*lo=0;
		*hi=0;

		float fraction=(100.0f-percentage)/200.0f;
		ptrdiff_t lowlevel  = static_cast<ptrdiff_t>(cumulated[N-1]*fraction);
		ptrdiff_t highlevel = static_cast<ptrdiff_t>(cumulated[N-1]*(1-fraction));

		ptrdiff_t lowdiff  = cumulated[N-1];
		ptrdiff_t highdiff = cumulated[N-1];

		ptrdiff_t diff=cumulated[N-1];
		for (size_t i=0; i<N; i++){
			diff=static_cast<ptrdiff_t>(std::abs(static_cast<double>(cumulated[i]-lowlevel)));
			if (diff<lowdiff) {
				lowdiff=diff;
				*lo=i;
			}

			diff=static_cast<ptrdiff_t>(std::abs(static_cast<double>(cumulated[i]-highlevel)));
			if (diff<highdiff) {
				highdiff=diff;
				*hi=i;
			}
		}
	}
	return 0;
}
}}
