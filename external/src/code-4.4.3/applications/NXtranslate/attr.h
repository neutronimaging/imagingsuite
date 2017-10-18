#ifndef __ATTR_H_GUARD
#define __ATTR_H_GUARD

#include <vector>
#include <string>
#include <napiconfig.h>
#include <napi.h>
#include <iostream>

// ==================== Attribute definition
class Attr{
 public:
  Attr(const std::string name, const void* val,const int len, const int type);
  Attr(const Attr&);
  ~Attr();
  Attr& operator=(const Attr&);

  std::string name();
  void *value();
  int length();
  int type();

 private:
  std::string __name;
  void *_value;
  int __length;
  int __type;
};
#endif
