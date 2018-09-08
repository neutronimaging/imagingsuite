//<LICENCE>

#include <iostream>

#include <tiffio.h>

#include "../../include/base/timage.h"
#include "../../include/base/KiplException.h"

namespace kipl { namespace io {

bool GetSlopeOffset(std::string msg, float &slope, float &offset)
{
	slope=1.0f;
	offset=0.0f;
	size_t pos=0;
	if ((pos=msg.find("slope = "))==std::string::npos)
		return false;
	slope = atof(msg.substr(pos+8).c_str());

	if ((pos=msg.find("offset = "))==std::string::npos)
		return false;

	offset = atof(msg.substr(pos+8).c_str());

	return true;
}

int KIPLSHARED_EXPORT GetTIFFDims(char const * const fname,size_t *dims)
{
	TIFF *image;
	//TIFFErrorHandler warn = TIFFSetWarningHandler(0);
	// Open the TIFF image
    if((image = TIFFOpen(fname, "r")) == nullptr){
        std::stringstream msg;
        msg.str("");
        msg<<"GetTIFFDims: Could not open "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	// We need to set some values for basic tags before we can add any data
	int nWidth=0;
	int nLength=0;
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&nWidth);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &nLength);
	
	dims[0]=static_cast<size_t>(nWidth);
	dims[1]=static_cast<size_t>(nLength);
    int frames=0;
    do {
            frames++;
    } while (TIFFReadDirectory(image));
	TIFFClose(image);

    return frames;
}





}}
