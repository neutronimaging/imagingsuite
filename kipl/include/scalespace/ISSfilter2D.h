//
// C++ Interface: ISSfilter
//
// Description: 
//
//
// Author: Anders Kaestner <perk@itoibm>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "../base/timage.h"
#include "../filters/filter.h"
#include "../math/sums.h"
#include "../logging/logger.h"

#include <sstream>

#define USEDIFF

namespace akipl { namespace scalespace {

template <typename T>
class ISSfilter {
	kipl::logging::Logger logger;
public:

	ISSfilter() : logger("ISSfilter"), m_bErrorPlot(false), m_fErrorPlot(NULL) {m_fEpsilon=1e-15;}
	~ISSfilter() {if (m_fErrorPlot!=NULL) delete [] m_fErrorPlot;}
	int Process(kipl::base::TImage<T,2> &img, float dTau, float dLambda, float dAlpha, int nN);
	void ErrorCurve(bool bErrorPlot) {m_bErrorPlot=bErrorPlot;}
    double const * ErrorCurve() {return m_fErrorPlot;}

private:
	enum Direction {
		dirX=0,
		dirY=1
	};
	kipl::base::TImage<T,2> m_f;
	kipl::base::TImage<T,2> m_v;
	float m_fTau;
	float m_fLambda;
	float m_fAlpha;
	float m_fEpsilon;
	bool m_bErrorPlot;
	double *m_fErrorPlot;
	int _SolveIteration(kipl::base::TImage<T,2> &img);
	int _LeadDiff(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &diff, Direction dir);
	int _LagDiff(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &diff, Direction dir);
	int _Gradient(kipl::base::TImage<T,2> &img,
									kipl::base::TImage<T,2> &diff,
									Direction dir);
	int _P(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &res);
};

template <typename T>
int ISSfilter<T>::Process(kipl::base::TImage<T,2> &img, float dTau, float dLambda, float dAlpha, int nN)
{
	std::ostringstream msg;

	msg.str("");
	msg<<"Filtering "<<img<<", tau="<<dTau<<", lambda="<<dLambda<<", alpha="<<dAlpha<<", N="<<nN;
	logger(kipl::logging::Logger::LogVerbose,msg.str());

	m_f=img;
	m_f.Clone();

	m_fTau=dTau;
	m_fLambda=dLambda;
	m_fAlpha=dAlpha;
	m_v.Resize(img.Dims());
	m_v=static_cast<T>(0);
	if (m_bErrorPlot) {
		if (m_fErrorPlot!=NULL)
			delete [] m_fErrorPlot;
		this->m_fErrorPlot=new double[nN+1];
	}

	for (int i=0; i<nN	; i++) {
		msg.str("");
		msg<<"ISS iteration: "<<i;
		logger(kipl::logging::Logger::LogVerbose,msg.str());
		_SolveIteration(img);
		if (m_bErrorPlot) {
			m_fErrorPlot[i]=kipl::math::meansquareddiff(img.GetDataPtr(),m_f.GetDataPtr(),img.Size());
		}
	}

	return nN;
}

template <typename T>
int ISSfilter<T>::_SolveIteration(kipl::base::TImage<T,2> &img)
{
	kipl::base::TImage<T,2> p;

	_P(img,p);
	T *pU=img.GetDataPtr();
	T *pV=m_v.GetDataPtr();
	T *pP=p.GetDataPtr();
	T *pF=m_f.GetDataPtr();

	const unsigned int N=img.Size();

	T tempFU;
	float fTauAlpha=m_fTau*m_fAlpha;
	for (unsigned int i=0; i<N ; i++) {
		tempFU=pF[i]-pU[i];
		pU[i]+=m_fTau*(pP[i]+m_fLambda*(tempFU+pV[i]));
		pV[i]+=fTauAlpha*tempFU;
	}

	return 0;
}

template <typename T>
int ISSfilter<T>::_Gradient(	    kipl::base::TImage<T,2> &img,
									kipl::base::TImage<T,2> &diff,
									Direction dir)
{
	float kernel[9];
	switch (dir) {
		case dirX : 
			kernel[0] = -3.0f/16.0f;  kernel[1] = 0.0f; kernel[2] = 3.0f/16.0f;
			kernel[3] = -10.0f/16.0f; kernel[4] = 0.0f; kernel[5] = 10.0f/16.0f;
			kernel[6] = -3.0f/16.0f;  kernel[7] = 0.0f; kernel[8] = 3.0f/16.0f;
			break;
		case dirY :
			kernel[0] = -3.0f/16.0f;  kernel[1] = -10.0f/16.0f;  kernel[2] = -3.0f/16.0f;
			kernel[3] = 0.0f;         kernel[4] = 0.0f;          kernel[5] = 0.0f;
			kernel[6] = 3.0f/16.0f;   kernel[7] = 10.0f/16.0f;   kernel[8] = 3.0f/16.0f;

			break;
	}

	size_t dims[2]={3,3};

	kipl::filters::TFilter<float,2> grad(kernel,dims);

	diff=grad(img,kipl::filters::FilterBase::EdgeMirror);

	return 0;
}

template <typename T>
int ISSfilter<T>::_LeadDiff(	kipl::base::TImage<T,2> &img,
									kipl::base::TImage<T,2> &diff,
									Direction dir)
{
	diff.Resize(img.Dims());
	diff=static_cast<T>(0);
	T *pDiff;
	T *pA, *pB;
    int x,y;

    size_t const * const dims=img.Dims();
    int sx=dims[0]-1;
    int sy=dims[1]-1;
    int d0=dims[0];
    int d1=dims[1];
	switch (dir) {
		case dirX : 
            for (y=0; y<d1; y++) {
				pA=img.GetLinePtr(y);
				pB=pA+1;
				pDiff=diff.GetLinePtr(y);
				
				for (x=0; x<sx; x++) 
					pDiff[x]=pB[x]-pA[x];

				pDiff[sx]=pDiff[sx-1];
			}
			break;
					
		case dirY :
			for (y=0; y<sy; y++) {
				pA=img.GetLinePtr(y);
				pB=img.GetLinePtr(y+1);
				pDiff=diff.GetLinePtr(y);
				
                for (x=0; x<d0; x++)
					pDiff[x]=pB[x]-pA[x];				
			}
			//memset(diff.GetLinePtr(sy),0,sizeof(T)*dims[0]);
			memcpy(diff.GetLinePtr(sy),diff.GetLinePtr(sy-1),sizeof(T)*dims[0]);

			break;
	}

	return 0;
}

template <typename T>
int ISSfilter<T>::_LagDiff( 	kipl::base::TImage<T,2> &img,
									kipl::base::TImage<T,2> &diff,
									Direction dir)
{
	diff.Resize(img.Dims());
	diff=static_cast<T>(0);

	T *pDiff;
	T *pA, *pB;
	size_t x,y;

	size_t const * const dims=img.Dims();
	int sx=dims[0]-1;	
	int sy=dims[1]-1;	
	switch (dir) {
		case dirX : 
			for (y=0; y<dims[1]; y++) {
				pB=img.GetLinePtr(y);
				pA=pB-1;
				pDiff=diff.GetLinePtr(y);

				for (x=1; x<dims[0]; x++) 
					pDiff[x]=pB[x]-pA[x];
				
				pDiff[0]=pDiff[1];

			}
			break;
					
		case dirY :

			for (y=1; y<dims[1]; y++) {
				pA=img.GetLinePtr(y-1);
				pB=img.GetLinePtr(y);
				pDiff=diff.GetLinePtr(y);
				
				for (x=0; x<dims[0]; x++)
					pDiff[x]=pB[x]-pA[x];
			}
			//memset(diff.GetLinePtr(0),0,sizeof(T)*dims[0]);
			memcpy(diff.GetLinePtr(0),diff.GetLinePtr(1),sizeof(T)*dims[0]);
			break;
	}

	return 0;
}
	
template <typename T>
int ISSfilter<T>::_P(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &res)
{
	kipl::base::TImage<T,2> dx,dy, d2x, d2y;

#ifdef USEDIFF
	_LagDiff(img,dx,dirX);
	_LagDiff(img,dy,dirY);
#else
	_Gradient(img,dx,dirX);
	_Gradient(img,dy,dirY);
#endif

	T *pX=dx.GetDataPtr();
	T *pY=dy.GetDataPtr();
	T denom;
	for (size_t i=0; i< dx.Size(); i++) {
		denom=static_cast<T>(1)/sqrt(pX[i]*pX[i]+pY[i]*pY[i]+m_fEpsilon);
		pX[i]*=denom;
		pY[i]*=denom;
	}

#ifdef USEDIFF
	_LeadDiff(dx,d2x,dirX);
	_LeadDiff(dy,d2y,dirY);
#else
	_Gradient(dx,d2x,dirX);
	_Gradient(dy,d2y,dirY);
#endif

	pX=d2x.GetDataPtr();
	pY=d2y.GetDataPtr();

	res.Resize(img.Dims());
	T *pRes=res.GetDataPtr();
	for (size_t i=0; i< res.Size(); i++)
		pRes[i]=pX[i]+pY[i];

	return 0;
}


}}

