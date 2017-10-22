//<LICENSE>

#include "../include/piercingpointestimator.h"

#include <algorithm>

#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>

#include <base/tprofile.h>
#include <base/tsubimage.h>
#include <base/textractor.h>
#include <math/tnt_utils.h>
#include <math/statistics.h>

namespace ImagingAlgorithms {

PiercingPointEstimator::PiercingPointEstimator() :
    logger("PiercingPointEstimator")
{

}

pair<float,float> PiercingPointEstimator::operator()(kipl::base::TImage<float,2> &img, size_t *roi)
{
    size_t crop[4];

    if (roi==nullptr) {
        float marg=0.05;
        crop[0]=static_cast<size_t>(img.Size(0)*marg);
        crop[1]=static_cast<size_t>(img.Size(1)*marg);
        crop[2]=static_cast<size_t>(crop[0]+(1-2*marg)*img.Size(0));
        crop[3]=static_cast<size_t>(crop[1]+(1-2*marg)*img.Size(1));
    }
    else {
        std::copy(roi,roi+4,crop);
    }

    kipl::base::TImage<float,2> cimg=kipl::base::TSubImage<float,2>::Get(img,crop);

    ComputeEstimate(cimg);

    pair<float,float> position=LocateMax();
    std::ostringstream msg;
    msg.str("");
    msg<<"Got max "<<position.first<<", "<<position.second;
    logger(logger.LogMessage,msg.str());

    position.first  += crop[0];
    position.second += crop[1];

    msg.str("");
    msg<<"Adjusted max "<<position.first<<", "<<position.second;
    logger(logger.LogMessage,msg.str());
    return position;
}

pair<float,float> PiercingPointEstimator::operator()(kipl::base::TImage<float,2> &img,
                             kipl::base::TImage<float,2> &dc,
                             bool gaincorrection,
                             size_t *roi)
{
    correctedImage=img-dc;
    if (gaincorrection) {
        int N=static_cast<int>(img.Size(0));
        float *profile=new float[N];

        kipl::base::HorizontalProjection2D(correctedImage.GetDataPtr(), correctedImage.Dims(), profile, true);

        kipl::math::Statistics stats;
        for (int i=1; i<N; ++i) {
            profile[i]-=profile[i-1];
            stats.put(profile[i]);
        }
        profile[0]=profile[1];
        profile[0]= abs(profile[0]) < 2.5*stats.s() ? 0 : profile[0];
        for (int i=1; i<N; ++i) {
            profile[i]= abs(profile[i]) < 2.5*stats.s() ? profile[i-1] : profile[i]+=profile[i-1];
        }

        float *pLine=nullptr;

        for (int y=0; y<static_cast<int>(correctedImage.Size(1)); ++y) {
            pLine=correctedImage.GetLinePtr(y);
            for (int x=0; x<N; ++x) {
                pLine[x]-=profile[x];
            }
        }

        delete [] profile;
    }

    return operator()(correctedImage,roi);
}

void PiercingPointEstimator::ComputeEstimate(kipl::base::TImage<float,2> &img)
{
    int matrix_cols = 6;

    Array2D< double > H(img.Size(),matrix_cols, 0.0);
    Array1D< double > I(img.Size(), 0.0);

    size_t i=0;
    for (double y=0; y<img.Size(1); ++y) {
        for (double x=0; x<img.Size(0); ++x,++i) {
                  H[i][0] = 1;
                  H[i][1] = x;
                  H[i][2] = y;
                  H[i][3] = x*y;
                  H[i][4] = x*x;
                  H[i][5] = y*y;
                  I[i]    = img[i];
            }

    }

    JAMA::QR<double> qr(H);
    parameters = qr.solve(I);

    std::ostringstream msg;
    msg<<parameters[0]<<", "
       <<parameters[1]<<", "
       <<parameters[2]<<", "
       <<parameters[3]<<", "
       <<parameters[4]<<", "
       <<parameters[5];

    logger(logger.LogMessage,msg.str());

}

/// Finding the coordinates of the max
/// $I(x,y) = a + b x + c y + d xy + e x^2 + f y^2$
///
///
/// $\partial I(x,y)/\partial x = b + d y + 2e * x = 0$
///
///  $\partial I(x,y)/ \partial y = c + d x + 2f * y = 0$
///
///  $\left(\begin{array}{cc}2e &d\\ d & 2f\end{array}\right) \left(\begin{array}{c}x \\y\end{array}\right)=\left(\begin{array}{c}-b\\-c\end{array}\right)$
pair<float,float> PiercingPointEstimator::LocateMax()
{
    std::ostringstream msg;
    logger(logger.LogMessage,"Enter locate max");
    pair<float,float> position;

    Array2D< float > H(2,2, 0.0);

    H[0][0] = 2*parameters[4];
    H[0][1] =   parameters[3];
    H[1][0] =   parameters[3];
    H[1][1] = 2*parameters[5];

    Array1D< float > I(2, 0.0);

    I[0]    =  -parameters[1];
    I[1]    =  -parameters[2];

    logger(logger.LogMessage,"Initialize");
    JAMA::QR<float> qr(H);
    Array1D< float > pos(2, 0.0);
    pos = qr.solve(I);

    logger(logger.LogMessage,"Equation solved");
    position = make_pair(pos[0], pos[1]);

    msg.str("");
    msg<<"Got pair "<<position.first<<", "<<position.second<<", ";
    logger(logger.LogMessage,msg.str());
    return position;
}

}
