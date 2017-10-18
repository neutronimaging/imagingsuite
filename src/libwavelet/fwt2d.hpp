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

#include "base_fwt2d.hpp"
#include "vec1d.hpp"
#include "wavelet_utils.hpp"


template<typename T>
class fwt2d : public base_fwt2d<T>
{
public:
   fwt2d(const std::string& file_file_name)
   : base_fwt2d<T>(file_file_name) {}

   fwt2d(const T* _tH, unsigned int _thL, int _thZ,
         const T* _tG, unsigned int _tgL, int _tgZ,
         const T*  _H, unsigned int  _hL, int  _hZ,
         const T*  _G, unsigned int  _gL, int  _gZ)
   : base_fwt2d<T>(_tH, _thL, _thZ, _tG, _tgL, _tgZ, _H, _hL, _hZ, _G, _gL, _gZ) {}

private:

   T clamp(const T& t) const
   {
      if (t < base_fwt2d<T>::coefficient_lower_bound_)
         return base_fwt2d<T>::coefficient_lower_bound_;
      else if (t > base_fwt2d<T>::coefficient_upper_bound_)
         return base_fwt2d<T>::coefficient_upper_bound_;
      else
         return t;
   }

   inline void apply_row_transform(const vec1D<T>& filter,
                                   const unsigned int& y,
                                   const unsigned int& k,
                                   const unsigned int& dimension,
                                   T** sour,
                                   T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = 2 * k + m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension))
         {
            n -= 2 * (1 + n - dimension);
         }
         value += filter[m] * sour[y][n];
      }
   }

   void transform_rows(T** dest, T** sour, const T& width, const T& height) const
   {
      T s = T(0.0);
      T d = T(0.0);

      T w2 = width / T(2.0);

      const vec1D<T>& tH = base_fwt2d<T>::tH();
      const vec1D<T>& tG = base_fwt2d<T>::tG();

      for (unsigned int y = 0; y < height; y++)
      {
         for (unsigned int k = 0; k < w2; k++)
         {
            s = 0.0;
            d = 0.0;

            apply_row_transform(tH,y,k,static_cast<unsigned int>(width),sour,s);
            apply_row_transform(tG,y,k,static_cast<unsigned int>(width),sour,d);
            dest[y][k]      = s;
            dest[y][k + static_cast<unsigned int>(w2)] = d;
         }
      }
   }

   inline void apply_col_transform(const vec1D<T>& filter,
                                   const unsigned int& x,
                                   const unsigned int& k,
                                   const unsigned int& dimension,
                                   T** sour,
                                   T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = 2 * k + m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension))
         {
            n -= 2 * (1 + n - dimension);
         }
         value += filter[m] * sour[n][x];
      }
   }

   void transform_cols(T** dest, T** sour, const T& width, const T& height) const
   {
      T s = T(0.0);
      T d = T(0.0);
      T h2 = height / T(2.0);

      const vec1D<T>& tH = base_fwt2d<T>::tH();
      const vec1D<T>& tG = base_fwt2d<T>::tG();

      for (unsigned int x = 0; x < width; x++)
      {
         for (unsigned int k = 0; k < h2; k++)
         {
            s = T(0.0);
            d = T(0.0);

            apply_col_transform(tH,x,k,static_cast<unsigned int>(height),sour,s);
            apply_col_transform(tG,x,k,static_cast<unsigned int>(height),sour,d);

            dest[k][x] = s;
            dest[k + static_cast<unsigned int>(h2)][x] = d;
         }
      }
   }

   inline void apply_row_synthesize1(const vec1D<T>& filter,
                                     const unsigned int& y,
                                     const unsigned int& k,
                                     const unsigned int& dimension,
                                     T** sour,
                                     T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = k - m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension))
         {
            n -= 2 * (1 + n - dimension);
         }
         value += filter[m] * sour[y][n];
      }
   }

   inline void apply_row_synthesize2(const vec1D<T>& filter,
                                     const unsigned int& y,
                                     const unsigned int& k,
                                     const unsigned int& dimension,
                                     T** sour,
                                     T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = k - m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension))
         {
            n -= 2 * (1 + n - dimension);
         }
         value += filter[m] * sour[y][n + dimension];
      }
   }

   void synthesize_rows(T** dest, T** sour, const T& width, const T& height) const
   {
      T  s2k = T(0.0);
      T s2k1 = T(0.0);

      const vec1D<T>& H2m = base_fwt2d<T>::H_2m();
      const vec1D<T>& G2m = base_fwt2d<T>::G_2m();
      const vec1D<T>& H2m1 = base_fwt2d<T>::H_2m1();
      const vec1D<T>& G2m1 = base_fwt2d<T>::G_2m1();

      for (unsigned int y = 0; y < (2 * height); y++)
      {
         for (unsigned int k = 0; k < width; k++)
         {
            s2k = T(0.0);
            s2k1 = T(0.0);

            apply_row_synthesize1(H2m,y,k,static_cast<unsigned int>(width),sour,s2k);
            apply_row_synthesize2(G2m,y,k,static_cast<unsigned int>(width),sour,s2k);

            apply_row_synthesize1(H2m1,y,k,static_cast<unsigned int>(width),sour,s2k1);
            apply_row_synthesize2(G2m1,y,k,static_cast<unsigned int>(width),sour,s2k1);

            //dest[y][2 * k] = clamp(T(2.0) * s2k);
            //dest[y][(2 * k) + 1] = clamp(T(2.0) * s2k1);

            dest[y][2 * k]       = (T(2.0) * s2k);
            dest[y][(2 * k) + 1] = (T(2.0) * s2k1);

         }
      }
   }

   inline void apply_col_synthesize1(const vec1D<T>& filter,
                                     const unsigned int& x,
                                     const unsigned int& k,
                                     const unsigned int& dimension,
                                     T** sour,
                                     T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = k - m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension)) n -= 2 * (1 + n - dimension); // BUG "n" become -1
         value += filter[m] * sour[n][x];
      }
   }

   inline void apply_col_synthesize2(const vec1D<T>& filter,
                                     const unsigned int& x,
                                     const unsigned int& k,
                                     const unsigned int& dimension,
                                     T** sour,
                                     T& value) const
   {
      int n = 0;
      for (int m = filter.begin(); m <= filter.end(); ++m)
      {
         n = k - m;
         if (n < 0) n = 0 - n;
         if (n >= static_cast<int>(dimension)) n -= 2 * (1 + n - dimension); // BUG "n" become -1
         value += filter[m] * sour[n + dimension][x];
      }
   }

   void synthesize_cols(T** dest, T** sour, const T& width, const T& height) const
   {
      T s2k  = T(0.0);
      T s2k1 = T(0.0);

      const vec1D<T>& H2m = base_fwt2d<T>::H_2m();
      const vec1D<T>& G2m = base_fwt2d<T>::G_2m();
      const vec1D<T>& H2m1 = base_fwt2d<T>::H_2m1();
      const vec1D<T>& G2m1 = base_fwt2d<T>::G_2m1();

      for (unsigned int x = 0; x < (2.0 * width); x++)
      {
         for (unsigned int k = 0; k < height - 0.9; k++)
         {
            s2k = T(0.0);
            s2k1 = T(0.0);
            apply_col_synthesize1(H2m,x,k,static_cast<unsigned int>(height),sour,s2k);
            apply_col_synthesize2(G2m,x,k,static_cast<unsigned int>(height),sour,s2k);

            apply_col_synthesize1(H2m1,x,k,static_cast<unsigned int>(height),sour,s2k1);
            apply_col_synthesize2(G2m1,x,k,static_cast<unsigned int>(height),sour,s2k1);

            //dest[2 * k][x] = clamp(T(2.0) * s2k);
            //dest[(2 * k) + 1][x] = clamp(T(2.0) * s2k1);
            dest[2 * k][x]       = (T(2.0) * s2k);
            dest[(2 * k) + 1][x] = (T(2.0) * s2k1);

         }
      }
   }
};

#endif
