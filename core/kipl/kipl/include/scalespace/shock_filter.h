

#ifndef __SHOCK_FILTER_H
#define __SHOCK_FILTER_H

#include <string>

#include "diff_filterbase.h"

namespace ScaleSpace {
/// Shock Laplacian filter according to Osher and Rudin
typedef enum {
		C4=0, /// 1 0 -1 gradient
		C8=1,  /// Gradient according to Jahne
		C12=2,
		Osher /// Oshers orientation stable Laplacian
	} LaplacianType;


/// Selects type of shock filter action
typedef enum {
		Osher_Filter=0, 			/// Oshers shock filter
		Alvarez_Mazorra_Filter=1	/// Alvarez Mazorras extension to Osher includes smoothing
	} ShockType;
	
/// stream interface to the laplacian enum
ostream & operator<<(ostream & os, LaplacianType &lt)
{
	switch (lt) {
	case C4: os<<"C4"; break;
	case C12: os<<"C12"; break;
	case Osher: os<<"Osher"; break;
	}
	return os;
}

/// stream interface to the shocktype enum
ostream & operator<<(ostream & os, ShockType &st)
{
	switch (st) {
	case Osher_Filter: os<<"Osher shock filter"; break;
	case Alvarez_Mazorra_Filter: os<<"Alvarez-Mazorra shock filter"; break;
	}
	return os;
}

/// An implementeation of Shock filter
template <class ImgType, int NDim>
class ShockFilter: public DiffusionBaseFilter<ImgType,NDim> {
public:
	/** \brief Constructor to set up the basic parameters of the shock filter
		\param Sigma width of the smoothing filter
		\param Tau time increment per iteration
		\param Lambda weighting of the diffusion effect
		\param N number of iterations
	*/
	ShockFilter(float Sigma, float Tau, float Lambda, int N);

	/// Empty destructor
	~ShockFilter() {}

	/** \brief Applies the shock filter on an image
		\param img the image to filter, the result will be returned through this image
	*/
	int operator () (CImage<ImgType,NDim> & img);
	/// Selector to the type of Laplacian to compute
	LaplacianType laplaceType;
	/// Selector to select the type of shock scheme to compute
	ShockType shockType;
	
	/// Weight factor between diffusion and shock in the Alvarez filter
	float c;
	
protected:
	/// Suppression of the diffusivity function in the base class
	int Diffusivity() { return 0;}
	
	/// No regularization will be done here
	int Regularization() {this->v=this->u; return 0;}
	
	/// Computes gradient and Laplacian to the image
	int GradientInfo();
	
	/// Solves the diff eq
	int Solver();
	
	/** \brief Initialize additional filters required by the shock filter
	\param sx length of the x-dimension
	\param sxy length of the xy-plane
	*/
	int InitFilters(int sx, int sxy);

	/** \brief Computes the Laplacian of the image

		probably deprecated by the implementation in the base class

		\param  img image to compute Laplacian of.
	*/
	int Laplacian(CImage<ImgType,NDim> &img);

	/** \brief Function to reduce the flux of the solution
		
		\param a left value
		\param b right value

		\retval min(|a|,|b|) 
	*/ 
	float FluxDelimiter(float &a, float &b);

		/** \brief Function to reduce the flux of the solution
		
		\param a left value
		\param b right value
		\param c top

		\retval min(|a|,|b|,|c|) 
	*/ 
	float FluxDelimiter(float &a, float &b, float &c);
	/// Kernel to compute the Laplacian Deprecated by the base class
	float laplaceKernel[3];

	/// Center value of the Laplacian kernel
	float laplaceCenter;

	/// Index vector to the Laplacian
	int indLaplace[9];
	//float s;

	/// Temporary image
	CImage<float,NDim> vv;
};

template <class ImgType, int NDim>
ShockFilter<ImgType,NDim>::ShockFilter(float Sigma, float Tau, float Lambda, int N): DiffusionBaseFilter<ImgType,NDim>(Sigma,Tau,N)
{
	laplaceType=Osher;
	shockType=Osher_Filter;
	this->GradType=Diff_Grad_Minus;
	c=Lambda;
}

template <class ImgType, int NDim>
int ShockFilter<ImgType,NDim>::operator () (CImage<ImgType,NDim> & img)
{
	cout<<"Processing with a "<<shockType<<endl;
	this->u=img;
	img.FreeImage();
	const unsigned int *dims=this->u.getDimsptr();
	
	this->dx.resize(dims);
	this->dy.resize(dims);
	if (shockType==Alvarez_Mazorra_Filter) {
		vv.resize(dims);
		laplaceType=Osher;
	}
	if (laplaceType==Osher)
		this->GradType=Diff_Grad_Minus;
	
	if (NDim==3)
		this->dz.resize(dims);
		
	this->g.resize(dims); // Don't forget to allocate g or (dx,dy,dz)

	InitFilters(dims[0], dims[0]*dims[1]);
	#ifdef USE_VIEWER
	if (this->fig) {
		this->fig->subplot(2,2,0); this->fig->colormap(CM_pink);
		this->fig->subplot(2,2,1); this->fig->colormap(CM_pink);
		this->fig->subplot(2,2,2); this->fig->colormap(CM_jet);
		this->fig->subplot(2,2,3); this->fig->colormap(CM_temp);
	}
	#endif

	char cntstr[8];
	char fname[512];
	char vname[16];
	Stopwatch timer;

	for (int i=0; i<this->Nit; i++) {			
		sprintf(cntstr,"[%d] ",i);
		cout<<setw(8)<<cntstr<<flush;

		if (this->sigma.front()>0) 
			this->UpdateGaussianFilter(dims[0], dims[0]*dims[1],i);

		GradientInfo(); 

		
		Solver();
	
	}
	
	this->g.FreeImage();
	this->dx.FreeImage();
	this->dy.FreeImage();

	img=this->u;
	this->u.FreeImage();
	return 1;
}



template <class ImgType, int NDim>
int ShockFilter<ImgType,NDim>::Solver()
{	
	const unsigned int *dims=this->u.getDimsptr();
	int pos;
	switch (shockType) {
		case Osher_Filter:
		for (pos=0; pos<this->u.N(); pos++) {
			if (this->g[pos]>0.0f) // check sign(laplacian)
				this->u[pos]-=this->tau*this->dx[pos];
			else if (this->g[pos]<0.0f)
				this->u[pos]+=this->tau*this->dx[pos];
				else
					this->u[pos]=this->u[pos];
			}
		break;
		case Alvarez_Mazorra_Filter:
		for (pos=0; pos<this->u.N(); pos++) {
			if (this->dy[pos]>0.0f) // check sign(laplacian)
				this->u[pos]+=this->tau*(-this->dx[pos]+c*this->vv[pos]);
			else if (this->dy[pos]<0.0f)
				this->u[pos]+=this->tau*(this->dx[pos]+c*this->vv[pos]);
				else
					this->u[pos]+=this->tau*c*this->vv[pos];
			}
		break;
	}

	return 0;
}

template <class ImgType, int NDim>
int ShockFilter<ImgType,NDim>::GradientInfo()
{
	Gradient(this->u); // Compute dx and dy
	Laplacian(this->u);
	
	int i,j,k; 
	const unsigned int * dims=this->dx.getDimsptr();
	int sx=dims[0], sxy=dims[0]*dims[1];
	
	if ((this->GradType==Jahne_Grad) || (this->GradType==Simple_Grad_Centered))
		for (i=0; i<this->u.N()-1; i++) 
			this->dx[i]=sqrt(this->dx[i]*this->dx[i]+this->dy[i]*this->dy[i]);
	else {
		float *pDx, *pDy, *pDz[2];
		int zstop=NDim==3 ? dims[2]-1: 1;
		
		for (i=0; i<zstop; i++)
			for (j=0; j<dims[1]-1; j++) {
				pDx=this->dx.getLineptr(j,i);
				pDy=this->dy.getLineptr(j,i);
				
				if (NDim==3) {
					pDz[0]=this->dz.getLineptr(j,i);
					pDz[1]=pDz[0]+sxy;
					for (k=0; k<dims[0]-1; k++)
						pDx[k]=sqrt(FluxDelimiter(pDx[k],pDx[k+1])+
									FluxDelimiter(pDy[k],pDy[k+sx])+
									FluxDelimiter(pDz[0][k],pDz[1][k]));
				}
				else {
					
					for (k=0; k<dims[0]-1; k++) {
						pDx[k]=sqrt(FluxDelimiter(pDx[k],pDx[k+1])+FluxDelimiter(pDy[k],pDy[k+sx]));
					}
				}
				pDx[dims[0]-1]=0.0f; // rigth edge=0
			}
		
		if (NDim==3) {
			pDx=this->dx.getSliceptr(dims[2]-1);
			for (j=0; j<sxy; j++) // bottom slice=0
					pDx[j]=0.0f;
			for (i=0; i<dims[2]-1; i++) {
				pDx=this->dx.getLineptr(dims[1]-1,i);
				for (j=0; j<dims[0]; j++) // back line=0
					pDx[j]=0.0f;
			}
		}
		else {
			pDx=this->dx.getLineptr(dims[1]-1);
			for (j=0; j<dims[0]; j++) // bottom line=0
					pDx[j]=0.0f;
		}
				
		if (shockType==Alvarez_Mazorra_Filter) 
			this->DiffusionBaseFilter<ImgType,NDim>::Regularization(this->g,this->dy);
	}
	return 0;
}

template <class ImgType, int NDim>
int ShockFilter<ImgType,NDim>::InitFilters(int sx,int sxy)
{	
	
	int indlaplacetmp[9]={-1,  0,    1, -sx,0,sx, -sxy, 0, sxy};
	memcpy(indLaplace,indlaplacetmp, 9*sizeof(int));
	float laplaceC4[]={1, -2, 1};
	float laplaceC12[]={1, 2, 1};
	switch (laplaceType) {
		case C4:
			memcpy(laplaceKernel,laplaceC4, 3*sizeof(float));
			laplaceCenter=0.0f;
			break;
		case C12:
			memcpy(laplaceKernel,laplaceC12, 3*sizeof(float));
			laplaceCenter=16;
			break;
	};

	return DiffusionBaseFilter<ImgType,NDim>::InitFilters(sx,sxy);
}

template <class ImgType, int NDim>
int ShockFilter<ImgType,NDim>::Laplacian(CImage<ImgType,NDim> &img)
{
	int i,j,k,x,y,z;
	
	
	float L=0;
	ImgType *pLine[3], *pG,*pDx[3],*pDy[3],*pDz[3], tmp,*pVV;
	
	const unsigned int *dims=img.getDimsptr();
	int zstart=0,zstop=dims[2];
	/*
	if (NDim==3) {
		zstart=1;
		zstop=dims[2]-1;
	}
	*/
	ImgType *lines[3];
	
	int dim, dim_offs;
	int m_sx,p_sx,m_sxy,p_sxy, sx=dims[0], sxy=dims[0]*dims[1];
	this->g=0.0f; 
	float h;
	if (NDim==3) {
		if (laplaceType==Osher) {
			float epsilon=1e-8;
			float Ix,Iy,Iz,Ix2,Iy2,Iz2,Ixx,Iyy,Izz,Ixy,Ixz,Iyz;
			Iz=0.0f;
			
			for (i=0; i<dims[2]; i++) {
				for (j=1; j<dims[1]-1; j++) {
					
					if ((i>0) && (i<dims[2]-1)) { // Main processing
						for (k=-1; k<=1; k++) {
							pLine[k+1]=img.getLineptr(j,i+k); 
							pDx[k+1]=this->dx.getLineptr(j,i+k);
							pDy[k+1]=this->dy.getLineptr(j,i+k);
							pDz[k+1]=this->dz.getLineptr(j,i+k);
						}
/*
						pLine[0]=img.getLineptr(j,i-1); pLine[1]=pLine[0]+sxy; pLine[2]=pLine[1]+sxy;
						pDx[0]=dx.getLineptr(j,i-1); pDx[1]=pDx[0]+sxy; pDx[2]=pDx[1]+sxy;
						pDy[0]=dy.getLineptr(j,i-1); pDy[1]=pDy[0]+sxy; pDy[2]=pDy[1]+sxy;
						pDz[0]=dz.getLineptr(j,i-1); pDz[1]=pDz[0]+sxy; pDz[2]=pDz[1]+sxy;
*/
					}
					
					if (i==0) { // Top processing
						pLine[0]=img.getLineptr(j,i); pLine[1]=pLine[0]; pLine[2]=pLine[1]+sxy;
						pDx[0]=this->dx.getLineptr(j,i); pDx[1]=pDx[0]; pDx[2]=pDx[1]+sxy;
						pDy[0]=this->dy.getLineptr(j,i); pDy[1]=pDy[0]; pDy[2]=pDy[1]+sxy;
						pDz[0]=this->dz.getLineptr(j,i); pDz[1]=pDz[0]; pDz[2]=pDz[1]+sxy;
					}
					
					if (i==dims[2]-1) { // Bottom processing
						pLine[0]=img.getLineptr(j,i-1); pLine[1]=pLine[0]+sxy; pLine[2]=pLine[1];
						pDx[0]=this->dx.getLineptr(j,i-1); pDx[1]=pDx[0]+sxy; pDx[2]=pDx[1];
						pDy[0]=this->dy.getLineptr(j,i-1); pDy[1]=pDy[0]+sxy; pDy[2]=pDy[1];
						pDz[0]=this->dz.getLineptr(j,i-1); pDz[1]=pDz[0]+sxy; pDz[2]=pDz[1];
					}
					
					pG=this->g.getLineptr(j,i);
					
					if (shockType==Alvarez_Mazorra_Filter)
						pVV=this->vv.getLineptr(j,i);
								
					for (k=1; k<dims[0]-1; k++) {
						m_sx=k-dims[0];
						p_sx=k+dims[0];
						
						Ix=0.5*(pDx[1][k]+pDx[1][k+1]); 
						
						Ix2=Ix*Ix;
						Ixx=pLine[1][k-1]+pLine[1][k+1]-2*pLine[1][k];
	
						Iy=0.5*(pDy[1][k]+pDy[1][p_sx]); 
	
						Iy2=Iy*Iy;
						Iyy=pLine[1][m_sx]+pLine[1][p_sx]-2*pLine[1][k];
	
						Ixy=0.25*((pDx[1][p_sx]+pDx[1][p_sx+1])-(pDx[1][m_sx]+pDx[1][m_sx+1])); 
						
							
						m_sxy=k-sxy;
						p_sxy=k+sxy;
					
						Iz=0.5*(pDz[1][k]+pDz[1][k]);
						
						Iz2=Iz*Iz;
						Izz=pLine[0][k]+pLine[2][k]-2*pLine[1][k];
						
						Ixz=0.25*((pDx[2][k]+pDx[2][k+1])-(pDx[0][k]+pDx[0][k+1]));
						Iyz=0.25*((pDy[2][k]+pDy[2][k+sx])-(pDy[0][k]+pDy[0][k+sx])); 

						h=1/(Ix2+Iy2+Iz2+epsilon);
						pG[k]=(Ixx*Ix2+Iyy*Iy2+Izz*Iz2+2*(Ixy*Ix*Iy+Ixz*Ix*Iz+Iyz*Iy*Iz))*h;

						if (shockType==Alvarez_Mazorra_Filter) 
							//pVV[k]=(Ixx*Iy2-2*(Ixy*Ix*Iy+Ixz*Ix*Iz+Iyz*Iy*Iz)+Iyy*Ix2+Izz*Iz2)/(Ix2+Iy2+Iz2+epsilon);
							pVV[k]=0.5*(Ixx*(Iy2+Iz2)+(Iyy+Izz)*Ix2-2*Ix*(Ixy*Iy+Ixz*Iz))*h;
							
						if (!(fabs(Ix)+fabs(Iy)+fabs(Iz))) {
							pG[k]=Ixx;
							if (shockType==Alvarez_Mazorra_Filter) 
								pVV[k]=Iyy;
						}
					}
						
				}
			}
		}
		else {
			for (y=1; y<dims[1]-1; y++) {
				pLine[0]=img.getLineptr(y);
				pG=this->g.getLineptr(y);
				for (x=1; x<dims[0]-1; x++) {
					pG[x]=ImgType(0);
					dim_offs=0;
					for (dim=0; dim<NDim; dim++) {
						tmp=0.0f;
						for (i=0; i<3; i++)
							tmp+=pLine[0][x+indLaplace[i+dim_offs]]*laplaceKernel[i];
						pG[x]+=tmp;
						dim_offs+=3;
					}
					if (laplaceCenter)
						pG[x]-=laplaceCenter*pLine[0][x];
				}
			}
		}
	}
	else {
		if (laplaceType==Osher) {
			float epsilon=1e-8;
			float Ix,Iy,Iz,Ix2,Iy2,Iz2,Ixx,Iyy,Izz,Ixy,Ixz,Iyz;
			
			for (j=1; j<dims[1]-1; j++) {
				pLine[0]=img.getLineptr(j);
					
				pG=this->g.getLineptr(j,i);
				pDx[0]=this->dx.getLineptr(j,i);
				pDy[0]=this->dy.getLineptr(j,i);
				if (shockType==Alvarez_Mazorra_Filter)
					pVV=this->vv.getLineptr(j);
							
				for (k=1; k<dims[0]-1; k++) {
					m_sx=k-dims[0];
					p_sx=k+dims[0];
					
					Ix=0.5*(pDx[0][k]+pDx[0][k+1]); 
					
					Ix2=Ix*Ix;
					Ixx=pLine[0][k-1]+pLine[0][k+1]-2*pLine[0][k];

					Iy=0.5*(pDy[0][k]+pDy[0][p_sx]); 

					Iy2=Iy*Iy;
					Iyy=pLine[0][m_sx]+pLine[0][p_sx]-2*pLine[0][k];

					Ixy=0.25*((pDx[0][p_sx]+pDx[0][p_sx+1])-(pDx[0][m_sx]+pDx[0][m_sx+1])); 
					
					
					pG[k]=(Ixx*Ix2+2*Ixy*Ix*Iy+Iyy*Iy2)/(Ix2+Iy2+epsilon);
					
					if (shockType==Alvarez_Mazorra_Filter) 
						pVV[k]=(Ixx*Iy2-2*Ixy*Ix*Iy+Iyy*Ix2)/(Ix2+Iy2+epsilon);

					if (!(fabs(Ix)+fabs(Iy))) {
						pG[k]=Ixx;
						if (shockType==Alvarez_Mazorra_Filter) 
							pVV[k]=Iyy;
				
					}
				}
			}
		}
		else {
			for (y=1; y<dims[1]-1; y++) {
				pLine[0]=img.getLineptr(y);
				pG=this->g.getLineptr(y);
				for (x=1; x<dims[0]-1; x++) {
					pG[x]=ImgType(0);
					dim_offs=0;
					for (dim=0; dim<NDim; dim++) {
						tmp=0.0f;
						for (i=0; i<3; i++)
							tmp+=pLine[0][x+indLaplace[i+dim_offs]]*laplaceKernel[i];
						pG[x]+=tmp;
						dim_offs+=3;
					}
					if (laplaceCenter)
						pG[x]-=laplaceCenter*pLine[0][x];
				}
			}
		}
	}

	return 0;
}

template <class ImgType, int NDim>
float ShockFilter<ImgType,NDim>::FluxDelimiter(float &a, float &b)
{
	if ((a*b<0.0f))
		return 0.0;
	
	float absA=fabs(a), absB=fabs(b), tmp=absA<absB ? absA : absB;
	
	return tmp*tmp;
}


template <class ImgType, int NDim>
float ShockFilter<ImgType,NDim>::FluxDelimiter(float &a, float &b, float &c)
{
	char t=((a>0.0f)<<2) + ((b>0.0f)<<1) + (c>0.0f);
	
	float absA=fabs(a), absB=fabs(b), absC=fabs(c), tmp;
	switch (t) {
	case 0:
	case 7:
		tmp=absA<absB ? absA : absB;
		tmp=tmp<absC ? tmp :absC;
		break;
	case 3:
		tmp=absB<absC ? absB : absC;
		break;
	case 5:
		tmp=absA<absC ? absA : absC;
		break;
	case 6:
		tmp=absA<absB ? absA : absB;
		break;
	case 1: case 2: case 4:
		return 0.0f;
	}

	return tmp*tmp;
}

}

#endif

