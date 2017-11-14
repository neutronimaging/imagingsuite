#ifndef __KIPTOOLMAINWINDOW_H
#define __KIPTOOLMAINWINDOW_H

#include <gtkmm.h>

#include <logging/logger.h>

#include <ModuleManagerWidget2.h>
#include <LogViewermm.h>
#include <PlotViewermm.h>
#include <ImageViewermm.h>

#include <KiplEngine.h>
#include <KiplFactory.h>
#include <KiplProcessConfig.h>

class KipToolMainWindow
{
protected:
	kipl::logging::Logger logger;

public:
	/// C'tor for the main window class. Initializes the user interface.
	/// \param refxml A reference to an glade xml object containing the user interface description
	KipToolMainWindow(Glib::RefPtr<Gtk::Builder> refxml, std::string path);
	virtual ~KipToolMainWindow(void);

	/// Gets the reference to the main window object
	Gtk::Window & get_window() const {return *main_window;}
protected:
	void SetupCallBacks();
	void UpdateConfig();
	void UpdateDialog();
	void LoadDefaults();

	void UpdateImageDisplay(int idx);
	void UpdatePlotView();
	void UpdateHistogramView();
	void ToggleROI();

	// Call-backs
	void on_menu_new();
	void on_menu_load();
	void on_menu_save();
	void on_menu_save_as();
	void on_menu_quit();
	void on_button_load_image();
	void on_button_save_image();
	void on_checkbutton_use_roi();
	void on_button_process();
	void on_image_slider();
	void on_module_selected();
	void on_checkbutton_show_original_histogram();



	KiplProcessConfig m_Config;
	std::string config_filename;
	kipl::base::TImage<float,3> m_Original;
	KiplFactory m_Factory;
	KiplEngine * m_Engine;
	std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > > m_PlotList;
	std::map<std::string, kipl::containers::PlotData<float,size_t> > m_HistogramList;
	kipl::containers::PlotData<float,size_t> m_OriginalHistogram;

	Gtk::Window            * main_window;
	Gtk::ImageMenuItem     * kt_menu_new;
	Gtk::ImageMenuItem     * kt_menu_open;
	Gtk::ImageMenuItem     * kt_menu_save;
	Gtk::ImageMenuItem     * kt_menu_saveas;
	Gtk::ImageMenuItem     * kt_menu_quit;
	Gtk::ImageMenuItem     * kt_menu_info;
	Gtk::Notebook          * nb_settings;
	Gtk::Entry             * kt_entry_project;
	Gtk::Entry             * kt_entry_sample;
	Gtk::Entry             * kt_entry_operator;
	Gtk::Entry             * kt_entry_description;
	Gtk::FileChooserButton * kt_filechooserbutton_input_path;
	Gtk::Entry             * kt_entry_filemaskin;
	Gtk::Button            * kt_button_load_image;
	Gtk::SpinButton        * kt_spinbutton_first_index;
	Gtk::SpinButton        * kt_spinbutton_last_index;
	Gtk::Button            * kt_button_save_image;
	Gtk::FileChooserButton * kt_filechooserbutton_output_path;
	Gtk::Entry             * kt_entry_filemask_out;
	Gtk::CheckButton       * kt_checkbutton_use_roi;
	Gtk::SpinButton        * kt_spinbutton_roi0;
	Gtk::SpinButton        * kt_spinbutton_roi1;
	Gtk::SpinButton        * kt_spinbutton_roi2;
	Gtk::SpinButton        * kt_spinbutton_roi3;
	Gtk::Alignment         * kt_alignment_process_chain;
	Gtk::Button            * kt_button_start_processing;
	Gtk::Alignment         * kt_alignment_messages;
	Gtk::Notebook          * nb_display;
	Gtk::Alignment         * kt_alignment_histogram;
	Gtk::Alignment         * kt_alignment_difference;
	Gtk::ComboBox          * kt_combobox_select_plot;
	Gtk::Alignment         * kt_alignment_plots;
	Gtk::Alignment         * kt_alignment_original;
	Gtk::Alignment         * kt_alignment_processed;
	Gtk::HScale            * kt_hscale_images;
	Gtk::CheckButton       * kt_checkbutton_originalhist;

	Gtk_addons::PlotViewer plotviewer_histogram;
	Gtk_addons::PlotViewer plotviewer_plots;

	Gtk_addons::ImageViewer imageviewer_original;
	Gtk_addons::ImageViewer imageviewer_result;
	Gtk_addons::ImageViewer imageviewer_diff;
	bool m_bRescaleViewers;

	Gtk_addons::LogViewer logviewer;

	ModuleManagerWidget2 modulemanager;

	const std::string application_path;
};

#endif
