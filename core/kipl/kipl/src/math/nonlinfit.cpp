//<LICENCE>

#include <QDebug>
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>

#include <tnt.h>
#include <jama_qr.h>

#include <armadillo>

#include "../../include/math/nonlinfit.h"
#include "../../include/math/jama_inverses.h"
#include "../../include/math/mathconstants.h"
#include "../../include/logging/logger.h"
#include "../../include/base/KiplException.h"
#include "../../include/strings/miscstring.h"

using namespace TNT;
using namespace JAMA;

namespace Nonlinear {

LevenbergMarquardt::LevenbergMarquardt(const double TOL, int iterations) :
    maxIterations(iterations),
    tol(TOL)
{
    // Constructor.
}

void LevenbergMarquardt::fit(arma::vec &x, arma::vec &y,
                             arma::vec &sig,
                             Nonlinear::FitFunctionBase &fn)
{
    Array1D<double> xx(x.size());
    std::copy(x.begin(),x.end(),&xx[0]);

    Array1D<double> yy(y.size());
    std::copy(y.begin(),y.end(),&yy[0]);

    Array1D<double> sigma(sig.size());
    std::copy(sig.begin(),sig.end(),&sigma[0]);

    fit(xx,yy,sigma,fn);
}

void LevenbergMarquardt::fit(Array1D<double> &x, Array1D<double> &y,
                             Array1D<double> &sig,
                             Nonlinear::FitFunctionBase &fn)
{
    //    Iterate to reduce the 2 of a fit between a set of data points x[0..ndat-1], y[0..ndat-1]
    //    with individual standard deviations sig[0..ndat-1], and a nonlinear function that depends
    //    on ma coefficients a[0..ma-1]. When 2 is no longer decreasing, set best-fit values
    //    for the parameters a[0..ma-1], and chisq D 2, covar[0..ma-1][0..ma-1], and
    //    alpha[0..ma-1][0..ma-1]. (Parameters held fixed will return zero covariances.)
    ma=fn.getNpars();
    mfit=fn.getNpars2fit();
    if ((x.dim1()!=y.dim1()) || (x.dim1()!=sig.dim1()))
        throw kipl::base::KiplException("Array size missmatch for the fitter",__FILE__,__LINE__);

    ndat=x.dim1();
 //   qDebug()<< "ndat:"<<ndat;

    covar=Array2D<double>(ma,ma);
    alpha=Array2D<double>(ma,ma);

    int j,k,l,iter,done=0;
    double alamda=.001,ochisq;

    Array1D<double> beta(ma),da(ma);
    Array1D<double> a(ma),atry(ma);
    Array2D<double> oneda(mfit,1), temp(mfit,mfit);

    fn.getPars(a);

    mrqcof(fn,x,y,sig,alpha,beta); // Initialization.

//    for (int i=0; i<temp.dim1(); ++i) {
//        for (int j=0; j<temp.dim2(); ++j) {
//            qDebug() << "i="<<i<<", j="<<j<<"="<<alpha[i][j];
//        }
//    }

    fn.getPars(atry);

    ochisq=chisq;

    for (iter=0;iter<maxIterations;iter++) {
        if ((iter % 100)==0)
            qDebug() <<"iteration:"<<iter<<"done:"<<done<<", alambda:"<<alamda<<", chisq:"<<chisq;

        if (done==NDONE) {
            alamda=0.; //Last pass. Use zero alamda.
        }

        for (j=0;j<mfit;j++) { //Alter linearized fitting matrix, by augmenting diagonal elements.
            for (k=0;k<mfit;k++)
                covar[j][k]=alpha[j][k];

            covar[j][j]=alpha[j][j]*(1.0+alamda);

            for (k=0;k<mfit;k++)
                temp[j][k]=covar[j][k];

            oneda[j][0]=beta[j];
        }

        gaussj(temp,oneda); //Matrix solution.

        for (j=0;j<mfit;j++) {
            for (k=0;k<mfit;k++)
                covar[j][k]=temp[j][k];
            da[j]=oneda[j][0];
        }

        if (done==NDONE) { //Converged. Clean up and return.
            covsrt(covar,fn);
            covsrt(alpha,fn);

            return;
        }

        for (j=0,l=0;l<ma;l++) //Did the trial succeed?
            if (fn.isFree(l)) atry[l]=fn[l]+da[j++];

        fn.setPars(atry);
        mrqcof(fn,x,y,sig,covar,da);

        if (abs(chisq-ochisq) < std::max(tol,tol*chisq)) done++;

        if (chisq < ochisq) { //Success, accept the new solution.
            alamda *= 0.1;
            ochisq=chisq;
            for (j=0;j<mfit;j++) {
                for (k=0;k<mfit;k++) alpha[j][k]=covar[j][k];
                beta[j]=da[j];
            }

            fn.setPars(atry);
        }
        else { //Failure, increase alamda.
            alamda *= 10.0;
            chisq=ochisq;
        }
    }
    throw kipl::base::KiplException("Fitmrq too many iterations",__FILE__,__LINE__);
}

void LevenbergMarquardt::mrqcof(Nonlinear::FitFunctionBase &fn, Array1D<double> &x, Array1D<double> &y,
                                Array1D<double> &sig, Array2D<double> &alpha, Array1D<double> &beta)
{
//Used by fit to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and to calculate 2.
    int i,j,k,l,m;
    double ymod,wt,sig2i,dy;
    Array1D<double> dyda(ma);
    for (j=0; j<mfit; j++) { //Initialize (symmetric) alpha, beta.
        for (k=0; k<=j; k++) alpha[j][k]=0.0;
            beta[j]=0.;
    }

    chisq=0.;

    for (i=0; i<ndat; i++) { //Summation loop over all data.
        fn(x[i],ymod,dyda);
        sig2i=1.0/(sig[i]*sig[i]);
        dy=y[i]-ymod;
        for (j=0,l=0;l<ma;l++) {
            if (fn.isFree(l)) {
                wt=dyda[l]*sig2i;
                for (k=0,m=0;m<l+1;m++)
                    if (fn.isFree(m)) alpha[j][k++] += wt*dyda[m];
                beta[j++] += dy*wt;
            }
        }
        chisq += dy*dy*sig2i; // And find 2.
    }
    for (j=1;j<mfit;j++) //Fill in the symmetric side.
        for (k=0;k<j;k++) alpha[k][j]=alpha[j][k];
}

void LevenbergMarquardt::covsrt(Array2D<double> &covar, Nonlinear::FitFunctionBase &fn)
{
//Expand in storage the covariance matrix covar, so as to take into account parameters that
//are being held fixed. (For the latter, return zero covariances.)
    int i,j,k;
    for (i=mfit;i<ma;i++)
        for (j=0;j<i+1;j++) covar[i][j]=covar[j][i]=0.0;
    k=mfit-1;
    for (j=ma-1;j>=0;j--) {
        if (fn.isFree(j)) {
            for (i=0;i<ma;i++) std::swap(covar[i][k],covar[i][j]);
            for (i=0;i<ma;i++) std::swap(covar[k][i],covar[j][i]);
            k--;
        }
    }
}

void LevenbergMarquardt::gaussj(Array2D<double> &a, Array2D<double> &b)
//Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. The input matrix
//is a[0..n-1][0..n-1]. b[0..n-1][0..m-1] is input containing the m right-hand side vectors.
//On output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of
//solution vectors.
{
    int i,icol,irow,j,k,l,ll,n=a.dim1(),m=b.dim2();
    double big,dum,pivinv;
    Array1D<int> indxc(n),indxr(n),ipiv(n); //These integer arrays are used for bookkeeping on
    for (j=0;j<n;j++)
        ipiv[j]=0; // the pivoting.

    for (i=0;i<n;i++) { //This is the main loop over the columns to be reduced
        big=0.0;
        for (j=0;j<n;j++) //This is the outer loop of the search for a pivot element
            if (ipiv[j] != 1)
                for (k=0;k<n;k++) {
                    if (ipiv[k] == 0) {
                        if (abs(a[j][k]) >= big) {
                            big=abs(a[j][k]);
                            irow=j;
                            icol=k;
                        }
                    }
                }
        ++(ipiv[icol]);

        if (irow != icol) {
            for (l=0;l<n;l++) std::swap(a[irow][l],a[icol][l]);
            for (l=0;l<m;l++) std::swap(b[irow][l],b[icol][l]);
        }
        indxr[i]=irow;
        indxc[i]=icol;

        if (a[icol][icol] == 0.0)
            throw kipl::base::KiplException("gaussj: Singular Matrix");

        pivinv=1.0/a[icol][icol];
        a[icol][icol]=1.0;

        for (l=0;l<n;l++)
            a[icol][l] *= pivinv;

        for (l=0;l<m;l++)
            b[icol][l] *= pivinv;

        for (ll=0;ll<n;ll++) // Next, we reduce the rows...
            if (ll != icol) { //...except for the pivot one, of course.
                dum=a[ll][icol];
                a[ll][icol]=0.0;
                for (l=0;l<n;l++)
                    a[ll][l] -= a[icol][l]*dum;
                for (l=0;l<m;l++)
                    b[ll][l] -= b[icol][l]*dum;
        }
    }
//        This is the end of the main loop over columns of the reduction. It only remains to unscramble
//        the solution in view of the column interchanges. We do this by interchanging pairs of
//        columns in the reverse order that the permutation was built up.
    for (l=n-1;l>=0;l--) {
        if (indxr[l] != indxc[l])
            for (k=0;k<n;k++)
                std::swap(a[k][indxr[l]],a[k][indxc[l]]);
    }
}

	//#####################################

	FitFunctionBase::FitFunctionBase(int n) 
	{
        m_pars=Array1D<double>(n);
        m_lock=new bool[n];
        std::fill(m_lock,m_lock+n,true);

		m_pars2fit=n;
		m_Npars=n;
	}	
	
    double & FitFunctionBase::operator[](int n)
	{
		if (n<m_Npars)
			return m_pars[n];
		else
			return m_pars[0];
	}
	
    int FitFunctionBase::setPars(Array1D<double> &pars)
	{
        m_pars=pars;

        return m_pars.dim1();
    }

    int FitFunctionBase::getPars(Array1D<double> &pars)
    {
        pars=m_pars;

        return 0;
    }
	
	int FitFunctionBase::getNpars() 
	{
		return m_Npars;
	}
	
    int FitFunctionBase::setLock(bool *lv)
	{
		m_pars2fit=0;
        std::copy(lv,lv+m_Npars,m_lock);

		for (int i=0; i<m_Npars; i++) 
            m_pars2fit+=(m_lock[i]==0);

        return m_pars2fit;
    }

    void FitFunctionBase::hold(const int i, const double val)
    {
        if (m_lock[i]==true) m_pars2fit--;

        m_lock[i]=false;
        m_pars[i]=val;

    }

    void FitFunctionBase::free(const int i)
    {
        if (m_lock[i]==false) m_pars2fit++;

        m_lock[i]=true;
    }

    bool FitFunctionBase::isFree(int n)
	{
		if (n<m_Npars)
			return m_lock[n];
		else 
            return true;
	}
	
	int FitFunctionBase::getNpars2fit()
	{
		return m_pars2fit;
	}
	
	FitFunctionBase::~FitFunctionBase() 
	{
		delete [] m_lock;
	}
	

	SumOfGaussians::SumOfGaussians(int n) : FitFunctionBase(3*n)
	{

	}

    double SumOfGaussians::operator()(double x)
	{
		int i;
        double arg,ex;

        double y=0.0;
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

    int SumOfGaussians::operator()(double x, double &y, Array1D<double> & dyda)
	{
		int i;
		long double fac,ex,arg;

		if (dyda.dim()!=m_Npars)
            dyda=Array1D<double>(m_Npars);

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

    int SumOfGaussians::Hessian(double UNUSED(x), Array2D<double> & UNUSED(hes))
	{

		cerr<<"The Hessian is not available"<<endl;
		return 1;

		/*
		if ((hes.num_rows()!=m_Npars) || (hes.num_cols()!=m_Npars))
		hes.newsize(m_Npars,m_Npars);

		return 1;
		*/
	}

    int SumOfGaussians::Jacobian(double UNUSED(x), Array2D<double> & UNUSED(jac))
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

    double Voight::operator()(double x)
    {
        long double diff=x-m_pars[1];
        return m_pars[0]*exp(-m_pars[2]*diff-(m_pars[3]*diff*diff*0.5));
    }

    int Voight::operator()(double x, double &y, Array1D<double> &dyda)
    {
        long double diff=static_cast<long double>(x-m_pars[1]);
        x=m_pars[0]*exp(-m_pars[2]*fabs(diff)-(m_pars[3]*diff*diff*0.5));

        return 0;
    }

    int Voight::Hessian(double x, Array2D<double> &hes)
    {

        return -1;
    }

    int Voight::Jacobian(double x, Array2D<double> &jac)
    {
        return 0;
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

    double Lorenzian::operator()(double x)
    {
       return m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));
    }

    int Lorenzian::operator()(double x, double &y, Array1D<double> &dyda)
    {
        y=m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));

        return 1;
    }

    int Lorenzian::Hessian(double x, Array2D<double> &hes)
    {
        return -1;
    }

    int Lorenzian::Jacobian(double x, Array2D<double> &jac)
    {
        return -1;
    }

    int Lorenzian::printPars()
    {
        char est=m_lock[0] ? ' ' : '*';

        cout<<est<<"gamma="<<m_pars[0];
        return 0;
    }

    //############################


    namespace armafit {


    LevenbergMarquardt::LevenbergMarquardt(const double TOL, int iterations) :
        maxIterations(iterations),
        tol(TOL)
    {
        // Constructor.
    }

    void LevenbergMarquardt::fit(arma::vec &x, arma::vec &y,
                                  arma::vec &sig,
                                  Nonlinear::armafit::FitFunctionBase &fn)
    {
        //    Iterate to reduce the 2 of a fit between a set of data points x[0..ndat-1], y[0..ndat-1]
        //    with individual standard deviations sig[0..ndat-1], and a nonlinear function that depends
        //    on ma coefficients a[0..ma-1]. When 2 is no longer decreasing, set best-fit values
        //    for the parameters a[0..ma-1], and chisq D 2, covar[0..ma-1][0..ma-1], and
        //    alpha[0..ma-1][0..ma-1]. (Parameters held fixed will return zero covariances.)
        ma=fn.getNpars();
        mfit=fn.getNpars2fit();
        if ((x.n_elem!=y.n_elem) || (x.n_elem!=sig.n_elem))
            throw kipl::base::KiplException("Array size missmatch for the fitter",__FILE__,__LINE__);

        ndat=x.n_elem;
     //   qDebug()<< "ndat:"<<ndat;

        covar=arma::mat(ma,ma);
        alpha=arma::mat(ma,ma);

        int j,k,l,iter,done=0;
        double alamda=.001,ochisq;

        arma::vec beta(ma),da(ma);
        arma::vec a(ma),atry(ma);
        arma::mat oneda(mfit,1), temp(mfit,mfit);

        fn.getPars(a);

        mrqcof(fn,x,y,sig,alpha,beta); // Initialization.

    //    for (int i=0; i<temp.dim1(); ++i) {
    //        for (int j=0; j<temp.dim2(); ++j) {
    //            qDebug() << "i="<<i<<", j="<<j<<"="<<alpha[i][j];
    //        }
    //    }

        fn.getPars(atry);

        ochisq=chisq;

        for (iter=0;iter<maxIterations;iter++)
        {
            if ((iter % 100)==0)
                qDebug() <<"iteration:"<<iter<<"done:"<<done<<", alambda:"<<alamda<<", chisq:"<<chisq;

            if (done==NDONE)
            {
                alamda=0.; //Last pass. Use zero alamda.
            }

            for (j=0;j<mfit;j++)
            { //Alter linearized fitting matrix, by augmenting diagonal elements.
                for (k=0;k<mfit;k++)
                    covar.at(j,k) = alpha.at(j,k);

                covar.at(j,j) = alpha.at(j,j)*(1.0+alamda);

                for (k=0;k<mfit;k++)
                    temp.at(j,k)=covar.at(j,k);

                oneda.at(j,0)=beta.at(j);
            }

            gaussj(temp,oneda); //Matrix solution.

            for (j=0;j<mfit;j++) {
                for (k=0;k<mfit;k++)
                    covar.at(j,k)=temp.at(j,k);
                da.at(j)=oneda.at(j,0);
            }

            if (done==NDONE) { //Converged. Clean up and return.
                covsrt(covar,fn);
                covsrt(alpha,fn);

                return;
            }

            for (j=0,l=0;l<ma;l++) //Did the trial succeed?
                if (fn.isFree(l)) atry[l]=fn[l]+da[j++];

            fn.setPars(atry);
            mrqcof(fn,x,y,sig,covar,da);

            if (abs(chisq-ochisq) < std::max(tol,tol*chisq)) done++;

            if (chisq < ochisq) { //Success, accept the new solution.
                alamda *= 0.1;
                ochisq=chisq;
                for (j=0;j<mfit;j++) {
                    for (k=0;k<mfit;k++) alpha.at(j,k)=covar.at(j,k);
                    beta[j]=da[j];
                }

                fn.setPars(atry);
            }
            else { //Failure, increase alamda.
                alamda *= 10.0;
                chisq=ochisq;
            }
        }
        throw kipl::base::KiplException("Fitmrq too many iterations",__FILE__,__LINE__);
    }

    void LevenbergMarquardt::mrqcof(Nonlinear::armafit::FitFunctionBase &fn, arma::vec &x, arma::vec &y,
                                    arma::vec &sig, arma::mat &alpha, arma::vec &beta)
    {
    //Used by fit to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and to calculate 2.
        int i,j,k,l,m;
        double ymod,wt,sig2i,dy;
        arma::vec dyda(ma);
        for (j=0; j<mfit; j++)
        { //Initialize (symmetric) alpha, beta.
            for (k=0; k<=j; k++) alpha.at(j,k)=0.0;
                beta[j]=0.;
        }

        chisq=0.;

        for (i=0; i<ndat; i++)
        { //Summation loop over all data.
            fn(x[i],ymod,dyda);
            sig2i=1.0/(sig[i]*sig[i]);
            dy=y[i]-ymod;
            for (j=0,l=0;l<ma;l++)
            {
                if (fn.isFree(l))
                {
                    wt=dyda[l]*sig2i;
                    for (k=0,m=0;m<l+1;m++)
                        if (fn.isFree(m)) alpha.at(j,k++) += wt*dyda[m];
                    beta[j++] += dy*wt;
                }
            }
            chisq += dy*dy*sig2i; // And find 2.
        }
        for (j=1;j<mfit;j++) //Fill in the symmetric side.
            for (k=0;k<j;k++) alpha.at(k,j)=alpha.at(j,k);
    }

    void LevenbergMarquardt::covsrt(arma::mat &covar, Nonlinear::armafit::FitFunctionBase &fn)
    {
    //Expand in storage the covariance matrix covar, so as to take into account parameters that
    //are being held fixed. (For the latter, return zero covariances.)
        int i,j,k;
        for (i=mfit;i<ma;i++)
            for (j=0;j<i+1;j++)
                covar.at(i,j)=covar.at(j,i)=0.0;
        k=mfit-1;
        for (j=ma-1;j>=0;j--) {
            if (fn.isFree(j)) {
                for (i=0;i<ma;i++) std::swap(covar.at(i,k),covar.at(i,j));
                for (i=0;i<ma;i++) std::swap(covar.at(k,i),covar.at(j,i));
                k--;
            }
        }
    }

    void LevenbergMarquardt::gaussj(arma::mat &a, arma::mat &b)
    //Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. The input matrix
    //is a[0..n-1][0..n-1]. b[0..n-1][0..m-1] is input containing the m right-hand side vectors.
    //On output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of
    //solution vectors.
    {
        int i,icol,irow,j,k,l,ll,n=a.n_rows,m=b.n_cols;
        double big,dum,pivinv;
        arma::vec indxc(n),indxr(n),ipiv(n); //These integer arrays are used for bookkeeping on
        for (j=0;j<n;j++)
            ipiv[j]=0; // the pivoting.

        for (i=0;i<n;i++)
        { //This is the main loop over the columns to be reduced
            big=0.0;
            for (j=0;j<n;j++) //This is the outer loop of the search for a pivot element
            {
                if (ipiv[j] != 1)
                {
                    for (k=0;k<n;k++)
                    {
                        if (ipiv[k] == 0)
                        {
                            if (abs(a.at(j,k)) >= big)
                            {
                                big=abs(a.at(j,k));
                                irow=j;
                                icol=k;
                            }
                        }
                    }
                }
            }
            ++(ipiv[icol]);

            if (irow != icol) {
                for (l=0;l<n;l++) std::swap(a.at(irow,l),a.at(icol,l));
                for (l=0;l<m;l++) std::swap(b.at(irow,l),b.at(icol,l));
            }
            indxr[i]=irow;
            indxc[i]=icol;

            if (a.at(icol,icol) == 0.0)
                throw kipl::base::KiplException("gaussj: Singular Matrix");

            pivinv=1.0/a(icol,icol);
            a.at(icol,icol) = 1.0;

            for (l=0;l<n;l++)
                a.at(icol,l) *= pivinv;

            for (l=0;l<m;l++)
                b.at(icol,l) *= pivinv;

            for (ll=0;ll<n;ll++) // Next, we reduce the rows...
                if (ll != icol) { //...except for the pivot one, of course.
                    dum=a.at(ll,icol);
                    a.at(ll,icol)=0.0;
                    for (l=0;l<n;l++)
                        a.at(ll,l) -= a.at(icol,l)*dum;
                    for (l=0;l<m;l++)
                        b.at(ll,l) -= b.at(icol,l)*dum;
            }
        }
    //        This is the end of the main loop over columns of the reduction. It only remains to unscramble
    //        the solution in view of the column interchanges. We do this by interchanging pairs of
    //        columns in the reverse order that the permutation was built up.
        for (l=n-1;l>=0;l--) {
            if (indxr[l] != indxc[l])
                for (k=0;k<n;k++)
                    std::swap(a.at(k,indxr[l]),a.at(k,indxc[l]));
        }
    }

        //#####################################

        FitFunctionBase::FitFunctionBase(int n)
        {
            m_pars=arma::vec(n);
            m_lock=new bool[n];
            std::fill(m_lock,m_lock+n,true);

            m_pars2fit=n;
            m_Npars=n;
        }

        double & FitFunctionBase::operator[](int n)
        {
            if (n<m_Npars)
                return m_pars[n];
            else
                return m_pars[0];
        }

        int FitFunctionBase::setPars(arma::vec &pars)
        {
            m_pars=pars;

            return m_pars.n_elem;
        }

        int FitFunctionBase::getPars(arma::vec &pars)
        {
            pars=m_pars;

            return 0;
        }

        int FitFunctionBase::getNpars()
        {
            return m_Npars;
        }

        int FitFunctionBase::setLock(bool *lv)
        {
            m_pars2fit=0;
            std::copy(lv,lv+m_Npars,m_lock);

            for (int i=0; i<m_Npars; i++)
                m_pars2fit+=(m_lock[i]==0);

            return m_pars2fit;
        }

        void FitFunctionBase::hold(const int i, const double val)
        {
            if (m_lock[i]==true) m_pars2fit--;

            m_lock[i]=false;
            m_pars[i]=val;

        }

        void FitFunctionBase::free(const int i)
        {
            if (m_lock[i]==false) m_pars2fit++;

            m_lock[i]=true;
        }

        bool FitFunctionBase::isFree(int n)
        {
            if (n<m_Npars)
                return m_lock[n];
            else
                return true;
        }

        int FitFunctionBase::getNpars2fit()
        {
            return m_pars2fit;
        }

        FitFunctionBase::~FitFunctionBase()
        {
            delete [] m_lock;
        }


        SumOfGaussians::SumOfGaussians(int n) : FitFunctionBase(3*n)
        {

        }

        double SumOfGaussians::operator()(double x)
        {
            int i;
            double arg,ex;

            double y=0.0;
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

        int SumOfGaussians::operator()(double x, double &y, arma::vec & dyda)
        {
            int i;
            long double fac,ex,arg;

            if (dyda.n_elem!=m_Npars)
                dyda=arma::vec(m_Npars);

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

        int SumOfGaussians::Hessian(double UNUSED(x), arma::mat &UNUSED)
        {

            cerr<<"The Hessian is not available"<<endl;
            return 1;

            /*
            if ((hes.num_rows()!=m_Npars) || (hes.num_cols()!=m_Npars))
            hes.newsize(m_Npars,m_Npars);

            return 1;
            */
        }

        int SumOfGaussians::Jacobian(double UNUSED(x), arma::mat & UNUSED(jac))
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

        double Voight::operator()(double x)
        {
            long double diff=x-m_pars[1];
            return m_pars[0]*exp(-m_pars[2]*diff-(m_pars[3]*diff*diff*0.5));
        }

        int Voight::operator()(double x, double &y, arma::vec &dyda)
        {
            long double diff=static_cast<long double>(x-m_pars[1]);
            x=m_pars[0]*exp(-m_pars[2]*fabs(diff)-(m_pars[3]*diff*diff*0.5));

            return 0;
        }

        int Voight::Hessian(double x, arma::mat &hes)
        {

            return -1;
        }

        int Voight::Jacobian(double x, arma::mat &jac)
        {
            return 0;
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

        double Lorenzian::operator()(double x)
        {
           return m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));
        }

        int Lorenzian::operator()(double x, double &y, Array1D<double> &dyda)
        {
            y=m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));

            return 1;
        }

        int Lorenzian::Hessian(double x, arma::mat &hes)
        {
            return -1;
        }

        int Lorenzian::Jacobian(double x, arma::mat &jac)
        {
            return -1;
        }

        int Lorenzian::printPars()
        {
            char est=m_lock[0] ? ' ' : '*';

            cout<<est<<"gamma="<<m_pars[0];
            return 0;
        }

    }
}

void string2enum(string &str, Nonlinear::eProfileFunction &e)
{
    std::string lowstr=kipl::strings::toLower(str);

    std::map<std::string,Nonlinear::eProfileFunction> convmap;

    convmap["gaussprofile"]=Nonlinear::eProfileFunction::fnSumOfGaussians;
    convmap["lorenzprofile"]=Nonlinear::eProfileFunction::fnLorenzian;
    convmap["voightprofile"]=Nonlinear::eProfileFunction::fnVoight;

    auto it=convmap.find(lowstr);

    if (it==convmap.end())
        throw kipl::base::KiplException("Profile function does not exist",__FILE__,__LINE__);

    e=it->second;


}

std::string enum2string(Nonlinear::eProfileFunction e)
{
    switch(e) {
    case Nonlinear::fnSumOfGaussians:
        return "GaussProfile";
        break;
    case Nonlinear::fnLorenzian:
        return "LorenzProfile";
        break;
    case Nonlinear::fnVoight:
        return "VoightProfile";
        break;
    }

    return "GaussProfile";
}

ostream &operator<<(ostream &s, Nonlinear::eProfileFunction e)
{
    std::string str;
    str=enum2string(e);

    s<<str;

    return s;
}
