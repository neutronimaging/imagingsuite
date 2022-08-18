#ifndef VOSTRIPECLEAN_H
#define VOSTRIPECLEAN_H

#include "ImagingAlgorithms_global.h"
#include <base/timage.h>
#include <list>
#include <logging/logger.h>

namespace ImagingAlgorithms
{
class IMAGINGALGORITHMSSHARED_EXPORT VoStripeClean
{
    kipl::logging::Logger logger;
public:
    VoStripeClean();
    /// \brief  Algorithm 3 in the paper. Remove stripes using the sorting technique. Work particularly well for removing partial stripes. Angular direction is along the axis 0
    /// \param sinogram: 2D array.
    /// \param size: window size of the median filter.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeStripeBasedSorting(kipl::base::TImage<float,2> &sinogram, size_t size, bool doTranspose);

    /// \brief Algorithm 2 in the paper. Remove stripes using the filtering technique. Angular direction is along the axis 0
    ///  \param sinogram: 2D array.
    ///  \param sigma: sigma of the Gaussian window which is used to separate the low-pass and high-pass components of the intensity profiles of each column.
    ///  \param size: window size of the median filter.
    ///  \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeStripeBasedFiltering(kipl::base::TImage<float,2> &sinogram, float sigma, size_t size, bool doTranspose);

    /// \brief Algorithm 1 in the paper. Remove stripes using the fitting technique. Angular direction is along the axis 0
    /// \param sinogram: 2D array.
    /// \param order: polynomical fit order.
    /// \param sigmax, sigmay: sigmas of the Gaussian window.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeStripeBasedFitting(kipl::base::TImage<float,2> &sinogram, int order, float sigmax, float sigmay, bool doTranspose);

    /// \brief Algorithm 5 in the paper. Remove large stripes by: locating stripes, normalizing to remove full stripes, using the sorting technique to remove partial stripes. Angular direction is along the axis 0.
    /// \param sinogram: 2D array.
    /// \param snr: ratio used to discriminate between useful information and noise.
    /// \param size: window size of the median filter.
    /// \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeLargeStripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size, bool doTranspose);

    ///  \brief Algorithm 6 in the paper. Remove unresponsive or fluctuating stripes by: locating stripes, correcting by interpolation. Angular direction is along the axis 0.
    ///  \param sinogram: 2D array.
    ///  \param snr: ratio used to discriminate between useful information and noise
    ///  \param size: window size of the median filter.
    ///  \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeUnresponsiveAndFluctuatingStripe(kipl::base::TImage<float,2> &sinogram, float snr, size_t size);

    /// \brief Remove all types of stripe artifacts by combining algorithm 6, 5, and 3.
    ///  Angular direction is along the axis 0.
    ///  \param sinogram: 2D array.
    ///  \param snr: ratio used to discriminate between useful information and noise
    ///  \param la_size: window size of the median filter to remove large stripes.
    ///  \param sm_size: window size of the median filter to remove small-to-medium stripes.
    ///  \returns stripe-removed sinogram.
    kipl::base::TImage<float,2> removeAllStripes(kipl::base::TImage<float,2> &sinogram, float snr, size_t la_size, size_t sm_size, bool doTranspose);
private:

    kipl::base::TImage<float, 2> detectStripe(kipl::base::TImage<float, 2> sinogram, float snr);

    /// \brief Create a 2D Gaussian window.
    /// \param height
    /// \param width: shape of the window.
    /// \param sigmax
    /// \param sigmay: sigmas of the window.
    /// \returns 2D window.
    kipl::base::TImage<float,2> _2dWindowEllipse(size_t height, size_t width, float sigmax, float sigmay);

    /// \brief Filtering an image using 2D Gaussian window.
    /// \param mat: 2D array.
    /// \param sigmax, sigmay: sigmas of the window.
    /// \param pad: padding for FFT
    /// \returns filtered image.
    kipl::base::TImage<float,2> _2dFilter(kipl::base::TImage<float,2> mat, float sigmax, float sigmay, size_t pad);

    ///    Algorithm 4 in the paper. Used to locate stripe positions.
    ///    Parameters: - listdata: 1D normalized array.
    ///                - snr: ratio used to discriminate between useful
    ///                    information and noise.
    ///    Return:     - 1D binary mask.
    std::vector<float> detect_stripe(std::vector<float> & listdata, float snr);

    /// Fills in interpolated values as replacement for stripes
    /// Implemented as inplace operation
    /// \param img The image to be processed
    /// \param mask A vector with non-zero values for positions to modify
    void interpolationFill(kipl::base::TImage<float,2> &img, std::vector<float> &mask);

    kipl::base::TImage<float,2> transpose(kipl::base::TImage<float, 2> &img, bool doIt);

};
}
#endif // VOSTRIPECLEAN_H




// ----------------------------------------------------------------------------
// Example of use:
// sinogram = remove_stripe_based_sorting(sinogram, 31)
// sinogram = remove_stripe_based_filtering(sinogram, 3, 31)
// sinogram = remove_stripe_based_fitting(sinogram, 1, 5, 20)
// sinogram = remove_unresponsive_and_fluctuating_stripe(sinogram1, 3, 81)
// sinogram = remove_large_stripe(sinogram1, 3, 81)
// sinogram = remove_all_stripe(sinogram, 3, 81, 31)

