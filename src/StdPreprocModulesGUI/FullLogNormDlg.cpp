/*
 * FullLogNormDlg.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: kaestner
 */

#include "stdafx.h"
#include "FullLogNormDlg.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>

FullLogNormDlg::FullLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("FullLogNormDlg", false,false, parent),
	m_checkbox_usedose("Use dose correction"),
	m_checkbox_uselut("Use look-up table computing"),
	m_bUseDose(false),
	m_bUseLUT(false)
{
    m_vbox_main.addWidget(&m_checkbox_usedose);
    m_vbox_main.addWidget(&m_checkbox_uselut);
    m_FrameMain.setLayout(&m_vbox_main);
    setWindowTitle("Configure the Full Log Norm module");
    UpdateDialog();

    show();
}

FullLogNormDlg::~FullLogNormDlg() {
}

int FullLogNormDlg::exec(ConfigBase * UNUSED(config), std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> UNUSED(img))
{
	UpdateDialogFromParameterList(parameters);
    UpdateDialog();
    std::ostringstream msg;

    int res=QDialog::exec();

    if (res == QDialog::Accepted) {
        UpdateParameterList(parameters);
        msg.str("");
        msg<<"Accepted and got "<<parameters.size()<<" parameters";
        logger(kipl::logging::Logger::LogMessage,msg.str());
	}

    return res;
}

void FullLogNormDlg::UpdateDialog()
{
    m_checkbox_usedose.setChecked(m_bUseDose);
    m_checkbox_uselut.setChecked(m_bUseLUT);
}

void FullLogNormDlg::UpdateParameters()
{
    m_bUseDose = m_checkbox_usedose.checkState();
    m_bUseLUT = m_checkbox_uselut.checkState();

}

void FullLogNormDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
	parameters.clear();
    UpdateParameters();
	parameters["usenormregion"]=kipl::strings::bool2string(m_bUseDose);
	parameters["uselut"]=kipl::strings::bool2string(m_bUseLUT);
}

void FullLogNormDlg::UpdateDialogFromParameterList(std::map<std::string, std::string> &parameters)
{
	m_bUseDose=kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));
	m_bUseLUT=kipl::strings::string2bool(GetStringParameter(parameters,"uselut"));
	UpdateDialog();
}

void FullLogNormDlg::ApplyParameters()
{}
