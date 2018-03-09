#ifndef IO_SERIALIZECONTAINERS_H
#define IO_SERIALIZECONTAINERS_H
#include <fstream>
#include <iostream>
#include <string>

namespace kipl {
namespace io {

template<typename T>
void serializeContainer(T a, T b, std::string fname)
{
    std::ofstream f(fname.c_str());
    for (T it=a; it<b; ++it) {
        f<<(*it)<<std::endl;
     //   cout<<(*it)<<std::endl;
    }


}

}
}

#endif // IO_SERIALIZECONTAINERS_H
