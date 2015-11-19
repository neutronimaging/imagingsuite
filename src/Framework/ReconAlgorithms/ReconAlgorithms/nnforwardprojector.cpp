#include <iostream>
#include <math/mathconstants.h>
#include <math/numfunc.h>

#include "nnforwardprojector.h"

NNForwardProjector::NNForwardProjector()
{

}

NNForwardProjector::~NNForwardProjector()
{

}

int NNForwardProjector::project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj)
{
    size_t dims[2]={max(slice.Size(0),slice.Size(1)), angles.size()};
    proj.Resize(dims);

    buildMask(slice.Dims());

    int proj_line=0;

    for (auto it=angles.begin(); it!=angles.end(); it++, proj_line++) {
        // Angles
        float theta = *it;           // Acquisition angle;
        float theta_rad = theta*fPi/180.0f;

        float *pProj=proj.GetLinePtr(proj_line);
        memset(pProj,0,sizeof(float)*proj.Size(0));
        if ((static_cast<int>((theta+45.0f)/90.0f) & 1) == 1) { // Check if rows or cols are preferred

            const float dx=(1.0f/sin(theta_rad));    // Row position increment;

            for (int row = 0; row < static_cast<int>(slice.Size(1)); row++) { // Iterate over the rows in the slice
                float *pRow=slice.GetLinePtr(row);
                float h=row-m_cy;
                float width=sqrt(m_cy*m_cy-h*h);   // Half width

                //float pc=m_cy*(sin(theta_rad)-cos(theta_rad))+m_cy + cos(theta_rad)*h;
                float pc=m_cy*(sin(theta_rad)-cos(theta_rad)) - cos(theta_rad)*h;

                float dp=width*sin(theta_rad);

                int p0=pc-dp;
                int p1=pc+dp;

//                if (p1<p0)
//                    swap(p0,p1);

//                const float m=m_cx - kipl::math::sgn(dx)*width;               // Position on row in slice
                const float m=m_cx - width;               // Position on row in slice
                for (int p=p0, i=0; p<=p1; p++,i++) {        // this is where more fancy weighting schemes are expected
                        int rowpos=static_cast<int>(floor(m+i*dx));
                        if ((0<=rowpos) && (rowpos<dims[1]))
                            pProj[p]+=pRow[rowpos];
                }
            }
        }
        else {

        }
    }

    return 0;
}
