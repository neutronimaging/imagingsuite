/*
(***************************************************************************)
(*                                                                         *)
(*                     Fast Wavelet Transform Library                      *)
(*                                                                         *)
(* Release Version 0.0.1                                                   *)
(* Copyright (c) 2007-2008 Yuriy Chesnokov                                 *)
(* Copyright (c) 2008 Arash Partow                                         *)
(* All Rights Reserved.                                                    *)
(*                                                                         *)
(* The Fast Wavelet Transform Library and all its components are supplied  *)
(* under the terms of the General Public License. The contents of the      *)
(* Fast Wavelet Transform Library and all its components may not be copied *)
(* or disclosed except in accordance with the terms of that agreement.     *)
(*                                                                         *)
(* URL: http://www.gnu.org/licenses/gpl.txt                                *)
(*                                                                         *)
(***************************************************************************)
*/


#ifndef INCLUDE_FWT2D_HPP
#define INCLUDE_FWT2D_HPP

#define HAVE_FWT_PADDING

#include "base_fwt2d.hpp"
#include "vec1d.hpp"
#include "wavelet_utils.hpp"
#include <fstream>

typedef enum ePadType {
	PadMirror=0,
	PadPeriodic,
	PadZero,
	PadSP0,
	PadSP1
};

template<typename T>
class fwt2d : public base_fwt2d<T>
{
public:
   fwt2d(const std::string& file_file_name)
   : base_fwt2d<T>(file_file_name), padtype(PadMirror) {}

   fwt2d(const T* _tH, unsigned int _thL, int _thZ,
         const T* _tG, unsigned int _tgL, int _tgZ,
         const T*  _H, unsigned int  _hL, int  _hZ,
         const T*  _G, unsigned int  _gL, int  _gZ)
   : base_fwt2d<T>(_tH, _thL, _thZ, _tG, _tgL, _tgZ, _H, _hL, _hZ, _G, _gL, _gZ), padtype(PadMirror){}

   void save_array(T *data, unsigned int len, char *fname) const
   {
	std::ofstream datafile(fname);

	for (unsigned int i=0; i<len; i++)
		datafile<<data[i]<<"\n";

   }

   ePadType padtype;
private:
   void pad_mirror(T * src, unsigned int nsrc, T * dst, unsigned int insert_pos) const
   {
	//   std::clog<<"mirror"<<std::endl;
	memcpy(dst+insert_pos,src,nsrc*sizeof(T));
	for (int i=0; i<insert_pos; i++) {
		dst[i]=src[insert_pos-i];
		dst[insert_pos+nsrc+i-1]=src[nsrc-1-i];
	}
   }

   void pad_periodic(T * src, unsigned int nsrc, T * dst, unsigned int insert_pos) const
   {
	 //  std::clog<<"periodic"<<std::endl;
	memcpy(dst+insert_pos,src,nsrc*sizeof(T));
	memcpy(dst,src+nsrc-insert_pos,insert_pos*sizeof(T));
	memcpy(dst+nsrc+insert_pos-1,src,insert_pos*sizeof(T));
   }

   void pad_zero(T * src, unsigned int nsrc, T * dst, unsigned int insert_pos) const
   {
	 //  std::clog<<"zero"<<std::endl;
	memcpy(dst+insert_pos,src,nsrc*sizeof(T));
	for (int i=0; i<insert_pos; i++) {
		dst[i]=0;
		dst[insert_pos+nsrc+i]=0;
	}
   }

   void pad_sp0(T * src, unsigned int nsrc, T * dst, unsigned int insert_pos) const
   {
	 //  std::clog<<"sp0"<<std::endl;
	memcpy(dst+insert_pos,src,nsrc*sizeof(T));
	int odd = nsrc & 1;
	if (odd)
		dst[insert_pos+nsrc]=src[nsrc-1];
	for (int i=0; i<insert_pos; i++) {
		dst[i]=src[0];
		dst[insert_pos+nsrc+i+odd]=src[nsrc-1];

	}
   }

   void pad_sp1(T * src, unsigned int nsrc, T * dst, unsigned int insert_pos) const
   {
	//   std::clog<<"sp1"<<std::endl;
	memcpy(dst+insert_pos,src,nsrc*sizeof(T));
	double k0=std::min((src[1]-src[0])/2.0,(src[3]-src[2])/2.0);
	double k1=std::min((src[nsrc-1]-src[nsrc-2])/2.0,(src[nsrc-3]-src[nsrc-4])/2.0);

	for (int i=0; i<insert_pos; i++) {
		dst[i]=src[0]+static_cast<T>(k0*(i-1-insert_pos));
		dst[insert_pos+nsrc+i]=src[nsrc-1]+static_cast<T>(k1*(i+1));
	}
   }

   T clamp(const T& t) const
   {
      if (t < base_fwt2d<T>::coefficient_lower_bound_)
         return base_fwt2d<T>::coefficient_lower_bound_;
      else if (t > base_fwt2d<T>::coefficient_upper_bound_)
         return base_fwt2d<T>::coefficient_upper_bound_;
      else
         return t;
   }

   void transform_rows(T** dest, T** sour, const T& width, const T& height) const
   {
	  //std::clog<<"t rows "<<std::endl;
      unsigned int w = static_cast<unsigned int>(width);
      unsigned int w2 = static_cast<unsigned int>(width / T(2.0));

      const vec1D<T>& tH = base_fwt2d<T>::tH();
      const vec1D<T>& tG = base_fwt2d<T>::tG();
      
      unsigned int filter_width=std::max(tH.size(),tG.size());

      T * row=new T[w+2*filter_width+16];
      memset(row,0,sizeof(T)*(w+2*filter_width+1));
      T * padded_row=row+filter_width;
      

      for (unsigned int y = 0; y < height; y++)
      {
    	  memset(dest[y],0,w*sizeof(T));
    	  switch (padtype) {
			  case PadMirror   : pad_mirror(sour[y],w,row,filter_width); break;
			  case PadPeriodic : pad_periodic(sour[y],w,row,filter_width); break;
			  case PadZero     : pad_zero(sour[y],w,row,filter_width); break;
			  case PadSP0      : pad_sp0(sour[y],w,row,filter_width); break;
			  case PadSP1      : pad_sp1(sour[y],w,row,filter_width); break;
			  default          : pad_mirror(sour[y],w,row,filter_width); break;
    	  }

		  for (int m = tH.begin(); m <= tH.end(); m++)
		  {
			  for (int k = 0; k < width/T(2.0); k++)
			  {
				  dest[y][k] += tH[m] * padded_row[m+(2*k)];
			  }
		  }

		  for (int m = tG.begin(); m <= tG.end(); m++)
		  {
			  for (int k = 0; k < width/T(2.0); k++)
			  {
				  dest[y][k+w2] += tG[m] * padded_row[m+(2*k)];
			  }
		  }

      }

      delete [] row;

   }



   void unpad_row(T * src, T * dst, unsigned int nsrc, unsigned int insert_pos) 
   {
	   memcpy(dst,src+insert_pos,nsrc*sizeof(T));
   }


   void get_col(T ** src, T* col, unsigned int xpos, unsigned int ypos, T len) const
   {
	   for (unsigned int c=0; c<len; c++)
		   col[c]=src[c+ypos][xpos];
   }

   void set_col(T* col, T ** dst, unsigned int xpos, unsigned int ypos, T len, unsigned int step=1) const
   {
	   if (step==1)
		   for (unsigned int c=0; c<len; c++)
			   dst[c+ypos][xpos]=col[c];
	   else
		   for (unsigned int c=0; c<len; c++)
		   	   dst[step*c+ypos][xpos]=col[c];
   }

   void transform_cols(T** dest, T** sour, const T& width, const T& height) const
   {
	//  std::clog<<"cols"<<std::endl;
      const vec1D<T>& tH = base_fwt2d<T>::tH();
      const vec1D<T>& tG = base_fwt2d<T>::tG();

      unsigned int h = static_cast<unsigned int>(height);
      unsigned int h2 = static_cast<unsigned int>(height / T(2.0));
   
      unsigned int filter_width=std::max(tH.size(),tG.size());

      T * col=new T[h+2*filter_width+16];
      T * dstcol=new T[h+1];
      
      T * padded_col=col+filter_width;
      

      for (unsigned int x = 0; x < width; x++)
      {
	get_col(sour,dstcol,x,0,height);

	switch (padtype) {
	 case PadMirror   : pad_mirror(dstcol,h,col,filter_width);   break;
	 case PadPeriodic : pad_periodic(dstcol,h,col,filter_width); break;
	 case PadZero     : pad_zero(dstcol,h,col,filter_width); break;
	 case PadSP0      : pad_sp0(dstcol,h,col,filter_width); break;
	 case PadSP1      : pad_sp1(dstcol,h,col,filter_width); break;
	 default : pad_mirror(dstcol,h,col,filter_width);   break;
	}

	memset(dstcol,0,h*sizeof(T));
	for (int m = tH.begin(); m <= tH.end(); m++)
        { 
      	  for (int k = 0; k < ceil(height/T(2.0)); k++)
          {
		dstcol[k] += tH[m] * padded_col[m+(2*k)];
	  }
	}

	set_col(dstcol,dest,x,0,(height/T(2.0)));
	
	memset(dstcol,0,h*sizeof(T));
	for (int m = tG.begin(); m <= tG.end(); m++)
        {
      	  for (int k = 0; k < ceil(height/T(2.0)); k++)
          {
		dstcol[k] += tG[m] * padded_col[m+(2*k)];
	  }
	}

 	set_col(dstcol,dest,x,ceil(height/T(2.0)),(height/T(2.0)));
      }
      delete [] col;
      delete [] dstcol;

   }

   void synthesize_rows(T** dest, T** sour, const T& width, const T& height) const
   {
	 // std::clog<<"synth rows"<<std::endl;
	  const vec1D<T>& H2m = base_fwt2d<T>::H_2m();
      const vec1D<T>& G2m = base_fwt2d<T>::G_2m();
      const vec1D<T>& H2m1 = base_fwt2d<T>::H_2m1();
      const vec1D<T>& G2m1 = base_fwt2d<T>::G_2m1();

      unsigned int w = static_cast<unsigned int>(width);
      T w2 = (width * T(2.0));

      unsigned int filter_width=std::max(std::max(H2m.size(),G2m.size()), std::max(H2m1.size(),G2m1.size()));

      T * trend_row            = new T[w+2*filter_width+16];
      T * trend_row_padded     = trend_row + filter_width;

      T * variation_row        = new T[w+2*filter_width+16];
      T * variation_row_padded = variation_row + filter_width;

      for (unsigned int y = 0; y < (2 * height); y++)
      {
    	  memset(dest[y],0,static_cast<unsigned int>(ceil(w2))*sizeof(T));

    	  switch (padtype) {
			  case PadMirror   :
				pad_mirror(sour[y],w,trend_row,filter_width);
				pad_mirror(sour[y]+w,w,variation_row,filter_width);
				break;
			  case PadPeriodic :
				pad_periodic(sour[y],w,trend_row,filter_width);
				pad_periodic(sour[y]+w,w,variation_row,filter_width);
				break;
			  case PadZero :
			  	pad_zero(sour[y],w,trend_row,filter_width);
			  	pad_zero(sour[y]+w,w,variation_row,filter_width);
			  	break;
			  case PadSP0 :
			  	pad_sp0(sour[y],w,trend_row,filter_width);
			  	pad_sp0(sour[y]+w,w,variation_row,filter_width);
			  	break;
			  case PadSP1 :
			  	pad_sp1(sour[y],w,trend_row,filter_width);
			  	pad_sp1(sour[y]+w,w,variation_row,filter_width);
			  	break;

			  default :
				pad_mirror(sour[y],w,trend_row,filter_width);
				pad_mirror(sour[y]+w,w,variation_row,filter_width);
				break;
    	  }

    	  for (int m = H2m.begin(); m <= H2m.end(); m++) {
    		  for (int k = 0; k < width; k++)
    			  dest[y][2 * k] += trend_row_padded[k-m]* H2m[m];
    	  }

    	  for (int m = G2m.begin(); m <= G2m.end(); m++) {
    		  for (int k = 0; k < width; k++)
    			  dest[y][2 * k] += variation_row_padded[k-m]* G2m[m];
    	  }

    	  for (int m = H2m1.begin(); m <= H2m1.end(); m++) {
    		  for (int k = 0; k < width; k++)
    			  dest[y][2 * k+1] += trend_row_padded[k-m]* H2m1[m];
    	  }

    	  for (int m = G2m1.begin(); m <= G2m1.end(); m++) {
    		  for (int k = 0; k < width; k++)
    			  dest[y][2 * k+1] += variation_row_padded[k-m]* G2m1[m];
    	  }

    	  for (int k = 0; k < w2; k++)
    		  dest[y][k] *= 2.0;

      }
   }

   void synthesize_cols(T** dest, T** sour, const T& width, const T& height) const
   {
      const vec1D<T>& H2m = base_fwt2d<T>::H_2m();
      const vec1D<T>& G2m = base_fwt2d<T>::G_2m();
      const vec1D<T>& H2m1 = base_fwt2d<T>::H_2m1();
      const vec1D<T>& G2m1 = base_fwt2d<T>::G_2m1();

      unsigned int h = static_cast<unsigned int>(height);
      T h2 = (height * T(2.0));

      unsigned int filter_width=std::max(std::max(H2m.size(),G2m.size()), std::max(H2m1.size(),G2m1.size()));

      T * trend_col            = new T[h+2*filter_width+1];
      T * trend_col_padded     = trend_col + filter_width;

      T * variation_col        = new T[h+2*filter_width+1];
      T * variation_col_padded = variation_col + filter_width;

      T * dstcol = new T[h+1];

      for (int x = 0; x < (2.0 * width); x++)
      {
    	  get_col(sour, dstcol, x, 0, height);
		  switch (padtype) {
			  case PadMirror   :
				pad_mirror(dstcol,h,trend_col,filter_width);
				break;
			  case PadPeriodic :
				pad_periodic(dstcol,h,trend_col,filter_width);
				break;
			  case PadZero :
				pad_zero(dstcol,h,trend_col,filter_width);
				break;
			  case PadSP0 :
				pad_sp0(dstcol,h,trend_col,filter_width);
				break;
			  case PadSP1 :
				pad_sp1(dstcol,h,trend_col,filter_width);
				break;
			  default :
				pad_mirror(dstcol,h,trend_col,filter_width);
				break;
		  }

		  get_col(sour, dstcol, x, h, height);
		  switch (padtype) {
			  case PadMirror   :
				pad_mirror(dstcol,h,variation_col,filter_width);
				break;
			  case PadPeriodic :
				pad_periodic(dstcol,h,variation_col,filter_width);
				break;
			  case PadZero :
				pad_zero(dstcol,h,variation_col,filter_width);
				break;
			  case PadSP0 :
				pad_sp0(dstcol,h,variation_col,filter_width);
				break;
			  case PadSP1 :
				pad_sp1(dstcol,h,variation_col,filter_width);
				break;
			  default :
				pad_mirror(dstcol,h,variation_col,filter_width);
				break;
		  }

		  // Even entries
		  memset(dstcol,0,h*sizeof(T));
    	  for (int m=H2m.begin(); m<=H2m.end(); m++) {
    		  for (int k = 0; k < height - 0.9; k++) {
    			  dstcol[k] += H2m[m] * trend_col_padded[k-m];
    		  }
    	  }

    	  for (int m=G2m.begin(); m<=G2m.end(); m++) {
    	      for (int k = 0; k < height - 0.9; k++) {
    		  	  dstcol[k] += G2m[m] * variation_col_padded[k-m];
			  }
		  }

    	  for (int k = 0; k < height; k++) {
    		  dstcol[k]*=T(2.0);
    	  }

    	  set_col(dstcol, dest, x, 0, height, 2);

    	  // Odd entries
   		  memset(dstcol,0,h*sizeof(T));
       	  for (int m=H2m1.begin(); m<=H2m1.end(); m++) {
       		  for (int k = 0; k < height - 0.9; k++) {
       		  	  dstcol[k] += H2m1[m] * trend_col_padded[k-m];
       		  }
       	  }

       	  for (int m=G2m1.begin(); m<=G2m1.end(); m++) {
       		  for (int k = 0; k < height - 0.9; k++) {

   				  dstcol[k] += G2m1[m] * variation_col_padded[k-m];
   			  }
   		  }

       	  for (int k = 0; k < height; k++) {
       		  dstcol[k]*=T(2.0);
       	  }

       	  set_col(dstcol, dest, x, 1, height, 2);
       }
   }
};

#endif
