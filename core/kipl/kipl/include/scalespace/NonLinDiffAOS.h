//<LICENSE>
#ifndef NONLINDIFFAOS_H
#define NONLINDIFFAOS_H

#include <float.h>
#include <sstream>
#include <ios>
#include <string>

#include "../base/timage.h"
#include "diff_filterbase.h"
#include "lambdaest.h"


namespace akipl{ namespace scalespace {

/// Implentation of non-linear diffusion filter using Weickert's AOS scheme
template <typename T, size_t NDim>
    class NonLinDiffusionFilter: public DiffusionBaseFilter<T,NDim>
	{
	public:
		/// \brief Base Constructor
        NonLinDiffusionFilter(kipl::interactors::InteractionBase *interactor=nullptr);
		/// \brief Destructor to clean up allocated memory
		~NonLinDiffusionFilter();

        /// \brief Constructor
        ///	\param Sigma scale parameter
        ///	\param Tau time increment
        ///	\param Lambda threshold point for the non-linear diffusivity function g
        ///	\param It number of iterations
        NonLinDiffusionFilter(float Sigma, float Tau, float Lambda, int It=10, kipl::interactors::InteractionBase *interactor=nullptr);

        /// \brief Constructor
        ///	\param Sigma scale parameter
        ///	\param Tau time increment
        ///	\param le pointer to a lambda estimator
        ///	\param It number of iterations
        NonLinDiffusionFilter(float Sigma, float Tau, LambdaEstBase<T,NDim> *le, int It=10, kipl::interactors::InteractionBase *interactor=nullptr);

        /// \brief Constructor
        ///	\param Sigma vector with scale parameters
        ///	\param Tau time increment
        ///	\param Lambda threshold point for the non-linear diffusivity function g
        /// \param It number of iterations
        NonLinDiffusionFilter(vector<float> &Sigma, float Tau, LambdaEstBase<T,NDim> *le, int It=10, kipl::interactors::InteractionBase *interactor=nullptr);

        /// \brief Applies a diffusion filter on an image
        ///	\param img input and out of the filter
		int operator()(kipl::base::TImage<float,NDim> &img);

        /// \brief Set lambda to a constant value
        /// \param Lambda the constant
        ///\param N size of the LUT
		int setLambda(float Lambda, int N=10000) {
			lambda=Lambda; ComputeLUT(N); return 0;
		}
		
        /// \brief Set a lambda value estimator
        /// \param le pointer to a lambda estimator
        /// \param N size of the LUT
        int setLambda(LambdaEstBase<T,NDim> *le, int N=10000);

	protected:
        int GradientInfo() { //AbsGradient(this->v,this->g,true);
            return 0;}
		int Solver() {AOSiteration(); return 0;}

        int Regularization() {return DiffusionBaseFilter<T,NDim>::Regularization(this->u,this->v);}

		/// \brief Performs one AOS iteration 
		int AOSiteration();

        /// \brief Solve a linear system of equations using Thomas algorithm for tri diagonal matrices
        /// \param alpha Main diagonal data
        /// \param beta Right diagonal data
        /// \param gamma Left diagonal data
        /// \param d Right hand side of equation system
        /// \param uk Solution
        /// \param N kernel size
        /// \note the function is tested
		int SolveThomas(float *alpha, float *beta, float *gamma, float *d, float *uk, int N);

        /// \brief get the line pointer in G
        /// \param i y-coordinate of the line
        /// \param k z-coordinate of the line
		virtual float * getG(int i, int k) {return this->g.GetLinePtr(i,k);}
		
		/// Compute the control image
		virtual int Diffusivity();

        /// \brief Compute the entries in the LUT
        /// \param N length of the LUT
		int ComputeLUT(int N);

        /// \brief Access the LUT
        ///\param s The function argument to to g(s)
		float getG_LUT(float s);

        /// \brief Allocates memory for the computational kernel
        ///	\param N length of the 1D signal
		int AllocateKernel(int N);

		/// \brief Deallocates the kernel memory
		int DeAllocateKernel();

		float lambda;
        LambdaEstBase<T,NDim> *lambdaest;
		
		int NKernel;

		float *p;
		float *q;
		float *d; /// Right hand side of the Equation system
		float *a; /// Storage vector for the main diagonal of the equation system
		float *b; /// Off-diagonal vectors of the equation system
		float *x; /// Solution vector		
		float *y; /// Vector containing the R solution of the LR decomposition
		float *l; /// L part of the LR decomposition
		float *m; /// Main diagonal of the LR decomposition

	};

template <typename T, size_t NDim>
NonLinDiffusionFilter<T,NDim>::NonLinDiffusionFilter(kipl::interactors::InteractionBase *interactor):
  DiffusionBaseFilter<T,NDim>(1.0f,0.25f,10,interactor),
  lambdaest(nullptr),
  p(nullptr),
  q(nullptr),
  d(nullptr),
  a(nullptr),
  b(nullptr),
  x(nullptr),
  y(nullptr),
  l(nullptr),
  m(nullptr)
{


}

template <typename T, size_t NDim>
NonLinDiffusionFilter<T,NDim>::NonLinDiffusionFilter(float Sigma, float Tau, float Lambda, int It,kipl::interactors::InteractionBase *interactor):
DiffusionBaseFilter<T,NDim>(Sigma,Tau,It,interactor),
    lambda(Lambda),
    lambdaest(nullptr),
    a(nullptr),
    b(nullptr),
    d(nullptr),
    p(nullptr),
    q(nullptr),
    x(nullptr),
    y(nullptr),
    m(nullptr),
    l(nullptr)
{
	this->sigma.clear();
	this->sigma.push_back(Sigma);

    this->NLut=10000;
	ComputeLUT(this->NLut);
}

//template <typename T, size_t NDim>
//NonLinDiffusionFilter<T,NDim>::NonLinDiffusionFilter(float Sigma,
// float Tau,
// LambdaEstBase<T,NDim> *le,
// int It,
// kipl::interactors::InteractionBase *interactor):
//    DiffusionBaseFilter<T,NDim>(Sigma,Tau,It,interactor)
//{
//	lambdaest=le;
//	a=nullptr;
//	b=nullptr;
//	d=nullptr;
//	p=nullptr;
//	q=nullptr;
//	x=nullptr;
//	y=nullptr;
//	m=nullptr;
//	l=nullptr;
//	this->NLut=10000;
//	this->lambda=1.0f;
//	
//	this->sigma.clear();
//	this->sigma.push_back(Sigma);
//	ComputeLUT(this->NLut);
//}
//
//template <typename T, size_t NDim>
//NonLinDiffusionFilter<T,NDim>::NonLinDiffusionFilter(vector<float> &Sigma,
//															float Tau, 
//															LambdaEstBase<T,NDim> *le,
//															int It,kipl::interactors::InteractionBase *interactor) :
// DiffusionBaseFilter<T,NDim>(Sigma,Tau,It,interactor)
//{
//	
//	this->lambdaest=le;
//	
//	a=nullptr;
//	b=nullptr;
//	d=nullptr;
//	p=nullptr;
//	q=nullptr;
//	x=nullptr;
//	y=nullptr;
//	m=nullptr;
//	l=nullptr;
//	this->lambda=1.0f;
//	this->NLut=10000;
//	ComputeLUT(this->NLut);
//}

template <typename T, size_t NDim>
NonLinDiffusionFilter<T,NDim>::~NonLinDiffusionFilter()
{
	DeAllocateKernel();
}


template <typename T, size_t NDim>
    int NonLinDiffusionFilter<T,NDim>::AllocateKernel(int N)
{
	if (N!=NKernel) {
		DeAllocateKernel();

		a=new float[N];
		b=new float[N];
		d=new float[N];
		p=new float[N];
		q=new float[N];
		x=new float[N];
		y=new float[N];
		m=new float[N];		
		l=new float[N];

		NKernel=N;
	}

	return 1;
}

template <typename T, size_t NDim>
    int NonLinDiffusionFilter<T,NDim>::DeAllocateKernel()
{

    if (a!=nullptr) delete [] a;
    if (b!=nullptr) delete [] b;
    if (d!=nullptr) delete [] d;
    if (p!=nullptr) delete [] p;
    if (q!=nullptr) delete [] q;
    if (x!=nullptr) delete [] x;
    if (y!=nullptr) delete [] y;
    if (m!=nullptr) delete [] m;
    if (l!=nullptr) delete [] l;

	return 1;
}

template <typename T, size_t NDim>
int NonLinDiffusionFilter<T,NDim>::operator()(kipl::base::TImage<float,NDim> &img)
{

    switch (NDim) {
    case 2:
        AllocateKernel(max(max(img.Size(0),img.Size(1)),img.Size(2)));
        break;
    case 3:
        AllocateKernel(max(max(img.Size(0),img.Size(1)),img.Size(2)));
        break;
    default:
        kipl::base::KiplException("Chosen dimension is not supported by non-linear diffusion filter",__FILE__,__LINE__);
        return 0;
    }
    this->u.Clone(img);
    img.FreeImage();

    this->v.resize(this->u.dims());
//    //g.resize(u.Dims()); // Don't forget to allocate g or (dx,dy,dz)

    this->InitFilters(this->u.Size(0), this->u.Size(0)*this->u.Size(1));
    auto dims=this->u.dims();

    kipl::profile::Timer timer;
    std::ostringstream msg;

    msg<<"Starting NLD with "<<this->Nit<<" iterations";
    this->logger.message("");
    for (int i=0; (i<this->Nit) && (this->updateStatus(float(i)/this->Nit,"Non-linear diffusion filter iteration")==false); ++i) {
				
        if (this->sigma.front()>0)
            this->UpdateGaussianFilter(dims[0], dims[0]*dims[1],i);
			

        this->g.FreeImage(); // Save memory for the temporary image during smoothing

        DiffusionBaseFilter<T,NDim>::Regularization(this->u,this->v);
        this->logger.verbose("Pre gradient");

        this->AbsGradient(this->v,this->g,true); // Compute the gradient magn of v, g=|(grad v)|^2
        this->logger.verbose("Post gradient");

        Diffusivity();		// Compute the diffusivity of g, g=G(g) from LUT
        this->logger.verbose("Post diffusivity");

        Solver();		// Solve one AOS step, u=AOS(u,g)
        this->logger.verbose("Post solver");
    }
	
    this->v.FreeImage();
    this->g.FreeImage();
    this->dx.FreeImage();
    this->dy.FreeImage();

    img.Clone(this->u);
    this->u.FreeImage();
		
	return 1;
}

template <typename T, size_t NDim>
    int NonLinDiffusionFilter<T,NDim>::AOSiteration()
{
	// AOSISO   Aditive Operator Splitting Isotropic Interation
	//
	//    y = AOSISO(x, d, t) calculates the new image "y" as the result of an
	//    isotropic (scalar) diffusion iteration on image "x" with diffusivity 
	//    "d" and steptime "t" using the AOS scheme.
	//
	//  - If "d" is constant the diffusion will be linear, if "d" is
	//    a matrix the same size as "x" the diffusion will nonlinear.
	//  - The stepsize "t" can be arbitrarially large, in contrast to the explicit
	//    scheme, where t < 0.25. Using larger "t" will only affect the quality
	//    of the diffused image. (Good choices are 5 < t < 20)
	//  - "x" must be a 2D image.

	// initialization
	//f=u; //memorysaving precuation (MSP 041115)
	this->v=this->u; // MSP 041115
    this->v.Clone(); // Use memcpy instead to save new/delete operations
    this->u=static_cast<T>(0);

	int sx=this->u.Size(0);
	int sy=this->u.Size(1);
	int sz=this->u.Size(2),sxy=sx*sy;
	int i,j,k;

	float *pG,*pF,*pU, *pX;
	
	for (k=0; k<sz; k++) {
		// Operating on Rows
		for (i=0; i<sy; i++) {
			pG=getG(i,k);
			//pF=f.GetLinePtr(i,k); // MSP 041115
			pF=this->v.GetLinePtr(i,k);

			for (j=0; j<sx-1; j++) {
				q[j] = pG[j]+pG[j+1]; //(d(1:end-1,:)+d(2:end,:));
				d[j]=pF[j]; // RHS of B u = d
			}
			d[j]=pF[j];
			p[0] = q[0];
			p[sx-1] = q[sx-2];

			for (j=1; j<sx-1; j++) 
				p[j]=q[j-1]+q[j]; //p(2:end-1,:) = ( q(1:end-1,:) + q(2:end,:) );

			for (j=0; j<sx; j++) // Compute the main diagonal
				a[j] = 1 + this->tau*p[j]; //(p is -1*p)

			for (j=0; j<sx-1; j++) // Compute the left and right off-diagonals
				b[j] = -this->tau*q[j];

			SolveThomas(a,b,b,d,x,sx);

			pU=this->u.GetLinePtr(i,k);			// Insert the solution in u
			pX=x;
			for (int j=0; j<sx; j++) 
				pU[j]=pX[j];
		}


		// Operating on Columns
		
		for (i=0; i<sx; i++) {
			pG=this->g.GetLinePtr(0,k)+i;
			//pF=f.getSliceptr(k)+i;
			pF=this->v.GetLinePtr(0,k)+i;
			for (j=0; j<sy-1; j++,pG+=sx, pF+=sx) {
				q[j] = *pG+*(pG+sx); //(d(1:end-1,:)+d(2:end,:));
				d[j] = *pF;
			}
			d[j]=*pF;

			p[0] = q[0];
			p[sy-1] = q[sy-2];

			for (j=1; j<sy-1; j++) 
				p[j]=q[j-1]+q[j]; //p(2:end-1,:) = ( q(1:end-1,:) + q(2:end,:) );


			for (j=0; j<sy; j++)
				a[j] = 1 + this->tau*p[j]; //(p is -1*p)

			for (j=0; j<sy-1; j++)
				b[j] = -this->tau*q[j];

			SolveThomas(a,b,b,d,x,sy);

			pU=this->u.GetLinePtr(0,k)+i;
			pX=x;
			if (NDim==2)
				for (int j=0; j<sy; j++, pU+=sx,pX++) 
					*pU=0.5f*(*pU+*pX);

			if (NDim==3)
				for (int j=0; j<sy; j++, pU+=sx,pX++) 
					*pU=*pU+*pX;

		}
	}

	// Operating on Pillars
	if (NDim==3) {
		float third=1/3.0f;
		for (i=0; i<sxy; i++) {
			pG=this->g.GetDataPtr()+i;
			//pF=f.GetDataPtr()+i;
			pF=this->v.GetDataPtr()+i;

			for (j=0; j<sz-1; j++,pG+=sxy, pF+=sxy) {
				q[j] = *pG+*(pG+sxy); //(d(1:end-1,:)+d(2:end,:));
				d[j] = *pF;
			}
			d[j]=*pF;

			p[0] = q[0];
			p[sy-1] = q[sy-2];

			for (j=1; j<sz-1; j++) 
				p[j]=q[j-1]+q[j]; //p(2:end-1,:) = ( q(1:end-1,:) + q(2:end,:) );


			for (j=0; j<sz; j++)
				a[j] = 1 + this->tau*p[j]; //(p is -1*p)

			for (j=0; j<sz-1; j++)
				b[j] = -this->tau*q[j];

			SolveThomas(a,b,b,d,x,sz);

			pU=this->u.GetDataPtr()+i;
			pX=x;
			

			for (int j=0; j<sz; j++, pU+=sxy,pX++) 
				*pU=(*pU+*pX)*third;

		}
	}

	return 1;
}

    template <typename T, size_t NDim>
        int NonLinDiffusionFilter<T,NDim>::SolveThomas(float *alpha, float *beta, float *gamma, float *d, float *uk, int N)
	{
		int i;
		m[0]=1/alpha[0];
		for (i=0; i<N-1; i++) {
			l[i]=gamma[i]*m[i];
			m[i+1]=1/(alpha[i+1]-l[i]*beta[i]);
		}

		y[0]=d[0];
		for (i=1; i<N; i++) 
			y[i]=d[i]-l[i-1]*y[i-1];

		uk[N-1]=y[N-1]*m[N-1];

		for (i=N-2; i>=0; i--)
			uk[i]=(y[i]-beta[i]*uk[i+1])*m[i];

/*
		m[0]=alpha[0];
		for (i=0; i<N-1; i++) {
			l[i]=gamma[i]/m[i];
			m[i+1]=alpha[i+1]-l[i]*beta[i];
		}

		y[0]=d[0];
		for (i=1; i<N; i++) 
			y[i]=d[i]-l[i-1]*y[i-1];

		uk[N-1]=y[N-1]/m[N-1];

		for (i=N-2; i>=0; i--)
			uk[i]=(y[i]-beta[i]*uk[i+1])/m[i];
*/
		return 1;
	}

template <typename T, size_t NDim>
        int NonLinDiffusionFilter<T,NDim>::Diffusivity()
	{
		float *pG=this->g.GetDataPtr();
		
        if (this->lambdaest !=nullptr) {
            float lambdaold=lambda;
            lambda=(*(this->lambdaest))(this->g);
            if (lambdaold!=lambda)
                ComputeLUT(this->NLut);
        }
		float invLUTindStep=1/this->LUTindStep;
		for (size_t i=0; i<this->g.Size(); i++) {
			if (pG[i]<=this->LUTindMin)
                pG[i]=static_cast<T>(1);
			else
				if (pG[i]>this->LUTindMax)
                    pG[i]=static_cast<T>(0);
				else
                    pG[i]=this->gLUT[static_cast<int>((pG[i]-this->LUTindMin)*invLUTindStep)];

			//pG++;
		}

		return 1;
	}

    template <typename T, size_t NDim>
        float NonLinDiffusionFilter<T,NDim>::getG_LUT(float s)
	{
		if (s<=this->LUTindMin)
			return 1;

		if (s>this->LUTindMax)
			return 0;


		return this->gLUT[int((s-this->LUTindMin)/this->LUTindStep)];
	}



    template <typename T, size_t NDim>
        int NonLinDiffusionFilter<T,NDim>::ComputeLUT(int N)
	{
		this->LUTindMin=lambda*pow(-3.315f/(log(NumLimitDiffusivity)),0.125f);
		this->LUTindMax=lambda*pow(-3.315f/(log(1.0f-1e-7f)),0.125f);
		this->LUTindStep=(this->LUTindMax-this->LUTindMin)/(N-1);
        if (this->gLUT)
            delete [] this->gLUT;
        std::cout<<"G(min)="<<this->LUTindMin<<" G(max)="<<this->LUTindMax<<std::endl;
		this->gLUT=new float[N];

		this->NLut=N;

		float *pGLut=this->gLUT;
		int i;
		float s;
		for (i=0, s=this->LUTindMin; i<this->NLut; i++, s+=this->LUTindStep)
			pGLut[i]=1.0f-exp(-3.315f/pow(s/lambda,8.0f));

		return 1;
	}
}
}
#endif
