//<LICENCE>

#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

#include "../../include/segmentation/thresholds.h"
#include "../../include/morphology/morphology.h"
#include "../../include/math/sums.h"
#include "../../include/strings/miscstring.h"

using namespace std;

namespace kipl { namespace segmentation {
/*
int Threshold_Entropy(size_t const * const Hist, 
		float const * const inter, 
		float re, 
		int &Tc, 
		int &T0, 
		int &T1)
{
	vector<double> p_hist, cum_p_hist;
	vector<double> Hs;
	vector<long> hist;
	
	vector<long>::iterator hist_pos;
	vector<double>::iterator p_pos, cum_pos, Hs_pos;
	double hist_sum=0;
	
	MedianFilter(Hist,hist,3);
	
	for (hist_pos=hist.begin(); hist_pos!=hist.end(); hist_pos++)
		hist_sum+=*hist_pos;
	
	hist_pos=hist.begin();
	double p;
	p=hist.front()/hist_sum;
	p_hist.push_back(p);
	cum_p_hist.push_back(p);
	Hs.push_back(-p*log(p));
	hist_pos++; 
	double prev_cum=p ,prev_entropy=Hs.front();

	for ( ;hist_pos!=hist.end(); hist_pos++) {
		p=(*hist_pos)/hist_sum;
		p_hist.push_back(p);
		if (p) 
			prev_entropy-=p*log(p);
		Hs.push_back(prev_entropy);
		prev_cum+=p;
		cum_p_hist.push_back(prev_cum);			
	}		
	
	double Hn=Hs.back();
	vector<double> phi;
	for (cum_pos=cum_p_hist.begin(), Hs_pos=Hs.begin(); Hs_pos!=Hs.end(); cum_pos++,Hs_pos++)
		phi.push_back(log(*cum_pos*(1-*cum_pos))+*Hs_pos/ *cum_pos+(Hn-*Hs_pos)/(1-*cum_pos));
	
	vector<double>::iterator th_pos;
	th_pos=max_element(phi.begin(),phi.end());
	Tc=th_pos-phi.begin();
	
	vector<double>::iterator phi_pos;
	vector<double>::iterator th0_pos;
	
	double ref_val=(1-re)* *th_pos;
	
	phi_pos=find_if(phi.begin(),th_pos,bind2nd(greater<double>(),ref_val));
	
	T0=phi_pos-phi.begin();
	
	th_pos++;
	phi_pos=find_if(th_pos,phi.end(),bind2nd(less<double>(),ref_val));
	T1=phi_pos-phi.begin();

	return 0;
}
*/

int Threshold_Otsu(size_t const * const hist, const size_t N)
{
	int t=0;

	double * P=new double[N];
	double sum_hist=static_cast<double>(kipl::math::sum(hist,N));
	
	double m1=0, q_old, m2,m=0;

	size_t i;
	for (i=0; i<N; i++) {
		P[i]=hist[i]/sum_hist;
		m+=i*P[i];
	}


	double q1=P[0];
	double sb=0, sb_max=0;
	vector<double> svec;

	for (i=1; i<N; i++) {
		q_old=q1;
		q1+=P[i];
		if ((q1) && (q1-1)) {
			m1=(q_old*m1+(i)*P[i])/q1;
			m2=(m-q1*m1)/(1-q1);
			sb=q1*(1-q1)*(m1-m2)*(m1-m2);
			svec.push_back(sb);
			if (sb_max<sb) {
				t=i;
				sb_max=sb;
			}
		}
		else {
			svec.push_back(sb);
		}
	}

	return t;
}

int Threshold_Rosin(size_t const * const /*hist*/, const TailType tail, const size_t /*median_filter_len*/) 
{
//	if ((tail!=tail_left) && (tail!=tail_right))
//		tail=tail_left;
	vector<long> h;
//	if (median_filter_len)
//		MedianFilter(hist,h,median_filter_len);
//	else
//		h=hist;

//	int max_pos=0, min_pos=0;
	vector<long>::iterator maxIt, first,last,thIt, it;
	int max,min;
	maxIt=max_element(h.begin(),h.end());
	int d=distance(h.begin(),maxIt);
	
	if (tail==tail_left) {
		last=maxIt;
		
		first=h.begin();
		if (maxIt==first) {
            std::cerr<<"Rosin: Histogram has max at the first bin, no left tail?"<<std::endl;
			return 0;
		}
		while ((!*first) && (first!=last))
			first++;
			
		if (first==last)
			first--;
		min=*first;
	}
	else {
		first=maxIt;
		last=h.end(); last--;
		if (maxIt==last) {
            std::cerr<<"Rosin: Histogram has max at the last bin, no right tail?"<<std::endl;
            return static_cast<int>(h.size()-1);
		}
		
		while ((!*last) && (last!=first)) 
			last--;
			
		if (last==first)
			last++;
		min=*last;
	//	d=distance(first,last);
	}
    d=distance(first,last);

	max=*maxIt;
	
	int i=0;

	double k=(*last-*first)/double(last-first);
	double ik=-1/k;
	double mf=tail==tail_left ? min : max;
	double mg,ifg;

	double C=0, C_max=0;
		
	for (i=0,it=first; it!=last; it++,i++) {
		mg=*it-ik*i;
		ifg=k*(mg-mf)/(k*k+1);
		C=(k*ifg+mf-*it)*(k*ifg+mf-*it)+(i-ifg)*(i-ifg);
		if (C>C_max) { 
			C_max=C; 
			thIt=it; 
		}
	}

	return thIt-h.begin();
}


}} // End namespace ImageThreshold

std::string enum2string(kipl::segmentation::CmpType cmp)
{
    std::string str;

    switch (cmp) {
    case kipl::segmentation::cmp_less : str="cmp_less"; break;
    case kipl::segmentation::cmp_greater : str="cmp_greater"; break;
    case kipl::segmentation::cmp_lesseq : str="cmp_less_equal"; break;
    case kipl::segmentation::cmp_greatereq : str="cmp_greater_equal"; break;
    case kipl::segmentation::cmp_noteq : str="cmp_not_equal"; break;
    case kipl::segmentation::cmp_eq : str="cmp_equal"; break;
    default: throw kipl::base::KiplException("Failed to convert cmp enum to string",__FILE__,__LINE__);
    }

    return str;
}

void string2enum(std::string str, kipl::segmentation::CmpType &cmp)
{
    std::map<std::string,kipl::segmentation::CmpType> m;

    m["cmp_less"]          = kipl::segmentation::cmp_less;
    m["cmp_greater"]       = kipl::segmentation::cmp_greater;
    m["cmp_less_equal"]    = kipl::segmentation::cmp_lesseq;
    m["cmp_greater_equal"] = kipl::segmentation::cmp_greatereq;
    m["cmp_not_equal"]     = kipl::segmentation::cmp_noteq;
    m["cmp_equal"]         = kipl::segmentation::cmp_eq;

    str=kipl::strings::toLower(str);

    if (m.find(str)!=m.end()) {
        cmp=m[str];
    }
    else {
        throw kipl::base::KiplException("Failed to convert string to CmpType",__FILE__,__LINE__);
    }
}

std::ostream & operator<<(std::ostream &s, kipl::segmentation::CmpType c)
{
    s<<enum2string(c);
    return s;
}
