#ifndef MORPHSPOTCLEAN_H
#define MORPHSPOTCLEAN_H

#include <base/timage.h>
#include <morphology/morphology.h>
#include <string>
#include <iostream>

namespace ImagingAlgorithms {

enum eMorphCleanMethod {
    MorphCleanHoles = 0,
    MorphCleanPeaks,
    MorphCleanBoth
};

class MorphSpotClean
{
public:
    MorphSpotClean();
    void Process(kipl::base::TImage<float,2> &img, float th);

    void setConnectivity(kipl::morphology::MorphConnect conn = kipl::morphology::conn8);
    void setCleanMethod(eMorphCleanMethod mcm);

protected:
    void PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded);
    void UnpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img);
    kipl::morphology::MorphConnect m_eConnectivity;
    eMorphCleanMethod              m_eMorphClean;
    int m_nEdgeSmoothLength;
};

}

std::string enum2string(ImagingAlgorithms::eMorphCleanMethod mc);
std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::eMorphCleanMethod mc);
void string2enum(std::string str, ImagingAlgorithms::eMorphCleanMethod &mc);
#endif // MORPHSPOTCLEAN_H
