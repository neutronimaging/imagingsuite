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
void RegionProperties<T0,T1>::filter(eRegProps property, const std::vector<float> &arg)
{
    for (auto region = regions.begin() ; region!=regions.end() ; ) 
    {
        bool erase = false;
        switch (property) 
        {
            case regprop_label : 
            break;
            
            case regprop_area :
                erase = ((region->second.area < arg[0]) || (arg[1] < region->second.area));
                break;
            
            case regprop_perimeter:
                erase = ((region->second.perimeter < arg[0]) || (arg[1] < region->second.perimeter));
                break;

            case regprop_spherity:
                erase = ((region->second.spherity < arg[0]) || (arg[1] < region->second.spherity));
                break;

            case regprop_cogx:
                erase = ((region->second.cog[0] < arg[0]) || (arg[1] < region->second.cog[0]));
                break;

            case regprop_cogy:
                erase = ((region->second.cog[1] < arg[0]) || (arg[1] < region->second.cog[1]));
                break;

            case regprop_cogz:
            break ;

            case regprop_wcogx:
                erase = ((region->second.wcog[0] < arg[0]) || (arg[1] < region->second.wcog[0]));
                break;

            case regprop_wcogy:
                erase = ((region->second.wcog[1] < arg[0]) || (arg[1] < region->second.wcog[1]));
                break;

            case regprop_wcogz:
            break ;

            case regprop_intensity :
                erase = ((region->second.intensity < arg[0]) || (arg[1] < region->second.intensity));
                break;
        }

        if (erase)
            regions.erase(region++);
        else
            ++region;
    }
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
            regions[currentLabel].area++;
            
            if (pImg!=nullptr) 
            {
                regions[currentLabel].intensity+=pImg[idx];
                regions[currentLabel].wcog[0]+=static_cast<float>(x)*pImg[idx];
                regions[currentLabel].wcog[1]+=static_cast<float>(y)*pImg[idx];
            }
            
            regions[currentLabel].cog[0]+=static_cast<float>(x);
            regions[currentLabel].cog[1]+=static_cast<float>(y);
            
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
                regions[currentLabel].perimeter++;

        }
    }

}

template <typename T0, typename T1>
void RegionProperties<T0,T1>::finalizeProperties()
{
    for (auto &item : regions) 
    {
        auto lbl  = item.first;
        auto area = item.second.area; 
        if (item.second.intensity!=0.0f)
        {
            item.second.wcog[0]/=item.second.intensity;
            item.second.wcog[1]/=item.second.intensity;
        }
        else 
        {
            item.second.wcog[0]/=area;
            item.second.wcog[1]/=area;
        }
        item.second.wcog[0]-=1.0f;
        item.second.wcog[1]-=1.0f;
        
        item.second.intensity/=area;
        item.second.cog[0]/=area;
        item.second.cog[1]/=area;
        item.second.cog[0]-=1.0f;
        item.second.cog[1]-=1.0f;

        item.second.spherity=static_cast<float>(area)/static_cast<float>(item.second.perimeter);
    }
}

template <typename T0, typename T1>
size_t RegionProperties<T0,T1>::count() const
{
    return regions.size();
}

template <typename T0, typename T1>
std::vector<T0> RegionProperties<T0,T1>::labels() const
{
    std::vector<T0> lbls;

    for (const auto & item : regions)
        lbls.push_back(item.first);

    return lbls;
}

template <typename T0, typename T1>
std::map<T0, size_t> RegionProperties<T0,T1>::area() const 
{
    std::map<T0, size_t> nArea;

    for (const auto item : regions)
        nArea[item.first]=item.second.area;

    return nArea;
}

template <typename T0, typename T1>
std::map<T0, size_t> RegionProperties<T0,T1>::perimeter() const 
{
    std::map<T0, size_t> nPerimeter;

    for (const auto item : regions)
        nPerimeter[item.first]=item.second.perimeter;   

    return nPerimeter;
}

template <typename T0, typename T1>
std::map<T0, float>  RegionProperties<T0,T1>::spherity() const
{
    std::map<T0, float> fSpherity;

    for (const auto item : regions)
        fSpherity[item.first]=item.second.spherity;

    return fSpherity;
}

template <typename T0, typename T1>
std::map<T0, float>  RegionProperties<T0,T1>::intensity() const 
{
    std::map<T0, float> fIntensity;

    for (const auto item : regions)
        fIntensity[item.first]=item.second.intensity;

    return fIntensity;
}

template <typename T0, typename T1>
std::map<T0, std::vector<float> >  RegionProperties<T0,T1>::cog() const
{
    std::map<T0, std::vector<float> > fCOG;

    for (const auto item : regions)
        fCOG[item.first]=item.second.cog;
    
    return fCOG;
}
 
template <typename T0, typename T1>
std::map<T0, std::vector<float> >  RegionProperties<T0,T1>::wcog() const
{
    std::map<T0, std::vector<float> > fCOG;

    for (const auto item : regions)
        fCOG[item.first]=item.second.wcog;
    
    return fCOG;
}

}}