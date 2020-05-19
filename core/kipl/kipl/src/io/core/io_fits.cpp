//<LICENCE>

#include <exception>
#include <sstream>
#include <stdint.h>
#include <vector>

#include <fitsio.h>

#include "../../../include/base/KiplException.h"

namespace kipl { namespace io {
int KIPLSHARED_EXPORT FITSDataType(unsigned char x) 	{ (void)x; return     TBYTE; }
int KIPLSHARED_EXPORT FITSDataType(char x)           	{ (void)x; return    TSBYTE; }
int KIPLSHARED_EXPORT FITSDataType(unsigned short x) 	{ (void)x; return   TUSHORT; }
int KIPLSHARED_EXPORT FITSDataType(short x) 			{ (void)x; return    TSHORT; }
//int KIPLSHARED_EXPORT FITSDataType(int32_t x) 		    { return      TINT; }
//int KIPLSHARED_EXPORT FITSDataType(uint32_t x) 	        { return     TUINT; }
int KIPLSHARED_EXPORT FITSDataType(int32_t x) 			{ (void)x; return     TLONG; }
int KIPLSHARED_EXPORT FITSDataType(uint32_t x)       	{ (void)x; return    TULONG; }
int KIPLSHARED_EXPORT FITSDataType(int64_t x)			{ (void)x; return TLONGLONG; }
int KIPLSHARED_EXPORT FITSDataType(float x) 			{ (void)x; return    TFLOAT; }
int KIPLSHARED_EXPORT FITSDataType(double x) 			{ (void)x; return   TDOUBLE; }

int KIPLSHARED_EXPORT FITSPixelSize(unsigned char x)    { (void)x; return     BYTE_IMG; }    // 8-bit unsigned integers
int KIPLSHARED_EXPORT FITSPixelSize(char x)             { (void)x; return    SBYTE_IMG; }    // 8-bit signed integers, equivalent to BITPIX = 8, BSCALE = 1, BZERO = -128
int KIPLSHARED_EXPORT FITSPixelSize(short x)            { (void)x; return    SHORT_IMG; }    // 16-bit signed integers
int KIPLSHARED_EXPORT FITSPixelSize(unsigned short x)   { (void)x; return   USHORT_IMG; }    // 16-bit unsigned integers, equivalent to  BITPIX = 16, BSCALE = 1, BZERO = 32768
int KIPLSHARED_EXPORT FITSPixelSize(int32_t x)          { (void)x; return     LONG_IMG; }    // 32-bit signed integers
int KIPLSHARED_EXPORT FITSPixelSize(uint32_t x)         { (void)x; return    ULONG_IMG; }    // 32-bit unsigned integers, equivalent to BITPIX = 32, BSCALE = 1, BZERO = 2147483648
int KIPLSHARED_EXPORT FITSPixelSize(int64_t x)          { (void)x; return LONGLONG_IMG; }    // 64-bit signed integers
int KIPLSHARED_EXPORT FITSPixelSize(float x)            { (void)x; return    FLOAT_IMG; }    // 32-bit single precision floating point
int KIPLSHARED_EXPORT FITSPixelSize(double x)           { (void)x; return   DOUBLE_IMG; }    // 64-bit double precision floating point


std::vector<size_t> KIPLSHARED_EXPORT GetFITSDims(const std::string &filename)
{
	fitsfile *fptr;
	int status=0;
	std::ostringstream msg;
    fits_open_image(&fptr, filename.c_str(), READONLY, &status);
	if (status!=0) {
		msg<<"Failed to open '"<<filename<<"' in GetFITSDims";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	int bitpix=0;
	int naxis;
    long naxes[8];

	try {
        fits_get_img_param(fptr, 8,  &bitpix, &naxis, naxes, &status);
	}
	catch (std::exception &e) {
		fits_close_file(fptr,&status);
		throw kipl::base::KiplException(e.what(), __FILE__, __LINE__);
	}
	catch (...) {
		fits_close_file(fptr,&status);
		throw kipl::base::KiplException("Unknown exception",__FILE__,__LINE__);
	}
	if (status!=0) {
		char err_text[512];

		fits_get_errstatus(status, err_text);
		std::stringstream msg;
		fits_close_file(fptr,&status);
		msg<<"ReadFITS: "<<err_text<<" ("<<filename<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
    std::vector<size_t> dims(naxes,naxes+naxis);

	fits_close_file(fptr, &status);
    return dims;
}
}}
