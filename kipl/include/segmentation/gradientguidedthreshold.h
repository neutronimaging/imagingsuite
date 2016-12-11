#ifndef GRADIENTGUIDEDTHRESHOLD_H
#define GRADIENTGUIDEDTHRESHOLD_H

#include <set>
#include "../base/timage.h"
#include "segmentationbase.h"

namespace kipl { namespace segmentation {

template <typename T0, typename T1, size_t N>
class gradientGuidedThreshold: public SegmentationBase<T0,T1,N> {
public:
    gradientGuidedThreshold();
    ~gradientGuidedThreshold();

    void setDataThreshold(T0 val);
    void setDataThreshold(std::set<T0> val);
    void setDataThreshold(std::vector<T0> val);
    void setDataThreshold(T0 *val, size_t Nelement);

    void setEdgeThreshold(float val);
    void setFilterWidth(int w);
    virtual void setParameters(std::map<std::string,std::string> parameters) ;
    virtual int operator()(kipl::base::TImage<T0,N> & img, kipl::base::TImage<T1,N> &seg);

private:
    std::set<T0> m_threshold;
    float m_edgeThreshold;
    int m_filterWidth;
};

}}

#include "core/gradientguidedthreshold.hpp"
#endif // GRADIENTGUIDEDTHRESHOLD_H
