//<LICENCE>

#include <cmath>

#include "../../include/generators/SequenceImage.h"
#include "../../include/base/timage.h"

using namespace std;

namespace kipl { namespace generators {

kipl::base::TImage<float,2> SequenceImage::MakeSequenceImage2D(size_t const * const dims, float start)
{
	kipl::base::TImage<float,2> img(dims);
	
	for (size_t i=0; i<img.Size(); i++)
		img[i]=static_cast<float>(i)+start;
		
	return img;
}

kipl::base::TImage<float,2> SequenceImage::MakeModImage2D(size_t N, size_t M)
{
	size_t dims[]={N,N};
	kipl::base::TImage<float,2> img(dims);
	
	for (size_t i=0; i<img.Size(); i++)
		img[i]=static_cast<float>((i+1)%M);
		
	return img;
}

kipl::base::TImage<float,2> SequenceImage::MakeAxisSumImage2D(size_t N)
{
	size_t dims[]={N,N};
	kipl::base::TImage<float,2> img(dims);
	
	for (size_t i=0, y=0; y<N; y++)
		for (size_t x=0; x<N; x++,i++)
		img[i]=static_cast<float>(x+y);

	return img;
}

kipl::base::TImage<float,3> SequenceImage::MakeSequenceImage3D(size_t const * const dims, float start)
{
	kipl::base::TImage<float,3> img(dims);
	
	for (size_t i=0; i<img.Size(); i++)
		img[i]=static_cast<float>(i)+start;
		
	return img;
}

}}
