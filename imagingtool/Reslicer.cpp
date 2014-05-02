//
// This file is part of the MuhRec reconstruction application by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2010-09-05 15:55:11 +0200 (So, 05 Sep 2010) $
// $Rev: 694 $
//

#include "stdafx.h"
#include "Reslicer.h"

#include <strings/filenames.h>
#include <base/KiplException.h>

#include <cstring>
#include <cmath>
#include <sstream>


TIFFReslicer::TIFFReslicer() :
logger("TIFFReslicer"),
m_nMaxBlock(128),
m_nSliceCount(0),
m_nCurrentFile(0),
m_nCurrentBlock(0),
m_nBlocks(0),
buffer(NULL)
{
	memset(m_DstImages,0,m_nMaxBlock*sizeof(TIFF *));
	TIFFSetWarningHandler(0);
}

TIFFReslicer::~TIFFReslicer() {
	for (size_t i=0; i<m_nMaxBlock; i++) {
		if (m_DstImages[i]!=NULL) {
			TIFFClose(m_DstImages[i]);
			m_DstImages[i]=NULL;
		}
	}
	if (buffer!=NULL)
		delete [] buffer;
}

int TIFFReslicer::process(std::string sSrcMask, size_t nFirst, size_t nLast, std::string sDstMask, kipl::base::eImagePlanes plane)
{
	std::ostringstream msg;
	std::string fname, ext;
	kipl::strings::filenames::MakeFileName(sSrcMask,nFirst,fname,ext,'#','0');
	msg<<"Getting header for "<<fname;
	logger(kipl::logging::Logger::LogMessage,msg.str());
	try {
        m_nBytesPerPixel=GetHeader(fname)/8;
	}
	catch (kipl::base::KiplException &e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}


	size_t nSliceBlocks = 0;
	size_t nBlockRest   = 0;
	size_t nImages      = nLast - nFirst + 1;

	switch (plane) {
			case kipl::base::ImagePlaneXY : logger(kipl::logging::Logger::LogWarning,"Plane XY is irrelevant for the reslicer."); return -1;
			case kipl::base::ImagePlaneXZ :
				m_nBytesPerLine = m_nBytesPerPixel * m_nImageDims[0];
				nSliceBlocks = m_nImageDims[1] / m_nMaxBlock;
				nBlockRest   = m_nImageDims[1] % m_nMaxBlock;
				break;
			case kipl::base::ImagePlaneYZ :
				m_nBytesPerLine = m_nBytesPerPixel * m_nImageDims[1];
				nSliceBlocks = m_nImageDims[0] / m_nMaxBlock;
				nBlockRest   = m_nImageDims[0] % m_nMaxBlock;
				break;
	}

	size_t nFirstSlice=0;
	size_t nLastSlice=0;
	size_t nBlock=0;
	char *pLineBuffer=new char[m_nBytesPerLine+16];

	try {
		for (nBlock=0; nBlock<nSliceBlocks; nBlock++) {
			size_t nFirstSlice = nBlock*m_nMaxBlock;
			size_t nLastSlice  = (nBlock+1)*m_nMaxBlock-1;
			CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[0],nImages);

			for (size_t nFileIndex=nFirst; nFileIndex<=nLast; nFileIndex++) {
				kipl::strings::filenames::MakeFileName(sSrcMask,nFileIndex,fname,ext,'#','0');
				LoadBuffer(fname);
                msg.str("");
               // msg<<"Bytes/pixel="<<m_nBytesPerPixel<<", Bytes/line="<<m_nBytesPerLine<<", dims="<<m_nImageDims[0]<<", "<<m_nImageDims[1];
               // logger(kipl::logging::Logger::LogMessage,msg.str());
				for (size_t nSliceIndex=0; nSliceIndex<m_nMaxBlock; nSliceIndex++) {
					size_t idx=nSliceIndex+nFirstSlice;
					if (plane == kipl::base::ImagePlaneXZ )
						WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,buffer+idx*m_nBytesPerLine);
					else {

//                        for (int i=0; i<m_nImageDims[1]; i++) {
//                            for (int j=0; j<m_nBytesPerPixel; j++) {
//                                pLineBuffer[i*m_nBytesPerPixel+j]=255; //buffer[idx*m_nBytesPerPixel+i*m_nBytesPerLine+j];
//                                //pLineBuffer[i*m_nBytesPerPixel+j]=buffer[idx*m_nBytesPerPixel+i*m_nBytesPerLine+j];
//                            }
//                        }
                        memset(pLineBuffer,160,m_nBytesPerLine);
                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,pLineBuffer);
					}

				}
			}
			CloseImages();
		}
	}
	catch (kipl::base::KiplException & e)
	{
		msg.str("");
		msg<<"Failed during full block processing with exception:\n"<<e.what();
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception & e)
	{
		msg.str("");
		msg<<"Failed during full block processing with exception:\n"<<e.what();
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

    try {
        if (nBlockRest!=0) {
            nFirstSlice = nSliceBlocks*m_nMaxBlock;
            nLastSlice  = nFirstSlice+nBlockRest;
            CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[0],nImages);

            for (size_t nFileIndex=nFirst; nFileIndex<nLast; nFileIndex++) {
                kipl::strings::filenames::MakeFileName(sSrcMask,nFileIndex,fname,ext,'#','0');
                LoadBuffer(fname);

                for (size_t nSliceIndex=0; nSliceIndex<=nBlockRest; nSliceIndex++) {
                    size_t idx=nSliceIndex+nFirstSlice;
                    if (plane == kipl::base::ImagePlaneXZ )
                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,buffer+idx*m_nBytesPerLine);
                    else {
                        for (int i=0; i<m_nImageDims[1]; i++)
                            for (int j=0; j<m_nBytesPerPixel; j++)
                                pLineBuffer[i*m_nBytesPerPixel+j]=buffer[idx*m_nBytesPerPixel+i*m_nBytesPerLine+j];

                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,pLineBuffer);
                    }
                }
            }
            CloseImages();
        }
    }
    catch (kipl::base::KiplException & e)
    {
        msg.str("");
        msg<<"Failed during rest block processing with kipl exception:\n"<<e.what();
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception & e)
    {
        msg.str("");
        msg<<"Failed during rest block processing with STL exception:\n"<<e.what();
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

	delete [] pLineBuffer;

	return 0;
}

float TIFFReslicer::progress()
{
	return 0.0f;
}

int TIFFReslicer::GetHeader(std::string fname)
{
	std::stringstream msg;
	TIFF *image;

	// Open the TIFF image
	if((image = TIFFOpen(fname.c_str(), "r")) == NULL){
		msg.str("");
		msg<<"Could not open image "<<fname;
		logger(kipl::logging::Logger::LogError,msg.str());
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

	// Check that it is of a type that we support
	if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &m_SrcInfo.nBitsPerSample) == 0) ){
		throw kipl::base::KiplException("Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}

	if((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &m_SrcInfo.nSampleFormat) == 0) ){
		logger(kipl::logging::Logger::LogWarning,"Either undefined or unsupported sample format - assuming unsigned integer");
		m_SrcInfo.nSampleFormat=1;
	}

	if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &m_SrcInfo.nSamplesPerPixel) == 0) || (m_SrcInfo.nSamplesPerPixel != 1)){
		throw kipl::base::KiplException("Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// We need to set some values for basic tags before we can add any data
	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,&m_nImageDims[0]);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &m_nImageDims[1]);

	char tmpstr[1024];
	TIFFGetField(image,TIFFTAG_ARTIST,tmpstr);
	m_SrcInfo.sArtist=tmpstr;
	TIFFGetField(image,TIFFTAG_COPYRIGHT,tmpstr);
	m_SrcInfo.sCopyright=tmpstr;
	TIFFGetField(image,270,tmpstr); // Description
	m_SrcInfo.sDescription=tmpstr;
	TIFFGetField(image,TIFFTAG_SOFTWARE,tmpstr);
	m_SrcInfo.sSoftware=tmpstr;

	int resunit=0;
	TIFFGetField(image, TIFFTAG_RESOLUTIONUNIT, &resunit);
	float resX,resY;
	TIFFGetField(image,TIFFTAG_XRESOLUTION,&resX);
	TIFFGetField(image,TIFFTAG_XRESOLUTION,&resY);

	switch (resunit) {
		case RESUNIT_NONE :
		case RESUNIT_INCH :
			m_SrcInfo.SetDPIX(resX);
			m_SrcInfo.SetDPIY(resY);
			break;
		case RESUNIT_CENTIMETER :
			m_SrcInfo.SetDPCMX(resX);
			m_SrcInfo.SetDPCMY(resY);
			break;
	}

	TIFFClose(image);

	m_nBytesPerPixel=m_SrcInfo.nBitsPerSample/8;

	if (m_nBytesPerPixel==0)
		throw kipl::base::KiplException("Unsupported number of bits",__FILE__,__LINE__);

	return 0;
}

int TIFFReslicer::LoadBuffer(std::string fname)
{
	std::stringstream msg;
	TIFF *image;
	uint16 photo, spp, fillorder,bps, sformat;
	tsize_t stripSize, stripCount;
	unsigned long imageOffset;
	long result;
	int stripMax;
	unsigned char tempbyte;
	unsigned long bufferSize, count;

	TIFFSetWarningHandler(0);
	// Open the TIFF image
	if((image = TIFFOpen(fname.c_str(), "r")) == NULL){
		msg.str();
		msg<<"LoadBuffer: Could not open image "<<fname;
		throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
	}

	// Check that it is of a type that we support
	if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) ){
		throw kipl::base::KiplException("LoadBuffer: Either undefined or unsupported number of bits per pixel",__FILE__,__LINE__);
	}
    m_nBytesPerPixel=bps/8;

	if((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sformat) == 0) ){
		sformat=1; // Assuming unsigned integer data if unknown
	}
	//std::cout<<"sformat "<<sformat<<std::endl;

	if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) || (spp != 1)){
		throw kipl::base::KiplException("LoadBuffer: Either undefined or unsupported number of samples per pixel",__FILE__,__LINE__);
	}

	// Read in the possibly multiple strips
	stripSize = TIFFStripSize (image);
	stripMax = TIFFNumberOfStrips (image);
	imageOffset = 0;

	//bufferSize = TIFFNumberOfStrips (image) * stripSize * stripMax;
	bufferSize = stripSize * stripMax;
	//std::cout<<"buffer size: "<<bufferSize<<", number of strips:"<<TIFFNumberOfStrips (image)<<", stripSize:" <<stripSize<<", stripMax:"<<stripMax<<std::endl;

	try {
		if (buffer!=NULL) {
			delete [] buffer;
			buffer=NULL;
		}
		if((buffer = new char[bufferSize]) == NULL){
			msg.str("");
			msg<<"LoadBuffer: Could not allocate"<<bufferSize<<" bytes for the uncompressed image";
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}
	}
	catch (std::bad_alloc & E) {
		msg.str("");
		msg<<"LoadBuffer: Could not allocate"<<bufferSize<<" bytes for the uncompressed image ("<<E.what()<<")";
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
		logger(kipl::logging::Logger::LogWarning,"Fixing the photometric interpretation");

		for(count = 0; count < bufferSize; count++)
			buffer[count] = ~buffer[count];
	}

	// Deal with fillorder
	if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0){
		fillorder=FILLORDER_MSB2LSB;
	}

	if(fillorder != FILLORDER_MSB2LSB){
		// We need to swap bits -- ABCDEFGH becomes HGFEDCBA
		logger(kipl::logging::Logger::LogWarning,"Fixing the fillorder");

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

	TIFFClose(image);

	return bps;
}

int TIFFReslicer::CreateHeaders(std::string mask, size_t nFirst, size_t nLast, size_t width, size_t height)
{
	std::ostringstream msg;
	msg<<"Creating headers for slabs "<<nFirst<<"-"<<nLast;
	logger(kipl::logging::Logger::LogMessage,msg.str());

	std::string fname,ext;

	for (size_t idx=nFirst, i=0; idx<=nLast; idx++, i++) {
		kipl::strings::filenames::MakeFileName(mask,idx,fname,ext,'#','0');
		if((m_DstImages[i] = TIFFOpen(fname.c_str(), "w")) == NULL){
			msg.str("");
			msg<<"WriteTIFF: Could not open "<<fname<<" for writing";
			logger(kipl::logging::Logger::LogError,msg.str());
			throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
		}

		size_t fileIdx=i;
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_IMAGEWIDTH,      width);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_IMAGELENGTH,     0);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_BITSPERSAMPLE,   m_SrcInfo.nBitsPerSample);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_SAMPLESPERPIXEL, m_SrcInfo.nSamplesPerPixel);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_SAMPLEFORMAT,    m_SrcInfo.nSampleFormat);

		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_ROWSPERSTRIP,    1);

		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_COMPRESSION,     COMPRESSION_NONE);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_MINISBLACK);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_FILLORDER,       FILLORDER_MSB2LSB);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);

		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_XRESOLUTION,     m_SrcInfo.GetDPIX());
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_YRESOLUTION,     m_SrcInfo.GetDPIY());
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_RESOLUTIONUNIT,  RESUNIT_INCH);
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_COPYRIGHT,       m_SrcInfo.sCopyright.c_str());
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_ARTIST,          m_SrcInfo.sArtist.c_str());
		TIFFSetField(m_DstImages[fileIdx], TIFFTAG_SOFTWARE,        m_SrcInfo.sSoftware.c_str());
		TIFFSetField(m_DstImages[fileIdx], 270,                     m_SrcInfo.sDescription.c_str()); // Description tag
	}

	logger(kipl::logging::Logger::LogMessage,"Headers are created");
	return 0;
}

int TIFFReslicer::WriteLine(TIFF *img, size_t line, char * pLineBuffer)
{
	TIFFSetField(img, TIFFTAG_IMAGELENGTH, line+1);
	if (TIFFWriteEncodedStrip(img, line, pLineBuffer, m_nBytesPerLine)==-1)
	{
		throw kipl::base::KiplException("Failed to write strip",__FILE__,__LINE__);
	}

	return 0;
}

int TIFFReslicer::CloseImages()
{
	size_t i;
	for (i=0; i<m_nMaxBlock; i++) {
		if (m_DstImages[i]!=NULL) {
			TIFFClose(m_DstImages[i]);
			m_DstImages[i]=NULL;
		}
	}

	return i;
}





