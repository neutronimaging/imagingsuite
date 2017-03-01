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
	if((image = TIFFOpen(fname, "r")) == NULL){
		TIFFClose(image);
		throw kipl::base::KiplException("GetTIFFDims: Could not open image.",__FILE__,__LINE__);
	}
	// We need to set some values for basic tags before we can add any data
	int nWidth=0;
	int nLength=0;
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&nWidth);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &nLength);
	
	dims[0]=static_cast<size_t>(nWidth);
	dims[1]=static_cast<size_t>(nLength);
	TIFFClose(image);

	return 0;
}

int KIPLSHARED_EXPORT WriteTIFF32(kipl::base::TImage<float,2> src,const char *fname)
{
	TIFF *image;
	std::stringstream msg;

	// Open the TIFF file
	if((image = TIFFOpen(fname, "w")) == NULL){
		msg.str("");
		msg<<"WriteTIFF: Could not open "<<fname<<" for writing";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

	// We need to set some values for basic tags before we can add any data
	TIFFSetField(image, TIFFTAG_IMAGEWIDTH, static_cast<int>(src.Size(0)));
	TIFFSetField(image, TIFFTAG_IMAGELENGTH, static_cast<int>(src.Size(1)));
	TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 32); // 32bits
	TIFFSetField(image, TIFFTAG_SAMPLEFORMAT, 3);   // IEEE floating point
	TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, src.Size(1));

	TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
	TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(image, TIFFTAG_XRESOLUTION, src.info.GetDPCMX());
	TIFFSetField(image, TIFFTAG_YRESOLUTION, src.info.GetDPCMY());
	TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_CENTIMETER);
	if (!src.info.sDescription.empty()) {
		TIFFSetField(image, 270, src.info.sDescription.c_str());
	}

	// Write the information to the file
	TIFFWriteEncodedStrip(image, 0, src.GetDataPtr(), src.Size()*sizeof(float));

	// Close the file
	TIFFClose(image);

	return 1;
}


}}
