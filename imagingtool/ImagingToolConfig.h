#ifndef __MERGECONFIG_H
#define __MERGECONFIG_H

#include <string>
#include <cstddef>
#include <libxml/xmlreader.h>
#include <base/kiplenums.h>
#include <set>

#include <logging/logger.h>

class ImagingToolConfig {
	kipl::logging::Logger logger;
public:
	class MergeConfig {
	public:
		MergeConfig();
		std::string WriteXML(size_t indent=4);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sPathA;
		std::string sPathB;
		std::string sPathDest;

		std::string sFileMaskA;
		std::string sFileMaskB;
		std::string sFileMaskDest;

		size_t nFirstA;
		size_t nLastA;
		size_t nStartOverlapA;

		size_t nOverlapLength;

		size_t nFirstB;
		size_t nLastB;
		size_t nFirstDest;

		bool bCropSlices;
		ptrdiff_t nCropOffset[2];
		size_t nCrop[4];

		bool bShowLocalMix;
	};

	class ResliceConfig {
	public:
		ResliceConfig() {}
		std::string WriteXML(size_t indent=4);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sSourcePath;
		std::string sDestinationPath;
		std::string sSourceMask;

		size_t nFirst;
		size_t nLast;
        size_t nFirstXZ;
        size_t nLastXZ;
        size_t nFirstYZ;
        size_t nLastYZ;

		bool bResliceXZ;
		bool bResliceYZ;

	};

	class Fits2TifConfig {
	public:
		Fits2TifConfig();
		std::string WriteXML(size_t indent=4);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sSourcePath;
		std::string sSourceMask;

		size_t nFirstSrc;
		size_t nLastSrc;
		std::set<size_t> skip_list;
		bool bCrop;
		size_t nCrop[4];
		kipl::base::eImageFlip flip;
		kipl::base::eImageRotate rotate;
		bool bReplaceZeros;

		std::string sDestPath;
		std::string sDestMask;
		size_t nFirstDest;
        bool bUseSpotClean;
        float fSpotThreshold;

        bool bSortGoldenScan;
        int nGoldenScanArc;
	};

	ImagingToolConfig() : logger("TomoMergeConfig") {}
	std::string WriteXML();
	void LoadConfigFile(std::string filename);

	MergeConfig merge;
	ResliceConfig reslice;
	Fits2TifConfig fits2tif;

};
#endif
