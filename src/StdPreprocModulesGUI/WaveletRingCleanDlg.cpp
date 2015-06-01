/*
 * WaveletRingCleanDlg.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: kaestner
 */

#include "stdafx.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <ModuleException.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include "WaveletRingCleanDlg.h"
#include <wavelets/wavelets.h>
#include <StripeFilter.h>
#include <list>

#include <QMessageBox>

WaveletRingCleanDlg::WaveletRingCleanDlg(QWidget *parent):
ConfiguratorDialogBase("MedianMixRingClean2Dlg",false,true, true,parent),
//m_frame_original("Original"),
//m_frame_processed("Processed"),
//m_frame_difference("Difference"),
lbl_waveletname("Wavelet name"),
lbl_levels("Lambda"),
lbl_sigma("Sigma"),
m_nLevels(4),
m_fSigma(0.1f),
m_sWaveletName("daub17")
{
	float data[16]={1,2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15 };
	size_t dims[2]={4,4};

    m_viewer_difference.set_image(data,dims);
    m_viewer_original.set_image(data,dims);
    m_viewer_processed.set_image(data,dims);

//    m_frame_original.(m_viewer_original);
//    m_frame_original.set_size_request(256,256);

//    m_frame_processed.add(m_viewer_processed);
//    m_frame_processed.set_size_request(256,256);

//    m_frame_difference.add(m_viewer_difference);
//    m_frame_difference.set_size_request(256,256);
    m_hbox_viewers.addWidget(&m_viewer_original);
    m_hbox_viewers.addWidget(&m_viewer_processed);
    m_hbox_viewers.addWidget(&m_viewer_difference);
    m_vbox_main.addLayout(&m_hbox_viewers);

	const int nDigits=5;
    m_entry_levels.setRange(1,10);

    m_entry_sigma.setDecimals(nDigits);
    m_entry_sigma.setRange(0.0,1.0);
    m_entry_sigma.setSingleStep(0.01);

    m_hbox_parameters.addWidget(&lbl_levels);
    m_hbox_parameters.addWidget(&m_entry_levels);

    m_hbox_parameters.addWidget(&lbl_sigma);
    m_hbox_parameters.addWidget(&m_entry_sigma);

    prepare_waveletcombobox();

    m_hbox_parameters.addWidget(&lbl_waveletname);
    m_hbox_parameters.addWidget(&m_combobox_wavelets);
    m_hbox_parameters.addWidget(&m_combobox_method);

    m_vbox_parameters.addLayout(&m_hbox_parameters);

    m_vbox_main.addLayout(&m_vbox_parameters);
    m_FrameMain.setLayout(&m_vbox_main);

    setWindowTitle("Configure wavelet ring clean");

    show();

    //m_entry_levels.signal_value_changed().connect(sigc::mem_fun(*this,&WaveletRingCleanDlg::on_change_level));
}

WaveletRingCleanDlg::~WaveletRingCleanDlg() {
}

void WaveletRingCleanDlg::prepare_waveletcombobox()
{
	std::ostringstream msg;

	kipl::wavelets::WaveletKernel<float> wk("daub4");
	std::list<std::string> wlist=wk.NameList();
	std::list<std::string>::iterator it;

//    m_combobox_wavelets.clear();

    int default_wavelet, idx;
	for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
        QString str=QString::fromStdString(*it);
        if (*it==m_sWaveletName)
			default_wavelet=idx;
        m_combobox_wavelets.addItem(str);
	}

    m_combobox_wavelets.setCurrentIndex(default_wavelet);

    m_combobox_method.addItem("verticalzero");
    m_combobox_method.addItem("verticalfft");
}

void WaveletRingCleanDlg::ApplyParameters()
{
	size_t dims[3]={m_Projections.Size(0), 1,m_Projections.Size(2)};
	kipl::base::TImage<float,3> img(dims);

	const size_t N=512;
	size_t hist[N];
	float axis[N];
	size_t nLo, nHi;

	kipl::base::TImage<float,2> origsino=GetSinogram(m_Projections,m_Projections.Size(1)>>1);

	memcpy(img.GetDataPtr(),origsino.GetDataPtr(),origsino.Size()*sizeof(float));

	kipl::base::Histogram(origsino.GetDataPtr(), origsino.Size(), hist, N, 0.0f, 0.0f, axis);

	kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    m_viewer_original.set_image(origsino.GetDataPtr(),origsino.Dims(),axis[nLo],axis[nHi]);

	std::map<std::string, std::string> parameters;
	UpdateParameters();
	UpdateParameterList(parameters);

	std::map<std::string,std::string> pars;

	try {
		cleaner.Configure(*m_Config, parameters);
		cleaner.Process(img, pars);
	}
	catch (kipl::base::KiplException &e) {
        QMessageBox errdlg(this);
        errdlg.setText("Failed to process sinogram.");

		logger(kipl::logging::Logger::LogWarning,e.what());
		return ;
	}

	kipl::base::TImage<float,2> procsino(origsino.Dims());
	memcpy(procsino.GetDataPtr(),img.GetDataPtr(), procsino.Size()*sizeof(float));

	memset(hist,0,N*sizeof(size_t));
	memset(axis,0,N*sizeof(float));
	kipl::base::Histogram(procsino.GetDataPtr(), procsino.Size(), hist, N, 0.0f, 0.0f, axis);
	kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    m_viewer_processed.set_image(procsino.GetDataPtr(), procsino.Dims(),axis[nLo],axis[nHi]);

	kipl::base::TImage<float,2> diff=procsino-origsino;
	memset(hist,0,N*sizeof(size_t));
	memset(axis,0,N*sizeof(float));
	kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
	kipl::base::FindLimits(hist, N, 95.0, &nLo, &nHi);
    m_viewer_difference.set_image(diff.GetDataPtr(), diff.Dims());
}

void WaveletRingCleanDlg::on_change_level()
{
}

int WaveletRingCleanDlg::exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
{
	m_Projections=img;

	m_Config=dynamic_cast<ReconConfig *>(config);

	try {
		m_nLevels = GetFloatParameter(parameters,"decnum");
		m_fSigma  = GetFloatParameter(parameters,"sigma");
		m_sWaveletName = GetStringParameter(parameters, "wname");
	}
	catch (ModuleException & e)
	{
		logger(kipl::logging::Logger::LogWarning,e.what());
		return false;
	}

	UpdateDialog();

	try {
        ApplyParameters();
	}
	catch (kipl::base::KiplException &e) {
		logger(kipl::logging::Logger::LogError,e.what());
		return false;
	}

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
		logger(kipl::logging::Logger::LogMessage,"Use settings");
		UpdateParameters();
		UpdateParameterList(parameters);
	}
	else
	{
		logger(kipl::logging::Logger::LogMessage,"Discard settings");
	}
    return res;
}

void WaveletRingCleanDlg::UpdateDialog()
{
    m_entry_levels.setValue(m_nLevels);
    m_entry_sigma.setValue(m_fSigma);
	int default_wavelet, idx;

	kipl::wavelets::WaveletKernel<float> wk("daub4");

	std::list<std::string> wlist=wk.NameList();
	std::list<std::string>::iterator it;

	for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
        if (*it==m_sWaveletName)
			default_wavelet=idx;
	}

    m_combobox_wavelets.setCurrentIndex(default_wavelet);
    m_combobox_method.setCurrentIndex(1);
}

void WaveletRingCleanDlg::UpdateParameters()
{
    m_sWaveletName    = m_combobox_wavelets.currentText().toStdString();
    m_nLevels         = m_entry_levels.value();
    m_fSigma          = m_entry_sigma.value();
    m_bParallel       = false;
    string2enum(m_combobox_method.currentText().toStdString(), m_eCleaningMethod);
}

void WaveletRingCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
	parameters["decnum"]   = kipl::strings::value2string(m_nLevels);
	parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
	parameters["wname"]    = m_sWaveletName;
	parameters["parallel"] = m_bParallel ? "true" : "false";
	parameters["method"]   = enum2string(m_eCleaningMethod);
}

kipl::base::TImage<float,2> WaveletRingCleanDlg::GetSinogram(kipl::base::TImage<float,3> &proj, size_t index)
{
    size_t dims[2]={proj.Size(0),proj.Size(2)};

    kipl::base::TImage<float,2> sino(dims);

    for (size_t i=0; i<proj.Size(2); i++)
        memcpy(sino.GetLinePtr(i),proj.GetLinePtr(index,i),sizeof(float)*proj.Size(0));

    return sino;
}


