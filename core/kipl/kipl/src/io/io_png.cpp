//<LICENSE>

#include <string>

#include "../../include/base/KiplException.h"

#include "../../include/io/io_png.h"
#include <string>
#include <vector>

namespace kipl {
namespace io {

std::vector<size_t> GetPNGDims(const std::string &fname)
{
    std::ignore = fname;

    throw kipl::base::KiplException("png support is not implemented",__FILE__,__LINE__);

    return std::vector<size_t>(0);
}

int KIPLSHARED_EXPORT ReadPNG(std::string fname, kipl::base::TImage<float,2> &img, int idx, size_t *roi)
{
    std::ignore = img;
    std::ignore = idx;
    std::ignore = roi;

    throw kipl::base::KiplException("png support is not implemented",__FILE__,__LINE__);

    return 0;
}

int KIPLSHARED_EXPORT WritePNG(std::string fname, kipl::base::TImage<float,2> & img, float low, float high)
{
    std::ignore = img;
    std::ignore = low;
    std::ignore = high;

    throw kipl::base::KiplException("png support is not implemented",__FILE__,__LINE__);

    return 0;
}


}
}

