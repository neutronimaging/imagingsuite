#include "../../include/math/nonlinfit.h"
#include "../../include/math/jama_inverses.h"
#include "../../include/logging/logger.h"

#include <tnt.h>
#include <jama_lu.h>
#include <cstring>
#include <sstream>
#include <cmath>

using namespace TNT;
using namespace JAMA;

namespace Nonlinear {
	int stddev(long double *x,long double *y,int ndata,CBaseFunction &fn,long double *sig);

	void mrqmin(long double *x,long double *y,long double *sig,int ndata,
		Array2D<long double> &covar, Array2D<long double> &alpha,
		long double &chisq,
		CBaseFunction &fn,
		long double &alamda);

	void mrqcof(long double *x,long double *y,long double *sig,int ndata, 
		Array2D<long double> &alpha,
		Array1D<long double> &beta,
		long double &chisq,
		CBaseFunction &fn);

	void covsrt(Array2D<long double> &covar, CBaseFunction &fn);

	//######################################

	int LevenbergMarquardt(double *_x,double *_y, int ndata, CBaseFunction &fn, double eps,double *s)
	{
		kipl::logging::Logger logger("LevenbergMarquardt");
		std::stringstream msg;

	//	int npars=fn.getNpars();
		int mfit=fn.getNpars2fit();
		Array2D<long double> covar(mfit,mfit),alpha(mfit,mfit);
		long double chisq,chiold,alambda;
		long double *x=new long double[ndata];
		long double *y=new long double[ndata];
		

		long double *sig=new long double[ndata];
		
		int i,n;
		
		for (i=0; i<ndata; i++) {
			x[i]=_x[i];
			y[i]=_y[i];
			}

		fn.PrintPars();
		if (s) {
			logger(kipl::logging::Logger::LogVerbose,"Have s");
			for (i=0; i<ndata; i++)
				sig[i]=s[i];
		}	
		else {
			logger(kipl::logging::Logger::LogVerbose,"Initialize s");
			for (i=0; i<ndata; i++)
				sig[i]=1;
			stddev(x,y,ndata,fn,sig);
		}

		logger(kipl::logging::Logger::LogVerbose,"Initialize mrqmin");
					
		alambda=-1;
		chisq=0.0;
		mrqmin(x,y,sig,ndata,covar,alpha,chisq,fn,alambda);
		n=0;
		i=0;
		chiold=0.0;


		// get the solution by iterations
		logger(kipl::logging::Logger::LogVerbose,"Iterate mrqmin toward the solution.");
		while ((n<3) && (i<2000)) {
			msg.str("");

			if ((fabs(chisq-chiold)<eps) && ((chisq-chiold)<=0))// && (chisq!=chiold))
				n++;
			else
				n=0;

			msg<<"Iteration:"<<i<<", termination count:"<<n<<", chisq:"<<chisq;
			logger(kipl::logging::Logger::LogVerbose,msg.str());
			if (!s)
				stddev(x,y,ndata,fn,sig);

			chiold=chisq;

			mrqmin(x,y,sig,ndata,covar,alpha,chisq,fn,alambda);
			msg.str("");
			msg<<"mrqmin gives alambda:"<<alambda<<" chisq:"<<chisq<<" eps"<<fabs(chisq-chiold);
			logger(kipl::logging::Logger::LogVerbose,msg.str());
			fn.PrintPars();
			i++;
		}

		// solve the final parameter-set
		alambda=0;
		if (!s)
			stddev(x,y,ndata,fn,sig);
		mrqmin(x,y,sig,ndata,covar,alpha,chisq,fn,alambda);
		delete [] sig;
		delete [] x;
		delete [] y;
		return 1;
	}


	int stddev(long double *x,long double *y,int ndata,
		CBaseFunction &fn,
		long double *sig)
	{
		long double yest,sigma;

		sigma=0;

        int i;
		for (i=0; i<ndata; i++) {
			yest=fn(x[i]);
			sigma+=(y[i]-yest)*(y[i]-yest);
		}
		//sigma=sqrt(sigma/(ndata-na));
		sigma=sqrt(sigma/(ndata-1));

		for (i=0; i<ndata; i++)
			sig[i]=sigma;

		return 1;
	}

	void mrqmin(long double *x,long double *y,long  double *sig,int ndata,
			Array2D<long double> &covar,Array2D<long double> &alpha,
		long double &chisq,
		CBaseFunction &fn,
		long double &alamda)
	{
		kipl::logging::Logger logger("mrqmin");
		std::stringstream msg;
		int j,k,l;
		int ma=fn.getNpars();
		static int mfit;
		static long double ochisq;
		static Array1D<long double> atry,beta,da,oneda;

		if (alamda < 0.0) {
			logger(kipl::logging::Logger::LogVerbose,"Initializing vectors");

			atry=Array1D<long double>(ma);
			beta=Array1D<long double>(ma);
			da=Array1D<long double>(ma);
			mfit=fn.getNpars2fit();
			oneda=Array1D<long double>(mfit);
			alamda=0.001;
			mrqcof(x,y,sig,ndata,alpha,beta,chisq,fn);
			ochisq=(chisq);
			for (j=0;j<ma;j++) atry[j]=fn[j];
		}
		
		for (j=0;j<mfit;j++) {
			for (k=0;k<mfit;k++)
				covar[j][k]=alpha[j][k];

			covar[j][j]=alpha[j][j]*(1.0+(alamda));
			oneda[j]=beta[j];
		}
		logger(kipl::logging::Logger::LogVerbose,"Solving equation system with LU");
		LU<long double> X(covar);
		oneda=X.solve(oneda);

		//TSVD<long double> X(covar); logger(kipl::logging::Logger::LogVerbose,"Solving equation system with SVD");


		for (j=0;j<mfit;j++) da[j]=oneda[j];
		if (alamda == 0.0) {
			covsrt(covar,fn);
			return;
		}

		Array1D<long double> tmp(ma);

		for (j=0,l=0;l<ma;l++) {
			tmp[l]=fn[l];
			if (!fn.IsLocked(l)) fn[l]=fn[l]+da[j++];
		}

		logger(kipl::logging::Logger::LogVerbose,"Calling mrqcof");
		mrqcof(x,y,sig,ndata,covar,da,chisq,fn);
		if (chisq < ochisq) {
			alamda *= 0.1;
			ochisq=chisq;
			for (j=0;j<mfit;j++) {
				for (k=0;k<mfit;k++)
					alpha[j][k]=covar[j][k];

				beta[j]=da[j];
			}
		} else {
			alamda *= 10.0;
			chisq=ochisq;
			for (j=0; j<ma; j++) fn[j]=tmp[j]; //restore old info
		}
	}

	void mrqcof(long double *x, long double *y,long double *sig, int ndata, 
		Array2D<long double> &alpha, 
		Array1D<long double> &beta,
		long double &chisq, 
		CBaseFunction &fn)
	{
		int i,j,k,l,m,mfit=0;
		long double ymod,wt,sig2i,dy;
		int ma=fn.getNpars();

		Array1D<long double> dyda(ma);

		mfit=fn.getNpars2fit();

		for (j=0;j<mfit;j++) {
			for (k=0;k<j;k++)
				alpha[j][k]=0.0;
			beta[j]=0.0;
		}
		chisq=0.0;

		for (i=0;i<ndata;i++) {
			fn(x[i],ymod,dyda);
			sig2i=1.0/(sig[i]*sig[i]);
			

			dy=y[i]-ymod;
			
			for (j=-1,l=0;l<ma;l++) {
				if (!fn.IsLocked(l)) {
					wt=dyda[l]*sig2i;
					for (j++,k=-1,m=0 ; m<l ; m++) {
					//	std::cout<<"j="<<j<<", k="<<k<<", m="<<m<<", l="<<l<<std::endl;
						if (!fn.IsLocked(m))
							alpha[j][++k] += wt*dyda[m];
					}
					beta[j] += dy*wt;
				}
			}
			//dy=log(y[i-1])-log(ymod);
			chisq += dy*dy*sig2i;
		}
		for (j=0;j<mfit;j++)
			for (k=0;k<j;k++) alpha[k][j]=alpha[j][k];

	}

	void covsrt(Array2D<long double> &covar, CBaseFunction &fn)
	{
		int i,j,k, mfit=fn.getNpars2fit();

		int ma=covar.dim2();

		for (i=mfit+1;i<=ma;i++)
			for (j=1;j<=i;j++) covar[i-1][j-1]=covar[j-1][i-1]=0.0;
		k=mfit-1;
		for (j=ma-1;j>=0;j--) {
			if (fn.IsLocked(j)) {
				for (i=0;i<ma;i++) swap(covar[i][k],covar[i][j]);
				for (i=0;i<ma;i++) swap(covar[k][i],covar[j][i]);
				k--;
			}
		}
	}

	//#####################################

	CBaseFunction::CBaseFunction(int n) 
	{
		m_pars=new long double[n]; 
		m_lock=new int[n];
		for (int i=0; i<n; i++)
			m_lock[i]=0;

		m_pars2fit=n;
		m_Npars=n;
	}	
	
	long double & CBaseFunction::operator[](int n)
	{
		if (n<m_Npars)
			return m_pars[n];
		else
			return m_pars[0];
	}
	
	int CBaseFunction::UpdatePars(long double *pars)
	{
		memcpy(m_pars,pars,sizeof(long double)*m_Npars);

		return 1;
	}
	
	int CBaseFunction::getNpars() 
	{
		return m_Npars;
	}
	
	int CBaseFunction::setLock(int *lv)
	{
		m_pars2fit=0;
		for (int i=0; i<m_Npars; i++) 
			m_pars2fit+=!(m_lock[i]=lv[i]);

		return m_pars2fit;
	}

	int CBaseFunction::IsLocked(int n) 
	{
		if (n<m_Npars)
			return m_lock[n];
		else 
			return 1;
	}
	
	int CBaseFunction::getNpars2fit()
	{
		return m_pars2fit;
	}
	
	CBaseFunction::~CBaseFunction() 
	{
		delete [] m_pars;
		delete [] m_lock;
	}
	

	SumOfGaussians::SumOfGaussians(int n) : CBaseFunction(3*n)
	{

	}

	long double SumOfGaussians::operator()(long double x)
	{
		int i;
		long double fac,ex,arg;

		long double y=0.0;
		for (i=0;i<m_Npars;i+=3) {

			arg=(x-m_pars[i+1])/m_pars[i+2];
			ex=exp(-arg*arg);
			fac=m_pars[i]*ex*2.0*arg;
			y += m_pars[i]*ex;
		}
		return y;
	}

	int SumOfGaussians::operator()(long double x, long double &y, Array1D<long double> & dyda)
	{
		int i;
		long double fac,ex,arg;

		if (dyda.dim()!=m_Npars)
			dyda=Array1D<long double>(m_Npars);

		y=0.0;
		for (i=0;i<m_Npars;i+=3) {

			arg=(x-m_pars[i+1])/m_pars[i+2];
			ex=exp(-arg*arg);
			fac=m_pars[i]*ex*2.0*arg;
			y += m_pars[i]*ex;
			dyda[i]=ex;
			dyda[i+1]=fac/m_pars[i+2];
			dyda[i+2]=fac*arg/m_pars[i+2];
		}
		return 1;
	}

    int SumOfGaussians::Hessian(long double UNUSED(x), Array2D<long double> & UNUSED(hes))
	{

		cerr<<"The Hessian is not available"<<endl;
		return 1;

		/*
		if ((hes.num_rows()!=m_Npars) || (hes.num_cols()!=m_Npars))
		hes.newsize(m_Npars,m_Npars);

		return 1;
		*/
	}

    int SumOfGaussians::Jacobian(long double UNUSED(x), Array2D<long double> & UNUSED(jac))
	{
		cerr<<"The Jacobian is not available"<<endl;
		return 1;
		/*
		if ((jac.num_rows()!=m_Npars) || (jac.num_cols()!=m_Npars))
		jac.newsize(m_Npars,m_Npars);

		return 1;
		*/
	}

	int SumOfGaussians::PrintPars()
	{
		int i;
		char *est=new char[m_Npars];

		for (i=0; i<m_Npars; i++)
			est[i]=m_lock[i] ? ' ': '*';


		for (i=0; i<m_Npars; i+=3) {

			cout<<est[i]<<"A="<<m_pars[i]<<
				" "<<est[i+1]<<"m="<<m_pars[i+1]<<
				" "<<est[i+2]<<"s="<<m_pars[i+2]<<endl;

		}

		delete [] est;
		return 1;
	}

}
