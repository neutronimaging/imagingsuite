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

WaveletRingCleanDlg::WaveletRingCleanDlg():
ConfiguratorDialogBase("MedianMixRingClean2Dlg"),
m_frame_original("Original"),
m_frame_processed("Processed"),
m_frame_difference("Difference"),
lbl_waveletname("Wavelet name"),
lbl_levels("Lambda"),
lbl_sigma("Sigma"),
m_button_refresh(Gtk::Stock::REFRESH),
m_nLevels(4),
m_fSigma(0.1f),
m_sWaveletName("daub25")
{
	float data[16]={1,2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15 };
	size_t dims[2]={4,4};

	m_viewer_difference.set_image(data,dims);
	m_viewer_original.set_image(data,dims);
	m_viewer_processed.set_image(data,dims);

	m_frame_original.add(m_viewer_original);
	m_frame_original.set_size_request(256,256);

	m_frame_processed.add(m_viewer_processed);
	m_frame_processed.set_size_request(256,256);

	m_frame_difference.add(m_viewer_difference);
	m_frame_difference.set_size_request(256,256);
	m_hbox_viewers.pack_end(m_frame_difference);
	m_hbox_viewers.pack_end(m_frame_processed);
	m_hbox_viewers.pack_end(m_frame_original);
	m_vbox_main.pack_start(m_hbox_viewers);

	const int nDigits=5;
	m_entry_levels.set_digits(0);
	m_entry_levels.get_adjustment()->configure(m_nLevels,1,10,1,1,0.0);

	m_entry_sigma.set_digits(nDigits);
	m_entry_sigma.get_adjustment()->configure(m_fSigma,0.0,1.0,0.01,0.1,0.0);

	m_hbox_parameters.pack_start(lbl_levels);
	m_hbox_parameters.pack_start(m_entry_levels);

	m_hbox_parameters.pack_start(lbl_sigma);
	m_hbox_parameters.pack_start(m_entry_sigma);

	prepare_waveletcombobox();

	m_hbox_parameters.pack_start(lbl_waveletname);
	m_hbox_parameters.pack_start(m_combobox_wavelets);
	m_hbox_parameters.pack_start(m_combobox_method);

	m_vbox_parameters.pack_start(m_hbox_parameters);
	m_vbox_parameters.pack_start(m_button_refresh);

	m_vbox_main.pack_start(m_vbox_parameters);
	get_vbox()->add(m_vbox_main);

	set_title("Configure wavelet ring clean");

	get_vbox()->show_all_children();

	m_button_refresh.signal_clicked().connect(sigc::mem_fun(*this,
		&WaveletRingCleanDlg::on_refresh));

	m_entry_levels.signal_value_changed().connect(sigc::mem_fun(*this,
			&WaveletRingCleanDlg::on_change_level));
}

WaveletRingCleanDlg::~WaveletRingCleanDlg() {
}

void WaveletRingCleanDlg::prepare_waveletcombobox()
{
	std::ostringstream msg;

	kipl::wavelets::WaveletKernel<float> wk("daub4");
	std::list<std::string> wlist=wk.NameList();
	std::list<std::string>::iterator it;

#if GTK_MAJOR_VERSION<3
	m_combobox_wavelets.clear_items();
#else
	m_combobox_wavelets.remove_all();
#endif
	int default_wavelet, idx;
	for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
		Glib::ustring str=*it;
		if (str==m_sWaveletName)
			default_wavelet=idx;
#if GTK_MAJOR_VERSION<3
		m_combobox_wavelets.append_text(str); // 2.4
#else
		m_combobox_wavelets.append(str);
#endif
	}

	m_combobox_wavelets.set_active(default_wavelet);
#ifdef _MSC_VER
	m_combobox_method.append_text("verticalzero");
	m_combobox_method.append_text("verticalfft");
#else
	m_combobox_method.append("verticalzero");
	m_combobox_method.append("verticalfft");
#endif

}

void WaveletRingCleanDlg::on_refresh()
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
		Gtk::MessageDialog errdlg("Failed to process sinogram.",false,Gtk::MESSAGE_ERROR);

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

bool WaveletRingCleanDlg::run(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
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
		on_refresh();
	}
	catch (kipl::base::KiplException &e) {
		logger(kipl::logging::Logger::LogError,e.what());
		return false;
	}

	gint res=Gtk::Dialog::run();

	if (res==Gtk::RESPONSE_OK) {
		logger(kipl::logging::Logger::LogMessage,"Use settings");
		UpdateParameters();
		UpdateParameterList(parameters);
		return true;
	}
	else
	{
		logger(kipl::logging::Logger::LogMessage,"Discard settings");
	}
	return false;
}

void WaveletRingCleanDlg::UpdateDialog()
{
	m_entry_levels.set_value(m_nLevels);
	m_entry_sigma.set_value(m_fSigma);
	int default_wavelet, idx;

	kipl::wavelets::WaveletKernel<float> wk("daub4");

	std::list<std::string> wlist=wk.NameList();
	std::list<std::string>::iterator it;

	for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
		Glib::ustring str=*it;
		if (str==m_sWaveletName)
			default_wavelet=idx;
	}

	m_combobox_wavelets.set_active(default_wavelet);
	m_combobox_method.set_active(0);
}

void WaveletRingCleanDlg::UpdateParameters()
{
	m_sWaveletName    = m_combobox_wavelets.get_active_text();
	m_nLevels         = m_entry_levels.get_value();
	m_fSigma          = m_entry_sigma.get_value();
	m_bParallel       = false;
	string2enum(m_combobox_method.get_active_text(), m_eCleaningMethod);
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

