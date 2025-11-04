#if !defined(SORTALG_HPP)
#define SORTALG_HPP

#include <vector>
#include <algorithm>
#include <numeric>   // std::iota
#include <iterator>  // std::distance

namespace kipl {
namespace algorithms {
template <class It>
std::vector<size_t> arg_sort(const It& begin, const It& end)
{
    std::vector<size_t> indices(std::distance(begin, end));
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
              [&begin](size_t a, size_t b) { return begin[a] < begin[b]; });
    return indices;
}

}}
#endif
