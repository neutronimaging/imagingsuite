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

class DLL_EXPORT KiplProcessConfig
{
protected:
	kipl::logging::Logger logger;

public:
	struct DLL_EXPORT cUserInformation {
		cUserInformation() ;
		cUserInformation(const cUserInformation &info);
		cUserInformation & operator=(const cUserInformation & info);
		std::string WriteXML(size_t indent=0);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sOperator;
		std::string sInstrument;
		std::string sProjectNumber;
		std::string sSample;
		std::string sComment;
	};

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
	void LoadConfigFile(std::string configfile);

	cUserInformation      mUserInformation;
	cSystemInformation    mSystemInformation;
	cImageInformation     mImageInformation;
	cOutImageInformation  mOutImageInformation;

	std::list<ModuleConfig> modules;

private:
	void ParseProcessChain(xmlTextReaderPtr reader);
};

#endif
