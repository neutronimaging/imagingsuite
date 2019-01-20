/*
 * Reslicer.h
 *
 *  Created on: Oct 31, 2011
 *      Author: kaestner
 */

#ifndef RESLICER_H_
#define RESLICER_H_

#include <base/kiplenums.h>
#include <base/imageinfo.h>
#include <logging/logger.h>
#include <tiffio.h>

class TIFFReslicer {
private:
	kipl::logging::Logger logger;
	size_t m_nMaxBlock;
public:
	TIFFReslicer();
	virtual ~TIFFReslicer();
    int process(std::string sSrcMask, int nFirst, int nLast, std::string sDstMask, kipl::base::eImagePlanes plane);
    int process();
	float progress();

    std::string WriteXML(size_t indent=4);
    void ParseXML(std::string fname);

    std::string m_sSourceMask;
    std::string m_sDestinationPath;
    std::string m_sDestinationMask;

    int m_nFirst;
    int m_nLast;
    int m_nFirstXZ;
    int m_nLastXZ;
    int m_nFirstYZ;
    int m_nLastYZ;

    bool m_bResliceXZ;
    bool m_bResliceYZ;
protected:
	int GetHeader(std::string fname);
	int CreateHeaders(std::string mask, size_t nFirst, size_t nLast, size_t width, size_t height);
    int WriteLine(TIFF *img, size_t line, size_t bytesPerLine, char * pLineBuffer);
	int CloseImages();
	int LoadBuffer(std::string fname);

	kipl::base::ImageInfo m_SrcInfo;
	kipl::base::ImageInfo m_DstInfo;

	int m_nImageDims[2];

	TIFF *m_DstImages[256];
	size_t m_nSliceCount;
	size_t m_nCurrentFile;
	size_t m_nCurrentBlock;
	size_t m_nBlocks;

	char *buffer;
	int m_nBytesPerPixel;
	int m_nBytesPerLine;
};

#endif /* RESLICER_H_ */
