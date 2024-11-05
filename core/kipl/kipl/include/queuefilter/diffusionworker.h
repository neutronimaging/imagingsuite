

#ifndef __DIFFUSIONWORKER_H
#define __DIFFUSIONWORKER_H

#include "linearfilterworker.h"
namespace akipl {namespace queuefilter {
template <class ImgType>
class DiffusionWorker: public BaseQueueWorker<ImgType>
{
public:
    DiffusionWorker(float Tau, float Sigma, float Lambda,  ostream & os=cout);

	int insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res);
	
	int reset();
	virtual int size() {return  gaussian->size();}
	virtual int kernelsize() {return gaussian->kernelsize();}
	int setDims(const size_t *Dims);
	~DiffusionWorker();
    
    virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
private:
	int ComputeLUT(int N);
	int Diffusivity(kipl::base::TImage<ImgType,2> &img);
	int AbsGradient(kipl::base::TImage<ImgType,2> &img);
	int AOSslice(kipl::base::TImage<ImgType,2> &img);
	int AOSiteration(kipl::base::TImage<ImgType,2> &img);
	int SolveThomas(ImgType *alpha, 
					ImgType *beta, 
					ImgType *gamma, 
					ImgType *d, 
					ImgType *uk, 
					int N);
	
	float getG_LUT(float s) 
	{
		if (s<=LUTindMin)
			return 1;

		if (s>LUTindMax)
			return 0;

		return gLUT[int((s-LUTindMin)/LUTindStep)];
	}
	
	deque<kipl::base::TImage<ImgType,2> > uQ;
	deque<kipl::base::TImage<ImgType,2> > gQ;
	deque<kipl::base::TImage<ImgType,2> > gradQ;
	deque<kipl::base::TImage<ImgType,2> > aosQ;
	
	kipl::base::TImage<ImgType,2> v;
	kipl::base::TImage<ImgType,2> u;
	kipl::base::TImage<ImgType,2> g;

	/// Input slice counter
	int cnt;
	/// Result slice counter
	int rescnt;
	
	float tau;
	float sigma;
	float lambda;
	
	string gradselector;
	
	int printQStatus();
	LinearFilterWorker<ImgType> *gaussian;
	
	float LUTindStep;
	float LUTindMax;
	float LUTindMin;
	ImgType *a;
	ImgType *b;
	ImgType *d;
	ImgType *p;
	ImgType *q;
	ImgType *x;
	ImgType *y;
	ImgType *m;
	ImgType *l;
	
	vector<float> gLUT;
};

template <class ImgType>
int DiffusionWorker<ImgType>::reset()
{
	gaussian->reset();
	uQ.clear();
	aosQ.clear();
	gradQ.clear();
	gQ.clear();

	cnt=-1;
	rescnt=-1;

	return 0;
}

template <class ImgType>
DiffusionWorker<ImgType>::DiffusionWorker(float Tau, 
											float Sigma, 
											float Lambda, 
											ostream & os) : BaseQueueWorker<ImgType>(os)
{
	sigma=Sigma;
	tau=Tau;
	lambda=Lambda;
	
	float Kernel[256];
    int N=static_cast<int>(floor(2.5*sigma));
	float sum=0;
	int i;
	for (i=-N; i<=N; i++) 
		sum+=(Kernel[i+N]=exp(-i*i/(2*sigma*sigma)));
		
	for (i=0; i<2*N+1; i++) 
		Kernel[i]=Kernel[i]/sum;
		
	gaussian=new LinearFilterWorker<ImgType>(Kernel, 2*N+1, os);
	cnt=-1;
	rescnt=-1;
	
    a=nullptr;
    b=nullptr;
    d=nullptr;
    p=nullptr;
    q=nullptr;
    x=nullptr;
    y=nullptr;
    m=nullptr;
    l=nullptr;
}

template <class ImgType>
DiffusionWorker<ImgType>::~DiffusionWorker() 
{
	delete gaussian;
	
	if (a) {
		delete [] a;
		delete [] b;
		delete [] d;
		delete [] p;
		delete [] q;
		delete [] x;
		delete [] y;
		delete [] m;
		delete [] l;
	}
}

template <class ImgType>
int DiffusionWorker<ImgType>::insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res)
{
	ostringstream msg;
	try {
		rescnt=gaussian->insert(img,res);
	}
	catch (kipl::base::KiplException & e) {
		msg.str("");
		msg<<"Failed during Gauss filter :\n"<<e.what();
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
	uQ.push_back(img);
	if (rescnt>-1) {
		if (!rescnt) { // Remove unusable edge slices from queue
			int N=gaussian->kernelsize()-(2+gaussian->kernelsize()/2);
			while (N--)
				uQ.pop_front();
		}

		try {
			AbsGradient(res);
		}
		catch (kipl::base::KiplException & e) {
			msg.str("");
			msg<<"Failed during AbsGradient :\n"<<e.what();
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
		try {
			Diffusivity(res);
		}
		catch (kipl::base::KiplException & e) {
			msg.str("");
			msg<<"Failed during Diffusivity :\n"<<e.what();
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}

		try {
			if (AOSslice(res)>2) {
				AOSiteration(res);
				uQ.pop_front();
				gQ.pop_front();
				aosQ.pop_front();

				cnt++;
			}
		}
		catch (kipl::base::KiplException & e) {
			msg.str("");
			msg<<"Failed during AOSiteration :\n"<<e.what();
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	} 

	return cnt;
}

template <class ImgType>
int DiffusionWorker<ImgType>::Diffusivity(kipl::base::TImage<ImgType,2> &img)
{
	float *pImg=img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++)
		pImg[i]=getG_LUT(pImg[i]); 

	gQ.push_back(img);

	return 0;
}

template <class ImgType>
int DiffusionWorker<ImgType>::AbsGradient(kipl::base::TImage<ImgType,2> &res)
{
	if (uQ.size()<3)
		return -1;

	res=0.0f;
	const float A=1.0f/16.0f;
	const float B=2.0f/16.0f;
	const float C=4.0f/16.0f;

	// dx^2+dy^2
	float dx,dy,dz;
	ImgType *pRes;
	float *u[3][3];
	//int x,xm1,xp1,y,z;
	for (size_t y=0; y<this->dims[1]; y++) {
		for (int i=0; i<3; i++) {
			u[i][0]=uQ[i].GetLinePtr(abs(static_cast<long>(y-1u)));
			u[i][1]=uQ[i].GetLinePtr(y);
			u[i][2]=uQ[i].GetLinePtr((y+1==this->dims[1]) ? this->dims[1]-2 : y+1 );
		}

		pRes=res.GetLinePtr(y);

		pRes[0]=0;
		pRes[this->dims[0]-1]=0;
		
		for (size_t x=1,xp1=2, xm1=0; x<(this->dims[0]-1); x++,xp1++,xm1++) 
		{
			dx=	A*( (u[0][0][xp1]-u[0][0][xm1])+
					(u[0][2][xp1]-u[0][2][xm1])+
					(u[2][0][xp1]-u[2][0][xm1])+
					(u[2][2][xp1]-u[2][2][xm1]))+
				B*( (u[1][0][xp1]-u[1][0][xm1])+
					(u[0][1][xp1]-u[0][1][xm1])+
					(u[2][1][xp1]-u[2][1][xm1])+
					(u[1][2][xp1]-u[1][2][xm1]))+
				C*( (u[1][1][xp1]-u[1][1][xm1]));

			dy=	A*( (u[0][2][xp1]+u[0][2][xm1])-
					(u[0][0][xp1]+u[0][0][xm1])+
					(u[2][2][xp1]+u[2][2][xm1])-
					(u[2][0][xp1]+u[2][0][xm1]))+
				B*( (u[1][2][xp1]+u[1][2][xm1])+
					(u[0][2][x]+u[2][2][x])-
					(u[1][0][xp1]+u[1][0][xm1])-
					(u[2][0][x]+u[0][0][x]))+
				C*( (u[1][0][x]-u[1][2][x]));

			dz=	A*( (u[2][0][xp1]-u[0][0][xp1])+
					(u[2][0][xm1]-u[0][0][xm1])+
					(u[2][2][xp1]-u[0][2][xp1])+
					(u[2][2][xm1]-u[0][2][xm1]))+
				B*( (u[2][0][x]-u[0][0][x])+
					(u[2][2][x]-u[0][2][x])+
					(u[2][1][xp1]-u[0][1][xp1])+
					(u[2][1][xm1]-u[0][1][xm1]))+
				C*( (u[2][1][x]-u[0][1][x]));

			pRes[x]=dx*dx+dy*dy+dz*dz;
		}
	}
	
	return 0;
}

template <class ImgType>
int DiffusionWorker<ImgType>::setDims(const size_t *Dims)
{
	u.Resize(Dims);
	v.Resize(Dims);
	g.Resize(Dims);

	if (gaussian)
		gaussian->setDims(Dims);

	if (a) {
		delete [] a;
		delete [] b;
		delete [] d;
		delete [] p;
		delete [] q;
		delete [] x;
		delete [] y;
		delete [] m;
		delete [] l;
	}
	int N=std::max(Dims[0],Dims[1]);

	a=new float[N];
	b=new float[N];
	d=new float[N];
	p=new float[N];
	q=new float[N];
	x=new float[N];
	y=new float[N];
	m=new float[N];
	l=new float[N];
	
	return BaseQueueWorker<ImgType>::setDims(Dims);
}

template <class ImgType>
int DiffusionWorker<ImgType>::ComputeLUT(int N)
{
	LUTindMin=lambda*pow(-3.315f/(log(numeric_limits<float>::min())),0.125f);
	LUTindMax=lambda*pow(-3.315f/(log(1.0f-1e-7f)),0.125f);
	LUTindStep=(LUTindMax-LUTindMin)/(N-1);
	gLUT.clear();
	
	this->logstream<<"G(min)="<<LUTindMin<<" G(max)="<<LUTindMax<<endl;
	
	float *pGLut=gLUT;
	int i;
	float s;
	for (i=0, s=LUTindMin; i<N; i++, s+=LUTindStep)
		this->gLut.push_back(1.0f-exp(-3.315f/pow(s/lambda,8.0f)));

	return 1;
}

template <class ImgType>
int DiffusionWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();
	parnames.push_back("sigma"); parvals.push_back(sigma);
	parnames.push_back("tau"); parvals.push_back(tau);
	parnames.push_back("lambda"); parvals.push_back(lambda);

	return parnames.size();
}


template <class ImgType>
int DiffusionWorker<ImgType>::AOSslice(kipl::base::TImage<ImgType,2> &img)
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
	kipl::base::TImage<float,2> g=img;
	kipl::base::TImage<float,2> v;

	v=uQ[1]; 
	img=0;

	int sx=u.Size(0);
	int sy=u.Size(1);
	//int sz=u.Size(2),sxy=sx*sy;
	int i,j;

	float *pG,*pF,*pU, *pX;
	
	
	// Operating on Rows
	for (i=0; i<sy; i++) {
		pG=g.GetLinePtr(i);
		//pF=f.getLineptr(i,k); // MSP 041115
		pF=v.GetLinePtr(i);

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
			a[j] = 1 + tau*p[j]; //(p is -1*p)

		for (j=0; j<sx-1; j++) // Compute the left and right off-diagonals
			b[j] = -tau*q[j];

		SolveThomas(a,b,b,d,x,sx);

		pU=img.GetLinePtr(i);			// Insert the solution in u
		pX=x;
		for (int j=0; j<sx; j++) 
			pU[j]=pX[j];
	}

	// Operating on Columns
	int pos;
	for (i=0; i<sx; i++) {
		pG=g.GetDataPtr()+i;
		pF=v.GetDataPtr()+i;
		for (j=0, pos=0; j<sy-1; j++,pos+=sx) {
			q[j] = pG[pos]+ pG[pos+sx]; //(d(1:end-1,:)+d(2:end,:));
			d[j] = pF[pos];
		}
		d[j]=pF[pos];

		p[0] = q[0];
		p[sy-1] = q[sy-2];

		for (j=1; j<sy-1; j++) 
			p[j]=q[j-1]+q[j]; //p(2:end-1,:) = ( q(1:end-1,:) + q(2:end,:) );


		for (j=0; j<sy; j++)
			a[j] = 1 + tau*p[j]; //(p is -1*p)

		for (j=0; j<sy-1; j++)
			b[j] = -tau*q[j];

		SolveThomas(a,b,b,d,x,sy);

		pU=u.GetDataPtr()+i;
	
		for (j=0, pos=0; j<sy; j++, pos+=sx) 
				pU[pos]=pU[pos]+x[j];
	}

	aosQ.push_back(u);

	return aosQ.size();
}

template <class ImgType>
int DiffusionWorker<ImgType>::AOSiteration(kipl::base::TImage<ImgType,2> &img)
{	
// Operating on Pillars
	float third=1.0f/3.0f;
	int j;
	size_t i;

	for (i=0; i<img.Size(); i++) {
			for (j=0; j<2; j++) {
				q[j] = gQ[j][i]+gQ[j+1][i]; //(d(1:end-1,:)+d(2:end,:));
				d[j] = uQ[j][i];
			}
			d[j]=uQ[j][i];
		
			p[0] = q[0];
			p[1] = q[0]+q[1]; //p(2:end-1,:) = ( q(1:end-1,:) + q(2:end,:) );
			p[2] = q[1];
		
			for (j=0; j<3; j++)
				a[j] = 1 + tau*p[j]; //(p is -1*p)
		
			for (j=0; j<3; j++)
				b[j] = -tau*q[j];
		
			SolveThomas(a,b,b,d,x,3);
		
			img[i]=(aosQ[1][i]+x[1])*third;
	}

	return 1;
}

template <class ImgType>
int DiffusionWorker<ImgType>::SolveThomas(ImgType *alpha, ImgType *beta, ImgType *gamma, ImgType *d, ImgType *uk, int N)
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

	return 1;
}

} //End namespace QueueFilter
}
#endif
/*



template <class ImgType, int NDim>
	int NonLinDiffusionFilter<ImgType,NDim>::AllocateKernel(int N)
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

template <class ImgType, int NDim>
	int NonLinDiffusionFilter<ImgType,NDim>::DeAllocateKernel()
{

	if (a) delete [] a;
	if (b) delete [] b;
	if (d) delete [] d;
	if (p) delete [] p;
	if (q) delete [] q;
	if (x) delete [] x;
	if (y) delete [] y;
	if (m) delete [] m;
	if (l) delete [] l;

	return 1;
}

template <class ImgType, int NDim>
int NonLinDiffusionFilter<ImgType,NDim>::operator()(kipl::base::TImage<float,NDim> &img)
{
	switch (NDim) {
	case 2:
		AllocateKernel(max(max(img.SizeX(),img.SizeY()),img.SizeZ()));
		break;
	case 3:
		AllocateKernel(max(max(img.SizeX(),img.SizeY()),img.SizeZ()));
		break;
	default:
		cerr<<"Chosen dimension is not supported"<<endl;
		return 0;
	}
	u=img;
	img.FreeImage();

	v.resize(u.getDimsptr()); 
	//g.resize(u.getDimsptr()); // Don't forget to allocate g or (dx,dy,dz)

	InitFilters(u.SizeX(), u.SizeX()*u.SizeY());
	const int *dims=u.getDimsptr();

	char cntstr[8];
	char fname[512];
	char vname[16];
	Stopwatch timer;
	#ifdef USE_VIEWER
	if (fig) {
		fig->subplot(2,2,0); fig->colormap(CM_pink);
		fig->subplot(2,2,1); fig->colormap(CM_pink);
		fig->subplot(2,2,2); fig->colormap(CM_jet);
		fig->subplot(2,2,3); fig->colormap(CM_jet);
	}
	#endif

	for (int i=0; i<Nit; i++) {			
		sprintf(cntstr,"[%d] ",i);
		cout<<setw(8)<<cntstr<<flush;
		
		
		if (sigma.front()>0) 
			UpdateGaussianFilter(dims[0], dims[0]*dims[1],i);
			
		g.FreeImage(); // Save memory for the temporary image during smoothing
		DiffusionBaseFilter<ImgType,NDim>::Regularization(u,v);

		AbsGradient(v,g,true); // Compute the gradient magn of v, g=|(grad v)|^2			
		Diffusivity();		// Compute the diffusivity of g, g=G(g) from LUT			

		Solver();		// Solve one AOS step, u=AOS(u,g)
	
	}
	
	v.FreeImage();
	g.FreeImage();
	dx.FreeImage();
	dy.FreeImage();

	img=u;
	u.FreeImage();
		
	return 1;
}

template <class ImgType, int NDim>
	int NonLinDiffusionFilter<ImgType,NDim>::AOSiteration()
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
	v=u; // MSP 041115
	u=0;

	int sx=u.SizeX();
	int sy=u.SizeY();
	int sz=u.SizeZ(),sxy=sx*sy;
	int i,j,k;

	float *pG,*pF,*pU, *pX;
	
	for (k=0; k<sz; k++) {
		// Operating on Rows
		for (i=0; i<sy; i++) {
			pG=getG(i,k);
			//pF=f.getLineptr(i,k); // MSP 041115
			pF=v.getLineptr(i,k);

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
				a[j] = 1 + tau*p[j]; //(p is -1*p)

			for (j=0; j<sx-1; j++) // Compute the left and right off-diagonals
				b[j] = -tau*q[j];

			SolveThomas(a,b,b,d,x,sx);

			pU=u.getLineptr(i,k);			// Insert the solution in u
			pX=x;
			for (int j=0; j<sx; j++) 
				pU[j]=pX[j];
		}


		// Operating on Columns
		
		for (i=0; i<sx; i++) {
			pG=g.getSliceptr(k)+i;
			//pF=f.getSliceptr(k)+i;
			pF=v.getSliceptr(k)+i;
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
				a[j] = 1 + tau*p[j]; //(p is -1*p)

			for (j=0; j<sy-1; j++)
				b[j] = -tau*q[j];

			SolveThomas(a,b,b,d,x,sy);

			pU=u.getSliceptr(k)+i;
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
			pG=g.getDataptr()+i;
			//pF=f.getDataptr()+i;
			pF=v.getDataptr()+i;

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
				a[j] = 1 + tau*p[j]; //(p is -1*p)

			for (j=0; j<sz-1; j++)
				b[j] = -tau*q[j];

			SolveThomas(a,b,b,d,x,sz);

			pU=u.getDataptr()+i;
			pX=x;
			

			for (int j=0; j<sz; j++, pU+=sxy,pX++) 
				*pU=(*pU+*pX)*third;

		}
	}

	return 1;
}

	template <class ImgType, int NDim>
		int NonLinDiffusionFilter<ImgType,NDim>::SolveThomas(float *alpha, float *beta, float *gamma, float *d, float *uk, int N)
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

		return 1;
	}

template <class ImgType, int NDim>
		int NonLinDiffusionFilter<ImgType,NDim>::Diffusivity()
	{
		float *pG=g.getDataptr();
		
		if (lambdaest) {
			float lambdaold=lambda;
			lambda=(*lambdaest)(g);
			if (lambdaold!=lambda)
				ComputeLUT(NLut);
		}
		float invLUTindStep=1/LUTindStep;
		for (int i=0; i<g.N(); i++) {
			if (pG[i]<=LUTindMin)
				pG[i]=ImgType(1);
			else
				if (pG[i]>LUTindMax)
					pG[i]=ImgType(0);
				else
					pG[i]=gLUT[int((pG[i]-LUTindMin)*invLUTindStep)];

			//pG++;
		}

		return 1;
	}

	template <class ImgType, int NDim>
		float NonLinDiffusionFilter<ImgType,NDim>::getG_LUT(float s) 
	{
		if (s<=LUTindMin)
			return 1;

		if (s>LUTindMax)
			return 0;


		return gLUT[int((s-LUTindMin)/LUTindStep)];
	}



	
*/
