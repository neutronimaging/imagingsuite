//<LICENCE>

#include "../../include/base/imagesamplers.h"

#include <tuple>

namespace kipl { namespace base {

void BinDimensions(size_t const * const origdims, size_t const * const nbin,size_t const * const resultdims)
{
    std::ignore = origdims;
    std::ignore = nbin;
    std::ignore = resultdims;

//	for (size_t i=0; i<nDims; i++) {
//		resultdims[i]=origdims[i]/nbin[i];
//	}
}

}
}
