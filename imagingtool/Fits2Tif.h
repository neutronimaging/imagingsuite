/*
 * Fits2Tif.h
 *
 *  Created on: Oct 22, 2012
 *      Author: kaestner
 */

#ifndef FITS2TIF_H_
#define FITS2TIF_H_

#include <logging/logger.h>
#include "imagingtoolconfig.h"


class Fits2Tif {
	kipl::logging::Logger logger;
public:
	Fits2Tif();
	virtual ~Fits2Tif();
    int process(ImagingToolConfig::Fits2TifConfig &config);

	float progress() {return 0.0f;}
protected:
    void BuildFileList(ImagingToolConfig::Fits2TifConfig &config, std::list<std::string> &filelist);

};

#endif /* FITS2TIF_H_ */
