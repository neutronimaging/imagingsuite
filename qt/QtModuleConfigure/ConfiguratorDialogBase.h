//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ConfiguratorDialogBase.h 1318 2012-05-21 14:24:22Z kaestner $
//


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MODULECONFIGURATOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MODULECONFIGURATOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __CONFIGURATORDIALOGBASE_H
#define __CONFIGURATORDIALOGBASE_H
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#define DLL_EXPORT __declspec(dllexport)

#else
#define DLL_EXPORT
#endif

#include <QDialog>
#include <QFrame>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <string>
#include <map>
#include <base/timage.h>
#include <logging/logger.h>

#include <ConfigBase.h>

class DLL_EXPORT ConfiguratorDialogBase : public QDialog
{
protected:
	kipl::logging::Logger logger;
public:
	ConfiguratorDialogBase(std::string name);
	virtual ~ConfiguratorDialogBase();
	
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)=0;

protected:
    virtual int exec() { return QDialog::exec(); }
	kipl::base::TImage<float,2> GetProjection(kipl::base::TImage<float,3> img, size_t n);
	kipl::base::TImage<float,2> GetSinogram(kipl::base::TImage<float,3> img, size_t n);

    QVBoxLayout m_LayoutMain;
    QFrame m_FrameMain;
    QDialogButtonBox m_ControlButtons;

	ConfigBase * m_Config;
};

#endif
