/*
 * Fits2Tif.h
 *
 *  Created on: Oct 22, 2012
 *      Author: kaestner
 */

#ifndef FITS2TIF_H_
#define FITS2TIF_H_

#include <logging/logger.h>
#include <base/timage.h>
#include "ImagingToolConfig.h"


class Fits2Tif {
	kipl::logging::Logger logger;
public:
	Fits2Tif();
	virtual ~Fits2Tif();
    int process(ImagingToolConfig::FileConversionConfig &config);
    int GetImage(std::list<kipl::base::TImage<float,2> > &imglist, std::string fname, ImagingToolConfig::FileConversionConfig &config);

	float progress() {return 0.0f;}
protected:
    std::string BuildFileList(ImagingToolConfig::FileConversionConfig &config, std::list<std::string> &filelist,bool bReversedIndex=false);
    int PrepareImage(kipl::base::TImage<float,2> &img);
};

#endif /* FITS2TIF_H_ */
