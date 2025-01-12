//<LICENCE>

#ifndef NONLINFIT_H
#define NONLINFIT_H
#include "../kipl_global.h"
#include <vector>

#include <armadillo>

#include "../logging/logger.h"

namespace Nonlinear {

enum eProfileFunction {
    fnSumOfGaussians,
    fnLorenzian,
    fnVoight
};



/// \brief Base class for mathematical function classes
///
///  The children of this class are intended to be used as target functions for non-linear curvefitting.
class KIPLSHARED_EXPORT FitFunctionBase
{
public:

    /// \brief Constructor
    ///
    ///	  \param n The number of parameters needed to describe the target function
    ///	  The contructor initializes and allocated the parameter value and lock vectors
    FitFunctionBase(int n=1);

    /// \brief Computes the value of the target function
    ///
    ///\param x Input argument
    virtual double operator() (double x)=0;

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(double x, double &y, arma::vec & dyda)=0;

    /// \brief Parameter access
    /// \param n index of the parameter to ba accessed
    /// \note If n>#pars will par[0] be returned
    double & operator[](int n);

    /// \brief Computes the numerical Hessian
    ///
    /// \param x The input argument
    /// \param hes The numerical Hessian at x
    virtual int Hessian(double x, arma::mat &hes)=0;

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(double x, arma::mat &jac)=0;

    /// \brief Updates the parameters with a new set
    ///	\param pars Array with the new parameters
    int setPars(arma::vec &pars);

    int getPars(arma::vec &pars);



    /// \brief Returns the number of parameters stored by the instance
    int getNpars();

    /// \brief Returns the number of parameters to be fitted
    int getNpars2fit();

    /// \brief Sets the lock vector
    ///	\param lv Array containing the lock values (0/1=estimate/lock).
    int setLock(const std::vector<bool> &lv);

    void hold(const int i, const double val);

    void free(const int i);

    /// \brief Returns the lock status of a parameter
    ///	\param n Index of parameter to be tested
    /// \note If n>#pars will 1 be returned
    bool isFree(int n);


    virtual ~FitFunctionBase();

    /// \brief Prints the values of the parameters stored by the instance
    ///		Estimated parameters are marked by a *
    virtual int printPars()=0;
protected:
    /// \brief Parameter array
    arma::vec m_pars;
    /// \brief Parameter lock array
    std::vector<bool> m_lock;
    /// \brief The number of parameters
    int m_Npars;
    /// \brief The number of parameters to be fitted
    int m_pars2fit;

};

int KIPLSHARED_EXPORT LeastMedianSquared(double *x, double *y, FitFunctionBase &fn);

class KIPLSHARED_EXPORT LevenbergMarquardt
{
//Object for nonlinear least-squares fitting by the Levenberg-Marquardt method, also including
//the ability to hold specified parameters at fixed, specified values. Call constructor to bind data
//vectors and fitting functions and to input an initial parameter guess. Then call any combination
//of hold, free, and fit as often as desired. fit sets the output quantities a, covar, alpha,
//and chisq.
    kipl::logging::Logger logger;
public:
    LevenbergMarquardt(const double TOL=1.e-3, int iterations=2500) ;

    void setTolerance(double t) { tol=fabs(t); }
    void setIterations(int N) { maxIterations=N; }
    double getTolerance() {return tol;}

    void fit(arma::vec &x,
             arma::vec &y,
             arma::vec &sig,
             Nonlinear::FitFunctionBase &fn);

private:


    static const int NDONE=4; //Convergence parameters.
    int maxIterations;
    int ndat, ma, mfit;

    double tol;
    arma::mat covar;
    arma::mat alpha;
    double chisq;

    void mrqcof(Nonlinear::FitFunctionBase &fn, arma::vec &x, arma::vec &y, arma::vec &sig, arma::mat &alpha, arma::vec &beta);
    void covsrt(arma::mat &covar, Nonlinear::FitFunctionBase &fn);
};

/// \brief Function implementation of a sum of Gaussians
class KIPLSHARED_EXPORT Gaussian: public Nonlinear::FitFunctionBase
{
public:
    /// \brief Constructor
    Gaussian() ;

    /// \brief Computes the function value
    ///
    /// \param x The argument
    /// \retval The method returns \f$A\,\exp{-\frac{(x-\mu)^2}{\sigma^2}}+b\f$
    virtual double operator()(double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(double x, double &y, arma::vec & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(double x, arma::mat & hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(double x, arma::mat &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~Gaussian() {}

};

/// \brief Function implementation of a sum of Gaussians
class KIPLSHARED_EXPORT SumOfGaussians: public Nonlinear::FitFunctionBase
{
public:
    /// \brief Constructor
    /// \param n Number of gassians in the sum
    ///
    /// \note The number of parameters stored by the instance are 3*n
    SumOfGaussians(int n=1) ;

    /// \brief Computes the function value
    ///
    /// \param x The argument
    /// \retval The method returns \f$\sum_{i=1}^{N}\exp{-\frac{(x-\mu)^2}{\sigma^2}}\f$
    virtual double operator()(double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(double x, double &y, arma::vec & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(double x, arma::mat &hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(double x, arma::mat &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~SumOfGaussians() {}

};

/// \brief Function implementation of a sum of Lorenzians
class KIPLSHARED_EXPORT Lorenzian: public Nonlinear::FitFunctionBase
{
public:
    /// \brief Constructor
    /// \param n Number of Lorenzians in the sum
    ///
    /// \note The number of parameters stored by the instance are 3*n
    Lorenzian() ;

    /// \brief Computes the function value
    ///
    /// \param x The argument
    /// \retval The method returns \f$\sum_{i=1}^{N}A_i*\frac{1}{\pi}\frac{1/2 \Gamma}{(x-x_i)^2+(1/2 \Gamma)^2}$
    virtual double operator()(double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(double x, double &y, arma::vec & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(double x, arma::mat &hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(double x, arma::mat &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~Lorenzian() {}

};

class KIPLSHARED_EXPORT Voight : public Nonlinear::FitFunctionBase
{
public:
    /// \brief Constructor
    ///
    Voight() ;

    /// \brief Computes the function value
    ///
    /// \param x The argument
    /// \retval The method returns \f$\sum_{i=1}^{N}A_i*\frac{1}{\pi}\frac{1/2 \Gamma}{(x-x_i)^2+(1/2 \Gamma)^2}$
    virtual double operator()(double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(double x, double &y, arma::vec & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(double x, arma::mat &hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(double x, arma::mat &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~Voight() {}
};

} // End namespace nonlinear

void KIPLSHARED_EXPORT string2enum(std::string &str, Nonlinear::eProfileFunction &e);
std::string KIPLSHARED_EXPORT enum2string(Nonlinear::eProfileFunction e);

std::ostream KIPLSHARED_EXPORT &  operator<<(std::ostream &s, Nonlinear::eProfileFunction e);


#endif

