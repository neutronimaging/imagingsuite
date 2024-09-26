//<LICENSE>

#ifndef STRIPEFILTERMGR_H_
#define STRIPEFILTERMGR_H_

#include "ImagingAlgorithms_global.h"
#include <iostream>

#include <base/timage.h>
#include <utilities/threadpool.h>
#include "StripeFilter.h"

namespace ImagingAlgorithms {


/// The class implements the wavelet based stripe removal filter proposed by Muench et al, Optics Express.
class IMAGINGALGORITHMSSHARED_EXPORT StripeFilterManager {
private:
    kipl::logging::Logger logger; ///< Logger instance for message reporting
public:
    /// \brief The constructor initializes the filter
    /// \note The filter is initialized for one image size only.
    /// \param dims Size of the image to filter
    /// \param wname Name of the wavelet base
    /// \param scale number of decomosition levels
    /// \param sigma High pass cut-off frequency
    StripeFilterManager(const std::vector<size_t> & dims, const std::string &wname, int scale, float sigma);

    /// \brief The constructor initializes the filter
    /// \note The filter is initialized for one image size only.
    /// \param dims Size of the image to filter
    /// \param wname Name of the wavelet base
    /// \param scale number of decomosition levels
    /// \param sigma High pass cut-off frequency
    // StripeFilterManager(const std::vector<int> &dims, const string &wname, int scale, float sigma);

    virtual ~StripeFilterManager();

    std::vector<int>   dims();
    std::string        waveletName();
    int                decompositionLevels();
    float              sigma();
    std::vector<float> filterWindow(int level);
    
    void configure(const std::vector<int> &dims, const std::string &wname, int scale, float sigma);
    
    // void setNumberOfThreads(int N);
    int  numberOfThreads();

    /// \brief Applies the stripe filter to a 2D image. The operation is inplace.
    /// \param img the image to process. The result will be stored into the same image
    /// \param op Selects filter operation
    void process(kipl::base::TImage<float,2> &img, eStripeFilterOperation op=VerticalComponentFFT);

    /// \brief Applies the stripe filter to a 3D image. The operation is inplace.
    /// \param img the image to process. The result will be stored into the same image
    /// \param op Selects filter operation
    void process(kipl::base::TImage<float,3> &img, eStripeFilterOperation op=VerticalComponentFFT);

protected:
    kipl::utilities::ThreadPool pool; ///< The thread pool for parallel processing
    std::vector<ImagingAlgorithms::StripeFilter *> filters; ///< The filter instances for each thread                              
};
}

#endif // STRIPEFILTERMGR_H_ 