//
// This file is part of the ModuleConfigurator library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:24:22 +0200 (Mon, 21 May 2012) $
// $Rev: 1318 $
// $Id: ConfiguratorDialogBase.cpp 1318 2012-05-21 14:24:22Z kaestner $
//


// ModuleConfigurator.cpp : Defines the exported functions for the DLL application.
//

#include "ConfiguratorDialogBase.h"
#include <QDialogButtonBox>
#include <QPushButton>
// This is the constructor of a class that has been exported.
// see ModuleConfigurator.h for the class definition

ConfiguratorDialogBase::ConfiguratorDialogBase(std::string name, bool emptyDialog, bool hasApply, bool needImages, QWidget *parent) :
    QDialog(parent),
    logger(name),
    m_ControlButtons(hasApply ? (QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply) : (QDialogButtonBox::Ok | QDialogButtonBox::Cancel)),
    m_bNeedImages(needImages)
{
    if (!emptyDialog) {
        setWindowTitle(QString::fromStdString(name));
        this->setLayout(&m_LayoutMain);
        m_LayoutMain.addWidget(&m_FrameMain);
        m_LayoutMain.addWidget(&m_ControlButtons);

        connect(&m_ControlButtons,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_ButtonBox_Clicked(QAbstractButton*)));
    }
}

ConfiguratorDialogBase::~ConfiguratorDialogBase()
{
}

int ConfiguratorDialogBase::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &UNUSED(img))
{
    return QDialog::exec();
}

void ConfiguratorDialogBase::on_ButtonBox_Clicked(QAbstractButton *button)
{
    std::ostringstream msg;
    QDialogButtonBox::StandardButton standardButton = m_ControlButtons.standardButton(button);

    logger(kipl::logging::Logger::LogMessage,"Clicked");
    switch(standardButton) {
        // Standard buttons:
        case QDialogButtonBox::Ok:
            logger(kipl::logging::Logger::LogMessage,"Ok");
            UpdateParameters();
            accept();
            break;
        case QDialogButtonBox::Cancel:
        logger(kipl::logging::Logger::LogMessage,"Cancel");
            reject();
            break;
        case QDialogButtonBox::Apply:
        logger(kipl::logging::Logger::LogMessage,"Apply");
            ApplyParameters();
            break;
        }
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetProjection(kipl::base::TImage<float,3> img, size_t n)
{
	kipl::base::TImage<float,2> proj(img.Dims());

	memcpy(proj.GetDataPtr(), img.GetLinePtr(0,n), sizeof(float)*proj.Size());

	return proj;
}

kipl::base::TImage<float,2> ConfiguratorDialogBase::GetSinogram(kipl::base::TImage<float,3> img, size_t n)
{
	size_t dims[2]={img.Size(0),img.Size(2)};
	kipl::base::TImage<float,2> sino(dims);

    if (n<=img.Size(1))
        throw kipl::base::KiplException("sinogram number out of bounds");

	for (size_t i=0; i<img.Size(2); i++)
		memcpy(sino.GetLinePtr(i),img.GetLinePtr(n,i),sizeof(float)*img.Size(0));

	return sino;
}
