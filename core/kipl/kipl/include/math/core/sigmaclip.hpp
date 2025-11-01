#ifndef KIPL_MATH_SIGMACLIP_HPP
#define KIPL_MATH_SIGMACLIP_HPP

#include <vector>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <iostream>

namespace kipl { namespace math {
template<typename T>
std::tuple<std::vector<T>, double, double>
sigma_clip(const std::vector<T> &data, double low = 3.0, double high = 3.0)
{
    using real_t = double;

    // copy and drop NaNs (for floating types)
    std::vector<T> tmp;
    tmp.reserve(data.size());
    if constexpr (std::is_floating_point_v<T>) 
    {
        for (auto v : data) 
        {
            if (!std::isnan(static_cast<real_t>(v)))
                tmp.push_back(v);
        }
    } 
    else 
    {
        tmp = data;
    }

    if (tmp.empty()) return { {}, 0.0, 0.0 };

    std::size_t prev_size = tmp.size() + 1;
    while (prev_size != tmp.size() && !tmp.empty()) 
    {
        prev_size = tmp.size();

        // mean
        real_t sum = std::accumulate(tmp.begin(), tmp.end(), real_t{0},
            [](real_t acc, const T &v){ return acc + static_cast<real_t>(v); });
        real_t mean = sum / tmp.size();
        
        // sample standard deviation (N-1)
        real_t ss = std::accumulate(tmp.begin(), tmp.end(), real_t{0},
            [mean](real_t acc, const T &v){
                real_t d = static_cast<real_t>(v) - mean;
                return acc + d * d;
            });
        real_t var = (tmp.size() > 1) ? ss / (tmp.size() - 1) : 0.0;
        real_t stddev = std::sqrt(var);

        real_t lower = mean - low * stddev;
        real_t upper = mean + high * stddev;

        auto it = std::remove_if(tmp.begin(), tmp.end(),
                                 [lower, upper](const T &v) {
                                     real_t val = static_cast<real_t>(v);
                                     return (val < lower) || (val > upper);
                                 });
        tmp.erase(it, tmp.end());
    }

    // compute final bounds based on remaining data
    double final_lower = 0.0, final_upper = 0.0;
    if (!tmp.empty()) {
        double sum = 0.0;
        for (auto &v : tmp) sum += static_cast<double>(v);
        double mean = sum / tmp.size();

        double ss = 0.0;
        for (auto &v : tmp) {
            double d = static_cast<double>(v) - mean;
            ss += d * d;
        }
        double var = (tmp.size() > 1) ? ss / (tmp.size() - 1) : 0.0;
        double stddev = std::sqrt(var);
        final_lower = mean - low * stddev;
        final_upper = mean + high * stddev;
    }

    return { tmp, final_lower, final_upper };
}

}} // namespace kipl::math  

#endif // KIPL_MATH_SIGMACLIP_HPP