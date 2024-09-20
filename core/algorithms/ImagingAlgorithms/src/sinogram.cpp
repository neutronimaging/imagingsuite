
#include <algorithm>
#include <base/timage.h>

namespace ImagingAlgorithms 
{
int ExtractSinogram(kipl::base::TImage<float,3> &projections, kipl::base::TImage<float,2> &sinogram, size_t idx)
{
    std::vector<size_t> dims = { projections.Size(0),
                                 projections.Size(2)};

    sinogram.resize(dims);

	for (size_t i=0; i<dims[1]; ++i)
	{
	    std::copy_n(projections.GetLinePtr(idx,i),dims[0],sinogram.GetLinePtr(i));
	}

	return 0;
}

int InsertSinogram(kipl::base::TImage<float,2> &sinogram, kipl::base::TImage<float,3> &projections, size_t idx)
{
    auto dims=sinogram.dims();

	for (size_t i=0; i<dims[1]; ++i)
	{
        std::copy_n(sinogram.GetLinePtr(i),dims[0],projections.GetLinePtr(idx,i));
	}

	return 0;
}

}
