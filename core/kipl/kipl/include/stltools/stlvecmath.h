//<LICENCE>

#ifndef STLVECMATH_H
#define STLVECMATH_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <stddef.h>

#include "../math/median.h"
#include "../kipl_global.h"

#include <QDebug>
//#include "statistics.h"
//#include "../misc/sortalg.h"
//#include "../image/imagethres.h"

// /// \brief Median filters the data in a vector
// ///
// /// \b Parameter:
// /// - data : vector containing the data to be filtered
// /// - out  : vector containing the result of the filter
// /// - len  : length of the filter window
//template<class T>
//int MedianFilter(std::vector<T> & data,std::vector<T> &out, size_t len)
//{
//    int len2=static_cast<int>(len)/2;
//	int l=0;
//	T *base=new T[len];
//	out.resize(data.size());

//    int sd=data.size();
//    int len2e=len2-(1-len%2);

//    for (int i=0; i<sd; i++) {
//		l=0;
//        for (ptrdiff_t j=-len2; j<len2e; j++) {
//            if ((j+i>=0) && (j+i)<sd) {
//				base[l++]=data[i+j];
//			}
//		}
//		T val=0;
//	//	kipl::math::median(base,len,&val);
//		// todo: Check the problems with median
//		out[i]=val;
//	}
	
//	delete [] base;
//	return 1;
//}

/// Subtracts vector b from vector a
template<class T>
std::vector<T> operator-(std::vector<T> &a, std::vector<T> &b)
{
    std::vector<T> diff(a.size());

	for (size_t i=0; i<a.size(); i++) 
		diff[i]=a[i]-b[i];

	return diff;
}

/// Adds vector a to vector b
template<class T>
std::vector<T> operator+(std::vector<T> &a, std::vector<T> &b)
{
    std::vector<T> s(a.size());

	for (size_t i=0; i<a.size(); i++) 
		s[i]=a[i]+b[i];

	return s;
}

/// Elementwise multiplication  vector a to vector b
template<class T>
std::vector<T> operator*(std::vector<T> &a, std::vector<T> &b)
{
    std::vector<T> s(a.size());

	for (size_t i=0; i<a.size(); i++) 
		s[i]=a[i]*b[i];

	return s;
}

/// Divides the vector by a constant
template<class T>
std::vector<double> operator/(std::vector<T> & a, double x)
{
    std::vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		tmp[i]=a[i]/x;

	return tmp;
}


/// Divides the vector by another vector with the same size
template<class T1, class T2>
std::vector<double> operator/(std::vector<T1> & a, std::vector<T2> &b)
{
	if (a.size() != b.size()) {
        throw std::length_error("stlvec math op/ : Vector size miss match");
	}

    std::vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		if (b[i])
            tmp[i]=a[i]/static_cast<double>(b[i]);
		else {
            std::cerr<<"stlvec math op/ : div by zero at i="<<i<<std::endl;
		}

	return tmp;
}

/// Divides the vector by a constant
template<class T >
std::vector<double> operator-(std::vector<T> & a, double x )
{
    std::vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		tmp[i]=a[i]-x;

	return tmp;
}

/// Computes the sum of the elements in vector v
template<class T>
T sum(std::vector<T> v)
{
    T sum=static_cast<T>(0);
    for (const auto &item : v)
        sum+=item;

	return sum;
}

/// Convolves the vector x with the filter kernel in H
template<class T>
std::vector<T> filter(std::vector<T> &x, std::vector<double> &H)
{
    std::vector<T> tmp(x.size());
    ptrdiff_t sx=static_cast<ptrdiff_t>(x.size());
    ptrdiff_t sH=static_cast<ptrdiff_t>(H.size());

    for (ptrdiff_t i=0; i<sx; i++) {
		tmp[i]=0;
        for (ptrdiff_t j=0; j<sH; j++) {
			if ((i-j)>=0)
				tmp[i]+=H[j]*x[i-j];
		}
	}
	
	return tmp;
}

/// Convolves the vector x with the filter kernel in H
template<class T>
std::vector<T> medianFilter(const std::vector<T> &x, size_t len)
{
    std::vector<T> res(x.size());
    std::vector<T> buffer(len);
    size_t oddLen = len % 2;
    size_t w = len/2 + oddLen;

    qDebug() <<"len="<<len<<", oddLen="<<oddLen;
    auto itX = x.begin();
    auto itR = res.begin()+w-1;
    for ( ; itX!=(x.end()-len+1); ++itX,++itR)
    {
        std::copy(itX,itX+len,buffer.begin());
        std::sort(buffer.begin(),buffer.end());
        if (oddLen)
            *itR=buffer[w-1];
        else
            *itR = (buffer[w]+buffer[w-1])/2;
    }

    std::fill(res.begin(),res.begin()+w,res[w-1]);
    std::fill(res.end()-(len-w),res.end(),*(res.end()-(len-w+1)));
    return res;
}

///// Computes the statistics of vector v
//template<class T>
//Math::Statistics vecstats(vector<T> v)
//{
//	Math::Statistics stat;

//	for (size_t i=0; i<v.size(); i++)
//		stat.add(v[i]);

//	return stat;
//}


/// Returns a vector with n elements having data in an increasing series
template<class T>
std::vector<T> FillVecSeries(T start, T stop, T step=static_cast<T>(1))
{
    size_t n=1+(stop-start)/step;

    std::vector<T> tmp(n);
	size_t i;
	T val;
	for (i=0, val=start; i<n; i++, val+=step) {
		tmp[i]=val;
	}

	return tmp;
}

/// \brief Returns a vector with n-1 elements containing v[i]-v[i-1]
template<class T>
std::vector<T> DiffVec(std::vector<T> &v)
{
    std::vector<T> res;
    auto i0=v.begin();
    auto i1=i0;
    ++i1;

    for ( ; i1!=v.begin(); ++i0,++i1)
        res.push_back(*i1-*i0);

	return v;
}

template <class T>
double median(std::vector<T> &v)
{
	double m;
	double *tmp;
	
    size_t N=v.size();
	tmp=new double[N];


	for (size_t i=0; i<N; i++)
		tmp[i]=v[i];

	heapsort(tmp,N);

	if (N%2)
		m=tmp[N/2];
	else
		m=(tmp[N/2]+tmp[N/2-1])*0.5;

	delete [] tmp;

	return m;
}

template <class T>
double mean(std::vector<T> &v)
{
	double sum=0;
    size_t N=v.size();
	
	for (size_t i=0; i<N; i++)
		sum+=v[i];

	return sum/N;
}

template <class T>
std::vector<T> vecabs(std::vector<T> &v)
{
    std::vector<T> tmp;
    tmp=v;

    for (auto & val : tmp)
        val = val<0 ? -val : val;

    return tmp;
}


/** \brief The method computes the cumulative sum of the contents of the vector

	\param v Input vector with values
	\param norm Normalize the vector if true

	\retval The contents of the output vector (x) is computed as x[i]=x[i-1]+v[i].
*/
template <class T>
std::vector<T> cumsum(std::vector<T> &v, bool norm=false)
{
	T tmp=0;
    std::vector<T> tv;
	tv.resize(v.size());

	tv[0]=v[0];
	
	for (size_t i=1; i<v.size(); i++) {
		tv[i]=v[i]+tv[i-1];
	}

	if (norm) {
        double scale=1/ tv[tv.size()-1];
        for (size_t i=0; i<tv.size(); i++)
        tv[i]=static_cast<T>(tv[i]*scale);
	}

	return tv;
}

/// \brief The method computes the cumulative sum of the contents of the vector
///
///	\param begin Start position Iterator
///	\param end End position iterator
///	\param tv Resulting cum sum vector
///
///	\returns The last sum value
template <class ForwardIterator, class T>
T cumsum(const ForwardIterator &begin,const ForwardIterator &end, std::vector<T> &tv)
{
	T prev,sum;
	tv.clear();
	ForwardIterator it;
	prev=0;

	for (it=begin; it!=end; it++) {
		sum=prev+*it;
		tv.push_back(sum);
		prev=sum;
	}
		
	return tv.back();
}

std::map<float,float> KIPLSHARED_EXPORT diff(std::map<float,float> data);

// /// \brief Find the first occurance of the test becoming true
// ///
// ///	\param v Input vector
// ///	\param val Test value
// ///	\param cmp Type of comparison
//template<class  T>
//int Find(vector<T> &v,T val, CmpType cmp)
//{
// //  int i;

//  typedef T tmptype;
  
//  T it;

//	for (size_t i=0; i<v.size(); i++) {
//    it=v[i];
//		switch(cmp){
//		case cmp_less:
//			if (it<val) return i;
//			break;
//		case cmp_greater:
//			if (it>val) return i;
//			break;
//		case cmp_lesseq:
//			if (it<=val) return i;
//			break;
//		case cmp_greatereq:
//			if (it>=val) return i;
//			break;
//		case cmp_eq:
//			if (it==val) return i;
//			break;
//		case cmp_noteq:
//			if (it!=val) return i;
//			break;
//		}
//	}

//	return -1;
//}


/*
template <class T>
int vechist(vector<T> &data, vector<int> &hist, vector<T> & interval, int N)
{
	double min,max;
	double k;
	if (interval.size()==2) {
		max=interval[1];
		min=interval[0];
	}
	else {
		Math::Statistics stats;
		stats=vecstats(data);
		max=stats.Max();
		min=stats.Min();
	}
	
	k=(max-min)/N;
    	interval.resize(N+1);
    	hist.resize(N);
    	int i;

    	for (i=0; i<N; i++) {
    		interval[i]=min+k*i;
		hist[i]=0;
	}
	
	interval[N]=max;
	int p;
    	for (i=0; i<data.size(); i++) {
		p=(int)floor((data[i]-min)/k);
		if ((p>=0) && (p<N)) {
			hist[p]++;
			continue;
		}
		
		if (p<0)
			hist[0]++;
		if (p>=N)
			hist[N-1]++;
	}

    	return 0;
}

*/
#endif
