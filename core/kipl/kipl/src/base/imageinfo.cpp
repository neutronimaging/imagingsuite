

#include <string>
#include <iostream>

#include "../../include/base/imageinfo.h"

std::ostream & operator<<(std::ostream &s, kipl::base::ImageInfo &info)
{
	s<<"Resolution x:"<<info.GetMetricX()<<", y:"<<info.GetMetricY()<<"\n"
		<<"Description : "<<info.sDescription<<"\n"
		<<"Software  : "<<info.sSoftware<<"\n"
		<<"Artist    : "<<info.sArtist<<"\n"
		<<"Copyright : "<<info.sCopyright<<std::endl;

	return s;
}


