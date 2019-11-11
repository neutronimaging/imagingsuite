#ifndef SAVITZKYGOLAYFILTER_H
#define SAVITZKYGOLAYFILTER_H

#include <vector>
namespace kipl {
namespace filters {
/// \brief Signal smoothing using the Savitzky-Golay filter
/// \test A unit test is implemented in tkiplfilter
template <typename T>
class SavitzkyGolayFilter
{
public:
    /// \brief enums to be used for the edge processing (not used in the current implementation)
    enum eSavGolInterp {sgMirror, sgConstant, sgNearest, sgInterp, sgWrap};

    /// \brief Empty constructor
    SavitzkyGolayFilter();

    /// \brief Applies the SavGol filter on the data represented in C style arrays
    /// \param signal
    /// \param N
    /// \param result
    /// \param order
    /// \param windowSize
    /// \note Not implemented.
    void operator()(T *signal, size_t N, T * result, int order, int windowSize);

    /// \brief Applies the SavGol filter on the data represented in STL vectors
    /// \param signal the signal to smooth
    /// \param windowLength length of the filter kernel
    /// \param polyOrder the polynomial order to fit
    /// \param deriv order of the derivative
    /// \param delta derivative step size
    /// \param mode boundary processing style
    /// \param cval unused parameter. Intented as value of constant boundary filling
    std::vector<T> operator()(const std::vector<T> &signal, int window_length, int polyOrder, int deriv=0, double delta=1.0,eSavGolInterp mode = sgInterp, T cval=0.0);

    /// \brief Computes the SavGol filter coefficients
    /// \param windowLength length of the filter kernel
    /// \param polyOrder the polynomial order to fit
    /// \param deriv order of the derivative
    /// \param delta derivative step size
    /// \returns a vector with the SavGol coefficients
    std::vector<T> coeffs(int windowLength, int polyOrder, int deriv=0, double delta=1.0);
private:
    void initialize(int order, int windowSize);
    void fitEdge(const std::vector<T> &x, int windowStart, int windowStop, int interpStart, int interpStop,
                  int polyorder, int deriv, double delta, std::vector<T> &y);

    void fitEdgesPolyfit(const std::vector<T> &x, int windowLength, int polyOrder, int deriv, double delta, std::vector<T> &y);
};
}}

#include "core/savitzkygolayfilter.hpp"

#endif // SAVITZKYGOLAYFILTER_H
