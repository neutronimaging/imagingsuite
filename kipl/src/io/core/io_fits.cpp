#include <fitsio.h>
#include "../../../include/base/KiplException.h"
#include <exception>
#include <sstream>

namespace kipl { namespace io {
int KIPLSHARED_EXPORT FITSDataType(unsigned char x) 	{ return TBYTE; }
int KIPLSHARED_EXPORT FITSDataType(char x)           	{ return TSBYTE; }
int KIPLSHARED_EXPORT FITSDataType(unsigned short x) 	{ return TUSHORT; }
int KIPLSHARED_EXPORT FITSDataType(short x) 			{ return TSHORT; }
int KIPLSHARED_EXPORT FITSDataType(int x) 				{ return TINT; }
int KIPLSHARED_EXPORT FITSDataType(unsigned int x) 	{ return TUINT; }
int KIPLSHARED_EXPORT FITSDataType(long x) 				{ return TLONG; }
int KIPLSHARED_EXPORT FITSDataType(long long x)			{ return TLONGLONG; }
int KIPLSHARED_EXPORT FITSDataType(unsigned long x) 	{ return TULONG; }
int KIPLSHARED_EXPORT FITSDataType(float x) 			{ return TFLOAT; }
int KIPLSHARED_EXPORT FITSDataType(double x) 			{ return TDOUBLE; }

void KIPLSHARED_EXPORT GetFITSDims(char const * const filename,size_t * dims)
{
	fitsfile *fptr;
	int status=0;
	std::ostringstream msg;
	fits_open_image(&fptr, filename, READONLY, &status);
	if (status!=0) {
		msg<<"Failed to open '"<<filename<<"' in GetFITSDims";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	int bitpix=0;
	int naxis;
	long naxes[3];

	try {
		fits_get_img_param(fptr, 2,  &bitpix, &naxis, naxes, &status);
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

	for (int i=0; i<naxis; i++)
		dims[i]=naxes[i];

	fits_close_file(fptr, &status);
}
}}
