#ifndef SPOTGENERATOR_H
#define SPOTGENERATOR_H

#include "../base/timage.h"

class SpotGenerator
{
public:
    enum BackgroundTypes {
        BackgroundFlat,
        BackgroundGradientX,
        BackgroundGradientY,
        BackgroundGradientXY
    };

    enum SpotShapes {
        SpotSquare,
        SpotRound,
        SpotGauss
    };

    enum InsertMethods {
        InsertReplace,
        InsertAdd,
        InsertMultiply
    };

    SpotGenerator();
    ~SpotGenerator();
    kipl::base::TImage<float,2> SpotLines(size_t *dims,
                              float *width,
                              float *ampl,
                              SpotShapes ss=SpotSquare,
                              InsertMethods ins=InsertReplace,
                              BackgroundTypes bg=BackgroundFlat,
                              float noise=0.0f);

private:
    kipl::base::TImage<float,2> m_Img;
    void InsertByReplace(kipl::base::TImage<float,2> &spot, int x, int y);
    void InsertByAdd(kipl::base::TImage<float,2> &spot, int x, int y);
    void InsertByMultiply(kipl::base::TImage<float,2> &spot, int x, int y);



};

#endif // SPOTGENERATOR_H
