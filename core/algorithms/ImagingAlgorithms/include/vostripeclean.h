#ifndef VOSTRIPECLEAN_H
#define VOSTRIPECLEAN_H

#include <base/timage.h>
#include <list>
class VoStripeClean
{
public:
    VoStripeClean();
    /// \brief  Algorithm 3 in the paper. Remove stripes using the sorting technique. Work particularly well for removing partial stripes. Angular direction is along the axis 0
    /// \param sinogram: 2D array.
    /// \param size: window size of the median filter.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeStripeBasedSorting(kipl::base::TImage<float,2> &sinogram, size_t size);

    /// \brief Algorithm 2 in the paper. Remove stripes using the filtering technique. Angular direction is along the axis 0
    ///  \param sinogram: 2D array.
    ///  \param sigma: sigma of the Gaussian window which is used to separate the low-pass and high-pass components of the intensity profiles of each column.
    ///  \param size: window size of the median filter.
    ///  \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> remove_stripe_based_filtering(kipl::base::TImage<float,2> &sinogram, float sigma, size_t size);

    /// \brief Algorithm 1 in the paper. Remove stripes using the fitting technique. Angular direction is along the axis 0
    /// \param sinogram: 2D array.
    /// \param order: polynomical fit order.
    /// \param sigmax, sigmay: sigmas of the Gaussian window.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> remove_stripe_based_fitting(kipl::base::TImage<float,2> &sinogram, int order, float sigmax, float sigmay);

    /// \brief Algorithm 5 in the paper. Remove large stripes by: locating stripes, normalizing to remove full stripes, using the sorting technique to remove partial stripes. Angular direction is along the axis 0.
    /// \param sinogram: 2D array.
    /// \param snr: ratio used to discriminate between useful information and noise.
    /// \param size: window size of the median filter.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> remove_large_stripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size);

private:
    /// \brief Create a 2D Gaussian window.
    /// \param height
    /// \param width: shape of the window.
    /// \param sigmax
    /// \param sigmay: sigmas of the window.
    /// \returns 2D window.
    kipl::base::TImage<float,2> _2d_window_ellipse(size_t height, size_t width, float sigmax, float sigmay);

    /// \brief Filtering an image using 2D Gaussian window.
    /// \param mat: 2D array.
    /// \param sigmax, sigmay: sigmas of the window.
    /// \param pad: padding for FFT
    /// \returns filtered image.
    kipl::base::TImage<float,2> _2d_filter(kipl::base::TImage<float,2> mat, float sigmax, float sigmay, size_t pad);

    ///    Algorithm 4 in the paper. Used to locate stripe positions.
    ///    Parameters: - listdata: 1D normalized array.
    ///                - snr: ratio used to discriminate between useful
    ///                    information and noise.
    ///    Return:     - 1D binary mask.
    std::list<size_t> detect_stripe(std::list<size_t >listdata, float snr);

};

#endif // VOSTRIPECLEAN_H












kipl::base::TImage<float,2> VoStripeClean::remove_unresponsive_and_fluctuating_stripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size)
{
//    """
//    Algorithm 6 in the paper. Remove unresponsive or fluctuating stripes by:
//    locating stripes, correcting by interpolation.
//    Angular direction is along the axis 0.
//    ---------
//    Parameters: - sinogram: 2D array.
//                - snr: ratio used to discriminate between useful
//                    information and noise
//                - size: window size of the median filter.
//    ---------
//    Return:     - stripe-removed sinogram.
//    """
//    sinogram = np.copy(sinogram) # Make it mutable
//    (nrow, _) = sinogram.shape
//    sinosmoothed = np.apply_along_axis(uniform_filter1d, 0, sinogram, 10)
//    listdiff = np.sum(np.abs(sinogram - sinosmoothed), axis=0)
//    nmean = np.mean(listdiff)
//    listdiffbck = median_filter(listdiff, size)
//    listdiffbck[listdiffbck == 0.0] = nmean
//    listfact = listdiff / listdiffbck
//    listmask = detect_stripe(listfact, snr)
//    listmask = binary_dilation(listmask, iterations=1).astype(listmask.dtype)
//    listmask[0:2] = 0.0
//    listmask[-2:] = 0.0
//    listx = np.where(listmask < 1.0)[0]
//    listy = np.arange(nrow)
//    matz = sinogram[:, listx]
//    finter = interpolate.interp2d(listx, listy, matz, kind='linear')
//    listxmiss = np.where(listmask > 0.0)[0]
//    if len(listxmiss) > 0:
//        matzmiss = finter(listxmiss, listy)
//        sinogram[:, listxmiss] = matzmiss
//    # Use algorithm 5 to remove residual stripes
//    #sinogram = remove_large_stripe(sinogram, snr, size)
//    return sinogram
    kipl::base::TImage<float,2> res;

    return res;
}

kipl::base::TImage<float,2> VoStripeClean::remove_all_stripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t la_size, size_t sm_size):
{
//    """
//    Remove all types of stripe artifacts by combining algorithm 6, 5, and 3.
//    Angular direction is along the axis 0.
//    ---------
//    Parameters: - sinogram: 2D array.
//                - snr: ratio used to discriminate between useful
//                    information and noise
//                - la_size: window size of the median filter to remove
//                    large stripes.
//                - sm_size: window size of the median filter to remove
//                    small-to-medium stripes.
//    ---------
//    Return:     - stripe-removed sinogram.
//    """
//    sinogram = remove_unresponsive_and_fluctuating_stripe(sinogram, snr, la_size)
//    sinogram = remove_large_stripe(sinogram, snr, la_size)
//    sinogram = remove_stripe_based_sorting(sinogram, sm_size)
//    return sinogram
    kipl::base::TImage<float,2> res;

    return res;
}

//# ----------------------------------------------------------------------------
//# Example of use:
//# sinogram = remove_stripe_based_sorting(sinogram, 31)
//# sinogram = remove_stripe_based_filtering(sinogram, 3, 31)
//# sinogram = remove_stripe_based_fitting(sinogram, 1, 5, 20)
//# sinogram = remove_unresponsive_and_fluctuating_stripe(sinogram1, 3, 81)
//# sinogram = remove_large_stripe(sinogram1, 3, 81)
//# sinogram = remove_all_stripe(sinogram, 3, 81, 31)
