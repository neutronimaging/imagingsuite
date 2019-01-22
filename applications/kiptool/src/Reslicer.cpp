//<LICENSE>

#include "stdafx.h"
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>

#include <libxml/xmlreader.h>

#include <QDebug>

#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <base/KiplException.h>

#include "Reslicer.h"


TIFFReslicer::TIFFReslicer() :
    logger("TIFFReslicer"),
    m_nMaxBlock(128),
    m_sSourceMask("/data/slices_####.tif"),
    m_sDestinationPath("/data"),
    m_sDestinationMask("verticalslices_####.tif"),

    m_nFirst(0),
    m_nLast(100),
    m_nFirstXZ(0),
    m_nLastXZ(0),
    m_nFirstYZ(0),
    m_nLastYZ(0),

    m_bResliceXZ(true),
    m_bResliceYZ(true),

    m_nSliceCount(0),
    m_nCurrentFile(0),
    m_nCurrentBlock(0),
    m_nBlocks(0),
    buffer(nullptr)
{
	memset(m_DstImages,0,m_nMaxBlock*sizeof(TIFF *));
    TIFFSetWarningHandler(nullptr);
}

TIFFReslicer::~TIFFReslicer() {
	for (size_t i=0; i<m_nMaxBlock; i++) {
        if (m_DstImages[i]!=nullptr) {
			TIFFClose(m_DstImages[i]);
            m_DstImages[i]=nullptr;
        }
	}
    if (buffer!=nullptr)
		delete [] buffer;
}

std::string TIFFReslicer::WriteXML(size_t indent)
{
    std::ostringstream xml;

    size_t blockindent=4;
    xml<<std::setw(indent)<<" "<<"<reslice>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<srcmask>"<<m_sSourceMask<<"</srcmask>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<dstpath>"<<m_sDestinationPath<<"</dstpath>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<first>"<<m_nFirst<<"</first>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<last>"<<m_nLast<<"</last>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<reslicexz>"<<kipl::strings::bool2string(m_bResliceXZ)<<"</reslicexz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<firstxz>"<<m_nFirstXZ<<"</firstxz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<lastxz>"<<m_nLastXZ<<"</lastxz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<resliceyz>"<<kipl::strings::bool2string(m_bResliceYZ)<<"</resliceyz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<firstyz>"<<m_nFirstYZ<<"</firstyz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<lastyz>"<<m_nLastYZ<<"</lastyz>\n";
    xml<<std::setw(indent)<<" "<<"</reslice>";

    return xml.str();
}

void TIFFReslicer::ParseXML(std::string fname)
{
    xmlTextReaderPtr reader;
    const xmlChar *name;
    const xmlChar *value;
    int ret;
    std::string sName;
    std::string sValue;
    std::ostringstream msg;

    reader = xmlReaderForFile(fname.c_str(), nullptr, 0);
    if (reader != nullptr) {
        ret = xmlTextReaderRead(reader);
        name = xmlTextReaderConstName(reader);

        if (name==nullptr) {
            throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
        }

        sName=reinterpret_cast<const char *>(name);
        msg.str(""); msg<<"Found "<<sName<<" expect reslice";
        logger(kipl::logging::Logger::LogMessage,msg.str());
        if (sName!="reslice") {
            msg.str();
            msg<<"Unexpected project contents in parameter file ("<<sName<<"!=reslice)";
            logger(kipl::logging::Logger::LogMessage,msg.str());
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }

        logger.verbose("Got reslice project");

        ret = xmlTextReaderRead(reader);

        while (ret == 1) {
            if (xmlTextReaderNodeType(reader)==1) {
                name = xmlTextReaderConstName(reader);
                ret=xmlTextReaderRead(reader);
                value = xmlTextReaderConstValue(reader);
                if (name==nullptr) {
                    throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
                }
                sName=reinterpret_cast<const char *>(name);

                if (value!=nullptr)
                    sValue=reinterpret_cast<const char *>(value);
                else {
                    msg.str("");
                    msg<<"Parameter "<<sValue<<" does not have a value";
                    logger.warning(msg.str());
                    sValue="Empty";
                }

                if (sName=="srcmask")
                    m_sSourceMask  = sValue;

                if (sName=="dstpath")
                    m_sDestinationPath   = sValue;

                if (sName=="first")
                    m_nFirst = std::stoi(sValue);

                if (sName=="last")
                    m_nLast  = std::stoi(sValue);

                if (sName=="reslicexz")
                    m_bResliceXZ   = kipl::strings::string2bool(sValue);

                if (sName=="firstxz")
                    m_nFirstXZ     = std::stoi(sValue);

                if (sName=="lastxz")
                    m_nLastXZ      = std::stoi(sValue);

                if (sName=="resliceyz")
                    m_bResliceYZ   = kipl::strings::string2bool(sValue);

                if (sName=="firstyz")
                    m_nFirstYZ = std::stoi(sValue);

                if (sName=="lastyz")
                    m_nLastYZ = std::stoi(sValue);


            }
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            std::stringstream str;
            str<<"Reslicer failed to parse "<<fname;
            logger.error(str.str());
            throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
        }
    } else {
        std::stringstream str;
        str<<"Reslicer config could not open "<<fname;
        logger.error(str.str());
        throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
    }
    logger.verbose("Parsing parameter file done");

}

int TIFFReslicer::process()
{
    std::string fmask=m_sDestinationPath;
    kipl::strings::filenames::CheckPathSlashes(fmask,true);
    fmask+=m_sDestinationMask;
    std::string a,b;
    size_t pos=fmask.find('#',0);
    if (pos==std::string::npos) {
        throw kipl::base::KiplException("Failed to create destination filename.\n There are no # in the file mask.",__FILE__,__LINE__);
    }
    a=fmask.substr(0,pos);
    b=fmask.substr(pos);

    std::ostringstream msg;

    if (m_bResliceXZ){
        fmask=a+"XZ_"+b;
        logger(kipl::logging::Logger::LogMessage,fmask);
        try {
            process(m_sSourceMask,m_nFirst,m_nLast,fmask,kipl::base::ImagePlaneXZ);
        }
        catch (kipl::base::KiplException &E) {
            msg.str("");
            msg<<"XZ-Reslicing failed with an exception\n"<<E.what();
            logger(kipl::logging::Logger::LogError,msg.str());

            return -1;
        }
        logger(kipl::logging::Logger::LogMessage,"Reslice XZ done.");

    }

    if (m_bResliceYZ) {
        fmask=a+"YZ_"+b;
        logger(kipl::logging::Logger::LogMessage,fmask);
        try {
            process(m_sSourceMask,m_nFirst,m_nLast,fmask,kipl::base::ImagePlaneYZ);
        }
        catch (kipl::base::KiplException &E) {
            msg.str("");
            msg<<"YZ-Reslicing failed with an exception\n"<<E.what();
            logger(kipl::logging::Logger::LogError,msg.str());

            return -1;
        }
        logger(kipl::logging::Logger::LogMessage,"Reslice YZ done.");
    }




    return 0;
}

int TIFFReslicer::process(std::string sSrcMask, int nFirst, int nLast, std::string sDstMask, kipl::base::eImagePlanes plane)
{
	std::ostringstream msg;
	std::string fname, ext;
	kipl::strings::filenames::MakeFileName(sSrcMask,nFirst,fname,ext,'#','0');
	msg<<"Getting header for "<<fname;
	logger(kipl::logging::Logger::LogMessage,msg.str());
	try {
        GetHeader(fname);
	}
	catch (kipl::base::KiplException &e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}


	size_t nSliceBlocks = 0;
	size_t nBlockRest   = 0;
    size_t nImages      = static_cast<size_t>(nLast - nFirst + 1);

    m_nBytesPerReadLine = m_nBytesPerPixel * m_nImageDims[0];
	switch (plane) {
			case kipl::base::ImagePlaneXY : logger(kipl::logging::Logger::LogWarning,"Plane XY is irrelevant for the reslicer."); return -1;
			case kipl::base::ImagePlaneXZ :
                m_nBytesPerWriteLine = m_nBytesPerPixel * m_nImageDims[0];
				nSliceBlocks = m_nImageDims[1] / m_nMaxBlock;
				nBlockRest   = m_nImageDims[1] % m_nMaxBlock;
				break;
			case kipl::base::ImagePlaneYZ :
                m_nBytesPerWriteLine = m_nBytesPerPixel * m_nImageDims[1];
                nSliceBlocks = m_nImageDims[0] / m_nMaxBlock;
                nBlockRest   = m_nImageDims[0] % m_nMaxBlock;
				break;
	}

	size_t nFirstSlice=0;
	size_t nLastSlice=0;
	size_t nBlock=0;

    char *pLineBuffer=new char[m_nBytesPerWriteLine+16];

	try {
		for (nBlock=0; nBlock<nSliceBlocks; nBlock++) {
			size_t nFirstSlice = nBlock*m_nMaxBlock;
			size_t nLastSlice  = (nBlock+1)*m_nMaxBlock-1;

                switch (plane) {
                case kipl::base::ImagePlaneXY :
                    logger(kipl::logging::Logger::LogWarning,"Plane XY is irrelevant for the reslicer.");
                    return -1;
                case kipl::base::ImagePlaneXZ :
                    CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[0],nImages);
                    break;
                case kipl::base::ImagePlaneYZ :
                    CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[1],nImages); // now YZ images have the right dimensions
                    break;
                }



            for (int nFileIndex=nFirst; nFileIndex<=nLast; nFileIndex++) {
				kipl::strings::filenames::MakeFileName(sSrcMask,nFileIndex,fname,ext,'#','0');
				LoadBuffer(fname);
                msg.str("");
                msg<<"Bytes/pixel="<<m_nBytesPerPixel<<", Bytes/line="<<m_nBytesPerReadLine<<", dims="<<m_nImageDims[0]<<", "<<m_nImageDims[1];
             //   logger(kipl::logging::Logger::LogMessage,msg.str());
                for (int nSliceIndex=0; nSliceIndex<m_nMaxBlock; nSliceIndex++) {
                    int idx=nSliceIndex+nFirstSlice;
					if (plane == kipl::base::ImagePlaneXZ )
                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,m_nBytesPerWriteLine, buffer+idx*m_nBytesPerReadLine);
					else {

                        for (int i=0; i<m_nImageDims[1]; i++) {
//                            std::copy_n(buffer+idx*m_nBytesPerPixel+i*m_nBytesPerLine,pLineBuffer+i*m_nBytesPerPixel,m_nBytesPerPixel);
                            for (int j=0; j<m_nBytesPerPixel; j++) {
                                pLineBuffer[i*m_nBytesPerPixel+j]=buffer[idx*m_nBytesPerPixel+i*m_nBytesPerReadLine+j];
                            }
                        }
                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,m_nBytesPerWriteLine,pLineBuffer);
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

    // Handling the block rest
    try {
        if (nBlockRest!=0) {
            nFirstSlice = nSliceBlocks*m_nMaxBlock;
            nLastSlice  = nFirstSlice+nBlockRest;

            switch (plane)
            {
            case kipl::base::ImagePlaneXY :
                logger(kipl::logging::Logger::LogWarning,"Plane XY is irrelevant for the reslicer.");
                return -1;
            case kipl::base::ImagePlaneXZ :
                CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[0],nImages);
                break;
            case kipl::base::ImagePlaneYZ :
                CreateHeaders(sDstMask,nFirstSlice,nLastSlice,m_nImageDims[1],nImages); // now YZ images have the right dimensions
                break;
            }

            for (int nFileIndex=nFirst; nFileIndex<=nLast; nFileIndex++) {
                kipl::strings::filenames::MakeFileName(sSrcMask,nFileIndex,fname,ext,'#','0');
                LoadBuffer(fname);

                for (size_t nSliceIndex=0; nSliceIndex<=nBlockRest; nSliceIndex++) {
                    int idx=nSliceIndex+nFirstSlice;
                    if (plane == kipl::base::ImagePlaneXZ )
                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst,m_nBytesPerWriteLine,buffer+idx*m_nBytesPerReadLine);
                    else {
                        for (int i=0; i<m_nImageDims[1]; i++)
                            for (int j=0; j<m_nBytesPerPixel; j++)
                                pLineBuffer[i*m_nBytesPerPixel+j]=buffer[idx*m_nBytesPerPixel+i*m_nBytesPerReadLine+j];

                        WriteLine(m_DstImages[nSliceIndex],nFileIndex-nFirst, m_nBytesPerWriteLine,pLineBuffer);
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
    if((image = TIFFOpen(fname.c_str(), "r")) == nullptr){
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

    TIFFSetWarningHandler(nullptr);
	// Open the TIFF image
    if((image = TIFFOpen(fname.c_str(), "r")) == nullptr){
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
        if (buffer!=nullptr) {
			delete [] buffer;
            buffer=nullptr;
		}
        if((buffer = new char[bufferSize]) == nullptr){
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
        if((m_DstImages[i] = TIFFOpen(fname.c_str(), "w")) == nullptr){
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

int TIFFReslicer::WriteLine(TIFF *img, size_t line, size_t bytesPerLine, char * pLineBuffer)
{
    TIFFSetField(img, TIFFTAG_IMAGELENGTH, line+1);
    if (TIFFWriteEncodedStrip(img, line, pLineBuffer, bytesPerLine)==-1)
	{
		throw kipl::base::KiplException("Failed to write strip",__FILE__,__LINE__);
	}

	return 0;
}

int TIFFReslicer::CloseImages()
{
	size_t i;
	for (i=0; i<m_nMaxBlock; i++) {
        if (m_DstImages[i]!=nullptr) {
			TIFFClose(m_DstImages[i]);
            m_DstImages[i]=nullptr;
        }
    }

	return i;
}





