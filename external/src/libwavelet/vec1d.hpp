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


#ifndef INCLUDE_VEC1D_HPP
#define INCLUDE_VEC1D_HPP

#include <iostream>

template<typename T>
class vec1D
{
public:

   vec1D(const unsigned int& size, const int& offset, const T* data = 0)
   : size_(size),
     offset_(offset),
     data_(0)
   {
      end_ = (size_ + offset_) - 1;
      data_ = new T[size_ * sizeof(T)];
      if (0 != data)
         for(unsigned int i = 0; i < size_; data_[i] = data[i],++i);
      else
         for(unsigned int i = 0; i < size_; data_[i] = T(0.0),++i);
      data_ -= offset_;
   }

   vec1D(const vec1D& v)
   : data_(0) { init(v); }

  ~vec1D() { close(); };

   inline int begin()         const { return offset_; }
   inline int end()           const { return end_;    }
   inline unsigned int size() const { return size_;   }

   inline T& operator()(int x)       { return data_[x]; }
   inline T  operator()(int x) const { return data_[x]; }
   inline T& operator[](int x)       { return data_[x]; }
   inline T  operator[](int x) const { return data_[x]; }

   inline const T* data()      const { return &data_[begin()]; }
   inline const T* data(int i) const { return &data_[i];       }

   inline const vec1D& operator=(const vec1D & v)
   {
      if (this != &v)
      {
         init(v);
      }
      return *this;
   }

   inline void display()
   {
      std::cout << "size: " << size_ << " offset: " << offset_ << std::endl;
      for(int i = offset_; i <= end_; ++i)
      {
         std::cout << i << " _ " << data_[i] << std::endl;
      }
   }

private:
   unsigned int size_;
   int offset_;
   T* data_;
   int end_;

   void init(const vec1D& v)
   {
      close();

      size_ = v.size();
      offset_ = v.begin();
      const T* data = v.data();

      end_ = (size_ + offset_) - 1;
      data_ = new T[size_ * sizeof(T)];

      for (unsigned int i = 0; i < size_; data_[i] = data[i], ++i);
      data_ -= offset_;
   }

   void close()
   {
      if (0 != data_)
      {
         data_ +=  offset_;
         delete[] data_;
      }
   }
};

#endif
