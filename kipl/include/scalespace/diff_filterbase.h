/***************************************************************************
 *   Copyright (C) 2004 by Anders Kaestner                                 *
 *   anders.kaestner@env.ethz.ch                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef __DIFF_FILTERBASE_H
#define __DIFF_FILTERBASE_H

#include <base/timage.h>
#include <io/io_matlab.h>
#include <profile/Timer.h>
#include "lambdaest.h"


//#include "../math/mathmisc.h"
#include <iostream>
#include <ios>
#include <string>
#include <limits>
#include <deque>


namespace akipl { namespace scalespace {
	const float NumLimitDiffusivity=numeric_limits<float>::min();
	
	typedef enum {
		Simple_Grad_Centered=0, /// 1 0 -1 gradient
		Jahne_Grad=1,  /// Gradient according to Jahne
		Simple_Grad_Minus=2, /// Gradient -1 1
		Diff_Grad_Minus=3,  /// Gradient -0.5 0.5
		BinDiff_Grad=4		/// Gradient Bz By Dx etc. 
	} GradientType;

//const GradientType DefaultGrad=Simple_Grad_Centered;
const GradientType DefaultGrad=Jahne_Grad;
//const GradientType DefaultGrad=BinDiff_Grad;

	/// An abstract base class for the family of diffusion related filters
	template <class ImgType, int NDim>
	class DiffusionBaseFilter
	{
	public:
		/// Base constructor
		DiffusionBaseFilter();
		
		/** \brief Constructor to set Regularization parameters
		\param Sigma Width of the smoothing filter
		\param Tau time increment for each iteration
		\param It number of iterations to repeat the filter
		*/
		DiffusionBaseFilter(float Sigma, float Tau, int It=10);
		
		/** \brief Constructor to set Regularization parameters
		\param Sigma vector with widths of the smoothing filter
		\param Tau time increment for each iteration
		\param It number of iterations to repeat the filter
		*/
		DiffusionBaseFilter(vector<float> &Sigma,float Tau, int It=10);

		/// destructor to clean up allocated kernel memory
		virtual ~DiffusionBaseFilter();

		/** Activity function, applies the filter on an image
		\param img the image to filtered, the result is returned though this parameter
		*/
		virtual int operator()(kipl::base::TImage<float,NDim> &img);
		int RegularizationParameters(float Sigma, float Tau,int It=10);
		int RegularizationParameters(vector<float> & Sigma, float Tau,int It=10);
		int ShowIterations(bool selection=true, int slicenum=-1);
		int SaveIterations(bool selection=true,const string &fname="./diff_iteration");
		/// Resets all temporary images to a minimum
		int FreeMem();
		/// Selector to select type of gradient 
		GradientType GradType;
	protected:
	
		/// Interface function to compute the gradient information
		virtual int GradientInfo() {return 0;}
		/// Interface function to call the diff eq solver
		virtual int Solver() {return 0;}// {AOSiteration(); return 0;}
		
		/// Compute the Laplacian of the input image
		int Laplacian();

		/** \brief Compute |grad(img)|
			\param img input image
			\param ag resulting gradient image
			\param squared switch to chose if |grad|^2 will be returned
		*/
		int AbsGradient(kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> & ag, bool squared=false);
		
		/// Compute the individual partial gradients
		int Gradient(kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &d , 
					int order, bool reset=true, bool squared=false);
		int DiffGradient(kipl::base::TImage<ImgType,NDim> &img, kipl::base::TImage<ImgType,NDim> &d , 
					int order, bool reset=true, bool squared=false);
		
		/// Compute the partial gradients
		int Gradient(kipl::base::TImage<ImgType,NDim> &img);

		/// Apply a Gauss filter to the input image 
		int Regularization(kipl::base::TImage<float,NDim> &img, kipl::base::TImage<float,NDim> &res);
		
		virtual int Regularization() {return 0;}
		
		virtual int Diffusivity() {return 0;}
		

		/** \brief allocates memory for the filter kernel
			\param N the longest side of the image
			*/
		int AllocateKernel(int N);
		/** \brief Deallocation of the kernel memory

			*/
		int DeAllocateKernel();

		/** \brief Intializes the filters 

			Sets the filter weights and the position reference vector

			\param sx Length of the x axis
			\param sxy Length of the xy plane
			*/
		virtual int InitFilters(int sx,int sxy);
		
		int initDiffCentered(int sx, int sxy);

		int initDiff(int sx, int sxy);
		
		int initDiff2(int sx, int sxy);

		int initBinDiff(int sx, int sxy);

		int initJahne(int sx, int sxy);
		
//		int splitKernelIndex(int ind,int &d, int &dx, int &dxy);


		int SaveSlice(kipl::base::TImage<ImgType,NDim> & img,int i ,const std::string &suffix="");
		
		/** \brief Update control information for Gaussian filters
			\param sx length of the x-dimension
			\param sxy length of a xy-plane
			\param cnt index of the sigma vector
		*/
		virtual int UpdateGaussianFilter(unsigned int sx,unsigned int sxy,int cnt);

		/// Number of iterations to repeat the filter
		int Nit;

		/// Vector containing scale parameters
		vector<float> sigma;

		/// Time increment per iteration
		float tau;
	
		/// Length of the LUT
		int NLut;
		/// Smallest index value to the LUT 
		float LUTindMin;

		/// Largest index value to the LUT
		float LUTindMax;

		/// Pointer to the LUT vector
		float *gLUT;

		/// Index increment for the access of the LUT
		float LUTindStep;

		/// vector containing the weights of the gradient kernel
		float GradKernel[27];
		/// index vector for the gradient, one set of indices per dimension to compute
		int IndGradKernel[81];
		/// Pointer to the 1D Gaussian filter kernel
		float *GaussKernel;
		
		int *IndGaussKernel;
		/// Number of gradient directions to compute
		int NGrad;
		/// NUmber of elements in the gradient kernel
		int NGradInd;
		/// NUmber of elements in the Gaussian kernel
		int NGauss;

		int Nlvec[2];

		int OffsetStepvec[2];

		int Stepvec[2];
		

		/// Input and output image from the iteration
		kipl::base::TImage<float,NDim> u;
		/// Gradient image
		kipl::base::TImage<float,NDim> v;
		/// Indicator to tell that derivatives are computed
		bool haveDerivatives;
		/// Partial derivative df/dx
		kipl::base::TImage<float,NDim> dx;
		/// Partial derivative df/dy
		kipl::base::TImage<float,NDim> dy;
		/// Partial derivative df/dz
		kipl::base::TImage<float,NDim> dz;
		/// Temporary original image
		kipl::base::TImage<float,NDim> f;
		/// Control image
		kipl::base::TImage<float,NDim> g;

		/// Help image for the visualization
		kipl::base::TImage<float,2> slice;

		/// index to a horizontal slice of 3D image to display
		int displayslice;
		/// index to a horizontal slice of 3D image to save
		int saveslice;
		/// Save image for each iteration
		bool save;
		/// Basename of the filename when iterations are saved
		char basename[512];
		string iteration_filename;
	};

	template <class ImgType, int NDim>
		DiffusionBaseFilter<ImgType,NDim>::DiffusionBaseFilter()
	{
		gLUT=NULL;
		
		//GradType=Jahne_Grad;
		GradType=DefaultGrad;
		GaussKernel=NULL;
		IndGaussKernel=NULL;
		RegularizationParameters(1.0f,0.25f,10);
		save=false;
	}

	template <class ImgType, int NDim>
		DiffusionBaseFilter<ImgType,NDim>::DiffusionBaseFilter(float Sigma, float Tau, int It)
	{
		gLUT=NULL;
		
		GaussKernel=NULL;
		IndGaussKernel=NULL;
		//GradType=Jahne_Grad;
		GradType=DefaultGrad;
		RegularizationParameters(Sigma,Tau,It);
		save=false;
	}

	template <class ImgType, int NDim>
	DiffusionBaseFilter<ImgType,NDim>::DiffusionBaseFilter(vector<float> & Sigma, float Tau, int It)
	{
		gLUT=NULL;
	
		GaussKernel=NULL;
		IndGaussKernel=NULL;
		//GradType=Jahne_Grad;
		GradType=DefaultGrad;
		RegularizationParameters(Sigma,Tau,It);
		save=false;
	}

	template <class ImgType, int NDim>
		DiffusionBaseFilter<ImgType,NDim>::~DiffusionBaseFilter()
	{

		if (gLUT) delete [] gLUT;
		if (GaussKernel) delete [] GaussKernel;
		if (IndGaussKernel) delete [] IndGaussKernel;
	}

	template <class ImgType, int NDim>
	int DiffusionBaseFilter<ImgType,NDim>::ShowIterations(bool selection, int slicenum)
	{
		return 1;
	}
	
	template <class ImgType, int NDim>
	int DiffusionBaseFilter<ImgType,NDim>::SaveIterations(bool selection,const string &fname)
	{
		if (selection) {
			iteration_filename=fname;
			save=true;
		}
		else {
			iteration_filename="";
			save=false;
		}
		
		return 1;
	}
	

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::operator()(kipl::base::TImage<float,NDim> &img)
	{
		

		u=img;
		img.FreeImage();

		v.Resize(u.Dims()); 
		g.Resize(u.Dims()); // Don't forget to allocate g or (dx,dy,dz)
	
		InitFilters(u.Size(0), u.Size(1)*u.Size(2));
        const size_t *dims=u.Dims();

		kipl::profile::Timer timer;
		std::ostringstream msg;
		for (int i=0; i<Nit; i++) {			
			msg.str("");
			msg<<"["<<i<<"]";
			cout<<setw(8)<<msg.str()<<flush;
			
			if (sigma.front()>0) 
				UpdateGaussianFilter(dims[0], dims[0]*dims[1],i);
			
			Regularization();
			GradientInfo(); // Compute the gradient magn of v, g=|(grad v)|^2			
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
		int DiffusionBaseFilter<ImgType,NDim>::RegularizationParameters(float Sigma, float Tau,int It)
	{
		this->sigma.clear();
		this->sigma.push_back(Sigma);

		tau=Tau;
		Nit=It;
		
		return 1;
	}

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::RegularizationParameters(vector<float> & Sigma, float Tau,int It)
	{
		this->sigma=Sigma;
		
		tau=Tau;
		Nit=It;
		
		return 1;
	}
	
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::FreeMem()
	{
		int dims[]={1,1,1};

		// f f.resize(dims);
		g.resize(dims);
		u.resize(dims);
		v.resize(dims);
		dx.resize(dims);
		dy.resize(dims);
		dz.resize(dims);

		return 1;
	}

template <class ImgType, int NDim>
int DiffusionBaseFilter<ImgType,NDim>::AbsGradient(kipl::base::TImage<ImgType,NDim> &img, 
													kipl::base::TImage<ImgType,NDim> &ag, 
													bool squared)
	{
		const size_t * dims=img.Dims();
		float *pG;

		ag.Resize(dims);
		for (int dim=0; dim<NGrad; dim++) {
			Gradient(img,ag,dim,false,true);
		}
		
		if (!squared)
			for (size_t i=0; i<img.Size(); i++)
				pG[i]=sqrt(pG[i]);

		return 1;
	}
	
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::Gradient(kipl::base::TImage<ImgType,NDim> &img)
	{
		Gradient(img,dx,0);
		Gradient(img,dy,1);
		if (NDim==3)
			Gradient(img,dz,2);
			
		return 0;
	}
		
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::DiffGradient(kipl::base::TImage<ImgType, NDim> & img, 
														kipl::base::TImage<ImgType,NDim> &d, 
														int order, bool reset, bool squared)
	{
		ImgType *pA,*pB, *pD;
		int offset;
        const size_t * dims=img.Dims();
		int startx,starty,startz;
		switch (order) {
			case 0: offset=1; startx=1;starty=0;startz=0; break;
			case 1: offset=dims[0]; startx=0;starty=1;startz=0; break;
			case 2: offset=dims[0]*dims[1]; startx=0;starty=0;startz=1; break;
			default: offset=1; startx=1;starty=0;startz=0; break; 
		}
		
		int x,y,z;
		if (reset) {
			d.Resize(img.Dims());
			d=0;
		}
		
		ImgType dtmp;

        for (z=startz; z<static_cast<int>(dims[2]); z++) {
            for (y=starty; y<static_cast<int>(dims[1]); y++) {
				pA=img.GetLinePtr(y,z);
				pB=pA-offset;
				pD=d.GetLinePtr(y,z);
                for (x=startx; x<static_cast<int>(dims[0]); x++) {
					dtmp=pA[x]-pB[x];
					pD[x]+=squared ? dtmp*dtmp : dtmp;
				}
			}
		}
		
		return 0;
	}
	
	
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::Gradient(kipl::base::TImage<ImgType, NDim> & img, 
														kipl::base::TImage<ImgType,NDim> &d, 
														int order, bool reset, bool squared) 
	{
		if (GradType==Diff_Grad_Minus) {
			return DiffGradient(img,d,order,reset,squared);
		}
		
        const size_t *dims=img.Dims();
		float *pV, *pD;//*pDX, *pDY, *pDZ;
		float tmp,ltmp,rtmp;
		int *indFilt=this->IndGradKernel+order*this->NGradInd;; // *IndFiltX, *IndFiltY, *IndFiltZ, *pI;
		int pos,ref,left,right;
		int startz, stopz,h,t;
		int sx=dims[0], sxy=dims[0]*dims[1];
		
		
		
		if (NDim==3) {
			//startz=0;
			startz=1;
			stopz=dims[2]-1;
		}
		else {
			startz=0;
			stopz=1;
		}
		
		if ((order<0) || (order>2))
			order=0;
			
		if (reset) {
			d.Resize(img.Dims());
			d=0;
		}
		
		
		//--------------------
		// Main processing loop
		for (h=startz; h<stopz; h++) {
			//----------------
			// Back line processing
			pD=d.GetLinePtr(dims[1]-1,h);
			pV=img.GetLinePtr(dims[1]-1,h);
		
			ltmp=0; rtmp=0;
			for (int k=0; k<NGradInd; k++) { // Left and right edge
				t=indFilt[k]%sxy;
				t=indFilt[k]<0 ? sxy+t : t;
				if (abs(t-sx)<=1)
					pos=indFilt[k]-sx;
				else
					pos=indFilt[k];
					
				ref=indFilt[k]%dims[0];
				ref=ref-(ref>0 ? sx : 0 );
				
				left=pos+(ref==-1); 
				ltmp+=pV[left]* GradKernel[k];
				
				
				right=dims[0]-1+pos-(ref<-1);
				rtmp+=pV[right]*GradKernel[k];
			}
			pD[0]+=!squared ? ltmp : ltmp*ltmp;
			pD[dims[0]-1]+=!squared ? rtmp : rtmp*rtmp;
            for (int j=1; j<(static_cast<int>(dims[0])-1); j++) { // Main part
				tmp=0;
				for (int k=0; k<NGradInd; k++) {
					t=indFilt[k]%sxy;
					t=indFilt[k]<0 ? sxy+t : t;
					if (abs(t-sx)<=1)
						pos=j+indFilt[k]-sx;
					else
						pos=j+indFilt[k];
					tmp+=pV[pos]* GradKernel[k];
				}
				pD[j]+=!squared ? tmp : tmp*tmp;
			}
			//----------------
			// Front line processing
			pD=d.GetLinePtr(0,h);
			pV=img.GetLinePtr(0,h);
		
			ltmp=0; rtmp=0;
			for (int k=0; k<NGradInd; k++) { // Left and right edge
				t=indFilt[k]%sxy;
				t=indFilt[k]>0 ? t-sxy : t;
				if (abs(t+sx)<=1)
					pos=indFilt[k]+sx;
				else
					pos=indFilt[k];
					
				ref=indFilt[k]%dims[0];
				ref=ref-(ref>0 ? sx : 0 );
				
				left=pos+(ref==-1); 
				ltmp+=pV[left]* GradKernel[k];
				
				
				right=dims[0]-1+pos-(ref<-1);
				rtmp+=pV[right]*GradKernel[k];
			}
			pD[0]+=!squared ? ltmp : ltmp*ltmp;
			pD[dims[0]-1]+=!squared ? rtmp : rtmp*rtmp;
			
			for (int j=1; j<dims[0]-1; j++) { // Main part
				tmp=0;
				for (int k=0; k<NGradInd; k++) {
					t=indFilt[k]%sxy;
					t=indFilt[k]>0 ? t-sxy : t;
					if (abs(t+sx)<=1) {
						//cout<<"."<<flush;
						pos=j+indFilt[k]+sx;
					}
					else
						pos=j+indFilt[k];
					tmp+=pV[pos]* GradKernel[k];
				}
				pD[j]+=!squared ? tmp : tmp*tmp;
			}
			//---------------------
			// Plane processing 
			for (int i=1; i<dims[1]-1; i++) { 
				pD=d.GetLinePtr(i,h);
				pV=img.GetLinePtr(i,h);
			
				ltmp=0; rtmp=0;
				for (int k=0; k<NGradInd; k++) { // Left and right edge
					ref=indFilt[k]%dims[0];
					ref=ref-(ref>0 ? sx : 0 );
					
					left=indFilt[k]+(ref==-1); 
					ltmp+=pV[left]* GradKernel[k];
					
					
					right=dims[0]-1+indFilt[k]-(ref<-1);
					rtmp+=pV[right]*GradKernel[k];
				}
				pD[0]+=!squared ? ltmp : ltmp*ltmp;
				pD[dims[0]-1]+=!squared ? rtmp : rtmp*rtmp;
				
				for (int j=1; j<dims[0]-1; j++) { // Main part
					tmp=0;
					for (int k=0; k<NGradInd; k++) {
						pos=j+indFilt[k];
						tmp+=pV[pos]* GradKernel[k];
					}
					pD[j]+=!squared ? tmp : tmp*tmp;
				}
		
			
			}
		}

		return 1;
	}

		
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::Regularization(kipl::base::TImage<float,NDim> &img,kipl::base::TImage<float,NDim> &res)
	{
	
        std::deque<ImgType *> lineQ;
				
		float *pV, *pU,*pFilt;
		int *pIndFilt;
		int pos, h,i,j,k;
	
        const size_t *dims=img.Dims();
		int sxy,sx;
		sx=dims[0];
		sxy=dims[0]*dims[1];

		int NGauss2=NGauss/2;
		int front, back;
		
		f=img;
        int d0=dims[0]-NGauss2;
        int xlimits[3][2]={{NGauss2,d0},{0,static_cast<int>(dims[0])},{0,static_cast<int>(dims[0])}};
        int ylimits[3][2]={{0,static_cast<int>(dims[1])},{NGauss2,static_cast<int>(dims[1]-NGauss2)},{0,static_cast<int>(dims[1])}};
        int zlimits[3][2]={{0,static_cast<int>(dims[2])},{0,static_cast<int>(dims[2])},{NGauss2,static_cast<int>(dims[2]-NGauss2)}};

		if (NDim!=3) {
			zlimits[0][0]=0; zlimits[0][1]=1;
			zlimits[1][0]=0; zlimits[1][1]=1;
			zlimits[2][0]=0; zlimits[2][1]=1;
			
		}

		for (int dim=0; dim<NDim; dim++) {
			pFilt=GaussKernel;
			pIndFilt=IndGaussKernel+dim*NGauss;
			
			if (dim==2) { // Edge processing top/bottom
					int bottom;
					j=0;
					lineQ.clear();
					while (lineQ.size()<NGauss) {
						lineQ.push_front(f.GetLinePtr(0,j));
						lineQ.push_back(f.GetLinePtr(0,j));
						j++;
					}
						
					for (i=0; i<zlimits[2][0]; i++) { // Top	
							pV=res.GetLinePtr(0,i);
							for (pos=0; pos<sxy; pos++) {
								pV[pos]=0.0f;
								for (h=0; h<NGauss; h++)
									pV[pos]+=lineQ[h][pos]*GaussKernel[h];
							}
						
						lineQ.pop_front();
						lineQ.push_back(f.GetLinePtr(0,j));
					}
					
					lineQ.clear();
					
					for (i=zlimits[dim][1]; i<dims[2]; i++) { // Bottom
						bottom=(dims[2]-i-1)*sxy;
                        for (j=0; j<static_cast<int>(dims[1]); j++) {
							pU=f.GetLinePtr(j,i);
							pV=res.GetLinePtr(j,i);
							
							
                            for (k=0; k<static_cast<int>(dims[0]); k++) {
								pV[k]=0.0f;
								for (h=0; h<NGauss; h++) { 
									if (pIndFilt[h]>bottom) 
										pos=k+2*bottom-pIndFilt[h]+sxy;
									else
										pos=k+pIndFilt[h];
	
									pV[k]+=pU[pos]*GaussKernel[h];
								}
							}
						}
					}
								
			}
			
			for (i=zlimits[dim][0]; i<zlimits[dim][1]; i++) {
				front=i*sxy;
				back=(i+1)*sxy-1;
				
				if (dim==1) { // Edge processing front/back

                    for (j=ylimits[dim][1]; j<static_cast<int>(dims[1]); j++) {
						pU=f.GetLinePtr(j,i);
						pV=res.GetLinePtr(j,i);
						
						back=(dims[1]-j-1)*sx;
                        for (k=0; k<static_cast<int>(dims[0]); k++) {
							pV[k]=0.0f;
							
							for (h=0; h<NGauss; h++) { // Fix right edge
								if (pIndFilt[h]>back)
									pos=k+2*back-pIndFilt[h]+sx;
								else
									pos=k+pIndFilt[h];

								pV[k]+=pU[pos]*GaussKernel[h];
							}
						}
					}

					for (j=0; j<ylimits[dim][0]; j++) {
						pU=f.GetLinePtr(j,i);
						pV=res.GetLinePtr(j,i);
						
						front=-sx*j;
                        for (k=0; k<static_cast<int>(dims[0]); k++) {
							pV[k]=0.0f;
							for (h=0; h<NGauss; h++) { // Fix left edge
								if (pIndFilt[h]<front)
									pos=k+(front-pIndFilt[h]);
								else
									pos=k+pIndFilt[h];

								pV[k]+=pU[pos]*GaussKernel[h];
							}
						}
					} 

                    for (j=ylimits[dim][1]; j<static_cast<int>(dims[1]); j++) {
						pU=f.GetLinePtr(j,i);
						pV=res.GetLinePtr(j,i);
						
						back=((dims[1]-1)-j)*sx;
                        for (k=0; k<static_cast<int>(dims[0]); k++) {
							pV[k]=0.0f;
							for (h=0; h<NGauss; h++) { // Fix back edge
								if (pIndFilt[h]>back) 
									pos=k+2*back-pIndFilt[h]+sx;
								else
									pos=k+pIndFilt[h];
								
								pV[k]+=pU[pos]*GaussKernel[h];
							}
						}
					} 
				}
				
				// Main processing of the slice
				for (j=ylimits[dim][0]; j<ylimits[dim][1]; j++) {
					pU=f.GetLinePtr(j,i);
					pV=res.GetLinePtr(j,i);
					
					for (k=xlimits[dim][0]; k<xlimits[dim][1]; k++) { //Main field
						pV[k]=0.0f;
						for (h=0; h<NGauss; h++) {
							pos=k+pIndFilt[h];
							pV[k]+=pU[pos]*GaussKernel[h];
						}
					}

					if (dim==0) { // Edge processing left/right
                        for (k=xlimits[0][1]; k<static_cast<int>(dims[0]); k++) {
							pV[k]=0.0f;
							int right=dims[0]-1-k;
							for (h=0; h<NGauss; h++) { // Fix right edge
								if (pIndFilt[h]>right)
									pos=k+2*right-pIndFilt[h]+1;
								else
									pos=k+pIndFilt[h];

								pV[k]+=pU[pos]*GaussKernel[h];
							}
						}
	
						for (k=0; k<xlimits[0][0]; k++) {
							pV[k]=0.0f;
							for (h=0; h<NGauss; h++) { // Fix left edge
								pos=k+pIndFilt[h];
								if (pos<0)		// Mirror edge
									pos=-pos-1;
								pV[k]+=pU[pos]*GaussKernel[h];
							}
						}

					}
				}
			}
			f=res;
		}
		
		f.FreeImage();
		return 1;
	}

	

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::UpdateGaussianFilter(unsigned int sx,unsigned int sxy,int cnt)
	{
		float s;
		if (cnt<sigma.size()) {
			s=sigma[cnt];
				
			//NGauss=int(ceil(s*1.96))*2+1;	// This must be made flexible depending on the value of sigma.
			NGauss=int(ceil(s*2))*2+1;
			cout<<"Gaussian kernel sigma:"<<s<<" size: "<<NGauss<<endl;
	
            if (GaussKernel)
                delete [] GaussKernel;

			GaussKernel=new float [NGauss];
	
            if (IndGaussKernel)
                delete [] IndGaussKernel;

			IndGaussKernel=new int [3*NGauss];
	
			int i,j;
	
			for (i=0; i<3; i++) {
				IndGaussKernel[i*NGauss]=0;
	
			}
			GaussKernel[0]=1;
			float Gsum=1;
			for (i=1,j=1; i<NGauss; i+=2,j++) {
				IndGaussKernel[i]=-j;IndGaussKernel[i+1]=j;
				IndGaussKernel[i+NGauss]=-j*sx;IndGaussKernel[i+NGauss+1]=j*sx;
				IndGaussKernel[i+2*NGauss]=-j*sxy;IndGaussKernel[i+2*NGauss+1]=j*sxy;
				GaussKernel[i]=exp(-(j*j)/(2*s*s));
				GaussKernel[i+1]=GaussKernel[i];
				Gsum+=2*GaussKernel[i];
			}
			
			for (i=0; i<NGauss; i++) 
				GaussKernel[i]=GaussKernel[i]/Gsum;
		}

		return 0;
	}
	
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::InitFilters(int sx,int sxy)
	{
		switch (GradType) {
			case Jahne_Grad:
				cout<<"Using Jahne gradient"<<endl;
				initJahne(sx,sxy);
				break;
			case Simple_Grad_Minus:
				cout<<"Using simple gradient minus (x[i]-x[i-1])/2"<<endl;
				initDiff2(sx,sxy);
				break;
			case Diff_Grad_Minus:
				cout<<"Using simple gradient minus (x[i]-x[i-1])"<<endl;
				initDiff(sx,sxy);
				break;
			case BinDiff_Grad:
				cout<<"Using binomial filtered gradient Bz*By*Dx"<<endl;
				initBinDiff(sx,sxy);
				break;
			default:
			case Simple_Grad_Centered:
				cout<<"Using simple centered gradient (x[i+1]-x[i-1])/2"<<endl;
				initDiffCentered(sx,sxy);
				break;
				
		}

        const size_t *dims=u.Dims();

		Nlvec[0]=dims[1]; 
		Nlvec[1]=dims[0];
		OffsetStepvec[0]=dims[1];
		OffsetStepvec[1]=1;
		Stepvec[0]=1;
		Stepvec[1]=dims[0];

		return 1;
	}

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::initJahne(int sx, int sxy)
	{
		NGradInd=6;
		switch (NDim) {
			case 2: NGrad=2; break;
			case 3: NGrad=6; break;
		}
		
		// xy-plane indices
		IndGradKernel[0]=-sx-1; IndGradKernel[1]=-1; IndGradKernel[2]=sx-1; 
		IndGradKernel[3]=-sx+1; IndGradKernel[4]=1; IndGradKernel[5]=sx+1;

		IndGradKernel[6]=-sx-1; IndGradKernel[7]=-sx; IndGradKernel[8]=-sx+1; 
		IndGradKernel[9]=sx-1; IndGradKernel[10]=sx; IndGradKernel[11]=sx+1;

		// xz-plane indices
		IndGradKernel[12]=-sxy-1; IndGradKernel[13]=-1; IndGradKernel[14]=sxy-1; 
		IndGradKernel[15]=-sxy+1; IndGradKernel[16]=1; IndGradKernel[17]=sxy+1;

		IndGradKernel[18]=-sxy-1; IndGradKernel[19]=-sxy; IndGradKernel[20]=-sxy+1; 
		IndGradKernel[21]=sxy-1; IndGradKernel[22]=sxy; IndGradKernel[23]=sxy+1;

		// yz-plane indices
		IndGradKernel[24]=-sxy-sx; IndGradKernel[25]=-sx; IndGradKernel[26]=sxy-sx; 
		IndGradKernel[27]=-sxy+sx; IndGradKernel[28]=sx; IndGradKernel[29]=sxy+sx;

		IndGradKernel[30]=-sxy-sx; IndGradKernel[31]=-sxy; IndGradKernel[32]=-sxy+sx; 
		IndGradKernel[33]=sxy-sx; IndGradKernel[34]=sxy; IndGradKernel[35]=sxy+sx;

		// gradient kernel
		GradKernel[0]=-3.0/32.0; GradKernel[1]=-10.0/32.0; GradKernel[2]=-3.0/32.0;
		GradKernel[3]=3.0/32.0; GradKernel[4]=10.0/32.0; GradKernel[5]=3.0/32.0;
		return 0;
	}

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::initDiff2(int sx, int sxy)
	{
		NGradInd=2; 
		switch (NDim) {
			case 2: NGrad=2; break;
			case 3: NGrad=3; break;
		}
		IndGradKernel[0]=-1; IndGradKernel[1]=0;
		IndGradKernel[2]=-sx; IndGradKernel[3]=0;
		IndGradKernel[4]=-sxy; IndGradKernel[5]=0;

		GradKernel[0]=-0.5; GradKernel[1]=0.5;

		return 0;
	}
	
	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::initDiff(int sx, int sxy)
	{
		NGradInd=2; 
		switch (NDim) {
			case 2: NGrad=2; break;
			case 3: NGrad=3; break;
		}
		IndGradKernel[0]=-1; IndGradKernel[1]=0;
		IndGradKernel[2]=-sx; IndGradKernel[3]=0;
		IndGradKernel[4]=-sxy; IndGradKernel[5]=0;

		GradKernel[0]=-1; GradKernel[1]=1;

		return 0;
	}

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::initDiffCentered(int sx, int sxy)
	{
		NGradInd=2;
		switch (NDim) {
			case 2: NGrad=2; break;
			case 3: NGrad=3; break;
		}
		IndGradKernel[0]=-1; IndGradKernel[1]=1;
		IndGradKernel[2]=-sx; IndGradKernel[3]=sx;
		IndGradKernel[4]=-sxy; IndGradKernel[5]=sxy;

		GradKernel[0]=-0.5; GradKernel[1]=0.5;
		return 0;
	}

	template <class ImgType, int NDim>
		int DiffusionBaseFilter<ImgType,NDim>::initBinDiff(int sx, int sxy)
	{
		
		int scale;
		if (NDim==2) {
			NGrad=2; scale=8; NGradInd=6;
			// xy-plane indices
			IndGradKernel[0]=-sx-1; IndGradKernel[1]=-1; IndGradKernel[2]=sx-1; 
			IndGradKernel[3]=-sx+1; IndGradKernel[4]=1; IndGradKernel[5]=sx+1;
	
			IndGradKernel[6]=-sx-1; IndGradKernel[7]=-sx; IndGradKernel[8]=-sx+1; 
			IndGradKernel[9]=sx-1; IndGradKernel[10]=sx; IndGradKernel[11]=sx+1;
	
			GradKernel[0]=-1.0f/scale; GradKernel[1]=-2.0f/scale; GradKernel[2]=-1.0f/scale;
			GradKernel[3]=1.0f/scale; GradKernel[4]=2.0f/scale; GradKernel[5]=1.0f/scale;

		}

		if (NDim==3) {
			NGrad=3; scale=36; NGradInd=18;
			
			// dx indices
			IndGradKernel[0]=-sxy-sx-1; IndGradKernel[1]=-sxy-1; IndGradKernel[2]=-sxy+sx-1; 
			IndGradKernel[3]=-sxy-sx+1; IndGradKernel[4]=-sxy+1; IndGradKernel[5]=-sxy+sx+1;
			
			IndGradKernel[6]=-sx-1; IndGradKernel[7]=-1; IndGradKernel[8]=sx-1; 
			IndGradKernel[9]=-sx+1; IndGradKernel[10]=1; IndGradKernel[11]=sx+1;
			
			IndGradKernel[12]=sxy-sx-1; IndGradKernel[13]=sxy-1; IndGradKernel[14]=sxy+sx-1; 
			IndGradKernel[15]=sxy-sx+1; IndGradKernel[16]=sxy+1; IndGradKernel[17]=sxy+sx+1;
			
			// dy indices
			IndGradKernel[18]=-sxy-sx-1; IndGradKernel[19]=-sxy-sx; IndGradKernel[20]=-sxy-sx+1; 
			IndGradKernel[21]=-sxy+sx-1; IndGradKernel[22]=-sxy+sx; IndGradKernel[23]=-sxy+sx+1;
			
			IndGradKernel[24]=-sx-1; IndGradKernel[25]=-sx; IndGradKernel[26]=-sx+1; 
			IndGradKernel[27]=sx-1; IndGradKernel[28]=sx; IndGradKernel[29]=sx+1;
			
			IndGradKernel[30]=sxy-sx-1; IndGradKernel[31]=sxy-sx; IndGradKernel[32]=sxy-sx+1; 
			IndGradKernel[33]=sxy+sx-1; IndGradKernel[34]=sxy+sx; IndGradKernel[35]=sxy+sx+1;
			
			// dz indices
			IndGradKernel[36]=-sxy-sx-1; IndGradKernel[37]=-sxy-sx; IndGradKernel[38]=-sxy-sx+1; 
			IndGradKernel[39]=sxy-sx-1; IndGradKernel[40]=sxy-sx; IndGradKernel[41]=sxy-sx+1; 
			
			IndGradKernel[42]=-sxy-1; IndGradKernel[43]=-sxy; IndGradKernel[44]=-sxy+1;
			IndGradKernel[45]=sxy-1; IndGradKernel[46]=sxy; IndGradKernel[47]=sxy+1;
			
			IndGradKernel[48]=-sxy+sx-1; IndGradKernel[49]=-sxy+sx; IndGradKernel[50]=-sxy-sx+1; 
			IndGradKernel[51]=sxy+sx-1; IndGradKernel[52]=sxy+sx; IndGradKernel[53]=sxy-sx+1; 
			
			
			// gradient kernel
			GradKernel[0]=-1.0f/scale; GradKernel[1]=-2.0f/scale; GradKernel[2]=-1.0f/scale;
			GradKernel[3]=1.0f/scale; GradKernel[4]=2.0f/scale; GradKernel[5]=1.0f/scale;
	
			GradKernel[6]=-2.0f/scale; GradKernel[7]=-4.0f/scale; GradKernel[8]=-2.0f/scale;
			GradKernel[9]=2.0f/scale; GradKernel[10]=4.0f/scale; GradKernel[11]=2.0f/scale;
	
			GradKernel[12]=-1.0f/scale; GradKernel[13]=-2.0f/scale; GradKernel[14]=-1.0f/scale;
			GradKernel[15]=1.0f/scale; GradKernel[16]=2.0f/scale; GradKernel[17]=1.0f/scale;

		}

		
		return 0;
	}
	
/*	template <class ImgType, int NDim>
	int DiffusionBaseFilter<ImgType,NDim>::splitKernelIndex(int ind, int sx,int sxy, int &d, int &dx, int &dxy)
	{
		int tmp;
		
		
		return 0;
	}
*/
	
	template <class ImgType, int NDim>
	int DiffusionBaseFilter<ImgType,NDim>::SaveSlice(kipl::base::TImage<ImgType,NDim> & img, int i, const string &suffix)
	{

//		if (!iteration_filename.empty()) {
//
//			img.ExtractSlice(slice,plane_XY,img.SizeZ()/2);
//			ostringstream fname,vname;
//			fname<<iteration_filename<<i<<"."<<suffix<<".mat";
//			vname<<suffix<<i;
//			WriteMAT(slice,fname.str().c_str(),vname.str().c_str());
//		}
    return 0;
		
	}
}
}
#endif
