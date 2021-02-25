//<LICENSE>

#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <string>

#include <base/timage.h>
#include <base/kiplenums.h>
#include <logging/logger.h>
#include "readerconfig_global.h"

class READERCONFIGSHARED_EXPORT ImageWriter
{
    kipl::logging::Logger logger;

public:
    ImageWriter();
    ~ImageWriter();

    void write(kipl::base::TImage<float,2> &img, std::string fname, kipl::base::eDataType dt = kipl::base::UInt16);
};

#endif // IMAGEWRITER_H
