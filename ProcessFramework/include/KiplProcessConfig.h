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

#ifndef __KIPLPROCESSCONFIG_H
#define __KIPLPROCESSCONFIG_H

#include <list>
#include <string>

#include <logging/logger.h>
#include <io/io_stack.h>

#include <ModuleConfig.h>
#include <ConfigBase.h>

class DLL_EXPORT KiplProcessConfig : public ConfigBase
{

public:
	struct DLL_EXPORT cSystemInformation {
		cSystemInformation();
		cSystemInformation(const cSystemInformation &a);
		cSystemInformation & operator=(const cSystemInformation &a);
		void ParseXML(xmlTextReaderPtr reader);

		size_t nMemory;
		kipl::logging::Logger::LogLevel eLogLevel;
		std::string WriteXML(size_t indent=0);
	};

	struct DLL_EXPORT cImageInformation {			
		cImageInformation();
		cImageInformation(const cImageInformation &a);
		cImageInformation & operator=(const cImageInformation &a);
		std::string WriteXML(size_t indent=0);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sSourcePath;
		std::string sSourceFileMask;

		bool bUseROI;
		size_t nROI[4];
		size_t nFirstFileIndex;
		size_t nLastFileIndex;
        size_t nStepFileIndex;
	};

	struct DLL_EXPORT cOutImageInformation {
		cOutImageInformation();
		cOutImageInformation(const cOutImageInformation &a);
		cOutImageInformation & operator=(const cOutImageInformation &a);
		std::string WriteXML(size_t indent=0);
		void ParseXML(xmlTextReaderPtr reader);

		bool bRescaleResult;
		kipl::io::eFileType eResultImageType;

		std::string sDestinationPath;
		std::string sDestinationFileMask;
	};

	KiplProcessConfig(void);
	~KiplProcessConfig(void);
	std::string WriteXML();

	cSystemInformation    mSystemInformation;
	cImageInformation     mImageInformation;
	cOutImageInformation  mOutImageInformation;

private:
    virtual void ParseConfig(xmlTextReaderPtr reader, std::string sName);
    virtual void ParseProcessChain(xmlTextReaderPtr reader);
};

#endif
