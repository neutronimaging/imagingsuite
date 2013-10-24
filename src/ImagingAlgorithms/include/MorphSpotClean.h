#ifndef MORPHSPOTCLEAN_H
#define MORPHSPOTCLEAN_H

#include <base/timage.h>

class MorphSpotClean
{
public:
    MorphSpotClean();
    void Process(kipl::base::TImage<float,2> &img);

};

#endif // MORPHSPOTCLEAN_H
