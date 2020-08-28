//<LICENSE>

#ifndef KIPLPROCESSCONFIG_H
#define KIPLPROCESSCONFIG_H
#include "ProcessFramework_global.h"
#include <list>
#include <string>

#include <logging/logger.h>
#include <io/analyzefileext.h>

#include <ModuleConfig.h>
#include <ConfigBase.h>

class PROCESSFRAMEWORKSHARED_EXPORT KiplProcessConfig : public ConfigBase
{

public:
    struct PROCESSFRAMEWORKSHARED_EXPORT cSystemInformation {
		cSystemInformation();
		cSystemInformation(const cSystemInformation &a);
		cSystemInformation & operator=(const cSystemInformation &a);
		void ParseXML(xmlTextReaderPtr reader);

		size_t nMemory;
		kipl::logging::Logger::LogLevel eLogLevel;
        std::string WriteXML(int indent=0);
	};

    struct PROCESSFRAMEWORKSHARED_EXPORT cImageInformation {
		cImageInformation();
		cImageInformation(const cImageInformation &a);
		cImageInformation & operator=(const cImageInformation &a);
        std::string WriteXML(int indent=0);
		void ParseXML(xmlTextReaderPtr reader);

		std::string sSourcePath;
		std::string sSourceFileMask;

		bool bUseROI;
                std::vector<size_t> nROI;
		size_t nFirstFileIndex;
		size_t nLastFileIndex;
        size_t nStepFileIndex;
        size_t nStride;
        size_t nRepeat;

        kipl::base::eImageFlip eFlip;
        kipl::base::eImageRotate eRotate;
	};

    struct PROCESSFRAMEWORKSHARED_EXPORT cOutImageInformation {
		cOutImageInformation();
		cOutImageInformation(const cOutImageInformation &a);
		cOutImageInformation & operator=(const cOutImageInformation &a);
        std::string WriteXML(int indent=0);
		void ParseXML(xmlTextReaderPtr reader);

		bool bRescaleResult;
		kipl::io::eFileType eResultImageType;

		std::string sDestinationPath;
		std::string sDestinationFileMask;
	};

    KiplProcessConfig(const std::string &appPath);
	~KiplProcessConfig(void);
	std::string WriteXML();

	cSystemInformation    mSystemInformation;
	cImageInformation     mImageInformation;
	cOutImageInformation  mOutImageInformation;

private:
    virtual void ParseConfig(xmlTextReaderPtr reader, std::string sName);
    virtual void ParseProcessChain(xmlTextReaderPtr reader);
    /// Sanity check on the number of slices to be reconstruct during Config
    virtual std::string SanitySlicesCheck() {return "";}
    /// Sanity message that warns about the number of slices that are being configured
    virtual std::string SanityMessage(bool mess) {return "";}
};

#endif
