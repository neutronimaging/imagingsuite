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


#ifndef INCLUDE_WAVELET_UTILS_HPP
#define INCLUDE_WAVELET_UTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <limits>

#include "vec1d.hpp"

namespace details
{
   template<typename T>
   inline vec1D<T>* load_filter_section(std::ifstream& stream)
   {
	   int L,Z;
      if (!stream)
      {
         return 0;
      }

      stream >> L >> Z;
	#undef max
      if (L == std::numeric_limits<int>::max())
      {
         std::cerr << "load_filter_section() - ERROR Failed to read L value." << std::endl;
         return 0;
      }

      if (Z == std::numeric_limits<int>::max())
      {
         std::cerr << "load_filter_section() - ERROR Failed to read Z value." << std::endl;
         return 0;
      }

      vec1D<T>* filter = new vec1D<T>(L,-Z);
      for (int i = filter->begin(); i <= filter->end(); i++)
      {
         int index;
         T tmp = std::numeric_limits<T>::max();
         stream >> index >> tmp;
         if (std::numeric_limits<T>::max() != tmp)
         {
            (*filter)(i) = tmp;
         }
         else
         {
            std::cerr << "load_filter_section() - ERROR Failed to read filter value " << i << std::endl;
            delete filter;
            return 0;
         }
      }
      return filter;
   }
}


enum wfl_result  {
                   wflUnknown      = 0,
                   wflOK           = 1,
                   wflFileNotFound = 2,
                   wflTHFailed     = 3,
                   wflTGFailed     = 4,
                   wflHFailed      = 5,
                   wflGFailed      = 6
                 };

template<typename T>
inline wfl_result wavelet_filter_loader(const std::string& file_name,
                                        vec1D<T>** tH, vec1D<T>** tG,
                                        vec1D<T>**  H, vec1D<T>**  G)
{
   std::ifstream stream(file_name.c_str());
   if (!stream)
   {
      return wflFileNotFound;
   }

   if (0 == (*tH = details::load_filter_section<T>(stream)))
   {
      std::cerr << "wavelet_filter_loader() : ERROR - Failed to load tH filter" << std::endl;
      return wflTHFailed;
   }

   if (0 == (*tG = details::load_filter_section<T>(stream)))
   {
      std::cerr << "wavelet_filter_loader() : ERROR - Failed to load tG filter" << std::endl;
      delete *tH;
      return wflTGFailed;
   }

   if (0 == (*H = details::load_filter_section<T>(stream)))
   {
      std::cerr << "wavelet_filter_loader() : ERROR - Failed to load H filter" << std::endl;
      delete *tH;
      delete *tG;
      return wflHFailed;
   }

   if (0 == (*G = details::load_filter_section<T>(stream)))
   {
      std::cerr << "wavelet_filter_loader() : ERROR - Failed to load G filter" << std::endl;
      delete *tH;
      delete *tG;
      delete *H;
      return wflGFailed;
   }
   return wflOK;
}


#ifdef WIN32
   #include <windows.h>
   #undef max
   #undef min
#else
  #include <sys/time.h>
  #include <sys/types.h>
#endif

#ifdef WIN32

   class timer
   {
   public:

      timer()      { QueryPerformanceFrequency(&clock_frequency); }
      void start() { QueryPerformanceCounter(&start_time);        }
      void stop()  { QueryPerformanceCounter(&stop_time);         }

      double time()
      {
         return (1.0 *(stop_time.QuadPart - start_time.QuadPart)) / (1.0 * clock_frequency.QuadPart);
      }

    private:

     LARGE_INTEGER start_time;
     LARGE_INTEGER stop_time;
     LARGE_INTEGER clock_frequency;
   };

   #else

   class timer
   {
   public:

      void start() { gettimeofday(&start_time, 0); }
      void stop()  { gettimeofday(&stop_time,  0); }

      double time()
      {
         double diff = (stop_time.tv_sec - start_time.tv_sec) * 1000000.0;
         if (stop_time.tv_usec > start_time.tv_usec)
            diff += (1.0 * (stop_time.tv_usec - start_time.tv_usec));
         else if (stop_time.tv_usec < start_time.tv_usec)
            diff -= (1.0 * (start_time.tv_usec - stop_time.tv_usec));

         return (diff / 1000000.0);
      }

    private:

     struct timeval start_time;
     struct timeval stop_time;
     struct timeval clock_frequency;
   };

   #endif

#endif
