//<LICENSE>

#ifndef STRIPEFILTER_H_
#define STRIPEFILTER_H_

#include "ImagingAlgorithms_global.h"
#include <iostream>

#include <base/timage.h>
#include <math/mathconstants.h>
#include <fft/fftbase.h>
#include <logging/logger.h>
#include <wavelets/wavelets.h>

namespace ImagingAlgorithms {

/// \brief Selector enum to select the type of filter operation in the vertical detail
enum eStripeFilterOperation {
    VerticalComponentZero = 0, ///< Set the vertical component to zero
    VerticalComponentFFT       ///< Apply a high-pass filter to the vertical component
};

/// The class implements the wavelet based stripe removal filter proposed by Muench et al, Optics Express.
class IMAGINGALGORITHMSSHARED_EXPORT StripeFilter {
private:
    kipl::logging::Logger logger; ///< Logger instance for message reporting
    static const size_t NLevels = 16;
public:
    /// \brief The constructor initializes the filter
    /// \note The filter is initialized for one image size only.
    /// \param dims Size of the image to filter
    /// \param wname Name of the wavelet base
    /// \param scale number of decomosition levels
    /// \param sigma High pass cut-off frequency
    StripeFilter(const std::vector<size_t> & dims, const std::string &wname, int scale, float sigma);

    /// \brief The constructor initializes the filter
    /// \note The filter is initialized for one image size only.
    /// \param dims Size of the image to filter
    /// \param wname Name of the wavelet base
    /// \param scale number of decomosition levels
    /// \param sigma High pass cut-off frequency
    StripeFilter(const std::vector<int> &dims, const string &wname, int scale, float sigma);

    virtual ~StripeFilter();

    std::vector<int> dims();
    bool checkDims(const std::vector<size_t> &dims);
    std::string waveletName();
    int decompositionLevels();
    float sigma();
    std::vector<float> filterWindow(int level);
    
    void configure(const std::vector<int> &dims, const std::string &wname, int scale, float sigma);
    void setNumberOfThreads(int N);
    int numberOfThreads();

    /// \brief Applies the stripe filter to an image.
    /// \param img the image to process. The result will be stored into the same image
    /// \param op Selects filter operation
    void process(kipl::base::TImage<float,2> &img, eStripeFilterOperation op=VerticalComponentFFT);

private:
    /// \brief Builds the Fourier filter window.
	void CreateFilterWindow();

    /// \brief Applies the Fourier filter operation to the vertical detail component
    /// \param img The vertical component to filter
    /// \param level Decomposition level of the vertical component
    void FilterVerticalStripes(kipl::base::TImage<float,2> &img, size_t level=0);

    /// \brief Set the vertial component to zero
    /// \param img The vertical component to manipulate
	void VerticalStripesZero(kipl::base::TImage<float,2> &img);

    /// \brief Copies a selected vertical line to a line buffer
    /// \param pSrc pointer to the image data buffer
    /// \param pLine pointer to the line buffer
    /// \param pos index of the vertical line to copy
    /// \param stride number of pixels between two horizontal lines
    /// \param len length of a line
	void GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len);

    /// \brief Copies a line buffer to a vertical line in the target image.
    /// \param pLine pointer to the line buffer
    /// \param pDest pointer to the image data buffer
    /// \param pos index of the vertical line to copy
    /// \param stride number of pixels between two horizontal lines
    /// \param len length of a line
	void SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len);

    bool m_bUseThreading;
    int m_nNumberOfThreads;

    kipl::wavelets::WaveletTransform<float> m_wt; ///< Instance of the wavelete transform
    int m_nScale;                              ///< Number of decomposition levels
    float m_fSigma;                               ///< Width of the Gaussian used to implement the highpass filter
    size_t m_nFFTlength[NLevels];                      ///< Length of the fft transforms performed on the different decomposition levels

    float *m_pDamping[NLevels];                        ///< Filter coefficients for the filter windows
    float *m_pLine;                               ///< Pointer to the line buffer
    std::complex<float> *m_pCLine;                ///< Pointer to the Fourier transformed line

    std::vector<int> wdims;                              ///< Dimensions of the image for which the filter is configured
    kipl::math::fft::FFTBaseFloat *fft[NLevels];       ///< FFT instances for each decomposition level.
};
}

/// \brief Converts the stripe filter operation enum value to a string
/// \param op The enum value to convert
/// \returns A string with the name of the enum value
/// \throws An ImagingException if the conversion fails
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eStripeFilterOperation op);

/// \brief Converts a string to a stripe filter enum value
/// \param str The string with the name
/// \param op The resulting value
/// \throws An ImagingException if the conversion fails
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::eStripeFilterOperation &op);

/// \brief Writes the name of stripe filter enum to a stream.
/// \param s The target stream.
/// \param op The enum value to write
/// \returns the modified stream
/// \throws An ImagingException if the conversion fails
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream & s, ImagingAlgorithms::eStripeFilterOperation op);

#endif /* STRIPEFILTER_H_ */
