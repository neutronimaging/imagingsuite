//<LICENSE>
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MODULECONFIGURATOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MODULECONFIGURATOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef CONFIGURATORDIALOGBASE_H
#define CONFIGURATORDIALOGBASE_H

#include "../src/stdafx.h"
#include "QtModuleConfigure_global.h"
//#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <string>
#include <map>
#include <base/timage.h>
#include <logging/logger.h>

#include <ConfigBase.h>

class QTMODULECONFIGURESHARED_EXPORT ConfiguratorDialogBase : public QDialog
{
    Q_OBJECT

protected:
	kipl::logging::Logger logger;
public:
    explicit ConfiguratorDialogBase(std::string name, bool emptyDialog, bool hasApply, bool needImages, QWidget *parent=nullptr);
    virtual ~ConfiguratorDialogBase();
    bool NeedImages() {return m_bNeedImages;}

    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);

protected:
    virtual int exec() { return QDialog::exec(); }
    virtual void UpdateDialog() = 0;
    virtual void UpdateParameters() = 0;
    virtual void ApplyParameters() = 0;

	kipl::base::TImage<float,2> GetProjection(kipl::base::TImage<float,3> img, size_t n);
	kipl::base::TImage<float,2> GetSinogram(kipl::base::TImage<float,3> img, size_t n);

    bool m_bNeedImages;

	ConfigBase * m_Config;
};

#endif
