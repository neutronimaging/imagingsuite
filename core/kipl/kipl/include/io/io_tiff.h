//<LICENSE>

#ifndef IO_TIFF_H
#define IO_TIFF_H

#include "../kipl_global.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>

#include <tiffio.h>

#include "../base/imagecast.h"
#include "../base/textractor.h"
#include "../base/timage.h"
#include "../base/imageinfo.h"
#include "../strings/filenames.h"
#include "../base/kiplenums.h"
#include "analyzefileext.h"


namespace kipl { namespace io {

/// \brief Writes an uncompressed TIFF image from any image data type (grayscale)
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .tif)
/// \param dt data type selector
///
///	Setting both bounds to zero results in full dynamics rescaled in the interval [0,255].
template <class ImgType,size_t N>
void WriteTIFF(const kipl::base::TImage<ImgType,N> & src,
               const std::string &fname,
               kipl::base::eDataType dt = kipl::base::UInt16,
               kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY,
               bool append=false)
{
    if ( (dt == kipl::base::UInt4) || (dt == kipl::base::UInt12))
        throw kipl::base::KiplException("WriteTIFF doesn't support 4- and 12-bit formats");

    TIFF *image;
    std::stringstream msg;
    std::vector<size_t> sliceDims;
    size_t nSlices=1;

    if (N==2)
    {
        sliceDims = {src.Size(0), src.Size(1)};
        nSlices = 1;
    }
    else
    {
        switch (plane)
        {
            case kipl::base::ImagePlaneXY :
                sliceDims = {src.Size(0), src.Size(1)};
                nSlices   = src.Size(2);
                break;
            case kipl::base::ImagePlaneXZ :
                sliceDims = {src.Size(0), src.Size(2)};
                nSlices   = src.Size(1);
                break;
            case kipl::base::ImagePlaneYZ :
                sliceDims = {src.Size(1), src.Size(2)};
                nSlices   = src.Size(0);
                break;
        }
    }

    size_t sliceSize = sliceDims[0] * sliceDims[1];

    // Open the TIFF file
    if (append)
    {
        if ((image = TIFFOpen(fname.c_str(), "a")) == nullptr)
        {
            msg.str("");
            msg<<"WriteTIFF: Could not open "<<fname<<" for appending";
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }

        while (TIFFLastDirectory(image)==false)
        {
            TIFFReadDirectory(image);
        }
    }
    else
    {
        if ( (image = TIFFOpen(fname.c_str(), "w")) == nullptr )
        {
            msg.str("");
            msg<<"WriteTIFF: Could not open "<<fname<<" for writing";
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }
    }


    kipl::base::TImage<ImgType,2> slice(sliceDims);

    for (size_t i=0; i<nSlices; ++i)
    {
        // We need to set some values for basic tags before we can add any data
        TIFFSetField(image, TIFFTAG_IMAGEWIDTH,         static_cast<int>(sliceDims[0]));
        TIFFSetField(image, TIFFTAG_IMAGELENGTH,        static_cast<int>(sliceDims[1]));
        TIFFSetField(image, TIFFTAG_BITSPERSAMPLE,      dt);
        TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL,    1);
        if ( dt == kipl::base::Float32 )
            TIFFSetField(image, TIFFTAG_SAMPLEFORMAT,       3); // Floating point
        else
            TIFFSetField(image, TIFFTAG_SAMPLEFORMAT,       1); // UInt

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

        if (src.info.sDescription.empty())
            TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, "slope = 1.0E0\noffset = 0.0E0");
        else
            TIFFSetField(image, TIFFTAG_IMAGEDESCRIPTION, src.info.sDescription.c_str());

        // Write the information to the file

        if (N==2)
        {
            std::copy_n(src.GetDataPtr(),sliceSize,slice.GetDataPtr());
        }
        else
        {
            slice=kipl::base::ExtractSlice(src,i,plane);
        }

        auto *pSlice=slice.GetLinePtr(0,i);

        switch (dt)
        {
            case kipl::base::UInt4 : break;
            case kipl::base::UInt8 :
            {
                kipl::base::TImage<unsigned char,2> tmp(sliceDims);
                copy_n(pSlice,sliceSize,tmp.GetDataPtr());

                TIFFWriteEncodedStrip(image, 0, tmp.GetDataPtr(), sliceSize*sizeof(unsigned char));
                break;
            }
            case kipl::base::UInt12 : break;
            case kipl::base::UInt16 :
            {
                kipl::base::TImage<unsigned short,2> tmp(sliceDims);
                copy_n(pSlice,sliceSize,tmp.GetDataPtr());

                TIFFWriteEncodedStrip(image, 0, tmp.GetDataPtr(), sliceSize*sizeof(unsigned short));
                break;
            }
            case kipl::base::Float32 :
            {
                kipl::base::TImage<float,2> tmp(sliceDims);
                copy_n(pSlice,sliceSize,tmp.GetDataPtr());

                TIFFWriteEncodedStrip(image, 0, tmp.GetDataPtr(), sliceSize*sizeof(float));
                break;
            }
        }


        if (N!=2)
            TIFFWriteDirectory(image);
    }

    TIFFClose(image);
}


/// \brief Gets the dimensions of a tiff image without reading the image
/// \param fname file name of the image file
/// \param dims array with the dimensions
int KIPLSHARED_EXPORT GetTIFFDims(const std::string & fname, size_t *dims);
std::vector<size_t> KIPLSHARED_EXPORT GetTIFFDims(const std::string & fname);

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
bool KIPLSHARED_EXPORT GetSlopeOffset(std::string msg, float &slope, float &offset);


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
template <class ImgType,size_t N>
void WriteTIFF(kipl::base::TImage<ImgType,N> src,
               const std::string &fname,
               ImgType lo, ImgType hi,
               kipl::base::eDataType dt = kipl::base::UInt16,
               kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY,
               bool append=false)
{
    std::stringstream msg;
    try
    {
        switch (dt)
        {
        case kipl::base::UInt4 : break;
        case kipl::base::UInt8 :
        {
            kipl::base::TImage<unsigned char,N> img;
            img=kipl::base::ImageCaster<unsigned char, ImgType, N>::cast(src,lo,hi);
            if (src.info.sDescription.empty())
            {
                float slope = (static_cast<float>(hi)-static_cast<float>(lo))/std::numeric_limits<unsigned char>::max();

                msg<<"slope = "<<scientific<<slope<<"\noffset = "<<scientific<<lo;
                src.info.sDescription=msg.str();
            }

            WriteTIFF(img,fname,kipl::base::UInt8,plane,append);
            break;
        }
        case kipl::base::UInt12 : break;
        case kipl::base::UInt16 :
        {
            kipl::base::TImage<unsigned short,N> img;
            img=kipl::base::ImageCaster<unsigned short, ImgType, N>::cast(src,lo,hi);
            if (src.info.sDescription.empty())
            {
                float slope = (static_cast<float>(hi)-static_cast<float>(lo))/std::numeric_limits<unsigned short>::max();

                msg<<"slope = "<<scientific<<slope<<"\noffset = "<<scientific<<lo;
                src.info.sDescription=msg.str();
            }

            WriteTIFF(img,fname,kipl::base::UInt16,plane,append);
            break;
        }
        case kipl::base::Float32 :
            WriteTIFF(src,fname,kipl::base::Float32,plane,append);
            break;
        }
	}
    catch (kipl::base::KiplException &E)
    {
		throw kipl::base::KiplException(E.what(),__FILE__,__LINE__);
	}

}

/// \brief Reads the contents of a tiff file and stores the contents in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file
///	
///	\return Error code 	
///	\retval 0 The writing failed
///	\retval 1 Successful 
template <class ImgType>
int ReadTIFF(kipl::base::TImage<ImgType,2> &src,const std::string & fname, size_t idx=0UL)
{
    kipl::logging::Logger logger("ReadTIFF");
    std::stringstream msg;
	TIFF *image;
    uint16_t photo, spp, fillorder,bps, sformat;
	tsize_t stripSize, stripCount;
	unsigned long imageOffset;
	long result;
	int stripMax;
	unsigned char *buffer, tempbyte;
	unsigned long bufferSize, count;

	TIFFSetWarningHandler(0);
	// Open the TIFF image
    if ((image = TIFFOpen(fname.c_str(), "r")) == nullptr)
    {
        msg.str("");
		msg<<"ReadTIFF: Could not open image "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
    int framecnt=0;
    while ((framecnt<static_cast<int>(idx)) && (image!=nullptr))
    {
        TIFFReadDirectory(image);
        framecnt++;
    }

    if (image == nullptr)
    {
        msg.str("");
        msg<<"ReadTIFF: Frame index exceeds the available frame in the file"<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

	// Check that it is of a type that we support
    if ((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) )
    {
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}

    if ((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sformat) == 0) )
    {
		sformat=1; // Assuming unsigned integer data if unknown
	}

    if (TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) 
    {
        logger.warning("TIFFTAG_SAMPLESPERPIXEL is not defined using default spp=1");
        spp=1;
    }
    
    if (spp != 1)
    {
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// Read in the possibly multiple strips
	stripSize = TIFFStripSize (image);
	stripMax = TIFFNumberOfStrips (image);
	imageOffset = 0;

	bufferSize = TIFFNumberOfStrips (image) * stripSize;
    try
    {
        if((buffer = new unsigned char[bufferSize]) == nullptr)
        {
			msg.str("");
			msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image";
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	}
    catch (std::bad_alloc & E)
    {
		msg.str("");
		msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image ("<<E.what()<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &E)
    {
		throw E;
	}


	int dimx,dimy;
	// We need to set some values for basic tags before we can add any data
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&dimx);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &dimy);

    for (stripCount = 0; stripCount < stripMax; stripCount++)
    {
        if ((result = TIFFReadEncodedStrip (image, stripCount,
					      buffer + imageOffset,
                          stripSize)) == -1)
        {
			msg.str("");
			msg<<"Read error on input strip number "<<static_cast<size_t>(stripCount);
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
    	imageOffset += result;
  	}

	// Deal with photometric interpretations
    if (TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0)
    {
		throw kipl::base::KiplException("Image has an undefined photometric interpretation",__FILE__,__LINE__);
	}
	
    if(photo == PHOTOMETRIC_MINISWHITE)
    {
		// Flip bits
        std::clog<<"Fixing the photometric interpretation"<<std::endl;
		
		for(count = 0; count < bufferSize; count++)
			buffer[count] = ~buffer[count];
	}
	
	// Deal with fillorder
    if (TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0)
    {
		fillorder=FILLORDER_MSB2LSB;
	}
	
    if (fillorder != FILLORDER_MSB2LSB)
    {
		// We need to swap bits -- ABCDEFGH becomes HGFEDCBA
        std::clog<<"Fixing the fillorder"<<std::endl;
		
        for(count = 0; count < bufferSize; count++)
        {
			tempbyte = 0;
            if (buffer[count] & 128) tempbyte += static_cast<char>(1);
            if (buffer[count] & 64)  tempbyte += static_cast<char>(2);
            if (buffer[count] & 32)  tempbyte += static_cast<char>(4);
            if (buffer[count] & 16)  tempbyte += static_cast<char>(8);
            if (buffer[count] & 8)   tempbyte += static_cast<char>(16);
            if (buffer[count] & 4)   tempbyte += static_cast<char>(32);
            if (buffer[count] & 2)   tempbyte += static_cast<char>(64);
            if (buffer[count] & 1)   tempbyte += static_cast<char>(128);

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

    switch (resunit)
    {
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
    std::vector<size_t> dims = { static_cast<size_t>(dimx), static_cast<size_t>(dimy) };
    src.resize(dims);

	const size_t cnSize=src.Size();
    switch (bps)
    {
	case 32:
        switch (sformat)
        {
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
        for (size_t i=0, j=0; i<cnSize; i+=2, j++)
        {
			src[i]=static_cast<ImgType>(buffer[j]>>4);
			src[i+1]=static_cast<ImgType>(buffer[j] & 15);
		}
		break;
	}

    src.info.nBitsPerSample = bps;
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
int ReadTIFF(kipl::base::TImage<ImgType,2> &src,const std::string &fname, const std::vector<size_t> &crop, size_t idx)
{
    kipl::logging::Logger logger("ReadTIFF");

    if (crop.empty())
    {
        return ReadTIFF(src,fname.c_str(),idx);
	}
	std::stringstream msg;
	TIFF *image;
    uint16_t photo, spp, fillorder,bps, sformat; 
    
	unsigned char *buffer, tempbyte;
	unsigned long bufferSize, count;

    TIFFSetWarningHandler(nullptr);
	// Open the TIFF image
    if ((image = TIFFOpen(fname.c_str(), "r")) == nullptr)
    {
        msg.str("");
		msg<<"ReadTIFF: Could not open image "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

    size_t framecnt=0;
    while ((framecnt<idx) && (image!=nullptr))
    {
        TIFFReadDirectory(image);
        framecnt++;
    }

    if (image == nullptr)
    {
        msg.str("");
        msg<<"ReadTIFF: Frame index exceeds the available frame in the file"<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

	// Check that it is of a type that we support
    if ((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) )
    {
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}

    if ((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sformat) == 0) )
    {
		sformat=1; // Assuming unsigned integer data if unknown
	}

    if (TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) 
    {
        logger.warning("TIFFTAG_SAMPLESPERPIXEL is not defined using default spp=1");
        spp=1;
    }

    if (spp != 1)
    {
		throw kipl::base::KiplException("ReadTIFF: Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// Read in the possibly multiple strips
    // tsize_t stripSize = TIFFStripSize (image);
    // int stripMax      = TIFFNumberOfStrips (image);
	// unsigned long imageOffset = 0;

	int dimx,dimy;
	// We need to set some values for basic tags before we can add any data
    TIFFGetField(image, TIFFTAG_IMAGEWIDTH,  &dimx);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &dimy);

    if (    (dimx<=static_cast<long>(crop[0])) ||
            (dimy<=static_cast<long>(crop[1])) ||
            (dimx<=static_cast<long>(crop[2])) ||
            (dimy<=static_cast<long>(crop[3])))
    {
        msg.str("");
        msg<<"Cropping region ("
           <<crop[0]<<", "
           <<crop[1]<<", "
           <<crop[2]<<", "
           <<crop[3]<<") is out of bounds ("<<dimx<<", "<<dimy<<") for file "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    int adjcrop[4]={static_cast<int>(min(static_cast<size_t>(dimx),crop[0])),
                    static_cast<int>(min(static_cast<size_t>(dimy),crop[1])),
                    static_cast<int>(min(static_cast<size_t>(dimx),crop[2])),
                    static_cast<int>(min(static_cast<size_t>(dimy),crop[3]))};
    if ((adjcrop[2]-adjcrop[0])==0) kipl::base::KiplException("Failed to crop image in X (x1<x0)",__FILE__,__LINE__);
    if ((adjcrop[3]-adjcrop[1])==0) kipl::base::KiplException("Failed to crop image in Y (y1<y0)",__FILE__,__LINE__);
	
    std::vector<size_t> imgdims = { static_cast<size_t>(adjcrop[2]-adjcrop[0]),
                                    static_cast<size_t>(adjcrop[3]-adjcrop[1]) };

	bufferSize = TIFFScanlineSize(image);
    try
    {
        if ( (buffer = new unsigned char[bufferSize]) == nullptr )
        {
			msg.str("");
			msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image";
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	}
    catch (std::bad_alloc & E)
    {
		msg.str("");
		msg<<"Could not allocate"<<bufferSize<<" bytes for the uncompressed image ("<<E.what()<<")";
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &E)
    {
		throw E;
	}

    src.resize(imgdims);
    for (int row=adjcrop[1]; row<adjcrop[3]; row++)
    {
        if (TIFFReadScanline(image,static_cast<tdata_t>(buffer), row, 0)!=1)
        {
			msg.str("");
			msg<<"ReadTIFFLine: an error occurred during reading scan line "<<row;
			TIFFClose(image);
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
		// Deal with photometric interpretations
        if (TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0)
        {
			throw kipl::base::KiplException("Image has an undefined photometric interpretation",__FILE__,__LINE__);
		}
		
        if ( photo == PHOTOMETRIC_MINISWHITE )
        {
			// Flip bits
            logger.verbose("Fixing the photometric interpretation");
			
			for(count = 0; count < bufferSize; count++)
				buffer[count] = ~buffer[count];
		}
		
		// Deal with fillorder
        if ( TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0 )
        {
			fillorder=FILLORDER_MSB2LSB;
		}
		
        if (fillorder != FILLORDER_MSB2LSB)
        {
			// We need to swap bits -- ABCDEFGH becomes HGFEDCBA
            logger(logger.LogVerbose,"Fixing the fillorder");
			
            for ( count = 0; count < bufferSize; count++ )
            {
				tempbyte = 0;
                if (buffer[count] & 128) tempbyte += 1;
                if (buffer[count] & 64)  tempbyte += 2;
                if (buffer[count] & 32)  tempbyte += 4;
                if (buffer[count] & 16)  tempbyte += 8;
                if (buffer[count] & 8)   tempbyte += 16;
                if (buffer[count] & 4)   tempbyte += 32;
                if (buffer[count] & 2)   tempbyte += 64;
                if (buffer[count] & 1)   tempbyte += 128;

				buffer[count] = tempbyte;
			}
		}

		ImgType *pLine=src.GetLinePtr(row-adjcrop[1]);

        switch (bps)
        {
			case 32:
                switch (sformat)
                {
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

    switch (resunit)
    {
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


/// \brief Reads the contents of a tiff file and stores the contents in the data type specified by the image
///	\param src the image to be stored
///	\param fname file name of the destination file (including extension .bmp)
///
///	\return Error code
///	\retval 0 The writing failed
///	\retval 1 Successful
template <class ImgType>
int ReadTIFF(kipl::base::TImage<ImgType,3> &src,const std::string & fname, const std::vector<size_t> &crop={})
{
    std::ostringstream msg;
    std::vector<size_t> dims=GetTIFFDims(fname.c_str());

    try
    {
        src.resize(dims);
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Failed to allocate 3D image for "<<fname<<"\n"<<e.what();
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg.str("");
        msg<<"Failed to allocate 3D image for "<<fname<<"\n"<<e.what();
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
        msg.str("");
        msg<<"Failed to allocate 3D image for "<<fname<<" with an unhandled exception";
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    kipl::base::TImage<float,2> img;
    int res=0;
    for (size_t i=0; i< dims[2]; ++i)
    {
        res=ReadTIFF(img,fname,crop,i);
        std::copy_n(img.GetDataPtr(),img.Size(),src.GetLinePtr(0,i));
    }
    src.info.nBitsPerSample = img.info.nBitsPerSample;

    return res;
}

/// \brief Reads a single line out of a tiff file
/// \param data buffer array for the data
/// \param row indexes the line to read
/// \param fname file name of the image file
template <class ImgType>
int ReadTIFFLine(ImgType *data,uint32_t row, const std::string & fname)
{
	std::stringstream msg;
	TIFF *image;

    if ((image = TIFFOpen(fname.c_str(), "r")) == nullptr)
    {
		msg.str("");
		msg<<"ReadTIFFLine: Could not open "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	
    if (TIFFReadScanline(image,static_cast<tdata_t>(data), row, 0) != 1)
    {
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
