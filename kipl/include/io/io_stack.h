//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//
#ifndef IOSTACK_H_
#define IOSTACK_H_
#include "../kipl_global.h"
#include "../strings/filenames.h"
#include "../base/timage.h"
#include "../base/kiplenums.h"
#include "../base/textractor.h"
#include "../base/tpermuteimage.h"
#include "io_tiff.h"
#include "io_matlab.h"
#include "io_fits.h"

#include <sstream>

namespace kipl { namespace io {

enum eFileType {
	MatlabVolume,
	MatlabSlices,
	TIFF8bits,
	TIFF16bits,
	TIFFfloat,
	PNG8bits,
	PNG16bits
};

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &s, kipl::io::eFileType ft);
void KIPLSHARED_EXPORT string2enum(const std::string str, kipl::io::eFileType &ft);
std::string KIPLSHARED_EXPORT enum2string(kipl::io::eFileType ft);

/// \brief Read a series of tiff image and stores as a 3D image
/// \brief img target image
/// \brief fname file name mask of the image sequence, must contain a *
/// \brief start first file number
/// \brief n number of files to read
/// \brief nstep file number increment
/// \brief crop coordinates for a crop region. The entire image is read if the crop is NULL
/// \returns bit per pixel
template<class ImgType>
int ReadImageStack(kipl::base::TImage<ImgType,3> & img, 
				  const std::string filemask,
				  const size_t start,
				  const size_t n,
				  const size_t nstep=1,
				  size_t const * const crop=NULL)
{
	kipl::base::TImage<ImgType,2> tmp; 
	std::string filename,ext;
	int bps=0;
	try {
		for (size_t i=0; i<n; i++) {
			kipl::strings::filenames::MakeFileName(filemask,start+i*nstep,filename,ext,'#','0');
			if ((ext==".tif") || (ext==".TIF"))
				bps=ReadTIFF(tmp,filename.c_str(),crop);
			else if (ext==".mat")
				ReadMAT(tmp,filename.c_str(),crop);
			else if (ext==".fits")
				ReadFITS(tmp,filename.c_str(),crop);
			else {
				std::ostringstream msg;
				msg<<"Unknown file type "<<ext;
				throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
			}

			if (i==0) {
				if (crop) {
					size_t dims[]={crop[2]-crop[0],crop[3]-crop[1],n};
					img.Resize(dims);
				}
				else {
					size_t dims[]={tmp.Size(0),tmp.Size(1),n};
					img.Resize(dims);
				}
			}
			
			kipl::base::InsertSlice(tmp,img,i,kipl::base::ImagePlaneXY);
		}
	}
	catch (kipl::base::KiplException & e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}
	catch (std::exception & e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}

	return bps;
}



/// \brief Writes a 3D image as a sequence of tiff files
///
///	The slice are read parallel to the XY-plane of the volume
///
///	\param img Target image (3D)
///	\param fname Pointer to path string (must contain a wildcard '*')
///	\param lo Lower bound for the quantification
///	\param hi Upper bound for the quantification
///	\param start Index of first file in the sequence
///	\param stop Index of the first slice after the sequence
///	\param cntstart start value of the file counter
///	
///	\todo Implement cropping
template<class ImgType>
int WriteImageStack(kipl::base::TImage<ImgType,3> img,const std::string fname,
		ImgType lo, ImgType hi, 
		const size_t start, const size_t stop, const size_t count_start=0,
		kipl::io::eFileType filetype=kipl::io::MatlabSlices,
		const kipl::base::eImagePlanes imageplane=kipl::base::ImagePlaneYZ,
		size_t *roi=NULL)
{
	if (stop<start)
		kipl::base::KiplException("Stop index must be greater than start index.",__FILE__, __LINE__);
	
	size_t nMaxSlices=0;
	switch (imageplane) {
		case kipl::base::ImagePlaneXY : 
			nMaxSlices=img.Size(2); break;
		case kipl::base::ImagePlaneXZ : 
			nMaxSlices=img.Size(1); break;
		case kipl::base::ImagePlaneYZ : 
			nMaxSlices=img.Size(0); break;
	}

	if (nMaxSlices<=start)
		kipl::base::KiplException("Start index can't be greater than number of slices.",__FILE__, __LINE__);
	
	if (nMaxSlices<stop)
		kipl::base::KiplException("Stop index can't be greater than number of slices.",__FILE__, __LINE__);
	
	std::string filename, ext, varname;
	
	kipl::base::TImage<ImgType,2> tmp;

#ifdef __GNUC__
	char slash='/';
#else
	char slash='\\';
#endif	
	kipl::base::TImage<float,2> ftmp;
	for (size_t i=start; i<stop; i++) {
		kipl::strings::filenames::MakeFileName(fname,static_cast<int>(i+count_start),filename,ext,'#','0');	
		tmp=kipl::base::ExtractSlice(img,i,imageplane,roi);
		tmp.info=img.info;
		switch (filetype)
		{
		case MatlabVolume : 
			kipl::base::KiplException("Matlab volume is not supported by the slice writer",__FILE__,__LINE__); 
			break;
		case MatlabSlices :
			varname=filename.substr(filename.rfind(slash)+1);
			varname=varname.substr(0,varname.find('.'));
			WriteMAT(tmp,filename.c_str(),varname.c_str());
			break;
		case TIFF8bits :
			WriteTIFF(tmp,filename.c_str(),lo,hi);
			break;
		case TIFF16bits :
			WriteTIFF(tmp,filename.c_str(),lo,hi);
			break;
		case TIFFfloat :
			ftmp.Resize(tmp.Dims());
			for (size_t i=0; i<tmp.Size(); i++) 
				ftmp[i]=tmp[i];
			WriteTIFF32(ftmp,filename.c_str());
			break;
		case PNG8bits :
			throw kipl::base::KiplException("8-bit png is not supported by slice writer",__FILE__,__LINE__);
			//WritePNG8(tmp,filename,lo,hi);
			break;
		case PNG16bits :
			throw kipl::base::KiplException("16-bit png is not supported by slice writer",__FILE__,__LINE__);
			//WritePNG16(tmp,filename,lo,hi);
			break;
		default:
			throw kipl::base::KiplException("Unknown file type in slice writer",__FILE__,__LINE__);
		}
	}
	

	return 1;
}

}}

#endif /*WRITESTACK_H_*/
