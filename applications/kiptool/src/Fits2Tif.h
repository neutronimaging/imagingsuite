//<LICENSE>

#ifndef __FITS2TIF_H
#define __FITS2TIF_H

#include <string>
#include <list>

#include <base/timage.h>
#include <logging/logger.h>

#include "ImagingToolConfig.h"


class Fits2Tif {
    kipl::logging::Logger logger;
public:
    Fits2Tif();

    ~Fits2Tif();

    int process(ImagingToolConfig::FileConversionConfig &config);

    int GetImage(std::list<kipl::base::TImage<float,2> > &imglist, std::string fname, ImagingToolConfig::FileConversionConfig &config);

    std::string BuildFileList(ImagingToolConfig::FileConversionConfig &config, std::list<std::string> &filelist, bool bReversedIndex);
private:
    ImagingToolConfig::FileConversionConfig m_Config;
};
#endif
