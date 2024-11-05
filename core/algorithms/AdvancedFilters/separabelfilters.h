

#ifndef __SEPARABLE_FILTERS_H_
#define __SEPARABLE_FILTERS_H_

#include "filter.h"

namespace Filter {

	/** \brief Implementation of a Gaussian filter
		
		The filter uses separability to enhance the performance
	*/
	template<class ImgType, int NDim>
	class CGaussianFilter : public CBaseFilter<ImgType,NDim> 
	{
		public:
			/// \brief Default constructor that creates a Gaussian with sigma=1.0
			CGaussianFilter();
			/**  \brief Parametric constructor that creates a Gaussian with width sigma
			
				\param sigma The width of the filter
			*/
			CGaussianFilter(double sigma);
			/// \brief Returns the length of the filter
			int KernelSize();
			
			/** \brief Operator call to the filter
				
				\param img the image to be filtered
				
				\return A filtered image
			*/
			CImage<double,NDim> operator*(CImage<ImgType,NDim> & img);
			int operator()(CImage<ImgType,NDim> &img);
			virtual ~CGaussianFilter();
		private:
			int CreateIndx(int imgx, int imgy=0);
			int CreateKernel(double sigma);
			int ProcessLine(double **pImg,double **pTmp, int *pIndex, int x_start, int x_stop);
			int ProcessLine2(ImgType **pImg,ImgType **pTmp, int *pIndx, int x_start, int x_stop);

			int ProcessSlice(CImage<double,NDim> &img, CImage<double,NDim> &tmp,int z,int d);
			double *kernel;
			double *edge_weight;
			double s;
			int length;
			int nkernel;
			
	
	};

//-----------------------------------------------------------
// Implementation
	template<class ImgType, int NDim>
	CGaussianFilter<ImgType,NDim>::CGaussianFilter()
	{
        kernel=nullptr;
        this->indx=nullptr;
        edge_weight=nullptr;
		CreateKernel(1.0);
	} 
	
	template<class ImgType, int NDim>
	CGaussianFilter<ImgType,NDim>::CGaussianFilter(double sigma)
	{
        kernel=nullptr;
        this->indx=nullptr;
        edge_weight=nullptr;
		
		CreateKernel(sigma);		
	} 
	
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::KernelSize()
	{
		return length;
	}
			
	template<class ImgType, int NDim>
	CImage<double,NDim> CGaussianFilter<ImgType,NDim>::operator*(CImage<ImgType,NDim> & img)
	{
		const int *dims=img.getDimsptr();
		
		CImage<double,NDim> tmp(dims), res;
			
		res=img.toDouble();
		
		CreateIndx(dims[0],dims[1]);
				
		double *pTmp;
		double *pImg;
		
		int l2=length>>1;
		int x,y,z,i,d; 
		int x_start[3]={l2,0,0};
		int x_stop[3]={dims[0]-l2,dims[0],dims[0]};
		int y_start[3]={0,l2,0};
		int y_stop[3]={dims[1],dims[1]-l2,dims[1]};
		int z_start[3]={0,0,l2};
		int z_stop[3]={dims[2],dims[2],dims[2]-l2};
		
		
			
		int *pIndx=this->indx;
		double *pKernel;
		
		for (d=0; d<NDim; d++) {
			for (z=z_start[d]; z<z_stop[d]; z++) {
				if (d==1) { // Top edge for y-dim
					for (y=0; y<y_start[d]; y++) {
						pImg=res.getLineptr(y,z);
						pTmp=tmp.getLineptr(y,z);
						for (x=0; x<x_stop[d]; x++,pImg++,pTmp++) {
								*pTmp=(*pImg)*kernel[l2-y]/edge_weight[l2];;
								for (i=l2-y+1; i<length; i++) 
									*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[y+l2];
							}
					}
				}					
				
				for (y=y_start[d]; y<y_stop[d]; y++) {
					pImg=res.getLineptr(y,z);
					pTmp=tmp.getLineptr(y,z);
					if (d==0) { // left edge for x-dim
						for (x=0; x<x_start[d]; x++,pImg++,pTmp++) {
							*pTmp=(*pImg)*kernel[l2-x]/edge_weight[l2];
							for (i=l2-x+1; i<length; i++) 
								*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[x+l2];
							}
					}

					ProcessLine(&pImg,&pTmp,pIndx,x_start[d],x_stop[d]);
					
					if (d==0) {	// right edge for x-dim
						for (x=x_stop[d]; x<dims[d]; x++,pImg++,pTmp++) {
							*pTmp=(*pImg)*kernel[0]/edge_weight[l2];
							for (i=1; i<length-(l2-(dims[d]-x)+1); i++) 
								*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[length-l2+dims[d]-x-1];
							}
					}
				}
				if (d==1) { // Bottom edge for y-dim
					for (y=y_stop[d]; y<dims[d]; y++) {
						pImg=res.getLineptr(y,z);
						pTmp=tmp.getLineptr(y,z);
						for (x=x_start[d]; x<x_stop[d]; x++,pImg++,pTmp++) {
								*pTmp=(*pImg)*kernel[0]/edge_weight[l2];
								for (i=1; i<length-(l2-(dims[d]-y)+1); i++) 
									*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[length-l2+dims[d]-y-1];
							}
					}
				}					
				
			}	
			res=tmp;
			pIndx+=length;
		}
		
		return res;
	}
	
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::ProcessLine(double **pImg,double **pTmp, int *pIndx, int x_start, int x_stop)
	{
		int x,i;
		
		for (x=x_start; x<x_stop; x++,(*pImg)++,(*pTmp)++) {
			**pTmp=(**pImg)*kernel[0];
			for (i=1; i<length; i++) 
				**pTmp+=(*pImg)[pIndx[i]]*kernel[i];	
		}
		
		return x;
	}

	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::ProcessLine2(ImgType **pImg,ImgType **pTmp, int *pIndx, int x_start, int x_stop)
	{
		int x,i;
		
		for (x=x_start; x<x_stop; x++,(*pImg)++,(*pTmp)++) {
			**pTmp=(**pImg)*kernel[0];
			for (i=1; i<length; i++) 
				**pTmp+=(*pImg)[pIndx[i]]*kernel[i];	
		}
		
		return x;
	}
		 
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::ProcessSlice(CImage<double,NDim> &img, CImage<double,NDim> &tmp,int z,int d)
	{
	
		return 1;
	}
	
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::operator()(CImage<ImgType,NDim> &img)
	{
		const unsigned int *dims=img.getDimsptr();
		
		CImage<ImgType,NDim> tmp(dims), res;
			
		//res=img;
		
		CreateIndx(dims[0],dims[1]);
				
		ImgType *pTmp;
		ImgType *pImg;
		
		int l2=length>>1;
		int x,y,z,i,d; 
		int x_start[3]={l2,0,0};
		int x_stop[3]={dims[0]-l2,dims[0],dims[0]};
		int y_start[3]={0,l2,0};
		int y_stop[3]={dims[1],dims[1]-l2,dims[1]};
		int z_start[3]={0,0,l2};
		int z_stop[3]={dims[2],dims[2],dims[2]-l2};
		
		
			
		int *pIndx=this->indx;
		double *pKernel;
		
		for (d=0; d<NDim; d++) {
			for (z=z_start[d]; z<z_stop[d]; z++) {
				if (d==1) { // Top edge for y-dim
					for (y=0; y<y_start[d]; y++) {
						pImg=img.getLineptr(y,z);
						pTmp=tmp.getLineptr(y,z);
						for (x=0; x<x_stop[d]; x++,pImg++,pTmp++) {
								*pTmp=(*pImg)*kernel[l2-y]/edge_weight[l2];;
								for (i=l2-y+1; i<length; i++) 
									*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[y+l2];
							}
					}
				}					
				
				for (y=y_start[d]; y<y_stop[d]; y++) {
					pImg=img.getLineptr(y,z);
					pTmp=tmp.getLineptr(y,z);
					if (d==0) { // left edge for x-dim
						for (x=0; x<x_start[d]; x++,pImg++,pTmp++) {
							*pTmp=(*pImg)*kernel[l2-x]/edge_weight[l2];
							for (i=l2-x+1; i<length; i++) 
								*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[x+l2];
							}
					}

					ProcessLine2(&pImg,&pTmp,pIndx,x_start[d],x_stop[d]);
					
					if (d==0) {	// right edge for x-dim
						for (x=x_stop[d]; x<dims[d]; x++,pImg++,pTmp++) {
							*pTmp=(*pImg)*kernel[0]/edge_weight[l2];
							for (i=1; i<length-(l2-(dims[d]-x)+1); i++) 
								*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[length-l2+dims[d]-x-1];
							}
					}
				}
				if (d==1) { // Bottom edge for y-dim
					for (y=y_stop[d]; y<dims[d]; y++) {
						pImg=img.getLineptr(y,z);
						pTmp=tmp.getLineptr(y,z);
						for (x=x_start[d]; x<x_stop[d]; x++,pImg++,pTmp++) {
								*pTmp=(*pImg)*kernel[0]/edge_weight[l2];
								for (i=1; i<length-(l2-(dims[d]-y)+1); i++) 
									*pTmp+=pImg[pIndx[i]]*kernel[i]/edge_weight[length-l2+dims[d]-y-1];
							}
					}
				}					
				
			}	
			img=tmp;
			pIndx+=length;
		}
		
		return 1;
	}
			
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::CreateIndx(int imgx, int imgy)
	{
		int l2=length>>1;
		
		int i,j,k;
		int dims[3]={1,imgx,imgx*imgy};
		
		
		for (i=0; i<NDim; i++) {
			for (k=0,j=-l2; k<length; j++,k++) {
				this->indx[i*length+k]=j*dims[i];
			}

		}
		
		return 1;
	}
	
	template<class ImgType, int NDim>
	int CGaussianFilter<ImgType,NDim>::CreateKernel(double sigma)
	{
		length=int(ceil(sigma*3)+(1-fmod(sigma*3,2)));
		
		s=sigma;
		nkernel=length*NDim;
		
		if (this->kernel) delete [] this->kernel;
		kernel=new double[length];
		if (this->edge_weight) delete [] this->edge_weight;
		
		this->edge_weight=new double[length];
		
		if (this->indx) delete [] this->indx;
		this->indx=new int[length*NDim];
			
		int i,j;
		int l2=length>>1;
		
		double sum=0;
		
		for (i=0,j=-l2; i<length; i++,j++) {
			this->kernel[i]=exp(-double(j*j)/(2*s*s));
			sum+=this->kernel[i];
		}
		
		for (i=0; i<length; i++) 
			this->kernel[i]/=sum;
			
		this->edge_weight[0]=this->kernel[0];
		for (i=1; i<length; i++)
			this->edge_weight[i]=this->edge_weight[i-1]+this->kernel[i];
			
				
		
	
		
		return 1;
	}
	
	template<class ImgType, int NDim>
	CGaussianFilter<ImgType,NDim>::~CGaussianFilter()
	{
		if (this->kernel) delete [] this->kernel;
        this->kernel=nullptr;
		if (this->indx) delete [] this->indx;
        this->indx=nullptr;
		
		if (this->edge_weight) delete [] this->edge_weight;
        this->edge_weight=nullptr;
	}
}
#endif
