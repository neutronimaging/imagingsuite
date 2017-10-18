//<LICENCE>

#ifndef __SHOCKWORKER_H
#define __SHOCKWORKER_H

#include <iostream>
#include <deque>
#include <image/image.h>
#include "linearfilterworker.h"
#include "basequeueworker.h"
#include <visualization/imgviewer.h>

using namespace std;
using namespace Image;
/**
@author Anders Kaestner
*/
namespace QueueFilter {
typedef enum {
		Osher_Filter=0, 			/// Oshers shock filter
		Alvarez_Mazorra_Filter=1	/// Alvarez Mazorras extension to Osher includes smoothing
	} ShockType;

template <class ImgType>
class ShockWorker: public BaseQueueWorker<ImgType>
{
public:
    ShockWorker(float Tau, float Sigma, float Amount, ShockType st, ostream & os=cout);

	int insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res);
	int reset();
	int size() {return  smoothLaplacian ? smoothLaplacian->size() : Nkernel;}
	virtual int kernelsize() {return Nkernel;}
    ~ShockWorker();
	int SetViewer(Visualization::CImgViewer *Fig) {this->fig=Fig; return 1;}

    /*!
        \fn QueueFilter::ShockWorker::setDims(const int *Dims)
     */
    int setDims(const unsigned int *Dims)
    {
		if (smoothLaplacian)
       		smoothLaplacian->setDims(Dims);
       u.resize(Dims);
	   laplacian.resize(Dims);
	   curvature.resize(Dims);
       return BaseQueueWorker<ImgType>::setDims(Dims);
    }
    virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
private:
	int first_derivatives();
	int second_derivatives();
	int absGradient();
	int solve(CImage<float,2> &absGrad,CImage<float,2> &laplacian,CImage<float,2> &curv);
	float FluxDelimiter(float a,float b);

	deque<CImage<ImgType,2> > srcQ;
	deque<CImage<ImgType,2> > dxQ;
	deque<CImage<ImgType,2> > dyQ;
	deque<CImage<ImgType,2> > dzQ;
	deque<CImage<ImgType,2> > absGradQ;
	deque<CImage<ImgType,2> > laplacianQ;
	deque<CImage<ImgType,2> > curvatureQ;

	CImage<ImgType,2> u;
	CImage<ImgType,2> laplacian;
	CImage<ImgType,2> curvature;
	CImage<ImgType,2> absGrad;

	float tau;
	float sigma;
	float amount;
	int line;
	int Nkernel,Nkernel2;
	int cnt;

	ShockType shockType;

	int printQStatus();
	LinearFilterWorker<ImgType> *smoothLaplacian;
};

template <class ImgType>
ShockWorker<ImgType>::ShockWorker(float Tau, float Sigma, float Amount, ShockType st, ostream & os):BaseQueueWorker<ImgType>(os)
{
	tau=Tau;
	sigma=Sigma;
	amount=Amount;
	shockType=st;
	float k[512];
	if (st==Osher_Filter) {
		Nkernel=3;
		Nkernel2=1;
		smoothLaplacian=NULL;
	}
	else {
		Nkernel2=int(2.5*sigma);
		Nkernel=Nkernel2*2+1; // Make Nkernel odd
		
		float sum=0.0f;
		int i,j;
		for (i=-Nkernel2,j=0; i<=Nkernel2;i++,j++) {
			k[j]=exp(-i*i/(2*sigma*sigma));
			sum+=k[j];
		}
		for (i=0; i<Nkernel;i++) 
			k[i]=k[i]/sum;
		
		this->smoothLaplacian=new LinearFilterWorker<ImgType>(k,Nkernel);
		this->smoothLaplacian->mirror_top=false;
		this->smoothLaplacian->mirror_bottom=false;
	}
	line=-1;
	cnt=0;
	this->workername="ShockWorker";
}

template <class ImgType>
ShockWorker<ImgType>::~ShockWorker()
{
	if (this->smoothLaplacian) delete this->smoothLaplacian;
}


template <class ImgType>
int ShockWorker<ImgType>::insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res)
{
	CImage<ImgType,2> smooth;
	if (this->fig) {
		this->fig->subplot(2,3,2); 
		this->fig->colormap(Visualization::CM_temp);
	}
//	if (srcQ.empty()) srcQ.push_back(img);
	cnt++;
	srcQ.push_back(img);
	if (srcQ.size()>1)
		first_derivatives();	
	
	if (srcQ.size()>2) {
		second_derivatives();

		dxQ.pop_front();
		dyQ.pop_front();
		dzQ.pop_front();
		
		absGradient();
		
		if (shockType==Alvarez_Mazorra_Filter) {
			if (cnt>Nkernel2+2) {
				curvatureQ.push_back(curvature);
			}
			else {
				srcQ.pop_front();
				absGradQ.pop_front();
			}
			
			if ((line=this->smoothLaplacian->insert(laplacian,smooth))>-1) {
								
				solve(absGradQ.front(),smooth,curvatureQ.front());
				
				srcQ.pop_front();
				absGradQ.pop_front();
				curvatureQ.pop_front();
				
				res=u;
			}
			
		}
		else {
			line++;
			
			srcQ.pop_front();
			solve(absGradQ.front(),laplacian,smooth);
			
			absGradQ.pop_front();
			
			res=u;
		}

	}
	
	return line;
}

template <class ImgType>
int ShockWorker<ImgType>::solve(CImage<float,2> &G,CImage<float,2> &L,CImage<float,2> &curv)
{
	int pos;
	u=srcQ.front();

	switch (shockType) {
		case Osher_Filter:
		for (pos=0; pos<u.N(); pos++) {
			if (L[pos]>0.0f) // check sign(laplacian)
				u[pos]-=tau*G[pos];
			else if (L[pos]<0.0f)
				u[pos]+=tau*G[pos];
				else
					u[pos]=u[pos];
			}
		break;
		case Alvarez_Mazorra_Filter:
		for (pos=0; pos<u.N(); pos++) {
			if (L[pos]>0.0f) // check sign(laplacian)
				u[pos]+=tau*(-G[pos]+amount*curv[pos]);
			else if (L[pos]<0.0f)
					u[pos]+=tau*(G[pos]+amount*curv[pos]);
				else
					u[pos]+=tau*amount*curv[pos];
			}
		break;
	}

	return 0;
}

template <class ImgType>
int ShockWorker<ImgType>::reset()
{
	dxQ.clear();
	dyQ.clear();
	dzQ.clear();
	srcQ.clear();
	absGradQ.clear();
	curvatureQ.clear();

	if (smoothLaplacian)
		this->smoothLaplacian->reset();
	line=-1;
	cnt=0;

	return 0;
}

template <class ImgType>
int ShockWorker<ImgType>::first_derivatives()
{
	int i,j;
	float *pImg, *pTmp;
	deque<float *> imgPQ;
	CImage<float,2> tmp(this->dims);
	
	// Compute du/dx
	for (i=0; i<this->dims[1]; i++) {
		pTmp=tmp.getLineptr(i);
		pImg=srcQ.back().getLineptr(i);
		pTmp[0]=0;
		pTmp[this->dims[0]-1]=0;
		for (j=1; j<this->dims[0]-1; j++) 
			pTmp[j]=pImg[j]-pImg[j-1];
	}
			
	if (dxQ.empty())
		dxQ.push_back(tmp);
	dxQ.push_back(tmp);
	
	// Compute du/dy
	pTmp=tmp.getLineptr(0);
	for (i=0; i<this->dims[0]; i++)
		pTmp[i]=0;
	memcpy(tmp.getLineptr(this->dims[1]-1),pTmp,this->dims[0]*sizeof(float));
	
	imgPQ.clear();
	imgPQ.push_back(srcQ.back().getLineptr(0));
	for (i=1; i<this->dims[1]-1; i++) {
		pTmp=tmp.getLineptr(i);
		imgPQ.push_back(srcQ.back().getLineptr(i));
		
		for (j=0; j<this->dims[0]; j++) 
			pTmp[j]=imgPQ[1][j]-imgPQ[0][j];

		imgPQ.pop_front();
	}
	
	if (dyQ.empty())
		dyQ.push_back(tmp);
	
	dyQ.push_back(tmp);
	imgPQ.clear();

	// Computing du/dz

	if (dzQ.empty()) {
		CImage<float,2> zero(this->dims);
		dzQ.push_back(zero);
	}

	deque<CImage<float,2> >::iterator srcIT=srcQ.end()-1;
	float  *pB=srcIT->getDataptr();
	srcIT--;
	float  *pA=srcIT->getDataptr();

	pTmp=tmp.getDataptr();
	for (i=0; i<tmp.N(); i++)
		pTmp[i]=pB[i]-pA[i];

	dzQ.push_back(tmp);


	return 0;
}

template <class ImgType>
int ShockWorker<ImgType>::second_derivatives()
{
	int i,j,k,x,y,z;
	int zstart=0,zstop=1;
	
	float L=0, h,tmp;
	float *pLine[3], *pLaplacian,*pDx[3],*pDy[3],*pDz[3], *pCurvature;
		
	int dim, dim_offs, kp1,km1;
	int m_sx,p_sx,m_sxy,p_sxy;
	int sx=this->dims[0], sxy=this->dims[0]*this->dims[1];
	laplacian=0.0f;
	curvature=0.0f;

	float epsilon=1e-8;
	float Ix,Iy,Iz,Ix2,Iy2,Iz2,Ixx,Iyy,Izz,Ixy,Ixz,Iyz;
	Iz=0.0f;
	
	for (j=1; j<this->dims[1]-1; j++) {
		for (i=0; i<3; i++) {
			pLine[i]=(srcQ.end()-3+i)->getLineptr(j);

			pDx[i]=dxQ[i].getLineptr(j);
			pDy[i]=dyQ[i].getLineptr(j);
			pDz[i]=dzQ[i].getLineptr(j);
		}

		pLaplacian=laplacian.getLineptr(j);
		if (shockType==Alvarez_Mazorra_Filter)
			pCurvature=curvature.getLineptr(j);
					
		for (k=1; k<this->dims[0]-1; k++) {
			kp1=k+1; km1=k-1;
			m_sx=k-sx;
			p_sx=k+sx;
			
			Ix=0.5*(pDx[1][k]+pDx[1][kp1]); // Dx=[-1 0. 1]/2
			Ix2=Ix*Ix;
			Ixx=pLine[1][km1]+pLine[1][kp1]-2*pLine[1][k]; // Dxx=[1 -2. 1]

			Iy=0.5*(pDy[1][k]+pDy[1][p_sx]); 
			Iy2=Iy*Iy;
			Iyy=pLine[1][m_sx]+pLine[1][p_sx]-2*pLine[1][k];

			
			Iz=0.5*(pDz[1][k]+pDz[2][k]);
			Iz2=Iz*Iz;
			Izz=pLine[0][k]+pLine[2][k]-2*pLine[1][k];

			Ixy=0.25*((pDx[1][p_sx]+pDx[1][p_sx+1])-(pDx[1][m_sx]+pDx[1][m_sx+1])); 
			
			Ixz=0.25*((pDx[2][k]+pDx[2][kp1])-(pDx[0][k]+pDx[0][kp1]));
			
			Iyz=0.25*((pDy[2][k]+pDy[2][p_sx])-(pDy[0][k]+pDy[0][p_sx])); 
			

			h=1/(Ix2+Iy2+Iz2+epsilon);
			pLaplacian[k]=(Ixx*Ix2+Iyy*Iy2+Izz*Iz2+2*(Ixy*Ix*Iy+Ixz*Ix*Iz+Iyz*Iy*Iz))*h;
					
			if (shockType==Alvarez_Mazorra_Filter) 
				pCurvature[k]=0.5*(Ixx*(Iy2+Iz2)+(Iyy+Izz)*Ix2-2*Ix*(Ixy*Iy+Ixz*Iz))*h;
				
			if (!(fabs(Ix)+fabs(Iy)+fabs(Iz))) {
				pLaplacian[k]=Ixx;
				if (shockType==Alvarez_Mazorra_Filter) 
					pCurvature[k]=Iyy;
			}
		}
	}
	

	return 0;
}

template <class ImgType>
int ShockWorker<ImgType>::absGradient() 
{
	int i,k;
	float *pImg,*pDXa, *pDXb, *pDYa,*pDYb, *pDZa,*pDZb;
	CImage<float,2> img(this->dims);
	
	int sx=this->dims[0];
	for (i=0; i<this->dims[1]-1; i++) {
		pDXa=dxQ[0].getLineptr(i);
		pDXb=pDXa+1;
		
		pDYa=dyQ[0].getLineptr(i);
		pDYb=dyQ[0].getLineptr(i+1);

		pDZa=dzQ[0].getLineptr(i);
		pDZb=dzQ[1].getLineptr(i);
	/*
		pDXa=dxQ[1].getLineptr(i);
		pDXb=pDXa+1;
		
		pDYa=dyQ[1].getLineptr(i);
		pDYb=dyQ[1].getLineptr(i+1);

		pDZa=dzQ[1].getLineptr(i);
		pDZb=dzQ[2].getLineptr(i);
		*/
		pImg=img.getLineptr(i);

		for (k=0; k<this->dims[0]-1; k++)
			pImg[k]=sqrt(FluxDelimiter(pDXa[k],pDXb[k])+
						FluxDelimiter(pDYa[k],pDYb[k])+
						FluxDelimiter(pDZa[k],pDZb[k]));
		
		pImg[this->dims[0]-1]=0.0f; // right edge=0
	}
	absGradQ.push_back(img);

	return 0;
}

template <class ImgType>
float  ShockWorker<ImgType>::FluxDelimiter(float a,float b)
{
	if ((a*b<0.0f))
		return 0.0;
	
	float absA=fabs(a), absB=fabs(b), tmp=absA<absB ? absA : absB;
	
	return tmp*tmp;	
}

template <class ImgType>
int ShockWorker<ImgType>::ShockWorker::printQStatus()
{
	this->logstream<<"Gradients Qs: dx="<<dxQ.size()<<", dy="<<dyQ.size()<<", dz="<<dzQ.size()<<", abs(grad)="<<absGradQ.size()<<", laplacian="<<curvatureQ.size()<<endl;
	//logstream<<"Smooth Q="<<smoothLaplacian->size()<<", srcQ="<<srcQ.size()<<endl;

	return 0;
}

template<class ImgType>
int ShockWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();
	parnames.push_back(string("tau")); parvals.push_back(tau);
	parnames.push_back(string("sigma")); parvals.push_back(sigma);
	parnames.push_back(string("amount")); parvals.push_back(amount);
	parnames.push_back(string("shocktype")); parvals.push_back(shockType);
	
	return 1;
}

}
#endif
