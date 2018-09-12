//<LICENCE>

#ifndef __IO_TIFF_H
#define __IO_TIFF_H

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sstream>

#include <tiffio.h>

#include "../base/imagecast.h"
#include "../base/timage.h"
#include "../base/imageinfo.h"
#include "../strings/filenames.h"
#include "../base/kiplenums.h"


namespace kipl { namespace io {

/// \brief Parses a string to find slope and offset.
///
/// This string could come the comment field in a tiff file created by the Octopus CT recontruction software.
/// It is intended to rescale the intensity of the graylevels in a tiff image.
/// \param msg The string to parse
/// \param slope the slope value extracted from the comment string
/// \param offset the offset
/// \returns If scaling information was found
/// \retval True if both parameters were found
/// \retval False if at least one parameter was missing.
bool GetSlopeOffset(std::string msg, float &slope, float &offset);

/// \brief Writes an uncompressed TIFF image from any image data type (grayscale)
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .tif)
///	\param lo lower limit on the data dynamics
///	\param hi upper limit on the data dynamics
///
///	Setting both bounds to zero results in full dynamics rescaled in the interval [0,255].
///
///	\return Error code 	
///	\retval 0 The writing failed
///	\retval 1 Successful 	
template <class ImgType>
int WriteTIFF(kipl::base::TImage<ImgType,2> src,const char *fname)
{
	TIFF *image;
	kipl::base::TImage<unsigned short,2> tmp;
	std::stringstream msg;
	tmp=kipl::base::ImageCaster<unsigned short, ImgType, 2>::cast(src);
		
//	TIFFErrorHandler warn = TIFFSetWarningHandler(0);
	// Open the TIFF file
	if((image = TIFFOpen(fname, "w")) == NULL){
		msg.str("");
		msg<<"WriteTIFF: Could not open "<<fname<<" for writing";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

	// We need to set some values for basic tags before we can add any data
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH,         static_cast<int>(src.Size(0)));
    TIFFSetField(image, TIFFTAG_IMAGELENGTH,        static_cast<int>(src.Size(1)));
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE,      16);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL,    1);
    TIFFSetField(image, TIFFTAG_SAMPLEFORMAT,       1);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP,       src.Size(1));
	
    TIFFSetField(image, TIFFTAG_COMPRESSION,        COMPRESSION_NONE);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC,        PHOTOMETRIC_MINISBLACK);
    TIFFSetField(image, TIFFTAG_FILLORDER,          FILLORDER_MSB2LSB);
    TIFFSetField(image, TIFFTAG_PLANARCONFIG,       PLANARCONFIG_CONTIG);
	
    TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT,     RESUNIT_CENTIMETER);
    TIFFSetField(image, TIFFTAG_XRESOLUTION,        src.info.GetDPCMX());
    TIFFSetField(image, TIFFTAG_YRESOLUTION,        src.info.GetDPCMY());

    TIFFSetField(image, TIFFTAG_COPYRIGHT,          src.info.sCopyright.c_str());
    TIFFSetField(image, TIFFTAG_ARTIST,             src.info.sArtist.c_str());
    TIFFSetField(image, TIFFTAG_SOFTWARE,           src.info.sSoftware.c_str());
	if (src.info.sDescription.empty()) {

        TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, "slope = 1.0E0\noffset = 0.0E0");
	}
	else
        TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, src.info.sDescription.c_str());
	
	// Write the information to the file
    TIFFWriteEncodedStrip(image, 0, tmp.GetDataPtr(), src.Size()*sizeof(unsigned short));
	
	// Close the file
	TIFFClose(image);
	
	return 1;
}

template <class ImgType>
int WriteTIFF(kipl::base::TImage<ImgType,2> src,const char *fname, ImgType lo, ImgType hi)
{
	kipl::base::TImage<unsigned short,2> img;
	try {
		img=kipl::base::ImageCaster<unsigned short, ImgType, 2>::cast(src,lo,hi);
		if (src.info.sDescription.empty()) {
			std::stringstream msg;

			float slope = (static_cast<float>(hi)-static_cast<float>(lo))/std::numeric_limits<unsigned short>::max();

			msg.precision(5);
            msg<<"slope = "<<scientific<<slope<<"\noffset = "<<scientific<<lo;
			src.info.sDescription=msg.str();
		}
	}
	catch (kipl::base::KiplException &E) {
		throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
	}

	return WriteTIFF(img,fname);
}

/// \brief Reads the contents of a tiff file and stores the contents in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file
///	
///	\return Error code 	
///	\retval 0 The writing failed
///	\retval 1 Successful 
template <class ImgType>
int ReadTIFF(kipl::base::TImage<ImgType,2> &src,const char *fname, size_t idx=0L)
{
    (void)idx; // Terminating unused parameter

    std::stringstream msg;
	TIFF *image;
	uint16 photo, spp, fillorder,bps, sformat;
	tsize_t stripSize, stripCount;
	unsigned long imageOffset;
	long result;
	int stripMax;
	unsigned char *buffer, tempbyte;
	unsigned long bufferSize, count;

	TIFFSetWarningHandler(0);
	// Open the TIFF image
	if((image = TIFFOpen(fname, "r")) == NULL){
		msg.str();
		msg<<"ReadTIFF: Could not open image "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
	// Check that it is of a type that we support
	if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) ){
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}
	if((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sformat) == 0) ){
		sformat=1; // Assuming unsigned integer data if unknown
	}
	if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) || (spp != 1)){

		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// Read in the possibly multiple strips
	stripSize = TIFFStripSize (image);
	stripMax = TIFFNumberOfStrips (image);
	imageOffset = 0;

	bufferSize = TIFFNumberOfStrips (image) * stripSize;
	try {
		if((buffer = new unsigned char[bufferSize]) == NULL){
			msg.str("");
			msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image";
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	}
	catch (std::bad_alloc & E) {
		msg.str("");
		msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image ("<<E.what()<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &E) {
		throw E;
	}


	int dimx,dimy;
	// We need to set some values for basic tags before we can add any data
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&dimx);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &dimy);

	for (stripCount = 0; stripCount < stripMax; stripCount++){
		if((result = TIFFReadEncodedStrip (image, stripCount,
					      buffer + imageOffset,
					      stripSize)) == -1){
			msg.str("");
			msg<<"Read error on input strip number "<<static_cast<size_t>(stripCount);
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
    	imageOffset += result;
  	}

	// Deal with photometric interpretations
	if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0){
		throw kipl::base::KiplException("Image has an undefined photometric interpretation",__FILE__,__LINE__);
	}
	
	if(photo == PHOTOMETRIC_MINISWHITE){
		// Flip bits
        std::clog<<"Fixing the photometric interpretation"<<std::endl;
		
		for(count = 0; count < bufferSize; count++)
			buffer[count] = ~buffer[count];
	}
	
	// Deal with fillorder
	if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0){
		fillorder=FILLORDER_MSB2LSB;
	}
	
	if(fillorder != FILLORDER_MSB2LSB){
		// We need to swap bits -- ABCDEFGH becomes HGFEDCBA
        std::clog<<"Fixing the fillorder"<<std::endl;
		
		for(count = 0; count < bufferSize; count++){
			tempbyte = 0;
			if(buffer[count] & 128) tempbyte += (char)1;
			if(buffer[count] & 64) tempbyte += (char)2;
			if(buffer[count] & 32) tempbyte += (char)4;
			if(buffer[count] & 16) tempbyte += (char)8;
			if(buffer[count] & 8) tempbyte += (char)16;
			if(buffer[count] & 4) tempbyte += (char)32;
			if(buffer[count] & 2) tempbyte += (char)64;
			if(buffer[count] & 1) tempbyte += (char)128;
			buffer[count] = tempbyte;
		}
	}

	char *tmpstr[1024];
	if (TIFFGetField(image,TIFFTAG_ARTIST,tmpstr))
		src.info.sArtist=tmpstr[0];
	if (TIFFGetField(image,TIFFTAG_COPYRIGHT,tmpstr))
		src.info.sCopyright=tmpstr[0];
	if (TIFFGetField(image,270,tmpstr)) // Description
		src.info.sDescription=tmpstr[0];
	if (TIFFGetField(image,TIFFTAG_SOFTWARE,tmpstr))
		src.info.sSoftware=tmpstr[0];


	int resunit=0;
	TIFFGetField(image, TIFFTAG_RESOLUTIONUNIT, &resunit);
	float resX,resY;
	TIFFGetField(image,TIFFTAG_XRESOLUTION,&resX);
    TIFFGetField(image,TIFFTAG_YRESOLUTION,&resY);

	switch (resunit) {
		case RESUNIT_NONE :
		case RESUNIT_INCH :
			src.info.SetDPIX(resX);
			src.info.SetDPIY(resY);
			break;
		case RESUNIT_CENTIMETER : 
			src.info.SetDPCMX(resX);
			src.info.SetDPCMY(resY);
			break;
	}

	TIFFClose(image);
	size_t dims[]={static_cast<size_t>(dimx), static_cast<size_t>(dimy)};
	src.Resize(dims);

	const size_t cnSize=src.Size();
	switch (bps) {
	case 32:
		switch (sformat) {
		case 1: // Unsigned integer
			for (size_t i=0; i<cnSize; i++)
				src[i]=static_cast<ImgType>((reinterpret_cast<int *>(buffer))[i]);
			break;
		case 2: // Signed integer
			for (size_t i=0; i<cnSize; i++)
				src[i]=static_cast<ImgType>((reinterpret_cast<unsigned int *>(buffer))[i]);
			break;
		case 3: // IEEE floating point
			for (size_t i=0; i<cnSize; i++)
				src[i]=static_cast<ImgType>((reinterpret_cast<float *>(buffer))[i]);
			break;
		default:
			break;
		}
		break;
	case 16:
		for (size_t i=0; i<cnSize; i++) 
			src[i]=(reinterpret_cast<unsigned short *>(buffer))[i];
		break;
	case 8:	
		for (size_t i=0; i<cnSize; i++) 
			src[i]=buffer[i];
		break;
	case 4:
		for (size_t i=0, j=0; i<cnSize; i+=2, j++) {
			src[i]=static_cast<ImgType>(buffer[j]>>4);
			src[i+1]=static_cast<ImgType>(buffer[j] & 15);
		}
		break;
	}

    src.info.nBitsPerSample=bps;
	delete [] buffer;

	return bps;
}

/// \brief Reads the contents of a tiff file and stores the contents in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .bmp)
///	
///	\return Error code 	
///	\retval 0 The writing failed
///	\retval 1 Successful 
template <class ImgType>
int ReadTIFF(kipl::base::TImage<ImgType,2> &src,const char *fname, size_t const * const crop, size_t idx=0L)
{
    (void)idx; // Terminating unused parameter warning

	if (crop==NULL) {
		return ReadTIFF(src,fname);
	}
	std::stringstream msg;
	TIFF *image;
	uint16 photo, spp, fillorder,bps, sformat;
	tsize_t stripSize;
	unsigned long imageOffset;

	int stripMax;
	unsigned char *buffer, tempbyte;
	unsigned long bufferSize, count;

	TIFFSetWarningHandler(0);
	// Open the TIFF image
	if((image = TIFFOpen(fname, "r")) == NULL){
		msg.str();
		msg<<"ReadTIFF: Could not open image "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
	// Check that it is of a type that we support
	if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) ){
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}
	if((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sformat) == 0) ){
		sformat=1; // Assuming unsigned integer data if unknown
	}
	if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) || (spp != 1)){
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// Read in the possibly multiple strips
	stripSize = TIFFStripSize (image);
	stripMax = TIFFNumberOfStrips (image);
	imageOffset = 0;

	int dimx,dimy;
	// We need to set some values for basic tags before we can add any data
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&dimx);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &dimy);
    int adjcrop[4]={static_cast<int>(min(static_cast<size_t>(dimx),crop[0])),
                    static_cast<int>(min(static_cast<size_t>(dimy),crop[1])),
                    static_cast<int>(min(static_cast<size_t>(dimx),crop[2])),
                    static_cast<int>(min(static_cast<size_t>(dimy),crop[3]))};
	if ((adjcrop[2]-adjcrop[0])==0) kipl::base::KiplException("Failed to crop image in X",__FILE__,__LINE__);
	if ((adjcrop[3]-adjcrop[1])==0) kipl::base::KiplException("Failed to crop image in Y",__FILE__,__LINE__);
	
    size_t imgdims[2]={static_cast<size_t>(adjcrop[2]-adjcrop[0]),static_cast<size_t>(adjcrop[3]-adjcrop[1])};

	bufferSize = TIFFScanlineSize(image);
	try {
        if((buffer = new unsigned char[bufferSize]) == NULL) {
			msg.str("");
			msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image";
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	}
	catch (std::bad_alloc & E) {
		msg.str("");
		msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image ("<<E.what()<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &E) {
		throw E;
	}

	src.Resize(imgdims);
	for (int row=adjcrop[1]; row<adjcrop[3]; row++) {
 		if (TIFFReadScanline(image,(tdata_t) buffer, row, 0)!=1) {
			msg.str("");
			msg<<"ReadTIFFLine: an error occurred during reading scan line "<<row;
			TIFFClose(image);
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
		// Deal with photometric interpretations
		if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0){
			throw kipl::base::KiplException("Image has an undefined photometric interpretation",__FILE__,__LINE__);
		}
		
		if(photo == PHOTOMETRIC_MINISWHITE){
			// Flip bits
            std::clog<<"Fixing the photometric interpretation"<<std::endl;
			
			for(count = 0; count < bufferSize; count++)
				buffer[count] = ~buffer[count];
		}
		
		// Deal with fillorder
		if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0){
			fillorder=FILLORDER_MSB2LSB;
		}
		
		if(fillorder != FILLORDER_MSB2LSB){
			// We need to swap bits -- ABCDEFGH becomes HGFEDCBA
            std::clog<<"Fixing the fillorder"<<std::endl;
			
			for(count = 0; count < bufferSize; count++){
				tempbyte = 0;
				if(buffer[count] & 128) tempbyte += (char)1;
				if(buffer[count] & 64) tempbyte += (char)2;
				if(buffer[count] & 32) tempbyte += (char)4;
				if(buffer[count] & 16) tempbyte += (char)8;
				if(buffer[count] & 8) tempbyte += (char)16;
				if(buffer[count] & 4) tempbyte += (char)32;
				if(buffer[count] & 2) tempbyte += (char)64;
				if(buffer[count] & 1) tempbyte += (char)128;
				buffer[count] = tempbyte;
			}
		}

		ImgType *pLine=src.GetLinePtr(row-adjcrop[1]);

		switch (bps) {
			case 32:
				switch (sformat) {
				case 1: // Unsigned integer
					for (int i=adjcrop[0]; i<adjcrop[2]; i++)
						pLine[i-adjcrop[0]]=static_cast<ImgType>((reinterpret_cast<int *>(buffer))[i]);
					break;
				case 2: // Signed integer
					for (int i=adjcrop[0]; i<adjcrop[2]; i++)
						pLine[i-adjcrop[0]]=static_cast<ImgType>((reinterpret_cast<unsigned int *>(buffer))[i]);
					break;
				case 3: // IEEE floating point
					for (int i=adjcrop[0]; i<adjcrop[2]; i++)
						pLine[i-adjcrop[0]]=static_cast<ImgType>((reinterpret_cast<float *>(buffer))[i]);
					break;
				default:
					break;
				}
				break;
			case 16:
				for (int i=adjcrop[0]; i<adjcrop[2]; i++)
					pLine[i-adjcrop[0]]=(reinterpret_cast<unsigned short *>(buffer))[i];
				break;
			case 8:	
				for (int i=adjcrop[0]; i<adjcrop[2]; i++)
					pLine[i-adjcrop[0]]=buffer[i];
				break;
			case 4:
				TIFFClose(image);
				delete [] buffer;
				throw kipl::base::KiplException("4-bit TIFF images are not supported in crop mode",__FILE__,__LINE__);

				break;
			}
	}
    src.info.nBitsPerSample=bps;

	char *tmpstr[1024];
	if (TIFFGetField(image,TIFFTAG_ARTIST,tmpstr))
		src.info.sArtist=tmpstr[0];
	if (TIFFGetField(image,TIFFTAG_COPYRIGHT,tmpstr))
		src.info.sCopyright=tmpstr[0];
	if (TIFFGetField(image,270,tmpstr)) // Description
		src.info.sDescription=tmpstr[0];
	if (TIFFGetField(image,TIFFTAG_SOFTWARE,tmpstr))
		src.info.sSoftware=tmpstr[0];


	int resunit=0;
	TIFFGetField(image, TIFFTAG_RESOLUTIONUNIT, &resunit);
	float resX,resY;
	TIFFGetField(image,TIFFTAG_XRESOLUTION,&resX);
    TIFFGetField(image,TIFFTAG_YRESOLUTION,&resY);

	switch (resunit) {
		case RESUNIT_NONE :
		case RESUNIT_INCH :
			src.info.SetDPIX(resX);
			src.info.SetDPIY(resY);
			break;
		case RESUNIT_CENTIMETER : 
			src.info.SetDPCMX(resX);
			src.info.SetDPCMY(resY);
			break;
	}

	TIFFClose(image);
		
	delete [] buffer;

	return bps;
}


/// \brief Writes an uncompressed TIFF image from any image data type (grayscale)
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .tif)
///
///	\return Error code
///	\retval 0 The writing failed
///	\retval 1 Successful
int KIPLSHARED_EXPORT WriteTIFF32(kipl::base::TImage<float,2> src,const char *fname);

/// \brief Gets the dimensions of a tiff image without reading the image
/// \param fname file name of the image file
/// \param dims array with the dimensions
int KIPLSHARED_EXPORT GetTIFFDims(char const * const fname, size_t *dims);

/// \brief Reads a single line out of a tiff file
/// \param data buffer array for the data
/// \param row indexes the line to read
/// \param fname file name of the image file
template <class ImgType>
int ReadTIFFLine(ImgType *data,uint32 row, const char *fname) 
{
	std::stringstream msg;
	TIFF *image;
//	 TIFFErrorHandler warn = 
	TIFFSetWarningHandler(0);
	if((image = TIFFOpen(fname, "r")) == NULL){
		msg.str("");
		msg<<"ReadTIFFLine: Could not open "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
 	if (TIFFReadScanline(image,(tdata_t) data, row, 0)!=1) {
		msg.str("");
		msg<<"ReadTIFFLine: an error occured during reading scan line "<<row;
		TIFFClose(image);
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	TIFFClose(image);

	return 1;
}



}}
#endif
