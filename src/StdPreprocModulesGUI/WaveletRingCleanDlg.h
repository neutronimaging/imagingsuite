/*
 * WaveletRingCleanDlg.h
 *
 *  Created on: Mar 1, 2012
 *      Author: kaestner
 */

#ifndef WaveletRingCleanDlg_H_
#define WaveletRingCleanDlg_H_
#include <gtkmm.h>
#include <base/timage.h>
#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <ImageViewermm.h>
#include <PlotViewermm.h>

#include <WaveletRingClean.h>
#include <StripeFilter.h>
#include <ReconConfig.h>

class WaveletRingCleanDlg: public ConfiguratorDialogBase {
public:
	WaveletRingCleanDlg();
	virtual ~WaveletRingCleanDlg();
	virtual bool run(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img);
private:
	void prepare_waveletcombobox();
	void UpdateDialog();
	void UpdateParameters();
	void UpdateParameterList(std::map<std::string, std::string> &parameters);
	kipl::base::TImage<float,2> GetSinogram(kipl::base::TImage<float,3> &proj, size_t index);

	void on_refresh();
	void on_change_level();
	Gtk::VBox m_vbox_main;
	Gtk::HBox m_hbox_viewers;

	Gtk::Frame m_frame_original;
	Gtk::Frame m_frame_processed;
	Gtk::Frame m_frame_difference;
	Gtk_addons::ImageViewer m_viewer_original;
	Gtk_addons::ImageViewer m_viewer_processed;
	Gtk_addons::ImageViewer m_viewer_difference;

	Gtk::VBox m_vbox_parameters;
	Gtk::HBox m_hbox_parameters;
	Gtk::SpinButton m_entry_levels;
	Gtk::SpinButton m_entry_sigma;
	Gtk::ComboBoxText m_combobox_wavelets;
	Gtk::ComboBoxText m_combobox_method;
	Gtk::Label lbl_waveletname;
	Gtk::Label lbl_levels;
	Gtk::Label lbl_sigma;

	Gtk::Button m_button_refresh;

	ReconConfig *m_Config;
	WaveletRingClean cleaner;
	kipl::base::TImage<float,3> m_Projections;
	int m_nLevels;
	float m_fSigma;
	std::string m_sWaveletName;
	bool m_bParallel;
	ImagingAlgorithms::StripeFilterOperation m_eCleaningMethod;

};

#endif /* WaveletRingCleanDlg_H_ */
