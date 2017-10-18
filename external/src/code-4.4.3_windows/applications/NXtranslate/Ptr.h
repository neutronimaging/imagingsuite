#ifndef __GUARD_PTR_H
#define __GUARD_PTR_H

#include <stdexcept>

template <class T> class Ptr{
 public:
  Ptr(): p(0), refptr(new std::size_t(1)){ }
  Ptr(T* t): p(t), refptr(new std::size_t(1)){ }
  Ptr(const Ptr& h): p(h.p), refptr(h.refptr){
    ++(*refptr);
  }
  Ptr& operator=(const Ptr& rhs){
    ++*rhs.refptr;

    // free the left-hand side, destroying pointers if appropriate
    if(--*refptr==0){
      delete refptr;
      delete p;
    }

    // copy in values from the right-hand side
    refptr=rhs.refptr;
    p=rhs.p;
    return *this;
  }
  ~Ptr(){
    if(--*refptr==0){
      delete refptr;
      delete p;
    }
  }

  operator bool() const{ return p; }
  T& operator*() const{
    if(p)
      return *p;
    throw std::runtime_error("unbound Ptr");
  }
  T* operator->() const{
    if(p)
      return p;
    throw std::runtime_error("unbound Ptr");
  }

 private:
  T* p;
  std::size_t* refptr;
};

#endif
