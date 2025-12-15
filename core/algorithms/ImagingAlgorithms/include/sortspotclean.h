#ifndef IMGALG_SORTSPOTCLEAN_H_
#define IMGALG_SORTSPOTCLEAN_H_
#include "ImagingAlgorithms_global.h"
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <list>


#include <base/timage.h>
#include <strings/miscstring.h>
#include <filters/filter.h>
#include <containers/ArrayBuffer.h>
#include <math/LUTCollection.h>
#include <logging/logger.h>
#include <interactors/interactionbase.h>

#include "pixelinfo.h"

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT SortSpotClean {
    kipl::logging::Logger m_logger;
public:
    SortSpotClean(bool processPatches=false, size_t patchSize=32, bool useThreads=false);
    ~SortSpotClean();

    void process(kipl::base::TImage<float,2>& image, float quantile, float threshold);
    
    kipl::base::TImage<float,2> getSpotMask() const { return m_mask; }
    kipl::base::TImage<float,2> getDifferenceImage() const { return m_diff; }  
private:
    void findAndCleanSpots(kipl::base::TImage<float,2>& image, float threshold, float quantile);
    std::vector<size_t> findSpots(kipl::base::TImage<float,2>& image, float quantile, float threshold);
    kipl::base::TImage<float,2> createSpotMask(kipl::base::TImage<float,2>& image, const std::vector<size_t>& spotPositions);
    bool m_processPatches;
    bool m_useThreads;
    size_t m_patchSize;
    kipl::base::TImage<float,2> m_mask; //!< Mask image indicating the spot positions
    kipl::base::TImage<float,2> m_diff;
};

}  // namespace ImagingAlgorithms

#endif  // IMGALG_SORTSPOTCLEAN_H_