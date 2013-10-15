#ifndef TRANSLATEPROJECTIONS_H
#define TRANSLATEPROJECTIONS_H

#include <base/timage.h>
#include <iostream>
#include <string>

namespace ImagingAlgorithms {

enum eTranslateSampling {
    TranslateNearest = 0,
    TranslateLinear,
    TranslateGaussian
};

class TranslateProjections
{
public:
    TranslateProjections();

    void translate(kipl::base::TImage<float, 3> &img, float slope, float intercept, eTranslateSampling sampler=TranslateLinear, float parameter=0.0f);

protected:
    void translateNearestNeighbor(kipl::base::TImage<float,2> &img, float shift);
    void translateLinear(kipl::base::TImage<float,2> &img, float shift);
    void translateGaussian(kipl::base::TImage<float,2> &img, float shift, float width);

};

}

std::string enum2string(ImagingAlgorithms::eTranslateSampling e);
void string2enum(std::string s, ImagingAlgorithms::eTranslateSampling &e);
std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::eTranslateSampling e);
#endif // TRANSLATEPROJECTIONS_H
