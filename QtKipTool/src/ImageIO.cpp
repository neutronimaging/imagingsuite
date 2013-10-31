#include "stdafx.h"
#include <KiplProcessConfig.h>
#include <KiplFrameworkException.h>
#include <base/KiplException.h>
#include <sstream>
#include <string>
#include <base/timage.h>

kipl::base::TImage<float,3> LoadVolumeImage(KiplProcessConfig & config)
{

	std::ostringstream msg;
	std::string fname=config.mImageInformation.sSourcePath+config.mImageInformation.sSourceFileMask;

	kipl::base::TImage<float,3> img;
	try {	
		if (config.mImageInformation.bUseROI==true) {
			kipl::io::ReadImageStack(img,fname,
				config.mImageInformation.nFirstFileIndex,
				config.mImageInformation.nLastFileIndex-config.mImageInformation.nFirstFileIndex,1,
				config.mImageInformation.nROI);
		}
		else {
			kipl::io::ReadImageStack(img,fname,
				config.mImageInformation.nFirstFileIndex,
				config.mImageInformation.nLastFileIndex-config.mImageInformation.nFirstFileIndex,1,
				NULL);
		}
	}
	catch (kipl::base::KiplException &e) {
		msg<<"KiplException with message: "<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		msg<<"STL Exception with message: "<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);	
	}
	catch (...) {
		msg<<"Unknown exception thrown while reading image";
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}

	return img;
}