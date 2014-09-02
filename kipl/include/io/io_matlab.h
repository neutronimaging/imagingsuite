#ifndef IO_MATLAB_H_
#define IO_MATLAB_H_
 /***************************************************************************
 *   Copyright (C) 2004 by Anders Kaestner                                 *
 *   anders.kaestner@env.ethz.ch                                           *
 *                                                                         *
 ***************************************************************************/
#include "../base/timage.h"
#include "../base/kiplenums.h"
#include "core/matlabio.h"
#include <iostream>
#include "../strings/filenames.h"
#include "../visualization/GNUPlot.h"

namespace kipl { namespace io {



/// \brief Writes any image data type into a double matrix in a MAT file 
/// \param img the image to be stored
/// \param fname file name of the destination file (including extension .mat)
/// \param name variable name in the matlab environment
/// \return Error code 	
/// \retval 0 The writing failed
/// \retval 1 Successful 
template <class ImgType,size_t NDim>
int WriteMATdbl(const kipl::base::TImage<ImgType,NDim> &img,const char * const fname,const char * const name);

/// \brief Writes any image data type into as a int32 matrix in a MAT file 
///
/// \param img the image to be stored
/// \param fname file name of the destination file (including extension .mat)
/// \param name variable name in the matlab environment
/// \return Error code 	
/// \retval 0 The writing failed
///	\retval 1 Successful 
template <class ImgType, size_t NDim>
int WriteMATint(const kipl::base::TImage<ImgType,NDim> &img, char const * const fname, char const * const name);

/// \brief Reads a MAT file containing any image data type and stores the contents in a double image
/// 
///	\param img the image to be stored
///	\param fname file name of the destination file (including extension .mat)
///	\return Error code 	
///	\retval 0 The reading failed
///	\retval 1 Successful 
int KIPLSHARED_EXPORT ReadMATdbl(kipl::base::TImage<double,2> &img,const char *fname);

/// \brief Writes any image data type into a int32 matrix in a MAT file 
///	\param img the image to be stored
///	\param fname file name of the destination file (including extension .mat)
///	\return Error code 	
///	\retval 0 The writing failed
///	\retval 1 Successful 
int KIPLSHARED_EXPORT ReadMATint(kipl::base::TImage<int,2> &img, char const * const fname);

template <class ImgType,size_t NDim>
int WriteMATdbl(const kipl::base::TImage<ImgType,NDim> &img,
		char const * const fname, 
		char const * const name)
{
	kipl::base::TImage<double,NDim> tmp;
	
	tmp=img.toDouble();
	
	WriteMATmatrix((void*)tmp.GetDataPtr(), img.Dims(), NDim, kipl::io::core::mxDOUBLE_CLASS, name, fname);
	
	return 1;
}


template <class ImgType,size_t  NDim>
int WriteMATint(const kipl::base::TImage<ImgType,NDim> &img,
		char const * const fname,
		char const * const name)
{
	kipl::base::TImage<int,NDim> tmp;
	
	tmp=img.toInt();
		
	WriteMATmatrix((void*)tmp.getDataptr(), img.getDimsptr(), NDim, kipl::io::core::mxINT32_CLASS, name, fname);
	return 1;
}

/// \brief Writes any image data type into as a matrix with the same data type in a MAT file 
///	\param img the image to be stored
///	\param fname file name of the destination file (including extension .mat)
///	\param name variable name in the matlab environment
///	\return 1 if successful, 0 if fail
template <typename ImgType, size_t NDim>
int WriteMAT(const kipl::base::TImage<ImgType,NDim> &img, 
		char const * const fname, 
		char const * const name)
{
	kipl::io::core::mxClassID mxType;
	
	mxType=kipl::io::core::GetMatlabClassID(static_cast<ImgType>(0));
		
	WriteMATmatrix((void*)img.GetDataPtr(), img.Dims(), NDim, mxType, name, fname);
	
	return 1;
}

/// \brief Reads the contents of the mat file and stores it in the data type specified by the image
///	\param tmp the image to be stored
///	\param fname file name of the destination file (including extension .mat)
///	\return 1 if successful, 0 if fail
template <typename ImgType, size_t NDim>
int ReadMAT(kipl::base::TImage<ImgType,NDim> &img, char const * const fname, size_t const * const nCrop=NULL)
{
	std::stringstream msg;
	kipl::base::TImage<ImgType,NDim> tmp;
	char * data=NULL, *name=NULL;
	
	size_t dims[8];
	int ndims=NDim;
	kipl::io::core::mxClassID type;
	
	try {
	ReadMATmatrix(&data, dims,ndims, type, &name, fname);
	}
	catch (const std::bad_alloc & E) {
		msg.str("");
		msg<<"ReadMATmatrix caused an STL exception: "<<E.what();
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

	if (type==kipl::io::core::GetMatlabClassID(static_cast<ImgType>(0))) {
		try {
			tmp.Resize(dims);
			memcpy(tmp.GetDataPtr(),data,tmp.Size()*sizeof(ImgType));
		}
		catch (const std::bad_alloc & E) {
			msg.str("");
			msg<<"ReadMAT failed to copy result to destination: "<<E.what();
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	} 
	else {
		try {
		tmp.Resize(dims);
		}
		catch (const std::bad_alloc & E) {
			msg.str("");
			msg<<"ReadMAT failed to copy result to destination: "<<E.what();
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
		catch (kipl::base::KiplException & E) {
			throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
		}
		ImgType *pTmp=tmp.GetDataPtr();

		for (size_t i=0; i<tmp.Size(); i++) {
			switch (type) {
			case kipl::io::core::mxSINGLE_CLASS:
				pTmp[i]=ImgType(((float *)data)[i]);
				break;
			case kipl::io::core::mxINT8_CLASS:
				pTmp[i]=ImgType(((char *)data)[i]);
				break;
			case kipl::io::core::mxUINT8_CLASS:
				pTmp[i]=ImgType(((unsigned char *)data)[i]);
				break;
			case kipl::io::core::mxINT16_CLASS:
				pTmp[i]=ImgType(((short *)data)[i]);
				break;
			case kipl::io::core::mxUINT16_CLASS:
				pTmp[i]=ImgType(((unsigned short *)data)[i]);
				break;
			case kipl::io::core::mxINT32_CLASS:
				pTmp[i]=ImgType(((int *)data)[i]);
				break;
			case kipl::io::core::mxDOUBLE_CLASS:
				pTmp[i]=ImgType(((double *)data)[i]);
				break;
			case kipl::io::core::mxUINT32_CLASS:
				pTmp[i]=ImgType(((unsigned int *)data)[i]);
				break;
			case kipl::io::core::mxUNKNOWN_CLASS:
			case kipl::io::core::mxCELL_CLASS:
			case kipl::io::core::mxSTRUCT_CLASS:
			case kipl::io::core::mxLOGICAL_CLASS:
			case kipl::io::core::mxINT64_CLASS:
			case kipl::io::core::mxUINT64_CLASS:
			case kipl::io::core::mxFUNCTION_CLASS:
			case kipl::io::core::mxOPAQUE_CLASS:
			case kipl::io::core::mxOBJECT_CLASS:
			case kipl::io::core::mxCHAR_CLASS:
			case kipl::io::core::mxSPARSE_CLASS:
				msg.str("");
				msg<<"File contains "<<type<<std::endl;
				throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
				break;		
			}
		}
	}
	
	delete [] name;
	delete [] data;
	img=tmp;
	return 0;
}

/// \brief Reads a sequence of 2D images from a MatLab .mat-file and stores them in a 3D image
///
///	The slice are stored parallel to the XY-plane of the volume
///
///	\param img Target image (3D)
///	\param fname Pointer to path string (must contain a sequence of X's on the filenumber position)
///	\param start Index of first file in the sequence
///	\param n Number of slices to read
///	\param nstep Iteration step during read
///	\param crop Pointer to coordinate array for cropping, not implemented
///	\param rot rotation of the read image, discrete 90deg steps
///
///	\todo Implement cropping
template<typename ImgType, size_t NDim>
int SliceReadMAT2(kipl::base::TImage<ImgType,NDim> & img, 
		char const * const fname, 
		size_t start,
		size_t n,
		size_t nstep=1,
		size_t *crop=NULL, 
		double rot=0.0, 
		std::ostream &logstream=std::cout)
{
	std::cerr<<"kipl::io::SliceReadMAT2 is not implemented."<<std::endl;
	return 0;
	
	if (NDim!=3) {
		cerr<<"SliceReadMAT2: NDim=3 is only supported"<<endl;
		return 0;
	}

	string filename, ext;
	char tmpstr[16];
	logstream<<"Reading slice:"<<endl;
	kipl::base::TImage<ImgType,2> tmp;
	for (int i=0; i<n; i++) {
		sprintf(tmpstr,"[%d]",i);
//		logstream<<std::setw(8)<<tmpstr<<std::flush;
//		MakeFileName(fname,i*nstep+start,filename,ext);
		ReadMAT(tmp,filename.c_str());
		
		if (i==0) {
			if (crop) {
				unsigned int dims[]={crop[2]-crop[0]+1,crop[3]-crop[1]+1,n};
				img.resize(dims);
			}
			else {
				unsigned int dims[]={tmp.SizeX(),tmp.SizeY(),n};
				img.resize(dims);
			}
		}
		
		if (crop) 
			tmp=tmp.SubImage(crop);
		
	//	img.InsertSlice(tmp,plane_XY,i);
	}

	return 1;
}

/// \brief Reads a sequence of 2D images from a MatLab .mat-file and stores them in a 3D image
///
///	The slice are stored parallel to the XY-plane of the volume
///
///	\param img Target image (3D)
///	\param fname Pointer to path string (must contain a wildcard '*')
///	\param start Index of first file in the sequence
///	\param n Number of slices to read
///	\param nstep Iteration step during read
///	\param crop Pointer to coordinate array for cropping, not implemented
///	\param rot rotation of the read image, discrete 90deg steps
///
///	\todo Implement cropping
template<typename ImgType>
int SliceReadMAT(kipl::base::TImage<ImgType,3u> & img,
		char const * const fname,
		int start,
		int n,
		int nstep=1,
		int const * const crop=NULL, 
		double rot=0.0, 
		std::ostream &logstream=std::cout)
{
	char fprefix[256],fsuffix[256];
	char path[256];
	
	kipl::strings::filenames::ExtractWildCard(fname,fprefix,fsuffix);

	char tmpstr[16];
	logstream<<"Reading slice:"<<endl;
	kipl::base::TImage<ImgType,2> tmp;
	for (int i=0; i<n; i++) {
		sprintf(tmpstr,"[%d]",i);
		//logstream<<setw(8)<<tmpstr<<flush;
		sprintf(path,"%s%04d%s",fprefix,start+i*nstep,fsuffix);
		ReadMAT(tmp,path);
		
		if (i==0) {
			if (crop) {
				size_t dims[]={crop[2]-crop[0]+1,crop[3]-crop[1]+1,n};
				img.Resize(dims);
				
			}
			else {
				size_t dims[]={tmp.Size(0),tmp.Size(1),n};
				img.Resize(dims);
			}
		}
		
		if (crop) 
			tmp=tmp.SubImage(crop);
		
		img.InsertSlice(tmp,kipl::base::ImagePlaneXY,i);
	}

	return 1;
}

/// \brief Writes a sequence of 2D images to a MatLab .mat-file and from a 3D image
///
///	The slice are read parallel to the XY-plane of the volume
///
///	\param img Target image (3D)
///	\param fname Pointer to path string (must contain a wildcard '*')
///	\param vname Pointer to variable string (must contain a wildcard '*')
///	\param start Index of first file in the sequence
///	\param n Number of slices to read (if n<0 will all slices be written)
///	\param nstep Iteration step during read
///	\param cntstart start value for the file counter
///	
///	\todo Implement cropping
template<typename ImgType>
int SliceWriteMAT(kipl::base::TImage<ImgType,3u> & img,
		const char *fname,
		const char *vname,
		int start=0, 
		int n=-1,
		int nstep=1, 
		int cntstart=-1)
{
	char fprefix[256],fsuffix[256],vprefix[256],vsuffix[256];
	char path[256],var[256];
	
	kipl::strings::filenames::ExtractWildCard(fname,fprefix,fsuffix);
	kipl::strings::filenames::ExtractWildCard(vname,vprefix,vsuffix);

	int stop;

	if (start+n*nstep>=img.SizeZ())
		stop=img.SizeZ();
	else 
		stop=start+n*nstep;
	if (n<0)
		stop=img.SizeZ();

	kipl::base::TImage<ImgType,2> tmp;
	int fnum;
	for (int i=start; i<stop; i+=nstep) {
		fnum= cntstart ==-1 ? i : i+cntstart;
		sprintf(path,"%s%04d%s",fprefix,fnum,fsuffix);
		sprintf(var,"%s%04d%s",vprefix,fnum,vsuffix);
		img.ExtractSlice(tmp,kipl::base::ImagePlaneXY,i);
		WriteMAT(tmp,path,var);
	}

	return 1;
}

void KIPLSHARED_EXPORT GetMATDims(char const * const filename,size_t * dims);

}}

#endif /*IO_MATLAB_H_*/
