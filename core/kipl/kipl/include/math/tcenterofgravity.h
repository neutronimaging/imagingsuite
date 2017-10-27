#ifndef TCENTEROFGRAVITY_H
#define TCENTEROFGRAVITY_H

#include "../kipl_global.h"
#include "../base/timage.h"
#include "../base/index2coord.h"
#include "../logging/logger.h"

namespace kipl {
namespace math {

class KIPLSHARED_EXPORT CenterOfGravity
{
    kipl::logging::Logger logger;
public:
    CenterOfGravity();
    ~CenterOfGravity();

    kipl::base::coords3Df findCenter(kipl::base::TImage<float,3> img, bool applythreshold=true);


private:
   void clearAllocation();
   void computeProfiles(kipl::base::TImage<float,3> img, bool applythreshold, float threshold);
   float computeCOG(float *profile,int N);
   float *profileX;
   float *profileY;
   float *profileZ;
   int dims[3];
};

}
}

#endif // TCENTEROFGRAVITY_H
