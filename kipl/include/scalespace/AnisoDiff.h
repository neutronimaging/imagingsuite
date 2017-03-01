//<LICENCE>

#ifndef __ANISODIFF_H
#define __ANISODIFF_H
#include <float.h>
#include <iostream>
#include <ios>
#include <string>

#include "../base/timage.h"
#include "lambdaest.h"
#include "../math/mathmisc.h"

#include "../io/io_matlab.h"
#include "diff_filterbase.h"

using namespace std;
using namespace Image;
using namespace Math;
using namespace Visualization;
using namespace Fileio;

namespace ScaleSpace {
/// Implementation of an anisotropic diffusion filter
template <class ImgType, int NDim>
	class AnisotropicDiffusionFilter: public DiffusionBaseFilter<ImgType,NDim>
	{
	public:
		AnisotropicDiffusionFilter() {RhoKernel=NULL; IndRhoKernel=NULL; rho.push_back(1.0f);}
		AnisotropicDiffusionFilter(float Sigma, float Rho, float Tau, float Lambda, int It=10);
		AnisotropicDiffusionFilter(float Sigma, float Rho, float Tau, LambdaEstBase<ImgType,NDim> *le, int It=10);
		AnisotropicDiffusionFilter(vector<float> &Sigma, vector<float> &Rho, float Tau, LambdaEstBase<ImgType,NDim> *le, int It=10);
		int operator()(CImage<float,NDim> &img);
		virtual ~AnisotropicDiffusionFilter();
		/** \brief Set lambda to a constant value
		\param Lambda the constant
		\param N size of the LUT
		*/
		int setLambda(float Lambda, int N=10000);
		
		/** \brief Set a lambda value estimator
		\param le pointer to a lambda estimator
		\param N size of the LUT
		*/
		int setLambda(LambdaEstBase<ImgType,NDim> *le, int N=10000);
	private:
		int GradientInfo(); 
		int Regularization() {return DiffusionBaseFilter<ImgType,NDim>::Regularization(u,v);}
		int Diffusivity();
		int Solver();
		int UpdateGaussianFilter(int sx,int sxy,int cnt);
		int StructureTensorComponent(CImage<ImgType,NDim> & a, CImage<ImgType,NDim> & b, CImage<ImgType,NDim> & res);
		/** \brief Compute the entries in the LUT

		\param N length of the LUT
		*/
		int ComputeLUT(int N);

		/** \brief Access the LUT
		\param s The function argument to to g(s)
		*/
        float getG_LUT(float s);

		/** \brief Allocates memory for the computational kernel
			\param N length of the 1D signal
		*/
		float *RhoKernel;
		int *IndRhoKernel;
		int NRho;
		vector<float> rho;
		CImage<ImgType,NDim> d11;
		CImage<ImgType,NDim> d12;
		CImage<ImgType,NDim> d22;
		CImage<ImgType,NDim> tmp;
		float lambda;
		LambdaEstBase<ImgType,NDim> *lambdaest;
		int NLut;
		int NKernel;
		
	};
	

	template <class ImgType, int NDim>
AnisotropicDiffusionFilter<ImgType,NDim>::AnisotropicDiffusionFilter(float Sigma, float Rho, float Tau, float Lambda, int It):
DiffusionBaseFilter<ImgType,NDim>(Sigma,Tau,It)
{
	RhoKernel=NULL;
	IndRhoKernel=NULL;
	rho.push_back(Rho);
}

template <class ImgType, int NDim>
AnisotropicDiffusionFilter<ImgType,NDim>::AnisotropicDiffusionFilter(float Sigma, float Rho, float Tau, LambdaEstBase<ImgType,NDim> *le, int It):
DiffusionBaseFilter<ImgType,NDim>(Sigma,Tau,It)
{
	RhoKernel=NULL;
	IndRhoKernel=NULL;
	rho.push_back(Rho);
}

template <class ImgType, int NDim>
AnisotropicDiffusionFilter<ImgType,NDim>::AnisotropicDiffusionFilter(vector<float> &Sigma, 
															vector<float> &Rho, 
															float Tau, 
															LambdaEstBase<ImgType,NDim> *le, 
															int It):
DiffusionBaseFilter<ImgType,NDim>(Sigma,Tau,It)
{
	rho=Rho;
	RhoKernel=NULL;
	IndRhoKernel=NULL;
}

template <class ImgType, int NDim>
AnisotropicDiffusionFilter<ImgType,NDim>::~AnisotropicDiffusionFilter()
{
	if (RhoKernel) delete [] RhoKernel;
	if (IndRhoKernel) delete [] IndRhoKernel;
}

template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::GradientInfo()
{ 
	Gradient(v); 

	return 0;
}


template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::operator()(CImage<float,NDim> &img)
{
	dx.resize(img.getDimsptr());
	dy.resize(img.getDimsptr());
	if (NDim==3)
		dz.resize(img.getDimsptr());
	return DiffusionBaseFilter<ImgType,NDim>::operator()(img);
}

template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::StructureTensorComponent(CImage<ImgType,NDim> & a, 
																	CImage<ImgType,NDim> & b, 
																	CImage<ImgType,NDim> & res)
{
	res.resize(a.getDimsptr());
	tmp.resize(a.getDimsptr());
	long i,j,k;
	for (i=0; i<a.N(); i++) 
		tmp[i]=a[i]*b[i];
	
	long pos;
	int dim, *pIndFilt;	
	float t;
	for (int dim=0; dim<NDim; dim++) {
		pIndFilt=IndRhoKernel+dim*NRho;
		for (int i=0; i<v.N(); i++) {
			t=0;
			for (int j=0; j<NRho; j++) {
				pos=i+pIndFilt[j];
				if ((pos>=0) && (pos<res.N())) 
					t+=tmp[pos]*RhoKernel[j] ;
				else {
					t=0;
					break;
				}
			}
			res[i]+=t;
		}
	}

	return 0;
}

template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::Diffusivity()
{
	cout<<"Diffusivity:"<<endl;
	cout<<"  Computing smoothed structure tensor: ";
	cout<<"s11 "<<flush;
	StructureTensorComponent(dx,dx,d11);
	cout<<"s12 "<<flush;
	StructureTensorComponent(dx,dy,d12);
	cout<<"s22 "<<endl;
	StructureTensorComponent(dy,dy,d22);
	
	if (lambdaest) {
			float lambdaold=lambda;
			lambda=(*lambdaest)(d11);
			if (lambdaold!=lambda)
				ComputeLUT(NLut);
	}
	
	int i;
	float c1, c2, c1p2, c2m1, s1m2, dd, alfa;
	CImage<float,2> aimg(d11.getDimsptr());
	CImage<float,2> cimg(d11.getDimsptr());
	cout<<" Computing diffusion tensor"<<endl;
	int prev_line=-1;
	const unsigned int *dims=d11.getDimsptr();
	
	for (i=0; i<d11.N(); i++) {
		s1m2=d11[i]-d22[i];
		alfa=sqrt(s1m2*s1m2+4*d12[i]*d12[i]);
		aimg[i]=d11[i]+d22[i]+alfa;
		
		//aimg[i]=alfa;
		c1=1;
		if (alfa) {
			c2=getG_LUT(d11[i]+d22[i]+alfa);
			cimg[i]=c2;
			c1p2=c2+1.0f;
			c2m1=1.0f-c2;
			dd=c2m1*s1m2/alfa;
			d11[i]=0.5f*(c1p2+dd);
			d12[i]=-c2m1*d12[i]/alfa;
			d22[i]=0.5f*(c1p2-dd);
		}
		else {
/*			if (i/dims[0]!=prev_line) {
				prev_line=i/dims[0];
				cout<<"-----------------------> alfa=0 at line "<<prev_line<<endl;
			}*/
			d11[i]=1;
			d12[i]=0;
			d22[i]=1;
		}
			
		
		
	}
	
	return 1;
}	

template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::Solver()
{
	cout<<"Solving equations: "<<endl;
	cout<<"  Compute du/dt: grad(u) "<<flush;
	Gradient(u);
	
	cout<<"j1/j2 "<<flush;
	int i;
	
	for (i=0; i< u.N(); i++) {
		d22[i]=d22[i]*dx[i]+d12[i]*dy[i]; // j1
		d11[i]=d12[i]*dx[i]+d11[i]*dy[i]; // j2
	}
 	
	
	cout<<"u=u+tau*du/dt "<<endl;
	//Gradient(d22,0); // dj1/dx
	//Gradient(d11,1); // dj2/dy
	
	for (i=0; i< u.N(); i++) 
		u[i]+=tau*(dx[i]+dy[i]);
	
	
	return 0;
}

template <class ImgType, int NDim>
int AnisotropicDiffusionFilter<ImgType,NDim>::UpdateGaussianFilter(int sx,int sxy,int cnt)
{
	float s;
	if (cnt<rho.size()) {
		s=rho[cnt];
			
		NRho=int(ceil(s*1.96))*2+1;	// This must be made flexible depending on the value of sigma.
		cout<<"Gaussian kernel rho:"<<s<<" size: "<<NRho<<endl;

		if (RhoKernel) delete [] RhoKernel;
		RhoKernel=new float [NRho];

		if (IndRhoKernel) delete [] IndRhoKernel;
		IndRhoKernel=new int [3*NRho];

		int i,j;

		for (i=0; i<3; i++) {
			IndRhoKernel[i*NRho]=0;

		}
		RhoKernel[0]=1/(2*pi*pow(s,NDim));

		for (i=1,j=1; i<NRho; i+=2,j++) {
			IndRhoKernel[i]=-j;IndRhoKernel[i+1]=j;
			IndRhoKernel[i+NRho]=-j*sx;IndRhoKernel[i+NRho+1]=j*sx;
			IndRhoKernel[i+2*NRho]=-j*sxy;IndRhoKernel[i+2*NRho+1]=j*sxy;
			RhoKernel[i]=exp(-(j*j)/(2*s*s))*RhoKernel[0];
			RhoKernel[i+1]=RhoKernel[i];
		}
	}
	return DiffusionBaseFilter<ImgType,NDim>::UpdateGaussianFilter(sx,sxy,cnt);
}

template <class ImgType, int NDim>
		float AnisotropicDiffusionFilter<ImgType,NDim>::getG_LUT(float s) 
	{
		if (s<=LUTindMin)
			return 1;

		if (s>LUTindMax)
			return 0;


		return gLUT[int((s-LUTindMin)/LUTindStep)];
	}



	template <class ImgType, int NDim>
		int AnisotropicDiffusionFilter<ImgType,NDim>::ComputeLUT(int N)
	{
		LUTindMin=lambda*pow(-3.315f/(log(NumLimitDiffusivity)),0.125f);
		LUTindMax=lambda*pow(-3.315f/(log(1.0f-1e-6f)),0.125f);
		LUTindStep=(LUTindMax-LUTindMin)/(N-1);
		if (gLUT) delete [] gLUT;
		cout<<"G(min)="<<LUTindMin<<" G(max)="<<LUTindMax<<endl;
		gLUT=new float[N];

		NLut=N;

		float *pGLut=gLUT;
		int i;
		float s;
		for (i=0, s=LUTindMin; i<NLut; i++, s+=LUTindStep)
			pGLut[i]=1.0f-exp(-3.315f/pow(s/lambda,8.0f));

		return 1;
	}

}
#endif
