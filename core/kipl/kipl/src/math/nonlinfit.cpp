//<LICENCE>

#include <QDebug>
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>

#include <math.h>
#include <cmath>
#include <tnt.h>
#include <jama_qr.h>
#include <jama_cholesky.h>
#include <jama_svd.h>
#include <tnt_cmat.h>
//#include <Faddeeva.hh>
#include <lmcurve.h>

#include "../../include/math/nonlinfit.h"
#include "../../include/math/gradient.h"
#include "../../include/math/jama_inverses.h"
#include "../../include/math/mathconstants.h"
#include "../../include/logging/logger.h"
#include "../../include/base/KiplException.h"
#include "../../include/strings/miscstring.h"

using namespace TNT;
using namespace JAMA;
using namespace std;


//using namespace Faddeeva;

namespace Nonlinear {

LevenbergMarquardt::LevenbergMarquardt(const double TOL, int iterations) :
    maxIterations(iterations),
    tol(TOL)
{
    // Constructor.
}

void LevenbergMarquardt::fit(Array1D<double> &x, Array1D<double> &y,
                             Array1D<double> &sig,
                             Nonlinear::FitFunctionBase &fn)
{
    //    Iterate to reduce the Chi2 of a fit between a set of data points x[0..ndat-1], y[0..ndat-1]
    //    with individual standard deviations sig[0..ndat-1], and a nonlinear function that depends
    //    on ma coefficients a[0..ma-1]. When Chi2 is no longer decreasing, set best-fit values
    //    for the parameters a[0..ma-1], and chisq = Chi2, covar[0..ma-1][0..ma-1], and
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
//        if ((iter % 100)==0)
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

//        std::cout << "matrix to invert" << std::endl;

//        for (j=0;j<mfit;j++) {
//            for (k=0;k<mfit;k++)
//            {
//                std::cout << temp[j][k] << " ";
//            }
//            std::cout << std::endl;
//        }

//        std::cout << std::endl;

//        std::cout << " b vector" << std::endl;

//        for (j=0;j<mfit;j++) {
//            for (k=0;k<1;k++)
//            {
//                std::cout <<  oneda[j][k] << " ";
//            }
//             std::cout << std::endl;
//        }

//        std::cout << std::endl;


        try {
            gaussj(temp,oneda); //Matrix solution.
//            svd(temp,oneda);
        }
        catch (kipl::base::KiplException & e) {
            qDebug() << "kiplException: " << QString::fromStdString(e.what());

        }
        catch (std::exception &e){
            qDebug() << "std::exception " << QString::fromStdString(e.what());

        }

//        for (j=0;j<mfit;j++) {
//            for (k=0;k<mfit;k++)
//            {
//                std::cout << temp[j][k] << " ";
//            }
//            std::cout << std::endl;
//        }

//        std::cout << std::endl;

//        for (j=0;j<mfit;j++) {
//            for (k=0;k<1;k++)
//            {
//                std::cout <<  oneda[j][k] << " ";
//            }
//             std::cout << std::endl;
//        }

//        std::cout << std::endl;




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
            if (fn.isFree(l)) {
                atry[l]=fn[l]+da[j++];
//                std::cout << atry[l] << " ";
            }

//        std::cout << std::endl;

        fn.setPars(atry);
        mrqcof(fn,x,y,sig,covar,da);

        if (abs(chisq-ochisq) < std::max(tol,tol*chisq)) done++;

        qDebug() << ochisq;
        qDebug() << chisq;

        if (chisq <= ochisq) { //Success, accept the new solution.
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
//    throw kipl::base::KiplException("Fitmrq too many iterations",__FILE__,__LINE__);
}

void LevenbergMarquardt::mrqcof(Nonlinear::FitFunctionBase &fn, Array1D<double> &x, Array1D<double> &y,
                                Array1D<double> &sig, Array2D<double> &alpha, Array1D<double> &beta)
{
//Used by fit to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and to calculate Chi2.
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

void LevenbergMarquardt::svd(Array2D<double> &a, Array2D<double> &b)
{
    JAMA::SVD<double> svd_solver(a);

    Array2D<double> U(a.dim1(),a.dim2());
    Array2D<double> UT(a.dim1(),a.dim2());
    Array2D<double> V(a.dim1(),a.dim2());
    Array2D<double> VT(a.dim1(),a.dim2());
    Array2D<double> s(a.dim1(),a.dim2());
    Array2D<double> sinv(a.dim1(),a.dim2(), 0.0); // inversed eigenvalues
    Array2D<double> s_reg(a.dim1(),a.dim2(), 0.0); // regularized eigenvalues
    Array2D<double> s_reg_inv(a.dim1(),a.dim2(), 0.0); // inversed regularized eigenvalue
    Array1D<double> reg_par(7); // regularization parameter
    Array1D<double> phi(reg_par.dim()); // regularization weight on the eigenvalue
    Array1D<double> sol_norm(reg_par.dim()); // norm of the regularized solution (=norm2(x_phi))
    Array1D<double> res_norm(reg_par.dim()); // norm of the residual (=norm2(A*x_phi-b))
    Array2D<double> F1; // auxiliary matrix for computing Ainv using SVD
    Array2D<double> F2; // auxiliary matrix for computing Ainv using SVD
    Array2D<double> F3(b.dim2(), b.dim1()); // auxiliary matrix for computing Ainv using SVD
    Array2D<double> F4(b.dim2(), b.dim1()); // auxiliary matrix for computing the residual norm for regularized SVD

    int i,j;


    svd_solver.getU(U); //U
    svd_solver.getV(V); //V
    svd_solver.getS(s); //s


//    // initialize matrices to zero
//    for(i = 0; i < s.dim1(); ++i){
//        for(j = 0; j < s.dim2(); ++j)
//        {
//            sinv[i][j] = 0.0; // first fill with zeros
//            s_reg[i][j] = 0.0;
//            s_reg_inv[i][j] = 0.0;
//        }
//    }



    // A = U*s*V'
    // A' = U'*s'*V
    // U and V are orthogonal, so their inverse is the transpose
    // s' = 1/S, the reciprocal of the eigenvalues
    // Tykhonov regularize s if the matrix is bad conditioned, i.e. if cond(A) = S[0]/S[last] is big, obtained by computing the L curve




// compute the inverse (=transpose) of U
    for(i = 0; i < U.dim1(); ++i)
        for(j = 0; j < U.dim2(); ++j)
        {
            UT[j][i]=U[i][j];
        }


//    std::cout << "singular values" << std::endl;
//    for(i = 0; i < s.dim1(); ++i){
//            std::cout <<  s[i][i] << ' ';
//    }

//    std::cout << std::endl;

// compute the inverse (=reciprocal) of s





//    std::cout << s.dim1() << std::endl;
    for(int i = 0; i < s.dim1(); ++i)
    {
        sinv[i][i] = 1.0/s[i][i]; // then compute the reciprocal
    }

 // compute the inverse (=transpose) of V' (not used)
    for(i = 0; i < V.dim1(); ++i)
        for(j = 0; j < V.dim2(); ++j)
        {
            VT[j][i]=V[i][j];
        }


    // Here I add the regularization (first I should in principle check the conditioning of the matrix)

    // fill the regularization parameter
    reg_par[0] = 0.001;
    reg_par[1] = 0.01;
    reg_par[2] = 0.1;
    reg_par[3] = 1.0;
    reg_par[4] = 10.0;
    reg_par[5] = 100.0;
    reg_par[6] = 1000.0;



    for (i=0; i<reg_par.dim(); ++i)
    {
        for (j=0; j<s.dim1(); ++j)
        {
            phi[j] = (s[j][j]*s[j][j])/(s[j][j]*s[j][j]+reg_par[i]*reg_par[i]);
            s_reg[j][j]= s[j][j]/phi[j];
            s_reg_inv[j][j] = 1.0/s_reg[j][j];
        }

        F1 = matmult(s_reg_inv, UT);  // this would be just the results without regularization
        F2 = matmult(V,F1); // this is the regularized inverse of A
        F3 = matmult(F2,b); // this is the solution computed with the regularized inverse of A
        F4 = matmult(a,F3);

        sol_norm[i] = 0.0;
        res_norm[i] = 0.0;

        for (j=0; j<s.dim1(); ++j)
        {

            sol_norm[i] += F3[0][j]*F3[0][j];
            res_norm[i] += (F4[0][j]-b[j][0])*(F4[0][j]-b[j][0]);

        }
        sol_norm[i] = log(sqrt(sol_norm[i]));
        res_norm[i] = (sqrt(res_norm[i]));
    }

    // from this compute the second order derivative of sol_norm with respect of res_norm

    Array1D<double> first_der(sol_norm.dim());
    Array1D<double> second_der(sol_norm.dim());

    kipl::math::num_gradient(sol_norm, res_norm, sol_norm.dim(), first_der);
    kipl::math::num_gradient(first_der, res_norm, sol_norm.dim(), second_der);

    // then I look for the max of the second derivative


    double *A = second_der;
    double max_el = *std::max_element(A, A+second_der.dim()); // this returns the value

    int index_max = std::distance(A, std::max_element(A, A+second_der.dim()));


    // select the max element for the final regularization

    for (j=0; j<s.dim1(); ++j)
    {
        phi[j] = (s[j][j]*s[j][j])/(s[j][j]*s[j][j]+reg_par[index_max]*reg_par[index_max]);
        s_reg[j][j]= s[j][j]/phi[j];
        s_reg_inv[j][j] = 1.0/s_reg[j][j];
    }

    F1 = matmult(sinv, UT);  // this would be just the results without regularization
//    F1 = matmult(s_reg_inv, UT); // this is with regularization
    F2 = matmult(V,F1);

    for(int i = 0; i < U.dim1(); ++i)
        for(int j = 0; j < U.dim2(); ++j)
        {
            a[i][j] = F2[i][j];
        }


//    std::cout << b.dim2() << ' ' << b.dim1() << std::endl;
    F3 = matmult(F2,b);



    for(int i = 0; i < U.dim1(); ++i)
        {
            b[i][0] = F3[0][i];
        }

}

	//#####################################

	FitFunctionBase::FitFunctionBase(int n) 
	{
        m_pars=Array1D<double>(n);
        m_lock=new bool[n];
        std::fill(m_lock,m_lock+n,true); // set all to free

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

    EdgeFunction::EdgeFunction(): FitFunctionBase (7)
    {

    }

    /// The lineshape is defined by 7 parameters, here:
    /// m_par[0] = t0 is the edge position
    /// m_par[1] = sigma is the Gaussian broadening
    /// m_par[2] = tau is the exponential decay of the trailing edge
    /// m_par[3] = a_{0} first linear parameter for the function after the edge
    /// m_par[4] = b_{0} second linear parameter for the function after the edge
    /// m_par[5] = a_{hkl} first linear parameter for the function before the edge
    /// m_par[6] = b_{hkl} second linear parameter for the function after the edge
    double EdgeFunction::operator()(double x)
    {
        double term3,term4,term5,edge,exp_after,exp_before;
        term3 = erfc(-1.0*(x-m_pars[0])/(m_pars[1]*dsqrt2));
        term4 = exp(-1.0*((x-m_pars[0])/m_pars[2])+((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])));
        term5 = erfc(-1.0*(x-m_pars[0])/(m_pars[1]*dsqrt2)+m_pars[1]/m_pars[2]);
        edge = 0.5*(term3-term4*term5); // myedgefunction
        exp_after = exp(-1.0*(m_pars[3]+m_pars[4]*x)); //myexp after
        exp_before = exp(-1.0*(m_pars[5]+m_pars[6]*x)); //my exp before
        return exp_after*(exp_before+(1-exp_before)*edge);
//        return term3;
    }


    /// The partial derivatives are defined as follows:
    /// dyda[0] = d(Edge_function) / d(x0)
    /// dyda[1] = d(Edge_function) / d(sigma)
    /// dyda[2] = d(Edge_function) / d(tau)
    /// dyda[3] = d(Edge_function) / d(a_{0})
    /// dyda[4] = d(Edge_function) / d(b_{0})
    /// dyda[5] = d(Edge_function) / d(a_{hkl})
    /// dyda[6] = d(Edge_function) / d(b_{hkl})
    int EdgeFunction::operator()(double x, double &y, Array1D<double> &dyda)
    {

        if (dyda.dim()!=m_Npars)
            dyda=Array1D<double>(m_Npars);

        y=0.0;
        double term3,term4,term5,edge,exp_after,exp_before;
        term3 = erfc(-1.0*(x-m_pars[0])/(m_pars[1]*dsqrt2));
        term4 = exp(-1.0*((x-m_pars[0])/m_pars[2])+((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])));
        term5 = erfc(-1.0*(x-m_pars[0])/(m_pars[1]*dsqrt2)+m_pars[1]/m_pars[2]);
        edge = 0.5*(term3-term4*term5); // myedgefunction
        exp_after = exp(-1.0*(m_pars[3]+m_pars[4]*x)); //myexp after
        exp_before = exp(-1.0*(m_pars[5]+m_pars[6]*x)); //my exp before
        y = exp_after*(exp_before+(1.0-exp_before)*edge);

        // Here I use the analytical expressions of the derivatives

        dyda[0] = 0.5*exp(-(m_pars[3]+m_pars[4]*x))
                *(1.0-exp(-(m_pars[5]+m_pars[6]*x)))*
                (

                    -1.0*(exp(-((x-m_pars[0])*(x-m_pars[0]))/(2.0*m_pars[1]*m_pars[1]))*dsqrt2/dsqrtPi)/m_pars[1]

                    +(exp(-(x-m_pars[0])/m_pars[2]+(m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])
                        -((-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1])+m_pars[1]/m_pars[2])*(-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1])+m_pars[1]/m_pars[2])))*dsqrt2/dsqrtPi)/m_pars[1]

                    -1.0*((exp(-(x-m_pars[0])/m_pars[2]+(m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])))
                     *erfc(-(x-m_pars[0])/(dsqrt2*m_pars[1])+m_pars[1]/m_pars[2]))/m_pars[2]
                    );



        dyda[1] = 0.5*exp(-(m_pars[3]+m_pars[4]*x))*
                (1.0-exp(-(m_pars[5]+m_pars[6]*x)))*
                (
                 -m_pars[1]*((exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2]))*
                  erfc(-(x-m_pars[0])/(dsqrt2*m_pars[1])+m_pars[1]/m_pars[2]))/(m_pars[2]*m_pars[2])

                 +(2.0*exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-
                     ((m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))*(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1])))-(x-m_pars[0])/m_pars[2])
                      *(1.0/m_pars[2]+(x-m_pars[0])/(dsqrt2*m_pars[1]*m_pars[1])))/dsqrtPi

                 -(dsqrt2/dsqrtPi*(x-m_pars[0])*exp(-((x-m_pars[0])*(x-m_pars[0]))/(2.0*m_pars[1]*m_pars[1])))/(m_pars[1]*m_pars[1])
                )
                ;


        dyda[2]= 0.5*exp(-(m_pars[3]+m_pars[4]*x))*
                (1.0-exp(-(m_pars[5]+m_pars[6]*x)))*
                (
                    -1.0*exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2])
                    *(-(m_pars[1]*m_pars[1])/(m_pars[2]*m_pars[2]*m_pars[2])+(x-m_pars[0])/(m_pars[2]*m_pars[2]))
                    * erfc(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))

                    - (2.0*m_pars[1]*exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-
                       (m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))*(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))-(x-m_pars[0])/(m_pars[2])))
                    /(dsqrtPi*m_pars[2]*m_pars[2])

                    );

        dyda[3]= -1.0*exp(-(m_pars[3]+m_pars[4]*x))*
                (
                    0.5*(1.0-exp(-(m_pars[5]+m_pars[6]*x)))*

                        (
                            erfc(-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1]))

                            -(exp((m_pars[1]*m_pars[1])/(2*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2]))
                            *erfc(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))
                            )

                            +exp(-(m_pars[5]+m_pars[6]*x))


                    );

        dyda[4]= x*(-1.0*exp(-(m_pars[3]+m_pars[4]*x)))*
                (
                     0.5*(1.0-exp(-(m_pars[5]+m_pars[6]*x)))*
                    (
                     erfc(-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1]))

                     -(exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2]))
                     *erfc(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))
                        )
                     +exp(-(m_pars[5]+m_pars[6]*x))
                    );


        dyda[5]= exp(-(m_pars[3]+m_pars[4]*x))*
                (
                    0.5*exp(-(m_pars[5]+m_pars[6]*x))*
                    (
                        erfc(-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1]))

                        -(exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2]))
                        *erfc(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))
                        )
                       -exp(-(m_pars[5]+m_pars[6]*x))
                    );

        dyda[6]= exp(-(m_pars[3]+m_pars[4]*x))*
                (
                    0.5*x*exp(-(m_pars[5]+m_pars[6]*x))*
                    (
                        erfc(-1.0*(x-m_pars[0])/(dsqrt2*m_pars[1]))

                        -(exp((m_pars[1]*m_pars[1])/(2.0*m_pars[2]*m_pars[2])-(x-m_pars[0])/m_pars[2]))
                        *erfc(m_pars[1]/m_pars[2]-(x-m_pars[0])/(dsqrt2*m_pars[1]))
                        )
                    -x*exp(-(m_pars[5]+m_pars[6]*x))
                    );
        return 1;
    }

    int EdgeFunction::Hessian(double x, Array2D<double> &hes)
    {
        return -1;
    }

    int EdgeFunction::Jacobian(double x, Array2D<double> &jac)
    {
        return -1;
    }

    int EdgeFunction::printPars()
    {
        int i;
        char *est=new char[m_Npars];

        for (i=0; i<m_Npars; i++)
            est[i]=m_lock[i] ? ' ': '*';

        cout<<m_lock[0]<<" x0="<<m_pars[0]<< endl;
        cout<<m_lock[1]<<" sigma="<<m_pars[1]<< endl;
        cout<<m_lock[2]<<" tau="<<m_pars[2]<< endl;
        cout<<m_lock[3]<<" a0="<<m_pars[3]<< endl;
        cout<<m_lock[4]<<" b0="<<m_pars[4]<< endl;
        cout<<m_lock[5]<<" ahkl="<<m_pars[5]<< endl;
        cout<<m_lock[6]<<" bhkl="<<m_pars[6]<< endl;

        delete [] est;

        return 0;
    }


}

void string2enum(string &str, Nonlinear::eProfileFunction &e)
{
    std::string lowstr=kipl::strings::toLower(str);

    std::map<std::string,Nonlinear::eProfileFunction> convmap;

    convmap["gaussprofile"]=Nonlinear::eProfileFunction::fnSumOfGaussians;
    convmap["lorenzprofile"]=Nonlinear::eProfileFunction::fnLorenzian;
    convmap["voightprofile"]=Nonlinear::eProfileFunction::fnVoight;
    convmap["edgeprofile"]=Nonlinear::eProfileFunction::fnEdgeFunction;

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
    case Nonlinear::fnEdgeFunction:
        return "EdgeProfile";
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
