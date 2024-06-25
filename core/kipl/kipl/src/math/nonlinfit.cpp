//<LICENCE>

#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <tuple>

#include <armadillo>

#include "../../include/math/nonlinfit.h"
#include "../../include/math/mathconstants.h"
#include "../../include/logging/logger.h"
#include "../../include/base/KiplException.h"
#include "../../include/strings/miscstring.h"

namespace Nonlinear {


LevenbergMarquardt::LevenbergMarquardt(const double TOL, int iterations) :
    logger("LevenbergMarquardt"),
    maxIterations(iterations),
    tol(TOL)
{
    // Constructor.
}

void LevenbergMarquardt::fit(arma::vec &x, arma::vec &y,
                              arma::vec &sig,
                              Nonlinear::FitFunctionBase &fn)
{
    //    Iterate to reduce the 2 of a fit between a set of data points x[0..ndat-1], y[0..ndat-1]
    //    with individual standard deviations sig[0..ndat-1], and a nonlinear function that depends
    //    on ma coefficients a[0..ma-1]. When 2 is no longer decreasing, set best-fit values
    //    for the parameters a[0..ma-1], and chisq D 2, covar[0..ma-1][0..ma-1], and
    //    alpha[0..ma-1][0..ma-1]. (Parameters held fixed will return zero covariances.)
    ma   = fn.getNpars();
    mfit = fn.getNpars2fit();

    if ((x.n_elem!=y.n_elem) || (x.n_elem!=sig.n_elem))
        throw kipl::base::KiplException("Array size missmatch for the fitter",__FILE__,__LINE__);

    ndat=x.n_elem;

    covar=arma::mat(ma,ma);
    alpha=arma::mat(ma,ma);

    int j,k,l,iter;
    int    done   = 0;
    double alamda = 0.001;
    double ochisq = 0;

    arma::vec beta(ma),da(ma);
    arma::vec a(ma),atry(ma);
    arma::mat oneda(mfit,1), temp(mfit,mfit);

    fn.getPars(a);

    mrqcof(fn,x,y,sig,alpha,beta); // Initialization.

    fn.getPars(atry);

    ochisq=chisq;

    std::ostringstream msg;
    for (iter=0;iter<maxIterations;iter++)
    {
        if ((iter % 100)==0)
        {
            msg.str(""); msg<<"iteration:"<<iter<<"done:"<<done<<", alambda:"<<alamda<<", chisq:"<<chisq;
            logger.message(msg.str());
        }

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

        oneda = arma::solve(temp,oneda);

        oneda.print();

        for (j=0;j<mfit;j++)
        {
            for (k=0;k<mfit;k++)
                covar.at(j,k)=temp.at(j,k);
            da.at(j)=oneda.at(j,0);
        }

        if (done==NDONE)
        { //Converged. Clean up and return.
            covsrt(covar,fn);
            covsrt(alpha,fn);

            return;
        }

        for (j=0,l=0;l<ma;l++) //Did the trial succeed?
            if (fn.isFree(l))
                atry[l]=fn[l]+da[j++];

        fn.setPars(atry);
        mrqcof(fn,x,y,sig,covar,da);

        if (abs(chisq-ochisq) < std::max(tol,tol*chisq)) done++;

        if (chisq < ochisq)
        { //Success, accept the new solution.
            alamda *= 0.1;
            ochisq=chisq;
            for (j=0;j<mfit;j++)
            {
                for (k=0;k<mfit;k++) alpha.at(j,k)=covar.at(j,k);
                beta[j]=da[j];
            }

            fn.setPars(atry);
        }
        else
        { //Failure, increase alamda.
            alamda *= 10.0;
            chisq=ochisq;
        }
    }
    if (maxIterations <= iter )
        throw kipl::base::KiplException("Fitmrq too many iterations",__FILE__,__LINE__);
}

void LevenbergMarquardt::mrqcof(Nonlinear::FitFunctionBase &fn, arma::vec &x, arma::vec &y,
                                arma::vec &sig, arma::mat &alpha, arma::vec &beta)
{
//Used by fit to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and to calculate 2.
    int i,j,k,l,m;
    double ymod,wt,sig2i,dy;

    arma::vec dyda(ma);
    for (j=0; j<mfit; j++)
    { //Initialize (symmetric) alpha, beta.
        for (k=0; k<=j; k++)
            alpha.at(j,k)=0.0;
        beta[j]=0.0;
    }

    chisq=0.0;

//    for (j=0,l=0;l<ma;l++)
//        qDebug() << "l"<<l<< (fn.isFree(l) ? "free" : "locked");

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

void LevenbergMarquardt::covsrt(arma::mat &covar, Nonlinear::FitFunctionBase &fn)
{
//Expand in storage the covariance matrix covar, so as to take into account parameters that
//are being held fixed. (For the latter, return zero covariances.)
    int i,j,k;
    for (i=mfit;i<ma;i++)
        for (j=0;j<i+1;j++)
            covar.at(i,j) = covar.at(j,i) = 0.0;
    k=mfit-1;
    for (j=ma-1;j>=0;j--)
    {
        if (fn.isFree(j))
        {
            for (i=0;i<ma;i++)
                std::swap(covar.at(i,k),covar.at(i,j));
            for (i=0;i<ma;i++)
                std::swap(covar.at(k,i),covar.at(j,i));
            k--;
        }
    }
}

    //#####################################

    FitFunctionBase::FitFunctionBase(int n) :
        m_lock(n,true)
    {
        m_pars=arma::vec(n);

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

    int FitFunctionBase::setLock(const std::vector<bool> &lv)
    {
        if (lv.size() != static_cast<size_t>(m_Npars))
            throw kipl::base::KiplException("Lock vector is not same size as number of parameters",__FILE__,__LINE__);

        m_pars2fit=0;
        m_lock = lv;

        for (int i=0; i<m_Npars; i++)
            m_pars2fit += m_lock[i] ? 0 : 1;

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
    }

    Gaussian::Gaussian() : FitFunctionBase(4)
    {

    }

    double Gaussian::operator()(double x)
    {
        double arg,ex;

        double y=0.0;

        arg = (x-m_pars[1])/m_pars[2];
        ex  = exp(-arg*arg*0.5);
        y   = m_pars[0]*ex + m_pars[3];

        return y;
    }

    int Gaussian::operator()(double x, double &y, arma::vec & dyda)
    {
        long double fac,ex,arg;

        if (dyda.n_elem!=static_cast<size_t>(m_Npars))
            dyda=arma::vec(m_Npars);

        y=0.0;

        arg = (x-m_pars[1])/m_pars[2];
        ex  = exp(-arg*arg*0.5);
        fac = 2.0*m_pars[0]*ex*arg;
        y   = m_pars[0]*ex + m_pars[3];
        dyda[0] = ex;
        dyda[1] = fac/m_pars[2];
        dyda[2] = fac*arg/m_pars[2];
        dyda[3] = 1;

        return 1;
    }

    int Gaussian::Hessian(double /*x*/, arma::mat & /*hes*/)
    {
        std::cerr<<"The Hessian is not available"<<std::endl;
        return 1;

//        if ((hes.num_rows()!=m_Npars) || (hes.num_cols()!=m_Npars))
//        hes.newsize(m_Npars,m_Npars);

//        return 1;

    }

    int Gaussian::Jacobian(double x, arma::mat & jac)
    {
        std::ignore = x;
        std::ignore = jac;

        std::cerr<<"The Jacobian is not available"<<std::endl;
        return 1;

//        if ((jac.num_rows()!=m_Npars) || (jac.num_cols()!=m_Npars))
//        jac.newsize(m_Npars,m_Npars);

//        return 1;
    }

    int Gaussian::printPars()
    {

        char *est=new char[m_Npars];

        for (int i=0; i<m_Npars; i++)
            est[i]=m_lock[i] ? ' ': '*';

        for (int i=0; i<m_Npars; i+=3)
        {
            std::cout<<est[i]  <<"A="<<m_pars[i]  <<" "
                <<est[i+1]<<"m="<<m_pars[i+1]<<" "
                <<est[i+2]<<"s="<<m_pars[i+2]<<std::endl;
        }

        delete [] est;
        return 1;
    }

    SumOfGaussians::SumOfGaussians(int n) : FitFunctionBase(3*n)
    {

    }

    double SumOfGaussians::operator()(double x)
    {
        int i;
        double arg,ex;

        double y=0.0;
        for (i=0;i<m_Npars;i+=3)
        {
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

        if (dyda.n_elem!=static_cast<size_t>(m_Npars))
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

    int SumOfGaussians::Hessian(double /*x*/, arma::mat & /*hes*/)
    {



        std::cerr<<"The Hessian is not available"<<std::endl;
        return 1;

        // if ((hes.num_rows()!=m_Npars) || (hes.num_cols()!=m_Npars))
        // hes.newsize(m_Npars,m_Npars);

        // return 1;
    }

    int SumOfGaussians::Jacobian(double x, arma::mat & jac)
    {
        std::ignore = x;
        std::ignore = jac;

        std::cerr<<"The Jacobian is not available"<<std::endl;
        return 1;

        // if ((jac.num_rows()!=m_Npars) || (jac.num_cols()!=m_Npars))
        // jac.newsize(m_Npars,m_Npars);

        // return 1;

    }

    int SumOfGaussians::printPars()
    {
        int i;
        char *est=new char[m_Npars];

        for (i=0; i<m_Npars; i++)
            est[i]=m_lock[i] ? ' ': '*';

        for (i=0; i<m_Npars; i+=3)
        {
            std::cout<<est[i]  <<"A="<<m_pars[i]  <<" "
                <<est[i+1]<<"m="<<m_pars[i+1]<<" "
                <<est[i+2]<<"s="<<m_pars[i+2]<<std::endl;
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
        std::ignore = y;
        std::ignore = dyda;

        long double diff=static_cast<long double>(x-m_pars[1]);
        x=m_pars[0]*exp(-m_pars[2]*fabs(diff)-(m_pars[3]*diff*diff*0.5));

        return 0;
    }

    int Voight::Hessian(double x, arma::mat &hes)
    {
        std::ignore = x;
        std::ignore = hes;

        return -1;
    }

    int Voight::Jacobian(double x, arma::mat &jac)
    {
        std::ignore = x;
        std::ignore = jac;

        return 0;
    }

    int Voight::printPars()
    {
        int i;
        char *est=new char[m_Npars];

        for (i=0; i<m_Npars; i++)
            est[i]=m_lock[i] ? ' ': '*';

        std::cout<<est[0]<<"A="<<m_pars[0]<<
            " "<<est[1]<<"m="<<m_pars[1]<<
            " "<<est[2]<<"gamma="<<m_pars[2]<<
            " "<<est[3]<<"sigma="<<m_pars[3]<<std::endl;

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

    int Lorenzian::operator()(double x, double &y, arma::vec & dyda)
    {
        std::ignore = dyda;

        y=m_pars[0]/(dPi*(x*x+m_pars[0]*m_pars[0]));

        return 1;
    }

    int Lorenzian::Hessian(double x, arma::mat &hes)
    {
        std::ignore = x;
        std::ignore = hes;
        return -1;
    }

    int Lorenzian::Jacobian(double x, arma::mat &jac)
    {
        std::ignore = x;
        std::ignore = jac;

        return -1;
    }

    int Lorenzian::printPars()
    {
        char est=m_lock[0] ? ' ' : '*';

        std::cout<<est<<"gamma="<<m_pars[0];
        return 0;
    }




}

void string2enum(std::string &str, Nonlinear::eProfileFunction &e)
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

std::ostream &operator<<(std::ostream &s, Nonlinear::eProfileFunction e)
{
    std::string str;
    str=enum2string(e);

    s<<str;

    return s;
}
