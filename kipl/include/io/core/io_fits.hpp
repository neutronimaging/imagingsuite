#ifndef IO_FITS_HPP_
#define IO_FITS_HPP_

#include "../../base/KiplException.h"
//#include <io/IO_fits.h>
#include <iostream>
#include <sstream>
#include <fitsio.h>

namespace kipl { namespace io {
/*
int FITSDataType(unsigned char x) 	{ return TBYTE; }
int FITSDataType(char x)           	{ return TSBYTE; }
int FITSDataType(unsigned short x) 	{ return TUSHORT; }
int FITSDataType(short x) 			{ return TSHORT; }
int FITSDataType(int x) 				{ return TINT; }
int FITSDataType(unsigned int x) 	{ return TUINT; }
int FITSDataType(long x) 				{ return TLONG; }
int FITSDataType(long long x)			{ return TLONGLONG; }
int FITSDataType(unsigned long x) 	{ return TULONG; }
int FITSDataType(float x) 			{ return TFLOAT; }
int FITSDataType(double x) 			{ return TDOUBLE; }
*/

int KIPLSHARED_EXPORT FITSDataType(unsigned char x);
int KIPLSHARED_EXPORT FITSDataType(char x);
int KIPLSHARED_EXPORT FITSDataType(unsigned short x);
int KIPLSHARED_EXPORT FITSDataType(short x);
int KIPLSHARED_EXPORT FITSDataType(int x);
int KIPLSHARED_EXPORT FITSDataType(unsigned int x);
int KIPLSHARED_EXPORT FITSDataType(long x);
int KIPLSHARED_EXPORT FITSDataType(long long x);
int KIPLSHARED_EXPORT FITSDataType(unsigned long x);
int KIPLSHARED_EXPORT FITSDataType(float x);
int KIPLSHARED_EXPORT FITSDataType(double x);

template <typename ImgType>
int ReadFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname, size_t const * const nCrop)
{
	using namespace std; 
	fitsfile *fptr;
	int status=0;
	fits_open_image(&fptr, fname, READONLY, &status);
	
	int bitpix=0;
	int naxis;
	long naxes[3];
	
	try {
		fits_get_img_param(fptr, 2,  &bitpix, &naxis, naxes, &status);
	}
	catch (std::exception &e) {
		throw kipl::base::KiplException(e.what(), __FILE__, __LINE__);
	}
	catch (...) {
		throw kipl::base::KiplException("Unknown exception",__FILE__,__LINE__);
	}
	if (status!=0) {
		char err_text[512];
		
		fits_get_errstatus(status, err_text);
		std::stringstream msg;
		msg<<"ReadFITS: "<<err_text<<" ("<<fname<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
	size_t dims[3];
	long coord[3]={1,0,1};
	if (nCrop==NULL) {
		dims[0]=static_cast<size_t>(naxes[0]);
		dims[1]=naxis < 2 ? 1 : static_cast<size_t>(naxes[1]);
	}
	else {
		if ((nCrop[2]<=nCrop[0]) || (nCrop[3]<=nCrop[1]))
			throw kipl::base::KiplException("Invalid cropping region",__FILE__,__LINE__);
			
		dims[0]=nCrop[2]-nCrop[0];
		dims[1]=nCrop[3]-nCrop[1];
		coord[0]=static_cast<long>(nCrop[0]+1);
	}
	src.Resize(dims);
	

	int datatype=FITSDataType(static_cast<ImgType>(0));
	
	const size_t cnStart = nCrop==NULL ? 0 : min(static_cast<size_t>(naxes[1]),nCrop[1]);
	const size_t cnStop  = nCrop==NULL ? dims[1] : min(static_cast<size_t>(naxes[1]),nCrop[3]);
	
	ImgType *pLine=new ImgType[naxes[0]];
	for (size_t i=cnStart, j=0; i<cnStop; i++,j++) {
		coord[1]=i+1;

		if (fits_read_pix(fptr, datatype, coord, static_cast<int>(dims[0]), 
				NULL, pLine, NULL, &status))
		{
			char err_text[512];
					fits_get_errstatus(status, err_text);
					std::stringstream msg;
					msg<<"ReadFITS: "<<err_text<<" ("<<fname<<")";
					throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
		
		memcpy(src.GetLinePtr(j),pLine,src.Size(0)*sizeof(ImgType));
	}

	delete [] pLine;
	fits_close_file(fptr, &status);
	
	return 0;
}

template <typename ImgType>
int WriteFITS(kipl::base::TImage<ImgType,2> & UNUSED(src),char const * const UNUSED(fname))
{
	throw kipl::base::KiplException("WriteFITS is not implemented");
	return 0;
}


}}
#endif /*IO_FITS_HPP_*/
