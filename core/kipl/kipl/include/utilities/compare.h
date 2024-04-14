#ifndef COMPARE_H
#define COMPARE_H

#include <cmath>
#include <algorithm>

template <typename T>
bool fuzzyAbsoluteCompare(T a, T b, T epsilon)
{
    return std::abs(a - b) < epsilon;
}   

template <typename T>
bool fuzzyRelativeCompare(T a, T b, T epsilon)
{
    return std::abs(a - b) < epsilon * std::max(std::abs(a), std::abs(b));
}

#endif