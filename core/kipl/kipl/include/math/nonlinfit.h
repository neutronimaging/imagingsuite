//<LICENCE>

#ifndef __NONLINFIT_H
#define __NONLINFIT_H

#include <tnt.h>

using namespace TNT;

namespace Nonlinear {
/// \brief Base class for mathematical function classes
///
///  The children of this class are intended to be used as target functions for non-linear curvefitting.
class FitFunctionBase
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
	virtual long double operator() (long double x)=0;

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
	virtual int operator()(long double x, long double &y, Array1D<long double> & dyda)=0;

    /// \brief Parameter access
    /// \param n index of the parameter to ba accessed
    /// \note If n>#pars will par[0] be returned
	long double & operator[](int n); 

    /// \brief Computes the numerical Hessian
    ///
    /// \param x The input argument
    /// \param hes The numerical Hessian at x
	virtual int Hessian(long double x, Array2D<long double> &hes)=0;

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
	virtual int Jacobian(long double x, Array2D<long double> &jac)=0;

    /// \brief Updates the parameters with a new set
    ///	\param pars Array with the new parameters
	int UpdatePars(long double *pars);

    /// \brief Returns the number of parameters stored by the instance
	int getNpars();

    /// \brief Sets the lock vector
    ///	\param lv Array containing the lock values (0/1=estimate/lock).
	int setLock(int *lv); 

    /// \brief Returns the lock status of a parameter
    ///	\param n Index of parameter to be tested
    /// \note If n>#pars will 1 be returned
    int isLocked(int n);

    /// \brief Returns the number of parameters to be fitted
	int getNpars2fit();
    virtual ~FitFunctionBase();

    /// \brief Prints the values of the parameters stored by the instance
    ///		Estimated parameters are marked by a *
    virtual int printPars()=0;
protected:
	/// \brief Parameter array
	long double *m_pars;
	/// \brief Parameter lock array
	int *m_lock;
	/// \brief The number of parameters
	int m_Npars;
	/// \brief The number of parameters to be fitted
	int m_pars2fit;

};


/// \brief Function implementation of a sum of Gaussians
class SumOfGaussians: public FitFunctionBase
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
	virtual long double operator()(long double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
	virtual int operator()(long double x, long double &y, Array1D<long double> & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
	virtual int Hessian(long double x, Array2D<long double> &hes);
	
    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
	virtual int Jacobian(long double x, Array2D<long double> &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
	virtual ~SumOfGaussians() {}

};

/// \brief Function implementation of a sum of Lorenzians
class Lorenzian: public FitFunctionBase
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
    virtual long double operator()(long double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(long double x, long double &y, Array1D<long double> & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(long double x, Array2D<long double> &hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(long double x, Array2D<long double> &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~Lorenzian() {}

};

class Voight : public FitFunctionBase
{
public:
    /// \brief Constructor
    ///
    Voight() ;

    /// \brief Computes the function value
    ///
    /// \param x The argument
    /// \retval The method returns \f$\sum_{i=1}^{N}A_i*\frac{1}{\pi}\frac{1/2 \Gamma}{(x-x_i)^2+(1/2 \Gamma)^2}$
    virtual long double operator()(long double x);

    /// \brief Computes information needed by the LevenbergMarquardt fitting
    ///
    /// \param x Input argument
    /// \param y Function value
    /// \param dyda partial derivatives of the target function at x
    virtual int operator()(long double x, long double &y, Array1D<long double> & dyda);

    /// \brief Computes the numerical Hessian
    ///
    ///	\param x The input argument
    ///	\param hes The numerical Hessian at x
    virtual int Hessian(long double x, Array2D<long double> &hes);

    /// \brief Computes the numerical Jacobian
    ///
    ///	\param x The input argument
    ///	\param jac The numerical Jacobian at x
    virtual int Jacobian(long double x, Array2D<long double> &jac);

    /// \brief Prints the parameters stored by the instance
    ///
    ///	The estimated parameters are marked by a *
    virtual int printPars();
    virtual ~Voight() {}
};

/// \brief Estimates the parameters of a targefunction using Levenberg-Marquardt algorithm
///
///  \param x Array containing the x data
///  \param y Array containing the y data
///  \param n length of the data vector
///  \param fn Instance of a target function
///  \param eps minimum error epsilon
///  \param s predefined standard deviation of the data

///  \author Numerical recipes
///  Provide the main structure of the code
///  \author Anders Kaestner
///  Change the code to use TNT and class based target functions.

int LevenbergMarquardt(double *x, double *y, int n, FitFunctionBase &fn, double eps, double *s=NULL);

int LeastMedianSquared(double *x, double *y, FitFunctionBase &fn);

void mrqmin(long double *x,long double *y,long double *sig,int ndata,
    Array2D<long double> &covar, Array2D<long double> &alpha,
    long double &chisq,
    FitFunctionBase &fn,
    long double &alamda);

} // End namespace nonlinear


#endif
