#ifndef REPAIRHOLE_HPP
#define REPAIRHOLE_HPP


#include <list>

#include <QDebug>

#include "../repairhole.h"
#include "../pixeliterator.h"

namespace kipl {
namespace morphology {

template <class T>
void RepairHoles(kipl::base::TImage<T,2> &img, std::list<size_t> &holelist, kipl::base::eConnectivity connect)
{
    const T markedPixel = std::numeric_limits<T>::max();

//    std::cout<< "img Size" << img.Size()<< std::endl;

    std::list<size_t> edgePixels;
    std::list<size_t> remainingPixels;

    std::list<pair<size_t,T> > processedPixels;

    kipl::base::PixelIterator neighborhood(img.Dims(), connect);
    for (auto pixIt=holelist.begin(); pixIt!=holelist.end(); ++pixIt)
    {
        img[*pixIt]=markedPixel;
        edgePixels.push_back(*pixIt);
    }
    int loopCnt=0;
    while (!edgePixels.empty())
    {
//        qDebug() << "Loop count" << loopCnt++ << "list size "<< edgePixels.size();
        while (!edgePixels.empty())
        {
            size_t pixPos=edgePixels.front();
            edgePixels.pop_front();

            neighborhood.setPosition(pixPos);
            double sum=0.0f;
            int hitcnt=0;
            bool isEdge=false;
            for (int idx=0; idx<neighborhood.neighborhoodSize(); ++idx) {
                size_t pixel=neighborhood.neighborhood(idx);


                if (pixel<img.Size()){
                    T value=img[pixel];
                    if (value!=markedPixel) {
//                        std::cout << pixel << " " << img[pixel] << " " << std::endl;
                        sum+=img[pixel];
                        hitcnt++;
                        isEdge=true;
                    }
                }
            }

//            qDebug() << "hit cnt"<<hitcnt<<" isedge"<<isEdge;
            if (isEdge)
            {
                processedPixels.push_back(std::make_pair(pixPos,static_cast<T>(sum/hitcnt)));
            }
            else
            {
                remainingPixels.push_back(pixPos);
            }
        }

//        qDebug() << "Size processed:" << processedPixels.size() <<" remaining "<< remainingPixels.size();
        for (auto p=processedPixels.begin(); p!=processedPixels.end(); ++p)
            img[p->first]=p->second;

        processedPixels.clear();

        edgePixels.clear();
        edgePixels=remainingPixels;
//        qDebug() << "Loop count" << loopCnt << "list size "<< edgePixels.size();
        remainingPixels.clear();

    }

}

}

}
#endif // REPAIRHOLE_HPP
