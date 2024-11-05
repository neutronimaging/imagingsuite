

#ifndef __ISSFILTER_H
#define __ISSFILTER_H

#include <iostream>

using namespace Fileio;

template <typename T, int NDims=2>
class ISSfilter {
	std::ostream & logstream;
public:
	friend class UnitISSFilter;
	ISSfilter(std::ostream &os=std::cout) :logstream(os) {m_dEpsilon=1e-20;}
  int Process(Image::CImage<T,NDims> &img, double dTau, double dLambda, double dAlpha, int nN);

private:
	enum Direction {
		dirX=0,
		dirY=1,
    dirZ=2
	};
  Image::CImage<T,NDims> m_f;
  Image::CImage<T,NDims> m_v;
	double m_dTau; 
  double m_dLambda;
  double m_dAlpha;
	double m_dEpsilon;
  int _SolveIteration(Image::CImage<T,NDims> &img);
  int _LeadDiff(Image::CImage<T,NDims> &img, Image::CImage<T,NDims> &diff, Direction dir);
  int _LagDiff(Image::CImage<T,NDims> &img, Image::CImage<T,NDims> &diff, Direction dir);
  int _P(Image::CImage<T,NDims> &img, Image::CImage<T,NDims> &res);
};

template <typename T, int NDims>
int ISSfilter<T, NDims>::Process(Image::CImage<T,NDims> &img, double dTau, double dLambda, double dAlpha, int nN)
{
	m_f=img;

	m_dTau=dTau; 
  m_dLambda=dLambda;
  m_dAlpha=dAlpha;
	m_v.resize(img.getDimsptr());

	for (int i=0; i<nN	; i++) {
		if (i%10==9)
			logstream<<i<<endl<<flush;
		else
			logstream<<i<<", "<<flush;

		_SolveIteration(img);
	}
	logstream<<endl;

	return nN;
}

template <typename T, int NDims>
int ISSfilter<T, NDims>::_SolveIteration(Image::CImage<T,NDims> &img)
{
	Image::CImage<T,NDims> p;

	_P(img,p);
	T *pU=img.getDataptr();
	T *pV=m_v.getDataptr();
	T *pP=p.getDataptr();
	T *pF=m_f.getDataptr();

	const unsigned int N=img.N();
	T tempU;
	T tempFU;
	double dTauAlpha=m_dTau*m_dAlpha;
	for (unsigned int i=0; i<N ; i++) {
		tempFU=pF[i]-pU[i];
		pU[i]+=(T)(m_dTau*(pP[i]+m_dLambda*(tempFU+pV[i])));
		pV[i]+=(T)(dTauAlpha*tempFU);
	}

	return 0;
}

template <typename T, int NDims>
int ISSfilter<T, NDims>::_LeadDiff(Image::CImage<T,NDims> &img, 
                                  Image::CImage<T,NDims> &diff, 
																	Direction dir)
{
	diff.resize(img.getDimsptr());
	T *pDiff;
	T *pA, *pB;
	unsigned int x,y,z;

	const unsigned int * dims=img.getDimsptr();
	unsigned int sx=dims[0]-1;	
	unsigned int sy=dims[1]-1;	
  unsigned int sz=dims[2]-1;
	unsigned int sxy=dims[0]*dims[1];
	switch (dir) {
		case dirX : 
      for (z=0; z<dims[2]; z++) {
			  for (y=0; y<dims[1]; y++) {
				  pA=img.getLineptr(y,z);
				  pB=pA+1;
				  pDiff=diff.getLineptr(y,z);
  				
				  for (x=0; x<sx; x++) 
					  pDiff[x]=pB[x]-pA[x];
				  pDiff[sx]=0;	
        }
      }
			break;
		case dirY :
      for (z=0; z<dims[2]; z++) {
			  for (y=0; y<sy; y++) {
				  pA=img.getLineptr(y,z);
				  pB=img.getLineptr(y+1,z);
				  pDiff=diff.getLineptr(y,z);
  				
				  for (x=0; x<dims[0]; x++) 
					  pDiff[x]=pB[x]-pA[x];				
			  }
			  memset(diff.getLineptr(sy,z),0,sizeof(T)*dims[0]);
      }

			break;
      case dirZ :
      memset(diff.getLineptr(0,sz),0,sizeof(T)*dims[1]*dims[0]);
      for (z=0; z<sz; z++) {
        pA=img.getLineptr(0,z);
				pB=img.getLineptr(0,z+1);
				pDiff=diff.getLineptr(0,z);
			  for (x=0; x<sxy; x++) 
					pDiff[x]=pB[x]-pA[x];
      }
			break;
	}

	return 0;
}

template <typename T, int NDims>
int ISSfilter<T, NDims>::_LagDiff( Image::CImage<T,NDims> &img, 
                                  Image::CImage<T,NDims> &diff,
																	Direction dir)
{
  
	diff.resize(img.getDimsptr());
	T *pDiff;
	T *pA, *pB;
	unsigned int x,y,z;

	const unsigned int * dims=img.getDimsptr();
	int sx=dims[0]-1;	
	int sy=dims[1]-1;	
  int sz=dims[2]-1;
	int sxy=dims[0]*dims[1];
	switch (dir) {
		case dirX : 
      for (z=0; z<dims[2]; z++) {
			  for (y=0; y<dims[1]; y++) {
				  pB=img.getLineptr(y,z);
				  pA=pB-1;
				  pDiff=diff.getLineptr(y,z);
				  pDiff[0]=(T)0;
				  for (x=1; x<dims[0]; x++) 
					  pDiff[x]=pB[x]-pA[x];
  				
        }
      }
			break;
					
		case dirY :
      for (z=0; z<dims[2]; z++) {
			  memset(diff.getLineptr(0,z),0,sizeof(T)*dims[0]);
			  for (y=1; y<dims[1]; y++) {
				  pA=img.getLineptr(y-1,z);
				  pB=img.getLineptr(y,z);
				  pDiff=diff.getLineptr(y,z);
  				
				  for (x=0; x<dims[0]; x++) 
					  pDiff[x]=pB[x]-pA[x];
			  }
      }
			break;
    case dirZ :
      memset(diff.getLineptr(0,0),0,sizeof(T)*dims[1]*dims[0]);
      for (z=1; z<dims[2]; z++) {
        pA=img.getLineptr(0,z-1);
				pB=img.getLineptr(0,z);
				pDiff=diff.getLineptr(0,z);
			  for (x=0; x<sxy; x++) 
					pDiff[x]=pB[x]-pA[x];
			}
      
			break;
	}

	return 0;
}
	
template <typename T, int NDims>
int ISSfilter<T, NDims>::_P(Image::CImage<T,NDims> &img, Image::CImage<T,NDims> &res)
{
  Image::CImage<T,NDims> dx,dy,dz, d2x, d2y,d2z;
	const unsigned int N=img.N();
	
	_LeadDiff(img,dx,dirX);
	_LeadDiff(img,dy,dirY);

  if (NDims==3) {
    _LeadDiff(img,dz,dirZ);
	}
	
	T *pX=dx.getDataptr();
	T *pY=dy.getDataptr();
  T *pZ=dz.getDataptr();

	T denom;
	if (NDims==3) {
		for (unsigned int i=0; i< N; i++) {
			denom=(T)(1/sqrt(pX[i]*pX[i]+pY[i]*pY[i]+pZ[i]*pZ[i]+m_dEpsilon));
			pX[i]*=denom;
			pY[i]*=denom;
			pZ[i]*=denom;
		}
	}
	else {
		for (unsigned int i=0; i< N; i++) {
			denom=(T)(1/sqrt(pX[i]*pX[i]+pY[i]*pY[i]+m_dEpsilon));
			pX[i]*=denom;
			pY[i]*=denom;
		}
	}

	d2x.resize(dx.getDimsptr());
	_LagDiff(dx,d2x,dirX);
	
	d2y.resize(dy.getDimsptr());
	_LagDiff(dy,d2y,dirY);

  if (NDims==3) {
		d2z.resize(dz.getDimsptr());
    _LagDiff(dz,d2z,dirZ);
	}

	pX=d2x.getDataptr();
	pY=d2y.getDataptr();
  pZ=d2z.getDataptr();

	res.resize(img.getDimsptr());
	T *pRes=res.getDataptr();
	if (NDims==3) {
		for (unsigned int i=0; i< N; i++) 
			pRes[i]=pX[i]+pY[i]+pZ[i];
	}
	else {
		for (unsigned int i=0; i< N; i++) 
			pRes[i]=pX[i]+pY[i];
	}
  
	return 0;
}

#endif
