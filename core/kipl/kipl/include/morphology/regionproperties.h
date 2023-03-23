//<LICENSE>

#include "../kipl_global.h"

#include <map>
#include <vector>
#include <string>

#include "../base/timage.h"

namespace kipl {namespace morphology {

enum eRegProps {
    regprop_label,
    regprop_area,
    regprop_perimeter,
    regprop_cogx,
    regprop_cogy,
    regprop_cogz,
    regprop_intensity
};

template <typename T0, typename T1>
class RegionProperties
{
    public:
        RegionProperties(kipl::base::TImage<T0,2> & lbl, kipl::base::TImage<T1,2> & img);
        size_t count() const;
        std::vector<T0> labels() const;
        std::map<T0, size_t> area()       const;
        std::map<T0, size_t> perimeter()  const;
        std::map<T0, float>  spherity()   const; 
        std::map<T0, float>  intensity()  const;
        std::map<T0, std::vector<float> > cog() const;
    private:
        void scanImage(kipl::base::TImage<T0,2> & lbl, kipl::base::TImage<T1,2> & img=kipl::base::TImage<T1,2>({0,0}));
        void finalizeProperties();
        std::map<T0, size_t>  nArea;
        std::map<T0, size_t>  nPerimeter;
        std::map<T0, float>   fSpherity;
        std::map<T0, float>   fIntensity;
        std::map<T0, std::vector<float> > fCOG;

};

}}

#include "core/regionproperties.hpp"