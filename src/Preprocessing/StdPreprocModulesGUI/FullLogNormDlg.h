/*
 * FullLogNormDlg.h
 *
 *  Created on: Feb 29, 2012
 *      Author: kaestner
 */

#ifndef FULLLOGNORMDLG_H_
#define FULLLOGNORMDLG_H_
#include "StdPreprocModulesGUI_global.h"

#include <ConfiguratorDialogBase.h>
#include <QVBoxLayout>
#include <QCheckBox>

class FullLogNormDlg: public ConfiguratorDialogBase {
public:
    FullLogNormDlg(QWidget * parent = NULL);
	virtual ~FullLogNormDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
	void UpdateParameterList(std::map<std::string, std::string> &parameters);
	void UpdateDialogFromParameterList(std::map<std::string, std::string> &parameters);

    QVBoxLayout m_vbox_main;
    QCheckBox m_checkbox_usedose;
    QCheckBox m_checkbox_uselut;

	bool m_bUseDose;
	bool m_bUseLUT;
};

#endif /* FULLLOGNORMDLG_H_ */
