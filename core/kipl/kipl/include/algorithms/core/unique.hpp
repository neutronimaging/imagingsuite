#if !defined(UNIQUE_HPP)
#define UNIQUE_HPP

#include <vector>
#include <algorithm>

namespace kipl {
namespace algorithms {
    
template<class T>
std::vector<T> unique_values(std::vector<T> v)
{
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    return v;
}

}}
#endif