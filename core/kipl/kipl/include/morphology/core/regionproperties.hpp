//<LICENSE>

#include <map>
#include <vector>
#include <string>

#include "../../base/timage.h"
#include "../pixeliterator.h"

namespace kipl {namespace morphology {

template <typename T0, typename T1>
RegionProperties<T0,T1>::RegionProperties(kipl::base::TImage<T0,2> & lbl, kipl::base::TImage<T1,2> & img)
{
    scanImage(lbl,img);
    finalizeProperties();
}

template <typename T0, typename T1>
void RegionProperties<T0,T1>::scanImage(kipl::base::TImage<T0,2> & lbl, kipl::base::TImage<T1,2> & img)
{
    auto pLbl = lbl.GetDataPtr();
    auto pImg = img.Size() == 1 ? nullptr : img.GetDataPtr();

    kipl::base::PixelIterator ng(lbl.dims(),kipl::base::conn4);

    size_t idx=0UL;
    for (size_t y=1UL; y<=lbl.Size(1); ++y)
    {
        for (size_t x=1UL; x<=lbl.Size(0); ++x,++idx) 
        {
            auto currentLabel = pLbl[idx];
            nArea[currentLabel]++;
            if (pImg!=nullptr)
                fIntensity[currentLabel]+=pImg[idx];
            if (nArea[currentLabel]==1) // Initialize cog item
            {
                fCOG[currentLabel]={static_cast<float>(x),static_cast<float>(y)};
            }
            else 
            {
                fCOG[currentLabel][0]+=static_cast<float>(x);
                fCOG[currentLabel][1]+=static_cast<float>(y);
            }

            ng.setPosition(idx);
            auto neighborhood = ng.neighborhood();
            bool isEdge=false;
            for (const auto &nidx : neighborhood)
            {
                if (pLbl[idx+nidx] != currentLabel)
                {
                    isEdge = true;
                    break;
                }
            }
            
            if (isEdge)
                nPerimeter[currentLabel]++;

        }
    }

}

template <typename T0, typename T1>
void RegionProperties<T0,T1>::finalizeProperties()
{
    for (const auto &item : nArea) 
    {
        auto lbl  = item.first;
        auto area = item.second; 
        fIntensity[lbl]/=area;
        fCOG[lbl][0]/=area;
        fCOG[lbl][1]/=area;
        fCOG[lbl][0]-=1.0f;
        fCOG[lbl][1]-=1.0f;
        fSpherity[lbl]=static_cast<float>(area)/static_cast<float>(nPerimeter[lbl]);
    }
}

template <typename T0, typename T1>
size_t RegionProperties<T0,T1>::count() const
{
    return nArea.size();
}

template <typename T0, typename T1>
std::vector<T0> RegionProperties<T0,T1>::labels() const
{
    std::vector<T0> lbls;

    for (const auto & item : nArea)
        lbls.push_back(item.first);

    return lbls;
}

template <typename T0, typename T1>
std::map<T0, size_t> RegionProperties<T0,T1>::area() const 
{
    return nArea;
}

template <typename T0, typename T1>
std::map<T0, size_t> RegionProperties<T0,T1>::perimeter() const 
{
    return nPerimeter;
}

template <typename T0, typename T1>
std::map<T0, float>  RegionProperties<T0,T1>::spherity() const
{
    return fSpherity;
}

template <typename T0, typename T1>
std::map<T0, float>  RegionProperties<T0,T1>::intensity() const 
{
    return fIntensity;
}

template <typename T0, typename T1>
std::map<T0, std::vector<float> >  RegionProperties<T0,T1>::cog() const
{
    return fCOG;
}
 

}}