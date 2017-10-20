//<LICENSE>

#ifndef TRANSLATEPROJECTIONS_H
#define TRANSLATEPROJECTIONS_H

#include "ImagingAlgorithms_global.h"

#include <iostream>
#include <string>

#include <base/timage.h>

namespace ImagingAlgorithms {

enum eTranslateSampling {
    TranslateNearest = 0,
    TranslateLinear,
    TranslateGaussian
};

class IMAGINGALGORITHMSSHARED_EXPORT TranslateProjections
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

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eTranslateSampling e);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string s, ImagingAlgorithms::eTranslateSampling &e);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &s, ImagingAlgorithms::eTranslateSampling e);
#endif // TRANSLATEPROJECTIONS_H
