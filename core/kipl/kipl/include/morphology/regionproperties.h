//<LICENSE>

#include "../kipl_global.h"

#include <map>
#include <vector>
#include <string>

#include "../base/timage.h"

namespace kipl { namespace morphology {

enum eRegProps {
    regprop_label,
    regprop_area,
    regprop_perimeter,
    regprop_spherity,
    regprop_cogx,
    regprop_cogy,
    regprop_cogz,
    regprop_wcogx,
    regprop_wcogy,
    regprop_wcogz,
    regprop_intensity
};

struct PropertyItem 
{
    PropertyItem() :
        count(0UL),
        area(0UL),
        perimeter(0UL),
        spherity(0.0f),
        intensity(0.0f),
        cog({0.0f,0.0f}),
        wcog({0.0f,0.0f})
    {}

    size_t count;
    size_t area;
    size_t perimeter;
    float spherity;
    float intensity;
    std::vector<float> cog;
    std::vector<float> wcog;
};

template <typename T0, typename T1>
class RegionProperties
{
    public:
        RegionProperties() {}
        RegionProperties(const kipl::base::TImage<T0,2> & lbl, const kipl::base::TImage<T1,2> & img);
        size_t               count()      const;
        std::vector<T0>      labels()     const;
        std::map<T0, size_t> area()       const;
        std::map<T0, size_t> perimeter()  const;
        std::map<T0, float>  spherity()   const; 
        std::map<T0, float>  intensity()  const;
        std::map<T0, std::vector<float> > cog() const;
        std::map<T0, std::vector<float> > wcog() const;

        void filter(eRegProps property, const std::vector<float> &arg);

    private:
        void scanImage(const kipl::base::TImage<T0,2> & lbl, const kipl::base::TImage<T1,2> & img=kipl::base::TImage<T1,2>({0,0}));
        void finalizeProperties();
        std::map<T0, PropertyItem>  regions;
        
};

}}

#include "core/regionproperties.hpp"