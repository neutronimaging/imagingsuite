//<LICENSE>

#ifndef STLMORPHOLOGY_H
#define STLMORPHOLOGY_H
#include <vector>
template <typename T>
std::vector<T> binaryDilation(const std::vector<T> &x, size_t iterations=1)
{
    std::vector<T> result(x.size(),static_cast<T>(0));
    std::vector<T> xx=x;


    for (size_t i=0; i<iterations; ++i)
    {
        auto xit = xx.begin();
        result[0]=*std::max_element(xit,xit+2);
        for (size_t j=1; j<xx.size()-1; ++j, ++xit)
        {
            result[j]= *std::max_element(xit,xit+3);
        }
        result.back() = *std::max_element(xx.rbegin(),xx.rbegin()+2);
        xx=result;
    }

    return result;
}

#endif // STLMORPHOLOGY_H
