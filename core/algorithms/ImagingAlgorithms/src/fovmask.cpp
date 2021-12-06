#include "../include/fovmask.h"

namespace ImagingAlgorithms {

FOVMask::FOVMask()
{

}

void FOVMask::setMask(kipl::base::TImage<short, 2> &img)
{
    maskImg.Clone(img);

    for (size_t i=1; i<(img.Size()-1); ++i)
    {
        if (( img[i] == 0 ) && ( img[i+1] != 0 ) )
            initialEdge.push_back(i);
        if (( img[i-1] != 0 ) && ( img[i] == 0 ) )
            initialEdge.push_back(i);
    }
}

kipl::base::TImage<float, 2> FOVMask::process(kipl::base::TImage<float, 2> &img)
{

}

}
