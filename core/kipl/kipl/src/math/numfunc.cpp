

#include <iostream>
#include <cmath>
#include "../../include/math/numfunc.h"

using namespace std;
namespace NRC {
double gammln(double xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;
	
	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}


double gammp(double a, double x)
{
	double gamser,gammcf,gln;
	
    if (x < 0.0 || a <= 0.0) cerr<<"Invalid arguments in routine gammp"<<std::endl;
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return 1.0-gammcf;
	}
}

double gammq(double a, double x)
{
	double gamser,gammcf,gln;
	
    if (x < 0.0 || a <= 0.0) cerr<<"Invalid arguments in routine gammq"<<std::endl;
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}

#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

void gcf(double *gammcf, double a, double x, double *gln)
{
	int i;
	double an,b,c,d,del,h;
	
	*gln=gammln(a);
	b=x+1.0-a;
	c=1.0/FPMIN;
	d=1.0/b;
	h=d;
	for (i=1;i<=ITMAX;i++) {
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
    if (i > ITMAX) cerr<<"a too large, ITMAX too small in gcf"<<std::endl;
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
}

void gser(double *gamser, double a, double x, double *gln)
{
	int n;
	double sum,del,ap;
	
	*gln=gammln(a);
	if (x <= 0.0) {
        if (x < 0.0) cerr<<"x less than 0 in routine gser"<<std::endl;
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
        cerr<<"a too large, ITMAX too small in routine gser"<<std::endl;
		return;
	}
}

double Q(double x)
{
	return 0.5*erffc(x/sqrt(2.0));
}

double Qinv(double p)
{
	
	double a1 = -39.69683028665376;
	double a2 = 220.9460984245205;
	double a3 = -275.9285104469687;
	double a4 = 138.3577518672690;
	double a5 =-30.66479806614716;
	double a6 = 2.506628277459239;
	
	double b1 = -54.47609879822406;
	double b2 = 161.5858368580409;
	double b3 = -155.6989798598866;
	double b4 = 66.80131188771972;
	double b5 = -13.28068155288572;
	
	double c1 = -0.007784894002430293;
	double c2 = -0.33223964580411365;
	double c3 = -2.400758277161838;
	double c4 = -2.549732539343734;
	double c5 = 4.374664141464968;
	double c6 = 2.938163982698783;
	
	double d1 = 0.007784695709041462;
	double d2 = 0.3224671290700398;
	double d3 = 2.445134137142996;
	double d4 = 3.754408661907416;
	
	//Define break-points.
	
	double p_low =  0.02425;
	double p_high = 1 - p_low;
	double q,x=0.0, r;

//	double sigma = 1.318;
//	double my = 9.357;
	
	
	//Rational approximation for lower region.
	
	if ((0 < p) && (p < p_low)) {
		q = sqrt(-2*log(p));
		x = (((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
	}
	
	//Rational approximation for central region.
	
	if ((p_low <= p) && (p <= p_high)) {
		q = p - 0.5;
		r = q*q;
		x = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*q / (((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1);
	}
	
	//Rational approximation for upper region.
	
	if ((p_high < p) && (p < 1)) {
		q = sqrt(-2*log(1-p));
		x = -(((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
	}
	//y = exp(sigma*x+my);
	
	
	if ((0<p) && (p<1)) {
		double e=0.5*erffc(-x/sqrt(2.0))-p;
		double u=e*sqrt(2*3.1415926)*exp(x*x/2);
		
		x=x-u/(1+x*u/2);
	}
	return -x;
}

double erfcc(double x)
{
	double t,z,ans;
	
	z=fabs(x);
	t=1.0/(1.0+0.5*z);
	ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
		t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
		t*(-0.82215223+t*0.17087277)))))))));
	return x >= 0.0 ? ans : 2.0-ans;
}

double erff(double x)
{
	return x < 0.0 ? -gammp(0.5,x*x) : gammp(0.5,x*x);
}

double erffc(double x)
{
	return x < 0.0 ? 1.0+gammp(0.5,x*x) : gammq(0.5,x*x);
}

double erffinv(double x)
{

	return Qinv((1-x)*0.5)/sqrt(2.0);
}



#undef ITMAX
#undef EPS
#undef FPMIN
} // end namespace NRC
