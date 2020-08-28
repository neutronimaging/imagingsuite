#ifndef IO_SERIALIZECONTAINERS_H
#define IO_SERIALIZECONTAINERS_H
#include <fstream>
#include <iostream>
#include <string>
#include <map>

namespace kipl {
namespace io {

template<typename T>
void serializeContainer(T a, T b, std::string fname)
{
    std::ofstream f(fname.c_str());
    for (T it=a; it!=b; ++it) {
        f<<(*it)<<std::endl;
    }
}

template<typename T, typename S>
void serializeMap(std::map<T,S> &m, std::string fname)
{
    std::ofstream f(fname.c_str());
    for (auto it=m.begin(); it!=m.end(); ++it) {
        f<<(it->first)<<"\t"<<(it->second)<<std::endl;
    }
}
}
}

#endif // IO_SERIALIZECONTAINERS_H
