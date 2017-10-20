#include <sstream>

#include <math/mathconstants.h>

#include "basicforwardprojector.h"

BasicForwardProjector::BasicForwardProjector() :
    ForwardProjectorBase("BasicForwardProjector")
{
}

int BasicForwardProjector::project(kipl::base::TImage<float, 2> &slice, std::list<float> &angles, kipl::base::TImage<float, 2> &proj)
{
    std::ostringstream msg;
    size_t dims[2]={slice.Size(0), angles.size()};
    proj.Resize(dims);

    float C=slice.Size(0)/2;

    int i=0;

    for (auto it = angles.begin(); it!=angles.end(); it++, i++ ) { // Loop over acquisition angles
        float *pLine=proj.GetLinePtr(i);
        memset(pLine,0,sizeof(float)*proj.Size(0));

        float cosphi=cos((*it)*fPi/180.0f);
        float sinphi=sin((*it)*fPi/180.0f);

        msg.str(""); msg<<"angle="<<*it<<" cos(phi)="<<cosphi;
        logger(kipl::logging::Logger::LogMessage,msg.str());

        for (int t=-C; t<C; t++) { // Loop over detector elements
            float xpos=t*sinphi;
            float ypos=t*cosphi;
            for (int s=-C; s<C; s++) {



            }
        }

    }
    return angles.size();
}
