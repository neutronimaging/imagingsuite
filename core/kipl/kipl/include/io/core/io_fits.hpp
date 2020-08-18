//<LICENSE>

#ifndef IO_FITS_HPP_
#define IO_FITS_HPP_

#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdint.h>

#include <fitsio.h>

#include "../../base/timage.h"
#include "../../base/KiplException.h"
#include "../io_fits.h"

namespace kipl { namespace io {

int KIPLSHARED_EXPORT FITSDataType(unsigned char x);
int KIPLSHARED_EXPORT FITSDataType(char x);
int KIPLSHARED_EXPORT FITSDataType(unsigned short x);
int KIPLSHARED_EXPORT FITSDataType(short x);
int KIPLSHARED_EXPORT FITSDataType(int32_t x);
int KIPLSHARED_EXPORT FITSDataType(uint32_t x);
int KIPLSHARED_EXPORT FITSDataType(int64_t x);
int KIPLSHARED_EXPORT FITSDataType(long long x);
int KIPLSHARED_EXPORT FITSDataType(int64_t x);
int KIPLSHARED_EXPORT FITSDataType(uint64_t x);
int KIPLSHARED_EXPORT FITSDataType(float x);
int KIPLSHARED_EXPORT FITSDataType(double x);



template <typename ImgType>
int ReadFITS(kipl::base::TImage<ImgType,2> &src,const std::string & fname, const std::vector<size_t> & nCrop, size_t idx)
{
    using namespace std;
    ostringstream msg;
	fitsfile *fptr;
	int status=0;
    fits_open_image(&fptr, fname.c_str(), READONLY, &status);
    if (status!=0) {
        msg.str("");
        char errtext[2048];
        fits_get_errstatus(status,errtext);
        msg<<"Failed opening "<<fname<<std::endl<<errtext;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

	int bitpix=0;
	int naxis;
    long naxes[10];
	
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
	

    std::vector<size_t> dims(naxis);
    long coord[3]={1,0,long(naxis == 3 ? idx +1: 1)};
    if (nCrop.empty())
    {
		dims[0]=static_cast<size_t>(naxes[0]);
		dims[1]=naxis < 2 ? 1 : static_cast<size_t>(naxes[1]);
	}
    else
    {
        if (    (naxes[0]<=static_cast<long>(nCrop[0])) ||
                (naxes[0]<=static_cast<long>(nCrop[2])) ||
                (naxes[1]<=static_cast<long>(nCrop[1])) ||
                (naxes[1]<=static_cast<long>(nCrop[3])))
        {
            msg.str("");
            msg<<"Cropping region ("<<nCrop[0]<<", "<<nCrop[1]<<", "<<nCrop[2]<<", "<<nCrop[3]<<") out of bounds ("<<naxes[0]<<", "<<naxes[1]<<") for file "<<fname;
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }

        if ((nCrop[2]<=nCrop[0]) || (nCrop[3]<=nCrop[1])) {
            msg.str("");
            msg<<"Invalid cropping region for file "<<fname<<" ("<<nCrop[0]<<", "<<nCrop[1]<<", "<<nCrop[2]<<", "<<nCrop[3]<<")";
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }
			
		dims[0]=nCrop[2]-nCrop[0];
		dims[1]=nCrop[3]-nCrop[1];
		coord[0]=static_cast<long>(nCrop[0]+1);
	}
    src.resize(dims);
	

	int datatype=FITSDataType(static_cast<ImgType>(0));
	
    const size_t cnStart = nCrop.empty() ? 0       : min(static_cast<size_t>(naxes[1]),nCrop[1]);
    const size_t cnStop  = nCrop.empty() ? dims[1] : min(static_cast<size_t>(naxes[1]),nCrop[3]);

    ImgType *pLine=new ImgType[naxes[0]+16];
	for (size_t i=cnStart, j=0; i<cnStop; i++,j++) {
        coord[1]=static_cast<int>(i+1);

		if (fits_read_pix(fptr, datatype, coord, static_cast<int>(dims[0]), 
                nullptr, pLine, nullptr, &status))
		{
			char err_text[512];
            fits_get_errstatus(status, err_text);
            std::stringstream msg;
            msg<<"ReadFITS: "<<err_text<<" ("<<fname<<")";
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}

        std::copy(pLine, pLine+src.Size(0), src.GetLinePtr(j));
	}

	delete [] pLine;
	fits_close_file(fptr, &status);
	
	return 0;
}

template <typename ImgType>
int WriteFITS(kipl::base::TImage<ImgType,2> & src,char const * const filename)
{

    fitsfile *fptr;                  // pointer to the FITS file, defined in fitsio.h
    int status;
    long  fpixel, nelements, exposure;

    char err_msg[128];

    // initialize FITS image parameters
    int bitpix   =  USHORT_IMG;     // 16-bit unsigned short pixel values
    long naxis    =   2;            // 2-dimensional image
    long naxes[2] ;
    naxes[0]= static_cast<long>(src.Size(0)); // Set image dimensions
    naxes[1]= static_cast<long>(src.Size(1)); // Set image dimensions

//    ImgType *array=new ImgType[naxes[1]];
//    // initialize pointers to the start of each row of the image
//    for( ii=1; ii<naxes[1]; ii++ )
//      array[ii] = array[ii-1] + naxes[0];

    remove(filename);                               // Delete old file if it already exists

    status = 0;                                     // initialize status before calling fitsio routines

    if (fits_create_file(&fptr, filename, &status)) // create new FITS file
    {
        fits_get_errstatus(status, err_msg);
        throw kipl::base::KiplException(err_msg,__FILE__,__LINE__);
    }

//     write the required keywords for the primary array image.
//     Since bitpix = USHORT_IMG, this will cause cfitsio to create
//     a FITS image with BITPIX = 16 (signed short integers) with
//     BSCALE = 1.0 and BZERO = 32768.  This is the convention that
//     FITS uses to store unsigned integers.  Note that the BSCALE
//     and BZERO keywords will be automatically written by cfitsio
//     in this case.

    if ( fits_create_img(fptr,  bitpix, static_cast<int>(naxis), naxes, &status) ) {
        fits_get_errstatus(status, err_msg);
        throw kipl::base::KiplException(err_msg,__FILE__,__LINE__);
    }

    fpixel = 1;                               // first pixel to write
    nelements = naxes[0] * naxes[1];          // number of pixels to write

    // write the array of unsigned integers to the FITS file
    if ( fits_write_img(fptr, FITSDataType(src[0]), fpixel, nelements, src.GetDataPtr(), &status) ) {
        fits_get_errstatus(status, err_msg);
        throw kipl::base::KiplException(err_msg,__FILE__,__LINE__);
    }

    // write another optional keyword to the header
    // Note that the ADDRESS of the value is passed in the routine
    exposure = 1500;
    char msg[]="Total Exposure Time";
    if ( fits_update_key(fptr, TLONG, "EXPOSURE", &exposure,
         msg, &status) )
    {
        fits_get_errstatus(status, err_msg);
        throw kipl::base::KiplException(err_msg,__FILE__,__LINE__);
    }


    if ( fits_close_file(fptr, &status) )                // close the file
    {
        fits_get_errstatus(status, err_msg);
        throw kipl::base::KiplException(err_msg,__FILE__,__LINE__);
    }

//   delete [] array;
	return 0;
}


}}
#endif /*IO_FITS_HPP_*/
