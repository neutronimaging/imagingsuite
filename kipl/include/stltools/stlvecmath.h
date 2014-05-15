// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef __stlvecmath_h
#define __stlvecmath_h
#include <vector>
#include <iostream>
#include "../math/median.h"
//#include "statistics.h"
//#include "../misc/sortalg.h"
//#include "../image/imagethres.h"

using namespace std;
//using namespace Visualization;

/// \brief Median filters the data in a vector
///
/// \b Parameter:
/// - data : vector containing the data to be filtered
/// - out  : vector containing the result of the filter
/// - len  : length of the filter window
template<class T>
int MedianFilter(vector<T> & data,vector<T> &out, size_t len)
{
	int len2=len/2;
	int l=0;
	T *base=new T[len];
	out.resize(data.size());

	for (size_t i=0; i<data.size(); i++) {
		l=0;
		for (size_t j=-len2; j<len2-(1-len%2); j++) {
			if ((j+i>=0) && (j+i)<data.size()) {
				base[l++]=data[i+j];
			}
		}
		T val=0;
	//	kipl::math::median(base,len,&val);
		// todo: Check the problems with median
		out[i]=val;
	}
	
	delete [] base;
	return 1;
}

/// Subtracts vector b from vector a
template<class T>
vector<T> operator-(vector<T> &a, vector<T> &b)
{
	vector<T> diff(a.size());

	for (size_t i=0; i<a.size(); i++) 
		diff[i]=a[i]-b[i];

	return diff;
}

/// Adds vector a to vector b
template<class T>
vector<T> operator+(vector<T> &a, vector<T> &b)
{
	vector<T> s(a.size());

	for (size_t i=0; i<a.size(); i++) 
		s[i]=a[i]+b[i];

	return s;
}

/// Elementwise multiplication  vector a to vector b
template<class T>
vector<T> operator*(vector<T> &a, vector<T> &b)
{
	vector<T> s(a.size());

	for (size_t i=0; i<a.size(); i++) 
		s[i]=a[i]*b[i];

	return s;
}

/// Divides the vector by a constant
template<class T>
vector<double> operator/(vector<T> & a, double x)
{
	vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		tmp[i]=a[i]/x;

	return tmp;
}


/// Divides the vector by another vector with the same size
template<class T1, class T2>
vector<double> operator/(vector<T1> & a, vector<T2> &b)
{
	if (a.size() != b.size()) {
		cerr<<"stlvec math op/ : Vector size miss match"<<endl;
		exit(0);
	}

	vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		if (b[i])
			tmp[i]=a[i]/(double)b[i];
		else {
			cerr<<"stlvec math op/ : div by zero at i="<<i<<endl;
		}

	return tmp;
}

/// Divides the vector by a constant
template<class T >
vector<double> operator-(vector<T> & a, double x )
{
	vector<double> tmp(a.size());
	
	for (size_t i=0; i<a.size(); i++) 
		tmp[i]=a[i]-x;

	return tmp;
}

/// Computes the sum of the elements in vector v
template<class T>
T sum(vector<T> v)
{
	T sum=(T) 0;
	for (size_t i=0; i<v.size(); i++)
		sum+=v[i];

	return sum;
}

/// Convolves the vector x with the filter kernel in H
template<class T>
vector<T> filter(vector<T> &x, vector<double> &H)
{
	vector<T> tmp(x.size());
	for (size_t i=0; i<x.size(); i++) {
		//cout<<i<<", ";
		tmp[i]=0;
		for (size_t j=0; j<H.size(); j++) {
			if ((i-j)>=0)
				tmp[i]+=H[j]*x[i-j];
		}
	}
	
	return tmp;
}
/*
/// Computes the statistics of vector v
template<class T>
Math::Statistics vecstats(vector<T> v) 
{
	Math::Statistics stat;

	for (size_t i=0; i<v.size(); i++)
		stat.add(v[i]);

	return stat;
}
*/
/// Returns a vector with n elements having value val
template<class T>
vector<T> FillVec(int n, T val=0)
{
	vector<T> tmp(n);

	for (size_t i=0; i<n; i++) {
		tmp[i]=val;
	}

	return tmp;
}

/// Returns a vector with n elements having data in an increasing series
template<class T>
vector<T> FillVecSeries(T start, T stop, T step=(T)1)
{
	int n=1+(stop-start)/step;

	vector<T> tmp(n);
	size_t i;
	T val;
	for (i=0, val=start; i<n; i++, val+=step) {
		tmp[i]=val;
	}

	return tmp;
}

/** \brief Returns a vector with n-1 elements containing v[i]-v[i-1] 

  \todo Implement the code
*/
template<class T>
vector<T> DiffVec(vector<T> &v)
{
	return v;
}

template <class T>
T Min(vector<T> &v,int *pos=NULL)
{
	T m=v[0];
	int ptmp=0;

	for (size_t i=0; i<v.size(); i++) {
		if (v[i]<m) {
			m=v[i];
			ptmp=i;
		}
	}

	if (pos)
		*pos=ptmp;

	return m;
}

template <class T>
T Max(vector<T> &v,int *pos=NULL)
{
	T m=v[0];
	int ptmp=0;

	for (size_t i=0; i<v.size(); i++) {
		if (v[i]>m) {
			m=v[i];
			ptmp=i;
		}
	}

	if (pos)
		*pos=ptmp;


	return m;
}

template <class T>
double median(vector<T> &v)
{
	double m;
	double *tmp;
	
	int N=v.size();
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
double mean(vector<T> &v)
{
	double sum=0;
	int N=v.size();
	
	for (size_t i=0; i<N; i++)
		sum+=v[i];

	return sum/N;
}

template <class T>
vector<T> vecabs(vector<T> &v)
{
  vector<T> tmp;
  tmp=v;

//  vector<T>::iterator it;
  
  for (size_t i=0; i<tmp.size(); i++)
    tmp[i]= tmp[i]<0 ? -tmp[i] : tmp[i];

	return tmp;
}


/** \brief The method computes the cumulative sum of the contents of the vector

	\param v Input vector with values
	\param norm Normalize the vector if true

	\retval The contents of the output vector (x) is computed as x[i]=x[i-1]+v[i].
*/
template <class T>
vector<T> cumsum(vector<T> &v, bool norm=false)
{
	T tmp=0;
	vector<T> tv;
	tv.resize(v.size());
  /*
	vector<T>::iterator it_v=v.begin();
	vector<T>::iterator it_tmp=tv.begin();
*/
	tv[0]=v[0];
	
	for (size_t i=1; i<v.size(); i++) {
		tv[i]=v[i]+tv[i-1];
	}

	if (norm) {
	double scale=1/ tv[tv.size()-1];
	for (size_t i=0; i<tv.size(); i++)
	tv[i]=(T)(tv[i]*scale);
	}

	return tv;
}

/** \brief The method computes the cumulative sum of the contents of the vector

	\param begin Start position Iterator
	\param end End position iterator
	\param tv Resulting cum sum vector
	
	\returns The last sum value
*/
template <class ForwardIterator, class T>
T cumsum(const ForwardIterator &begin,const ForwardIterator &end, vector<T> &tv)
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

/*
/// \brief Find the first occurance of the test becoming true
///
///	\param v Input vector
///	\param val Test value
///	\param cmp Type of comparison
template<class  T>
int Find(vector<T> &v,T val, CmpType cmp)
{
//  int i;

  typedef T tmptype;
  
  T it;

	for (size_t i=0; i<v.size(); i++) {
    it=v[i];
		switch(cmp){
		case cmp_less:
			if (it<val) return i;
			break;
		case cmp_greater:
			if (it>val) return i;
			break;
		case cmp_lesseq:
			if (it<=val) return i;
			break;
		case cmp_greatereq:
			if (it>=val) return i;
			break;
		case cmp_eq:
			if (it==val) return i;
			break;
		case cmp_noteq:
			if (it!=val) return i;
			break;
		}
	}

	return -1;
}
*/

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

template<class T>
Math::Statistics vecstats(vector<T> &v, double outliers, int N=256) 
{
	Math::Statistics stat;
	if ((outliers>=1) || (outliers<0)) 
		outliers=0;
	
//	Gnuplot fig("test");
	
	if (outliers) {
		vector<int> hist;
		vector<double> interval;
		vector<double> cumhist;
		int a,b;	
		vechist(v,hist,interval,N);
		//fig.plot(hist);
		hist=cumsum(hist);
		//fig.plot(hist);
		double low_tail, high_tail;
		a=Find(hist,int(outliers*0.5*v.size()),cmp_greater);
		low_tail=interval[a];
		b=Find(hist,int((1-outliers*0.5)*v.size()),cmp_greater);
		high_tail=interval[b];
		
		for (size_t i=0; i<v.size(); i++)
			if ((v[i]>=low_tail) && (v[i]<=high_tail))
				stat.add(v[i]);
	}
	else {
		for (size_t i=0; i<v.size(); i++)
			stat.add(v[i]);
	}

	return stat;
}
*/
#endif
