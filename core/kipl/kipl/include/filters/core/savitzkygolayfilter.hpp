//<LICENSE>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "../savitzkygolayfilter.h"
#include "../../base/KiplException.h"
#include "../../math/mathfunctions.h"
#include "../../stltools/stlvecmath.h"
#include <armadillo>
#include "../../math/linfit.h"

namespace kipl {
namespace filters {

template <typename T>
SavitzkyGolayFilter<T>::SavitzkyGolayFilter()
{
    static_assert(std::is_floating_point<T>::value,
                     "SavitzkyGolayFilter can only be instantiated with floating point types");

}

template<typename T>
void SavitzkyGolayFilter<T>::initialize(int /*order*/, int /*windowSize*/)
{

}

template<typename T>
void SavitzkyGolayFilter<T>::operator()(T * /*signal*/, size_t /*N*/, T * /*result*/, int /*order*/, int /*windowSize*/)
{

}

/// \brief Apply a Savitzky-Golay filter to an array.
/// This is a 1-d filter.  If `x`  has dimension greater than 1, `axis`
/// determines the axis along which the filter is applied.
/// Parameters
/// ----------
/// x : array_like
///     The data to be filtered.  If `x` is not a single or double precision
///     floating point array, it will be converted to type ``numpy.float64``
///     before filtering.
/// window_length : int
///     The length of the filter window (i.e. the number of coefficients).
///     `window_length` must be a positive odd integer. If `mode` is 'interp',
///     `window_length` must be less than or equal to the size of `x`.
/// polyorder : int
///     The order of the polynomial used to fit the samples.
///     `polyorder` must be less than `window_length`.
/// deriv : int, optional
///     The order of the derivative to compute.  This must be a
///     nonnegative integer.  The default is 0, which means to filter
///     the data without differentiating.
/// delta : float, optional
///     The spacing of the samples to which the filter will be applied.
///     This is only used if deriv > 0.  Default is 1.0.
/// cval : scalar, optional
///    Value to fill past the edges of the input if `mode` is 'constant'.
///     Default is 0.0.
/// Returns
/// -------
/// y : ndarray, same shape as `x`
///     The filtered data.
template <typename T>
std::vector<T> SavitzkyGolayFilter<T>::operator()(const std::vector<T> &signal,int windowLength, int polyOrder, int deriv, double delta,eSavGolInterp /*mode*/, T /*cval*/)
{
    if (polyOrder<0)
        throw kipl::base::KiplException("Negative poly orders are not valid",__FILE__,__LINE__);

    if (windowLength %2 == 0)
        throw kipl::base::KiplException("Even window length is not good",__FILE__,__LINE__);

    if (delta<=0.0)
        throw kipl::base::KiplException("The derivative step must be strict positive.",__FILE__,__LINE__);

    std::vector<T> result;

    auto cvec = coeffs(windowLength, polyOrder, deriv, delta);

    result = convolve1d(signal, cvec);
    fitEdgesPolyfit(signal, windowLength, polyOrder, deriv, delta, result);

    return result;
}

/// \brief Compute the coefficients for a 1-d Savitzky-Golay FIR filter.
/// \param window_length : The length of the filter window (i.e. the number of coefficients). `window_length` must be an odd positive integer.
/// \param polyorder : The order of the polynomial used to fit the samples. `polyorder` must be less than `window_length`.
/// \param deriv : (optional) The order of the derivative to compute.  This must be a nonnegative integer.  The default is 0, which means to filter the data without differentiating.
/// \param delta : (optional) The spacing of the samples to which the filter will be applied. This is only used if deriv > 0.
template <typename T>
std::vector<T> SavitzkyGolayFilter<T>::coeffs(int windowLength, int polyOrder, int deriv, double delta)
{
    std::vector<T> cvec(windowLength);

    if (polyOrder >= windowLength)
        throw kipl::base::KiplException("polyorder must be less than window_length.",__FILE__,__LINE__);

    auto res    = std::div(windowLength, 2);
    int halflen = res.quot;
    int rem     = res.rem;
    int pos     = halflen;

    if (rem == 0)
        throw kipl::base::KiplException("window_length must be odd.",__FILE__,__LINE__);

    if (windowLength<=pos)
        throw kipl::base::KiplException("pos must be nonnegative and less than window_length.",__FILE__,__LINE__);

    if (deriv > polyOrder)
    {
        std::fill(cvec.begin(),cvec.end(),static_cast<T>(0.0));
        return cvec;
    }

    // Form the design matrix A.  The columns of A are powers of the integers
    // from -pos to window_length - pos - 1.  The powers (i.e. rows) range
    // from 0 to polyorder.  (That is, A is a vandermonde matrix, but not
    // necessarily square.)
    arma::mat A(static_cast<arma::uword>(polyOrder+1),static_cast<arma::uword>(windowLength)) ;

    float x=-pos;
    for (arma::uword i=0; i<static_cast<arma::uword>(windowLength); ++i, ++x)
    {
        for (arma::uword order = 0 ; order<=static_cast<arma::uword>(polyOrder); ++order)
        {
            A(order,i)=std::pow(x,order);
        }
    }

    arma::vec y(static_cast<arma::uword>(polyOrder+1));

    y.fill(0.0);
    y(static_cast<arma::uword>(deriv))= kipl::math::factorial(deriv) / std::pow(delta,deriv);

    arma::vec c=arma::solve(A,y);

    for (arma::uword i=0; i<c.n_elem; ++i)
    {
        cvec[i]= c(i);
    }

    return cvec;
}

/// Given an n-d array `x` and the specification of a slice of `x` from
/// `window_start` to `window_stop` along `axis`, create an interpolating
/// polynomial of each 1-d slice, and evaluate that polynomial in the slice
/// from `interp_start` to `interp_stop`.  Put the result into the
/// corresponding slice of `y`.
template <typename T>
void SavitzkyGolayFilter<T>::fitEdge(const std::vector<T> &x, int windowStart, int windowStop, int interpStart, int interpStop,
              int polyOrder, int deriv, double delta, std::vector<T> &y)
{
    // Get the edge into a (window_length, -1) array.
    std::vector<T> xEdge(windowStop-windowStart);
    std::copy(x.begin()+windowStart, x.begin()+windowStop,xEdge.begin());
    std::vector<T> xaxis(xEdge.size());

    double xval=0;
    for (auto &xitem : xaxis)
        xitem = xval++;

    // Fit the edges.  poly_coeffs has shape (polyorder + 1, -1),
    // where '-1' is the same as in xx_edge.
    auto polyCoeffs = kipl::math::polyFit(xaxis, xEdge, polyOrder);

    if (deriv > 0)
        polyCoeffs = kipl::math::polyDeriv(polyCoeffs, deriv);

    // Compute the interpolated values for the edge.
    xaxis.clear();

    for (xval = interpStart - windowStart; xval<interpStop - windowStart; ++xval)
        xaxis.push_back(xval);

    auto values = kipl::math::polyVal(xaxis,polyCoeffs) ;

    for (auto &value : values)
            value = value/ std::pow(delta,deriv);

    std::copy(values.begin(),values.end(),y.begin()+interpStart);
}


///  Use polynomial interpolation of x at the low and high ends of the axis
///  to fill in the halflen values in y.
///  This function just calls _fit_edge twice, once for each end of the axis.
template <typename T>
void SavitzkyGolayFilter<T>::fitEdgesPolyfit(const std::vector<T> &x, int windowLength, int polyOrder, int deriv, double delta, std::vector<T> &y)
{
    int halflen = windowLength/2;
    fitEdge(x, 0, windowLength, 0, halflen, polyOrder, deriv, delta, y);
    int n = x.size();
    fitEdge(x, n - windowLength, n, n - halflen, n, polyOrder, deriv, delta, y);

}

}
}
/*
    >>> from scipy.signal import savgol_coeffs
    >>> savgol_coeffs(5, 2)
    array([-0.08571429,  0.34285714,  0.48571429,  0.34285714, -0.08571429])
    >>> savgol_coeffs(5, 2, deriv=1)
    array([ 2.00000000e-01,  1.00000000e-01,  2.07548111e-16, -1.00000000e-01,
           -2.00000000e-01])
    Note that use='dot' simply reverses the coefficients.
    >>> savgol_coeffs(5, 2, pos=3)
    array([ 0.25714286,  0.37142857,  0.34285714,  0.17142857, -0.14285714])
    >>> savgol_coeffs(5, 2, pos=3, use='dot')
    array([-0.14285714,  0.17142857,  0.34285714,  0.37142857,  0.25714286])
    `x` contains data from the parabola x = t**2, sampled at
    t = -1, 0, 1, 2, 3.  `c` holds the coefficients that will compute the
    derivative at the last position.  When dotted with `x` the result should
    be 6.
    >>> x = np.array([1, 0, 1, 4, 9])
    >>> c = savgol_coeffs(5, 2, pos=4, deriv=1, use='dot')
    >>> c.dot(x)
    6.0
    """

    # An alternative method for finding the coefficients when deriv=0 is
    #    t = np.arange(window_length)
    #    unit = (t == pos).astype(int)
    #    coeffs = np.polyval(np.polyfit(t, unit, polyorder), t)
    # The method implemented here is faster.

    # To recreate the table of sample coefficients shown in the chapter on
    # the Savitzy-Golay filter in the Numerical Recipes book, use
    #    window_length = nL + nR + 1
    #    pos = nL + 1
    #    c = savgol_coeffs(window_length, M, pos=pos, use='dot')

    if polyorder >= window_length:
        raise ValueError("polyorder must be less than window_length.")

    halflen, rem = divmod(window_length, 2)

    if rem == 0:
        raise ValueError("window_length must be odd.")

    if pos is None:
        pos = halflen

    if not (0 <= pos < window_length):
        raise ValueError("pos must be nonnegative and less than "
                         "window_length.")

    if use not in ['conv', 'dot']:
        raise ValueError("`use` must be 'conv' or 'dot'")

    if deriv > polyorder:
        coeffs = np.zeros(window_length)
        return coeffs

    # Form the design matrix A.  The columns of A are powers of the integers
    # from -pos to window_length - pos - 1.  The powers (i.e. rows) range
    # from 0 to polyorder.  (That is, A is a vandermonde matrix, but not
    # necessarily square.)
    x = np.arange(-pos, window_length - pos, dtype=float)
    if use == "conv":
        # Reverse so that result can be used in a convolution.
        x = x[::-1]

    order = np.arange(polyorder + 1).reshape(-1, 1)
    A = x ** order

    # y determines which order derivative is returned.
    y = np.zeros(polyorder + 1)
    # The coefficient assigned to y[deriv] scales the result to take into
    # account the order of the derivative and the sample spacing.
    y[deriv] = factorial(deriv) / (delta ** deriv)

    # Find the least-squares solution of A*c = y
    coeffs, _, _, _ = lstsq(A, y)

    return coeffs


def _polyder(p, m):
    """Differentiate polynomials represented with coefficients.
    p must be a 1D or 2D array.  In the 2D case, each column gives
    the coefficients of a polynomial; the first row holds the coefficients
    associated with the highest power.  m must be a nonnegative integer.
    (numpy.polyder doesn't handle the 2D case.)
    """

    if m == 0:
        result = p
    else:
        n = len(p)
        if n <= m:
            result = np.zeros_like(p[:1, ...])
        else:
            dp = p[:-m].copy()
            for k in range(m):
                rng = np.arange(n - k - 1, m - k - 1, -1)
                dp *= rng.reshape((n - m,) + (1,) * (p.ndim - 1))
            result = dp
    return result


def _fit_edge(x, window_start, window_stop, interp_start, interp_stop,
              axis, polyorder, deriv, delta, y):
    """
    Given an n-d array `x` and the specification of a slice of `x` from
    `window_start` to `window_stop` along `axis`, create an interpolating
    polynomial of each 1-d slice, and evaluate that polynomial in the slice
    from `interp_start` to `interp_stop`.  Put the result into the
    corresponding slice of `y`.
    """

    # Get the edge into a (window_length, -1) array.
    x_edge = axis_slice(x, start=window_start, stop=window_stop, axis=axis)
    if axis == 0 or axis == -x.ndim:
        xx_edge = x_edge
        swapped = False
    else:
        xx_edge = x_edge.swapaxes(axis, 0)
        swapped = True
    xx_edge = xx_edge.reshape(xx_edge.shape[0], -1)

    # Fit the edges.  poly_coeffs has shape (polyorder + 1, -1),
    # where '-1' is the same as in xx_edge.
    poly_coeffs = np.polyfit(np.arange(0, window_stop - window_start),
                             xx_edge, polyorder)

    if deriv > 0:
        poly_coeffs = _polyder(poly_coeffs, deriv)

    # Compute the interpolated values for the edge.
    i = np.arange(interp_start - window_start, interp_stop - window_start)
    values = np.polyval(poly_coeffs, i.reshape(-1, 1)) / (delta ** deriv)

    # Now put the values into the appropriate slice of y.
    # First reshape values to match y.
    shp = list(y.shape)
    shp[0], shp[axis] = shp[axis], shp[0]
    values = values.reshape(interp_stop - interp_start, *shp[1:])
    if swapped:
        values = values.swapaxes(0, axis)
    # Get a view of the data to be replaced by values.
    y_edge = axis_slice(y, start=interp_start, stop=interp_stop, axis=axis)
    y_edge[...] = values


def _fit_edges_polyfit(x, window_length, polyorder, deriv, delta, axis, y):
    """
    Use polynomial interpolation of x at the low and high ends of the axis
    to fill in the halflen values in y.
    This function just calls _fit_edge twice, once for each end of the axis.
    """
    halflen = window_length // 2
    _fit_edge(x, 0, window_length, 0, halflen, axis,
              polyorder, deriv, delta, y)
    n = x.shape[axis]
    _fit_edge(x, n - window_length, n, n - halflen, n, axis,
              polyorder, deriv, delta, y)


def savgol_filter(x, window_length, polyorder, deriv=0, delta=1.0,
                  axis=-1, mode='interp', cval=0.0):
    """ Apply a Savitzky-Golay filter to an array.
    This is a 1-d filter.  If `x`  has dimension greater than 1, `axis`
    determines the axis along which the filter is applied.
    Parameters
    ----------
    x : array_like
        The data to be filtered.  If `x` is not a single or double precision
        floating point array, it will be converted to type ``numpy.float64``
        before filtering.
    window_length : int
        The length of the filter window (i.e. the number of coefficients).
        `window_length` must be a positive odd integer. If `mode` is 'interp',
        `window_length` must be less than or equal to the size of `x`.
    polyorder : int
        The order of the polynomial used to fit the samples.
        `polyorder` must be less than `window_length`.
    deriv : int, optional
        The order of the derivative to compute.  This must be a
        nonnegative integer.  The default is 0, which means to filter
        the data without differentiating.
    delta : float, optional
        The spacing of the samples to which the filter will be applied.
        This is only used if deriv > 0.  Default is 1.0.
    axis : int, optional
        The axis of the array `x` along which the filter is to be applied.
        Default is -1.
    mode : str, optional
        Must be 'mirror', 'constant', 'nearest', 'wrap' or 'interp'.  This
        determines the type of extension to use for the padded signal to
        which the filter is applied.  When `mode` is 'constant', the padding
        value is given by `cval`.  See the Notes for more details on 'mirror',
        'constant', 'wrap', and 'nearest'.
        When the 'interp' mode is selected (the default), no extension
        is used.  Instead, a degree `polyorder` polynomial is fit to the
        last `window_length` values of the edges, and this polynomial is
        used to evaluate the last `window_length // 2` output values.
    cval : scalar, optional
        Value to fill past the edges of the input if `mode` is 'constant'.
        Default is 0.0.
    Returns
    -------
    y : ndarray, same shape as `x`
        The filtered data.
    See Also
    --------
    savgol_coeffs
    Notes
    -----
    Details on the `mode` options:
        'mirror':
            Repeats the values at the edges in reverse order.  The value
            closest to the edge is not included.
        'nearest':
            The extension contains the nearest input value.
        'constant':
            The extension contains the value given by the `cval` argument.
        'wrap':
            The extension contains the values from the other end of the array.
    For example, if the input is [1, 2, 3, 4, 5, 6, 7, 8], and
    `window_length` is 7, the following shows the extended data for
    the various `mode` options (assuming `cval` is 0)::
        mode       |   Ext   |         Input          |   Ext
        -----------+---------+------------------------+---------
        'mirror'   | 4  3  2 | 1  2  3  4  5  6  7  8 | 7  6  5
        'nearest'  | 1  1  1 | 1  2  3  4  5  6  7  8 | 8  8  8
        'constant' | 0  0  0 | 1  2  3  4  5  6  7  8 | 0  0  0
        'wrap'     | 6  7  8 | 1  2  3  4  5  6  7  8 | 1  2  3
    .. versionadded:: 0.14.0
    Examples
    --------
    >>> from scipy.signal import savgol_filter
    >>> np.set_printoptions(precision=2)  # For compact display.
    >>> x = np.array([2, 2, 5, 2, 1, 0, 1, 4, 9])
    Filter with a window length of 5 and a degree 2 polynomial.  Use
    the defaults for all other parameters.
    >>> savgol_filter(x, 5, 2)
    array([1.66, 3.17, 3.54, 2.86, 0.66, 0.17, 1.  , 4.  , 9.  ])
    Note that the last five values in x are samples of a parabola, so
    when mode='interp' (the default) is used with polyorder=2, the last
    three values are unchanged.  Compare that to, for example,
    `mode='nearest'`:
    >>> savgol_filter(x, 5, 2, mode='nearest')
    array([1.74, 3.03, 3.54, 2.86, 0.66, 0.17, 1.  , 4.6 , 7.97])
    """
    if mode not in ["mirror", "constant", "nearest", "interp", "wrap"]:
        raise ValueError("mode must be 'mirror', 'constant', 'nearest' "
                         "'wrap' or 'interp'.")

    x = np.asarray(x)
    # Ensure that x is either single or double precision floating point.
    if x.dtype != np.float64 and x.dtype != np.float32:
        x = x.astype(np.float64)

    coeffs = savgol_coeffs(window_length, polyorder, deriv=deriv, delta=delta)

    if mode == "interp":
        if window_length > x.size:
            raise ValueError("If mode is 'interp', window_length must be less "
                             "than or equal to the size of x.")

        # Do not pad.  Instead, for the elements within `window_length // 2`
        # of the ends of the sequence, use the polynomial that is fitted to
        # the last `window_length` elements.
        y = convolve1d(x, coeffs, axis=axis, mode="constant")
        _fit_edges_polyfit(x, window_length, polyorder, deriv, delta, axis, y)
    else:
        # Any mode other than 'interp' is passed on to ndimage.convolve1d.
        y = convolve1d(x, coeffs, axis=axis, mode=mode, cval=cval)

    return y
*/
