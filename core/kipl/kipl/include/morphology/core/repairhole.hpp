//LICENSE
#ifndef REPAIRHOLE_HPP
#define REPAIRHOLE_HPP


#include <list>

#include "../repairhole.h"
#include "../pixeliterator.h"

namespace kipl {
namespace morphology {

template <class T>
void RepairHoles(kipl::base::TImage<T,2> &img, std::list<size_t> &holelist, kipl::base::eConnectivity connect)
{
    const T markedPixel = std::numeric_limits<T>::max();

    std::list<size_t> edgePixels;
    std::list<size_t> remainingPixels;

    std::list<pair<size_t,T> > processedPixels;

    kipl::base::PixelIterator neighborhood(img.dims(), connect);
    for (const auto  & pixIt : holelist)
    {
        img[pixIt]=markedPixel;
        edgePixels.push_back(pixIt);
    }

    while (!edgePixels.empty())
    {
        while (!edgePixels.empty())
        {
            size_t pixPos=edgePixels.front();
            edgePixels.pop_front();

            neighborhood.setPosition(pixPos);
            double sum=0.0f;
            int hitcnt=0;
            bool isEdge=false;

            for (const auto &neighborPix : neighborhood.neighborhood())
            {
                size_t pixel = pixPos + neighborPix;

                if (pixel<img.Size())
                {
                    T value=img[pixel];

                    if (value!=markedPixel)
                    {
                        sum+=img[pixel];
                        ++hitcnt;
                        isEdge=true;
                    }
                }
            }

            if (isEdge)
            {
                processedPixels.push_back(std::make_pair(pixPos,static_cast<T>(sum/hitcnt)));
            }
            else
            {
                remainingPixels.push_back(pixPos);
            }
        }

        for (auto & p : processedPixels)
            img[p.first]=p.second;

        processedPixels.clear();

        edgePixels.clear();
        edgePixels=remainingPixels;

        remainingPixels.clear();
    }

}

}

}
#endif // REPAIRHOLE_HPP
