//<LICENCE>

#include <QDebug>
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>

#include <tnt.h>
#include <jama_qr.h>

#include "../../include/math/nonlinfit.h"
#include "../../include/math/jama_inverses.h"
#include "../../include/math/mathconstants.h"
#include "../../include/logging/logger.h"

using namespace TNT;
using namespace JAMA;
namespace NonlinearDev {
struct Fitmrq {
//Object for nonlinear least-squares fitting by the Levenberg-Marquardt method, also including
//the ability to hold specified parameters at fixed, specified values. Call constructor to bind data
//vectors and fitting functions and to input an initial parameter guess. Then call any combination
//of hold, free, and fit as often as desired. fit sets the output quantities a, covar, alpha,
//and chisq.
    static const int NDONE=4, ITMAX=1000; //Convergence parameters.
    int ndat, ma, mfit;
    VecDoub_I &x,&y,&sig;
    double tol;
    void (*funcs)(const Doub, VecDoub_I &, Doub &, VecDoub_O &);
    VecBool ia;
    VecDoub a; //Output values. a is the vector of fitted coefficients, covar is its covariance matrix, alpha is the curvature matrix, and chisq is the value of 2 fothe fit.
    MatDoub covar;
    MatDoub alpha;
    double chisq;

    void Fitmrq(VecDoub_I &xx, VecDoub_I &yy, VecDoub_I &ssig,
                VecDoub_I &aa,
                void funks(const Doub, VecDoub_I &, Doub &, VecDoub_O &),
                const double TOL=1.e-3) :
        ndat(xx.size()),
        ma(aa.size()),
        x(xx),
        y(yy),
        sig(ssig),
        tol(TOL),
        funcs(funks),
        ia(ma),
        alpha(ma,ma),
        a(aa),
        covar(ma,ma)
    {
        // Constructor.
        // Binds references to the data arrays xx, yy, and ssig, and to a user-supplied
        // function funks that calculates the nonlinear fitting function and its derivatives. Also inputs
        // the initial parameters guess aa (which is copied, not modified) and an optional convergence
        // tolerance TOL. Initializes all parameters as free (not held).
        for (int i=0;i<ma;i++)
            ia[i] = true;
     }

    void hold(const int i, const doub val)
    {
        ia[i]=false;
        a[i]=val;
    }

    void free(const Int i)
    {
        ia[i]=true;
    }

    void fit()
    {
        //    Iterate to reduce the 2 of a fit between a set of data points x[0..ndat-1], y[0..ndat-1]
        //    with individual standard deviations sig[0..ndat-1], and a nonlinear function that depends
        //    on ma coefficients a[0..ma-1]. When 2 is no longer decreasing, set best-fit values
        //    for the parameters a[0..ma-1], and chisq D 2, covar[0..ma-1][0..ma-1], and
        //    alpha[0..ma-1][0..ma-1]. (Parameters held fixed will return zero covariances.)

        int j,k,l,iter,done=0;
        double alamda=.001,ochisq;
        VecDoub atry(ma),beta(ma),da(ma);
        mfit=0;
        for ( j=0; j<ma; j++)
            if (ia[j]) mfit++;

        MatDoub oneda(mfit,1), temp(mfit,mfit);

        mrqcof(a,alpha,beta); // Initialization.

        for (j=0;j<ma;j++)
            atry[j]=a[j];

        ochisq=chisq;

        for (iter=0;iter<ITMAX;iter++) {
            if (done==NDONE) alamda=0.; //Last pass. Use zero alamda.
                for (j=0;j<mfit;j++) { //Alter linearized fitting matrix, by augmenting diagonal elements.
                    for (k=0;k<mfit;k++) covar[j][k]=alpha[j][k];
                covar[j][j]=alpha[j][j]*(1.0+alamda);
                for (k=0;k<mfit;k++)
                    temp[j][k]=covar[j][k];
                oneda[j][0]=beta[j];
            }

            gaussj(temp,oneda); //Matrix solution.

            for (j=0;j<mfit;j++) {
                for (k=0;k<mfit;k++) covar[j][k]=temp[j][k];
                    da[j]=oneda[j][0];
            }

            if (done==NDONE) { //Converged. Clean up and return.
                covsrt(covar);
                covsrt(alpha);
                return;
            }

            for (j=0,l=0;l<ma;l++) //Did the trial succeed?
                if (ia[l]) atry[l]=a[l]+da[j++];

            mrqcof(atry,covar,da);

            if (abs(chisq-ochisq) < MAX(tol,tol*chisq)) done++;

            if (chisq < ochisq) { //Success, accept the new solution.
                alamda *= 0.1;
                ochisq=chisq;
                for (j=0;j<mfit;j++) {
                    for (k=0;k<mfit;k++) alpha[j][k]=covar[j][k];
                    beta[j]=da[j];
                }
                for (l=0;l<ma;l++)
                    a[l]=atry[l];
            }
            else { //Failure, increase alamda.
                alamda *= 10.0;
                chisq=ochisq;
            }
        }
        throw("Fitmrq too many iterations");
    }

    void mrqcof(VecDoub_I &a, MatDoub_O &alpha, VecDoub_O &beta)
    {
    //Used by fit to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and to calculate 2.
        int i,j,k,l,m;
        double ymod,wt,sig2i,dy;
        VecDoub dyda(ma);
        for (j=0; j<mfit; j++) { //Initialize (symmetric) alpha, beta.
            for (k=0; k<=j; k++) alpha[j][k]=0.0;
                beta[j]=0.;
        }

        chisq=0.;

        for (i=0; i<ndat; i++) { //Summation loop over all data.
            funcs(x[i],a,ymod,dyda);
            sig2i=1.0/(sig[i]*sig[i]);
            dy=y[i]-ymod;
            for (j=0,l=0;l<ma;l++) {
                if (ia[l]) {
                    wt=dyda[l]*sig2i;
                    for (k=0,m=0;m<l+1;m++)
                        if (ia[m]) alpha[j][k++] += wt*dyda[m];
                    beta[j++] += dy*wt;
                }
            }
            chisq += dy*dy*sig2i; // And find 2.
        }
        for (j=1;j<mfit;j++) //Fill in the symmetric side.
            for (k=0;k<j;k++) alpha[k][j]=alpha[j][k];
    }

    void covsrt(MatDoub_IO &covar)
    {
    //Expand in storage the covariance matrix covar, so as to take into account parameters that
    //are being held fixed. (For the latter, return zero covariances.)
        int i,j,k;
        for (i=mfit;i<ma;i++)
        for (j=0;j<i+1;j++) covar[i][j]=covar[j][i]=0.0;
        k=mfit-1;
        for (j=ma-1;j>=0;j--) {
            if (ia[j]) {
                for (i=0;i<ma;i++) SWAP(covar[i][k],covar[i][j]);
                for (i=0;i<ma;i++) SWAP(covar[k][i],covar[j][i]);
                k--;
            }
        }
    }
};

}

namespace Nonlinear {
	int stddev(long double *x,long double *y,int ndata,FitFunctionBase &fn,long double *sig);



	void mrqcof(long double *x,long double *y,long double *sig,int ndata, 
		Array2D<long double> &alpha,
		Array1D<long double> &beta,
		long double &chisq,
		FitFunctionBase &fn);

	void covsrt(Array2D<long double> &covar, FitFunctionBase &fn);

	//######################################

	int LevenbergMarquardt(double *_x,double *_y, int ndata, FitFunctionBase &fn, double eps,double *s)
	{
		kipl::logging::Logger logger("LevenbergMarquardt");
		std::stringstream msg;

	//	int npars=fn.getNpars();
		int mfit=fn.getNpars2fit();
        Array2D<long double> covar(mfit,mfit);
        Array2D<long double> alpha(mfit,mfit);

		long double chisq,chiold,alambda;
		long double *x=new long double[ndata];
		long double *y=new long double[ndata];
		long double *sig=new long double[ndata];
        long double absdiff=0.0;
		
		int i,n;
		
        std::copy(_x,_x+ndata,x);
        std::copy(_y,_y+ndata,y);

        fn.printPars();
        if (s!=nullptr) {
			logger(kipl::logging::Logger::LogVerbose,"Have s");
            std::copy(s,s+ndata,sig);
		}	
		else {
			logger(kipl::logging::Logger::LogVerbose,"Initialize s");
            std::fill(sig,sig+ndata,1.0);

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
            absdiff=fabs(chisq-chiold);
            if ((absdiff<eps) && ((chisq-chiold)<=0))// && (chisq!=chiold))
				n++;
			else
				n=0;

            msg<<"Iteration:"<<i<<", termination count:"<<n<<", chisq:"<<chisq<<", absdiff:"<<absdiff;
            logger(kipl::logging::Logger::LogMessage,msg.str());
            if (s==nullptr)
				stddev(x,y,ndata,fn,sig);

			chiold=chisq;

			mrqmin(x,y,sig,ndata,covar,alpha,chisq,fn,alambda);
			msg.str("");
            msg<<"mrqmin gives alambda:"<<alambda<<" chisq:"<<chisq<<" eps:"<<fabs(chisq-chiold);
            logger(kipl::logging::Logger::LogMessage,msg.str());
            fn.printPars();
			i++;
		}

		// solve the final parameter-set
		alambda=0;
        if (s==nullptr)
			stddev(x,y,ndata,fn,sig);
		mrqmin(x,y,sig,ndata,covar,alpha,chisq,fn,alambda);
		delete [] sig;
		delete [] x;
		delete [] y;
		return 1;
	}


	int stddev(long double *x,long double *y,int ndata,
		FitFunctionBase &fn,
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

        std::fill(sig,sig+ndata,sigma);

		return 1;
	}

	void mrqmin(long double *x,long double *y,long  double *sig,int ndata,
			Array2D<long double> &covar,Array2D<long double> &alpha,
		long double &chisq,
		FitFunctionBase &fn,
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
            ochisq=chisq;
            for (j=0;j<ma;j++)
                atry[j]=fn[j];
        }
		
        for (j=0;j<mfit;j++) {
            for (k=0;k<mfit;k++)
                covar[j][k]=alpha[j][k];

            covar[j][j]=alpha[j][j]*(1.0+(alamda));
            oneda[j]=beta[j];
        }
        logger(kipl::logging::Logger::LogVerbose,"Solving equation system with QR");
        QR<long double> X(covar);

        oneda=X.solve(oneda);

        for (int i=0; i<mfit; ++i)
            da[i]=oneda[i];

        if (alamda == 0.0) {
            covsrt(covar,fn);
            covsrt(alpha,fn);
            return;
        }

        Array1D<long double> tmp(ma);

        for (j=0,l=0;l<ma;l++) {
            tmp[l]=fn[l];
            if (!fn.isLocked(l))
                fn[l]=fn[l]+da[j++];
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
            for (j=0; j<ma; j++)
                fn[j]=tmp[j]; //restore old info
        }
	}

	void mrqcof(long double *x, long double *y,long double *sig, int ndata, 
		Array2D<long double> &alpha, 
		Array1D<long double> &beta,
		long double &chisq, 
		FitFunctionBase &fn)
	{
		int i,j,k,l,m,mfit=0;
		long double ymod,wt,sig2i,dy;
		int ma=fn.getNpars();

		Array1D<long double> dyda(ma);

		mfit=fn.getNpars2fit();
       // qDebug()<<"mfit:"<<mfit<<", ma:"<<ma;

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
			
            for (j=0,l=0;l<ma;l++) {
                if (!fn.isLocked(l)) {
					wt=dyda[l]*sig2i;
                    for (k=0,m=0 ; m<l ; m++) {
                        if (!fn.isLocked(m))
                            alpha[j][k++] += wt*dyda[m];
					}
                    beta[j] += dy*wt;
                    j++;
				}
			}
			chisq += dy*dy*sig2i;
		}
        for (j=1;j<mfit;j++)
			for (k=0;k<j;k++) alpha[k][j]=alpha[j][k];

	}

	void covsrt(Array2D<long double> &covar, FitFunctionBase &fn)
	{
        int mfit=fn.getNpars2fit();

		int ma=covar.dim2();

        for (int i=mfit; i<ma; ++i)
            for (int j=0; j<i; ++j) covar[i][j]=covar[j][i]=0.0;
        int k=mfit-1;
        for (int j=ma-1;j>=0;--j) {
            if (!fn.isLocked(j)) {
                for (int i=0; i<ma; ++i)
                    swap(covar[i][k],covar[i][j]);
                for (int i=0; i<ma; ++i)
                    swap(covar[k][i],covar[j][i]);
				k--;
			}
		}
	}


	//#####################################

	FitFunctionBase::FitFunctionBase(int n) 
	{
		m_pars=new long double[n]; 
		m_lock=new int[n];
        std::fill(m_lock,m_lock+n,0);

		m_pars2fit=n;
		m_Npars=n;
	}	
	
	long double & FitFunctionBase::operator[](int n)
	{
		if (n<m_Npars)
			return m_pars[n];
		else
			return m_pars[0];
	}
	
	int FitFunctionBase::UpdatePars(long double *pars)
	{
		memcpy(m_pars,pars,sizeof(long double)*m_Npars);

		return 1;
	}
	
	int FitFunctionBase::getNpars() 
	{
		return m_Npars;
	}
	
	int FitFunctionBase::setLock(int *lv)
	{
		m_pars2fit=0;
        std::copy(lv,lv+m_Npars,m_lock);

		for (int i=0; i<m_Npars; i++) 
            m_pars2fit+=(m_lock[i]==0);

		return m_pars2fit;
	}

    int FitFunctionBase::isLocked(int n)
	{
		if (n<m_Npars)
			return m_lock[n];
		else 
			return 1;
	}
	
	int FitFunctionBase::getNpars2fit()
	{
		return m_pars2fit;
	}
	
	FitFunctionBase::~FitFunctionBase() 
	{
		delete [] m_pars;
		delete [] m_lock;
	}
	

	SumOfGaussians::SumOfGaussians(int n) : FitFunctionBase(3*n)
	{

	}

	long double SumOfGaussians::operator()(long double x)
	{
		int i;
        long double arg,ex;

		long double y=0.0;
		for (i=0;i<m_Npars;i+=3) {
//            arg=(x-m_pars[i+1])/m_pars[i+2];
//            y+=m_pars[i]*exp(-0.5*arg*arg);
//			arg=(x-m_pars[i+1])/m_pars[i+2]; This is the derivative code!
//			ex=exp(-arg*arg);
//			fac=m_pars[i]*ex*2.0*arg;
//			y += m_pars[i]*ex;

            arg=(x-m_pars[i+1])/m_pars[i+2];
            ex=exp(-arg*arg);
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
            fac=m_pars[i]*ex*2.*arg;
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

    int SumOfGaussians::printPars()
	{
		int i;
		char *est=new char[m_Npars];

		for (i=0; i<m_Npars; i++)
			est[i]=m_lock[i] ? ' ': '*';

        for (i=0; i<m_Npars; i+=3)
        {
            cout<<est[i]  <<"A="<<m_pars[i]  <<" "
                <<est[i+1]<<"m="<<m_pars[i+1]<<" "
                <<est[i+2]<<"s="<<m_pars[i+2]<<endl;
        }

		delete [] est;
        return 1;
    }

    Voight::Voight() : FitFunctionBase(4)
    {

    }

    long double Voight::operator()(long double x)
    {
        long double diff=x-m_pars[1];
        return m_pars[0]*exp(-m_pars[2]*diff-(m_pars[3]*diff*diff*0.5));
    }

    int Voight::operator()(long double x, long double &y, Array1D<long double> &dyda)
    {
        long double diff=x-m_pars[1];
        x=m_pars[0]*exp(-m_pars[2]*fabs(diff)-(m_pars[3]*diff*diff*0.5));

        return 0;
    }

    int Voight::Hessian(long double x, Array2D<long double> &hes)
    {

        return -1;
    }

    int Voight::Jacobian(long double x, Array2D<long double> &jac)
    {

    }

    int Voight::printPars()
    {
        int i;
        char *est=new char[m_Npars];

        for (i=0; i<m_Npars; i++)
            est[i]=m_lock[i] ? ' ': '*';

        cout<<est[0]<<"A="<<m_pars[0]<<
            " "<<est[1]<<"m="<<m_pars[1]<<
            " "<<est[2]<<"gamma="<<m_pars[2]<<
            " "<<est[3]<<"sigma="<<m_pars[3]<<endl;

        delete [] est;
        return 1;
    }

    Lorenzian::Lorenzian() : FitFunctionBase(1)
    {

    }

    long double Lorenzian::operator()(long double x)
    {
       return m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));
    }

    int Lorenzian::operator()(long double x, long double &y, Array1D<long double> &dyda)
    {
        y=m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));

        return 1;
    }

    int Lorenzian::Hessian(long double x, Array2D<long double> &hes)
    {
        return -1;
    }

    int Lorenzian::Jacobian(long double x, Array2D<long double> &jac)
    {
        return -1;
    }

    int Lorenzian::printPars()
    {
        char est=m_lock[0] ? ' ' : '*';

        cout<<est<<"gamma="<<m_pars[0];
    }

}
