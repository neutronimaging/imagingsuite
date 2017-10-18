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


#ifndef INCLUDE_BASE_FWT2D_HPP
#define INCLUDE_BASE_FWT2D_HPP

#include <iostream>
#include <algorithm>
#include <limits>
#include <string>
#include <cmath>
#include <cassert>

#include "vec1d.hpp"
#include "wavelet_utils.hpp"


template<typename T>
class base_fwt2d
{
public:

   enum Status {
                 eOK                 = 0,
                 eOKSuppliedBuffer   = 1,
                 eFailedToOpenFilter = 2,
                 eFailedToLoadTH     = 3,
                 eFailedToLoadTG     = 4,
                 eFailedToLoadH      = 5,
                 eFailedToLoadG      = 6,
                 eUnknown            = 7
                };

   enum Band {
               eLL = 0,
               eHH = 1,
               eLH = 2,
               eHL = 3
             };


   base_fwt2d(const std::string& filter_file_name);

   base_fwt2d(const T* tH, unsigned int thL, int thZ,
              const T* tG, unsigned int tgL, int tgZ,
              const T* H,  unsigned int hL,  int hZ,
              const T* G,  unsigned int gL,  int gZ);

   virtual ~base_fwt2d();

   std::string status(Status& status) const;                                              //get status after constructor

   void init(const unsigned int& width, const unsigned int& height);                     //allocate buffers for transforms  status=1
   void set_coeffcient_bounds(const T& coefficient_lower_bound, const T& coefficient_upper_bound);

   void close(void);                                                                     //deallocate buffers

   inline unsigned int&     J()       { return J_;      }
   inline unsigned int  width() const { return width_;  }
   inline unsigned int height() const { return height_; }
   inline T*  spectrum()        const { return spec;    }
   inline T** spectrum2d()      const { return spec2d;  }


   //FWT transforms
   bool transform(const unsigned int& scales);                //forward transform data in spec buffer
   bool transform(const T* data, const unsigned int& scales); //forward transform DC corrected data

   bool synthesize();                                         //synthesis fwt spectrum in spec buffer
   bool synthesize(T* data);                                  //synthesis to DC corrected data buffer
   void display_filters();

   void LL_band_bound(                          unsigned int& x, unsigned int& y, unsigned int& width, unsigned int& height);
   void HH_band_bound(const unsigned int level, unsigned int& x, unsigned int& y, unsigned int& width, unsigned int& height);
   void LH_band_bound(const unsigned int level, unsigned int& x, unsigned int& y, unsigned int& width, unsigned int& height);
   void HL_band_bound(const unsigned int level, unsigned int& x, unsigned int& y, unsigned int& width, unsigned int& height);

   void set_band(const Band& band, const unsigned int& level, const T& value);
   void apply_threshold(const Band& band, const unsigned int& level, const T& lower_threshold, const T& upper_threshold);
   unsigned int band_coefficient_count(const Band& band, const unsigned int& level);

   void set_band_all_levels(const Band& band, const T& value);
   void apply_threshold_all_levels(const Band& band, const T& lower_threshold, const T& upper_threshold);
   unsigned int band_coefficient_count_all_levels(const Band& band);

protected:

   inline const vec1D<T>& tH   () const { return *tH_;   }
   inline const vec1D<T>& tG   () const { return *tG_;   }
   inline const vec1D<T>& H_2m () const { return *H2m_;  }
   inline const vec1D<T>& G_2m () const { return *G2m_;  }
   inline const vec1D<T>& H_2m1() const { return *H2m1_; }
   inline const vec1D<T>& G_2m1() const { return *G2m1_; }
   inline unsigned int    TH   () const { return TH_;    }

   virtual void transform_rows (T** dest, T** sour, const T& w, const T& h) const = 0;
   virtual void transform_cols (T** dest, T** sour, const T& w, const T& h) const = 0;
   virtual void synthesize_rows(T** dest, T** sour, const T& w, const T& h) const = 0;
   virtual void synthesize_cols(T** dest, T** sour, const T& w, const T& h) const = 0;

   T coefficient_upper_bound_;
   T coefficient_lower_bound_;

private:

   Status status_;   //constructor status 0 - not initilized,
                     //1 - class allocated its own buffers, separate for spec and tspec
                     //2 - class was supplied with buffers for spec and tspec

   vec1D<T>* tH_;                     //transform filters
   vec1D<T>* tG_;
   vec1D<T>* H_;                      //synthesis filters (never used directly)
   vec1D<T>* G_;
   vec1D<T>* H2m_;                    //even 2m coeffs from H,G filters
   vec1D<T>* G2m_;
   vec1D<T>* H2m1_;                   //odd 2m+1 coeffs from H,G filters
   vec1D<T>* G2m1_;

   unsigned int J_;                   //number of scales
   unsigned int TH_;                  //threshold for denoising

   unsigned int width_;               //array width
   unsigned int height_;              //array height
   T* spec;                           //2*width*height fwt spectrum
   T** spec2d;                        //spec 2D version with addreses from spec
   T** tspec2d;                       //tspec 2D version with addreses from spec

   void makeHGsynth(void);            //create 2m,2m1 filters from H,G   -2,-1,0,1,2,3

   //not intended functions
   base_fwt2d(const base_fwt2d& basefwt);
   const base_fwt2d& operator=(const base_fwt2d& basefwt);
};

template<typename T>
base_fwt2d<T>::base_fwt2d(const std::string& filter_file_name)
: coefficient_upper_bound_(0.0),
  coefficient_lower_bound_(0.0),
  status_(eUnknown),
  tH_(0),
  tG_(0),
  H_(0),
  G_(0),
  H2m_(0),
  G2m_(0),
  H2m1_(0),
  G2m1_(0),
  J_(0),
  TH_(0),
  width_(0),
  height_(0),
  spec(0),
  //tspec(0),
  spec2d(0),
  tspec2d(0)
{
   wfl_result result = wflUnknown;
   if (wflOK == (result = wavelet_filter_loader<T>(filter_file_name,&tH_,&tG_,&H_,&G_)))
   {
      makeHGsynth();
      status_ = eOK;

      for (int m = tH_->begin(); m <= tH_->end(); m++) { assert(  (-1.0 <= (*tH_)[m])  &&   ((*tH_)[m] <= 1.0)); }
      for (int m = tG_->begin(); m <= tG_->end(); m++) { assert( (-1.0 <= (*tG_)[m]) &&  ((*tH_)[m] <= 1.0)); }
      for (int m = H_->begin();  m <= H_->end();  m++) { assert( (-1.0 <= (*H_)[m])  &&  ((*H_)[m]  <= 1.0)); }
      for (int m = G_->begin();  m <= G_->end();  m++) { assert( (-1.0 <= (*G_)[m])  &&  ((*G_)[m]  <= 1.0)); }
   }
   else
   {
      switch(result)
      {
         case wflFileNotFound : status_ = eFailedToOpenFilter; return;
         case wflTHFailed     : status_ = eFailedToLoadTH;     return;
         case wflTGFailed     : status_ = eFailedToLoadTG;     return;
         case wflHFailed      : status_ = eFailedToLoadH;      return;
         case wflGFailed      : status_ = eFailedToLoadG;      return;
         default              : status_ = eUnknown;            return;
      }
   }
}

template<typename T>
base_fwt2d<T>::base_fwt2d(const T* tH, unsigned int thL, int thZ,
                          const T* tG, unsigned int tgL, int tgZ,
                          const T* H,  unsigned int hL,  int hZ,
                          const T* G,  unsigned int gL,  int gZ)
: coefficient_upper_bound_(0.0),
  coefficient_lower_bound_(0.0),
  status_(0),
  tH_(0),
  tG_(0),
  H_(0),
  G_(0),
  H2m_(0),
  G2m_(0),
  H2m1_(0),
  G2m1_(0),
  J_(0),
  TH_(0),
  width_(0),
  height_(0),
  spec(0),
  spec2d(0),
  tspec2d(0)
{
   tH_ = new vec1D<T>(thL, -thZ, tH);
   tG_ = new vec1D<T>(tgL, -tgZ, tG);
   H_  = new vec1D<T>( hL,  -hZ,  H);
   G_  = new vec1D<T>( gL,  -gZ,  G);
   makeHGsynth();
}

template<typename T>
base_fwt2d<T>::~base_fwt2d()
{
   if (tH_  ) delete tH_;
   if (tG_  ) delete tG_;
   if (H_   ) delete H_;
   if (G_   ) delete G_;
   if (H2m_ ) delete H2m_;
   if (G2m_ ) delete G2m_;
   if (H2m1_) delete H2m1_;
   if (G2m1_) delete G2m1_;
   close();                //close spec,tspec,spec2d,tspec2d buffers
}

template<typename T>
void base_fwt2d<T>::set_coeffcient_bounds(const T& coefficient_lower_bound, const T& coefficient_upper_bound)
{
   coefficient_lower_bound_ = coefficient_lower_bound;
   coefficient_upper_bound_ = coefficient_upper_bound;
}

template<typename T>
std::string base_fwt2d<T>::status(Status& status_value) const
{
   status_value = status_;
   switch (status_)
   {
      case eOK                : return std::string("OK");
      case eOKSuppliedBuffer  : return std::string("OK - User supplied buffer");
      case eFailedToOpenFilter: return std::string("ERROR - Failed to open filter file");
      case eFailedToLoadTH    : return std::string("ERROR - Failed to load tH filter");
      case eFailedToLoadTG    : return std::string("ERROR - Failed to load tG filter");
      case eFailedToLoadH     : return std::string("ERROR - Failed to load H filter");
      case eFailedToLoadG     : return std::string("ERROR - Failed to load G filter");
      case eUnknown           : return std::string("ERROR - Unknown status");
      default                 : return std::string("ERROR - Invalid status");
   }
}

template<typename T>
void base_fwt2d<T>::makeHGsynth()
{
   int size2m    = 0;
   int size2m1   = 0;
   int offset2m  = 0;
   int offset2m1 = 0;

   //arrange H2m,H2m1
   for (int m = H_->begin(); m <= H_->end(); ++m)
   {
      if ((m % 2) == 0)
         size2m++;
      else
         size2m1++;
   }

   offset2m  = (H_->begin() - (H_->begin() % 2)) / 2;
   offset2m1 = (H_->begin() + (H_->begin() % 2)) / 2;

   H2m_  = new vec1D<T>(size2m,  offset2m);
   H2m1_ = new vec1D<T>(size2m1, offset2m1);

   for (int m = H2m_->begin(); m <= H2m_->end(); ++m)
   {
      (*H2m_)(m) = (*H_)(2 * m);
   }

   for (int m = H2m1_->begin(); m <= H2m1_->end(); ++m)
   {
      (*H2m1_)(m) = (*H_)(2 * m + 1);
   }

   size2m = 0;
   size2m1 = 0;
   //arrange G2m,G2m1
   for (int m = G_->begin(); m <= G_->end(); ++m)
   {
      if (m % 2)
         size2m1++;
      else
         size2m++;
   }
   offset2m  = (G_->begin() - (G_->begin() % 2)) / 2;
   offset2m1 = (G_->begin() + (G_->begin() % 2)) / 2;

   G2m_  = new vec1D<T>(size2m,  offset2m );
   G2m1_ = new vec1D<T>(size2m1, offset2m1);

   for (int m = G2m_->begin(); m <= G2m_->end(); ++m)
   {
      (*G2m_)(m)  = (*G_)(2 * m);
   }

   for (int m = G2m1_->begin(); m  <= G2m1_->end(); ++m)
   {
      (*G2m1_)(m) = (*G_)(2 * m + 1);
   }
}

template<typename T>
void base_fwt2d<T>::init(const unsigned int& width, const unsigned int& height)
{
   if ((width != width_) || (height != height_))
   {
	  std::cout<<"base_fwt2d::init w="<<width<<", h="<<height<<std::endl;
      close();
      width_ = width;
      height_ = height;

      spec = new T[2 * width * (height+2)];

      spec2d = new T*[2 * height+2];
      for (unsigned int j = 0; j < 2 * (height+1); ++j)
      {
         spec2d[j] = &spec[j * width];
      }

      tspec2d = &spec2d[height+1];

      status_ = eOK;
   }
}

template<typename T>
void base_fwt2d<T>::close(void)
{
   width_ = 0;
   height_ = 0;
   if (eOK == status_)
   {
      if (spec != 0)
      {
         delete[] spec;
         spec = 0;
      }
   }
   if (spec2d != 0)
   {
      delete[] spec2d;
      spec2d = 0;
   }
   status_ = eUnknown;
}

template<typename T>
bool base_fwt2d<T>::transform(const unsigned int& scales)
{
   if ((eOK != status_) && (eOKSuppliedBuffer != status_))
   {
      return false;
   }

   J_ = scales;
   T w = width_;
   T h = height_;

   for(unsigned int j = 0; j < J_; ++j)
   {
	//   std::clog<<"base rows"<<std::endl;
	  transform_rows(tspec2d, spec2d, w, h);
	//  std::clog<<"base cols"<<std::endl;
      transform_cols(spec2d, tspec2d, w, h);
    // std::clog<<"level "<<j<<" done"<<std::endl;
      w /= T(2.0);
      h /= T(2.0);
   }

   return true;
}

template<typename T>
bool base_fwt2d<T>::transform(const T* data, const unsigned int& scales)
{
   if ((eOK != status_) && (eOKSuppliedBuffer != status_))
   {
      return false;
   }

   //std::copy(data,data + (width_ * height_),spec);
   memcpy(spec,data,width_ * height_*sizeof(T));
   return transform(scales);
}

template<typename T>
bool base_fwt2d<T>::synthesize()
{
   if ((eOK != status_) && (eOKSuppliedBuffer != status_))
   {
      return false;
   }

   T w = (width_ /  (std::pow(T(2.0), T(1.0) * J_)));
   T h = (height_ / (std::pow(T(2.0), T(1.0) * J_)));

   unsigned int orignal_J = J_;
   while (J_ > 0)
   {
      synthesize_cols(tspec2d, spec2d, w, h);
      synthesize_rows(spec2d, tspec2d, w, h);
      w *= T(2.0);
      h *= T(2.0);
      --J_;
   }
   J_ = orignal_J;
   return true;
}

template<typename T>
bool base_fwt2d<T>::synthesize(T* data)
{
   if (synthesize())
   {
    //  std::copy(spec,spec + (width_ * height_),data);
	   memcpy(data,spec,width_*height_*sizeof(T));
      return true;
   }
   else
      return false;
}


template<typename T>
void base_fwt2d<T>::display_filters()
{
   printf("tH\n");
   for (int i = tH_->begin(); i <= tH_->end(); i++)
   printf(" %2d  %g\n", i, (*tH_)(i));
   printf("\ntG\n");
   for (int i = tG_->begin(); i <= tG_->end(); i++)
   printf(" %2d  %g\n", i, (*tG_)(i));
   printf("\nH\n");
   for (int i = H_->begin(); i <= H_->end(); i++)
   printf(" %2d  %g\n", i, (*H_)(i));
   printf("\nG\n");
   for (int i = G_->begin(); i <= G_->end(); i++)
   printf(" %2d  %g\n", i, (*G_)(i));
   printf("\n\nH2m\n");
   for (int i = H2m_->begin(); i <= H2m_->end(); i++)
   printf(" %2d  %g\n", 2*i, (*H2m_)(i));
   printf("\nH2m1\n");
   for (int i = H2m1_->begin(); i <= H2m1_->end(); i++)
   printf(" %2d  %g\n", 2*i + 1, (*H2m1_)(i));
   printf("\nG2m\n");
   for (int i = G2m_->begin(); i <= G2m_->end(); i++)
   printf(" %2d  %g\n", 2*i, (*G2m_)(i));
   printf("\nG2m1\n");
   for (int i = G2m1_->begin(); i <= G2m1_->end(); i++)
   printf(" %2d  %g\n", 2*i + 1, (*G2m1_)(i));
}

template<typename T>
void base_fwt2d<T>::LL_band_bound(unsigned int& x, unsigned int& y,
                                  unsigned int& width, unsigned int& height)
{
   width  = static_cast<unsigned int>((1.0 *  width_) / std::pow(2.0, 1.0 * J_));
   height = static_cast<unsigned int>((1.0 * height_) / std::pow(2.0, 1.0 * J_));
   x = 0;
   y = 0;
}

template<typename T>
void base_fwt2d<T>::HH_band_bound(const unsigned int level,
                                  unsigned int& x, unsigned int& y,
                                  unsigned int& width, unsigned int& height)
{
   double w = (1.0 *  width_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(w) - w) >= 0.5)
   {
      x = static_cast<unsigned int>(std::floor(w));
      width = static_cast<unsigned int>(std::ceil(w));
   }
   else
   {
      x = static_cast<unsigned int>(w);
      width = static_cast<unsigned int>(w);
   }

   double h = (1.0 * height_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(h) - h) >= 0.5)
   {
      y = static_cast<unsigned int>(std::floor(h));
      height = static_cast<unsigned int>(std::ceil(h));
   }
   else
   {
      y = static_cast<unsigned int>(h);
      height = static_cast<unsigned int>(h);
   }
}

template<typename T>
void base_fwt2d<T>::LH_band_bound(const unsigned int level,
                                  unsigned int& x, unsigned int& y,
                                  unsigned int& width, unsigned int& height)
{
   double w = (1.0 *  width_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(w) - w) >= 0.5)
   {
      x = static_cast<unsigned int>(std::floor(w));
      width = static_cast<unsigned int>(std::ceil(w));
   }
   else
   {
      x = static_cast<unsigned int>(w);
      width = static_cast<unsigned int>(w);
   }

   double h = (1.0 * height_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(h) - h) >= 0.5)
   {
      height = static_cast<unsigned int>(std::floor(h));
   }
   else
   {
      height = static_cast<unsigned int>(h);
   }
   y = 0;
}

template<typename T>
void base_fwt2d<T>::HL_band_bound(const unsigned int level,
                                  unsigned int& x, unsigned int& y,
                                  unsigned int& width, unsigned int& height)
{
   double w = (1.0 *  width_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(w) - w) >= 0.5)
   {
      width = static_cast<unsigned int>(std::floor(w));
   }
   else
   {
      width = static_cast<unsigned int>(w);
   }

   x = 0;

   double h = (1.0 * height_) / std::pow(2.0, 1.0 *(level + 1));
   if (std::abs(std::floor(h) - h) >= 0.5)
   {
      y = static_cast<unsigned int>(std::floor(h));
      height = static_cast<unsigned int>(std::ceil(h));
   }
   else
   {
      y = static_cast<unsigned int>(h);
      height = static_cast<unsigned int>(h);
   }
}

template<typename T>
void base_fwt2d<T>::set_band(const Band& band, const unsigned int& level, const T& value)
{
   unsigned int b_x = 0;
   unsigned int b_y = 0;
   unsigned int b_width = 0;
   unsigned int b_height = 0;

   switch(band)
   {
      case eHH : HH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLH : LH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eHL : HL_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLL : LL_band_bound(b_x,b_y,b_width,b_height); break;
      default : return;
   }

   for(unsigned int y = b_y; y < (b_y + b_height); ++y)
   {
      for(T* x_it = spec2d[y] + b_x; x_it != (spec2d[y] + b_x + b_width); ++x_it)
      {
          *x_it = value;
      }
   }
}

template<typename T>
unsigned int base_fwt2d<T>::band_coefficient_count(const Band& band, const unsigned int& level)
{
   unsigned int b_x = 0;
   unsigned int b_y = 0;
   unsigned int b_width = 0;
   unsigned int b_height = 0;
   switch(band)
   {
      case eHH : HH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLH : LH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eHL : HL_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLL : LL_band_bound(b_x,b_y,b_width,b_height); break;
      default : return 0;
   }
   return b_width * b_height;
}


template<typename T>
void base_fwt2d<T>::apply_threshold(const Band& band,
                                    const unsigned int& level,
                                    const T& lower_threshold, const T& upper_threshold)
{
   if (upper_threshold < lower_threshold)
   {
      return;
   }

   unsigned int b_x = 0;
   unsigned int b_y = 0;
   unsigned int b_width = 0;
   unsigned int b_height = 0;

   switch(band)
   {
      case eHH : HH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLH : LH_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eHL : HL_band_bound(level,b_x,b_y,b_width,b_height); break;
      case eLL : LL_band_bound(b_x,b_y,b_width,b_height); break;
      default : return;
   }

   for(unsigned int y = b_y; y < (b_y + b_height); ++y)
   {
      for(T* x_it = spec2d[y] + b_x; x_it != (spec2d[y] + b_x + b_width); ++x_it)
      {
         T coefficient = *x_it;
         if ((lower_threshold <= coefficient) || (coefficient <= lower_threshold))
         {
            coefficient = T(0.0);
         }

      }
   }
}

template<typename T>
void base_fwt2d<T>::set_band_all_levels(const Band& band, const T& value)
{
   for (unsigned int level = 0; level < J_; ++level)
   {
      set_band(band,level,value);
   }
}

template<typename T>
void base_fwt2d<T>::apply_threshold_all_levels(const Band& band,
                                               const T& lower_threshold, const T& upper_threshold)
{
   for (unsigned int level = 0; level < J_; ++level)
   {
      apply_threshold(band,level,lower_threshold,upper_threshold);
   }
}

template<typename T>
unsigned int base_fwt2d<T>::band_coefficient_count_all_levels(const Band& band)
{
   unsigned int count = 0;
   for (unsigned int level = 0; level < J_; ++level)
   {
      count += band_coefficient_count(band,level);
   }
   return count;
}

#endif
