#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <string>

#include <base/timage.h>

class ImageWriter
{
public:
    ImageWriter();
    ~ImageWriter();

    void write(kipl::base::TImage<float,2> &img, std::string fname);
};

#endif // IMAGEWRITER_H
