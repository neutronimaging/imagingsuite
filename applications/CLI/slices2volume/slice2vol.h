#ifndef SLICE2VOL_H
#define SLICE2VOL_H

#include <string>

class Slice2Vol
{
public:
    Slice2Vol();

    void process(std::string filemask, size_t first, size_t last, size_t bin, std::string destname);
};

#endif // SLICE2VOL_H
