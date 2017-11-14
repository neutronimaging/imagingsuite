#include "stdafx.h"
#undef max
#include "KipToolMainWindow.h"
#include <ModuleException.h>
#include <KiplFrameworkException.h>
#include "ImageIO.h"
#include <sstream>

#include <base/thistogram.h>

KipToolMainWindow::KipToolMainWindow(Glib::RefPtr<Gtk::Builder> refxml,string path) :
	logger("KipToolMainWindow"),
	config_filename(""),
	m_Engine(NULL),
	m_OriginalHistogram(1024),
	main_window(NULL),
	m_bRescaleViewers(false),
	modulemanager("kiptool",path),
	application_path(path)
{
	refxml->get_widget("main_window",main_window);
	refxml->get_widget("kt_menu_new",kt_menu_new);
	refxml->get_widget("kt_menu_open",kt_menu_open);
	refxml->get_widget("kt_menu_save",kt_menu_save);
	refxml->get_widget("kt_menu_saveas",kt_menu_saveas);
	refxml->get_widget("kt_menu_quit",kt_menu_quit);
	refxml->get_widget("kt_menu_info",kt_menu_info);
	refxml->get_widget("nb_settings",nb_settings);
	refxml->get_widget("kt_entry_project",kt_entry_project);
	refxml->get_widget("kt_entry_sample",kt_entry_sample);
	refxml->get_widget("kt_entry_operator",kt_entry_operator);
	refxml->get_widget("kt_entry_description",kt_entry_description);
	refxml->get_widget("kt_filechooserbutton_input_path",kt_filechooserbutton_input_path);
	refxml->get_widget("kt_entry_filemaskin",kt_entry_filemaskin);
	refxml->get_widget("kt_button_load_image",kt_button_load_image);
	refxml->get_widget("kt_spinbutton_first_index",kt_spinbutton_first_index);
	refxml->get_widget("kt_spinbutton_last_index",kt_spinbutton_last_index);
	refxml->get_widget("kt_button_save_image",kt_button_save_image);
	refxml->get_widget("kt_filechooserbutton_output_path",kt_filechooserbutton_output_path);
	refxml->get_widget("kt_entry_filemask_out",kt_entry_filemask_out);
	refxml->get_widget("kt_checkbutton_use_roi",kt_checkbutton_use_roi);
	refxml->get_widget("kt_spinbutton_roi0",kt_spinbutton_roi0);
	refxml->get_widget("kt_spinbutton_roi1",kt_spinbutton_roi1);
	refxml->get_widget("kt_spinbutton_roi2",kt_spinbutton_roi2);
	refxml->get_widget("kt_spinbutton_roi3",kt_spinbutton_roi3);
	refxml->get_widget("kt_alignment_process_chain",kt_alignment_process_chain);
	refxml->get_widget("kt_button_start_processing",kt_button_start_processing);
	refxml->get_widget("kt_alignment_messages",kt_alignment_messages);
	refxml->get_widget("nb_display",nb_display);
	refxml->get_widget("kt_alignment_histogram",kt_alignment_histogram);
	refxml->get_widget("kt_alignment_difference",kt_alignment_difference);
	refxml->get_widget("kt_combobox_select_plot",kt_combobox_select_plot);
	refxml->get_widget("kt_alignment_plots",kt_alignment_plots);
	refxml->get_widget("kt_alignment_original",kt_alignment_original);
	refxml->get_widget("kt_alignment_processed",kt_alignment_processed);
	refxml->get_widget("kt_hscale_images",kt_hscale_images);
	refxml->get_widget("kt_checkbutton_originalhist",kt_checkbutton_originalhist);

	float tmp[256]; 
	memset(tmp,0,sizeof(float)*256);
	size_t dims[2]={16,16};
	imageviewer_diff.set_image(tmp,dims);
	imageviewer_original.set_image(tmp,dims);
	imageviewer_result.set_image(tmp,dims);

	kt_alignment_histogram->add(plotviewer_histogram);
	kt_alignment_histogram->show_all_children();

	kt_alignment_plots->add(plotviewer_plots);
	kt_alignment_plots->show_all_children();
	
	kt_alignment_difference->add(imageviewer_diff);
	kt_alignment_difference->show_all_children();

	kt_alignment_original->add(imageviewer_original);
	kt_alignment_original->show_all_children();

	kt_alignment_processed->add(imageviewer_result);
	kt_alignment_processed->show_all_children();

	kt_alignment_process_chain->add(modulemanager);
	kt_alignment_process_chain->show_all_children();

	logger.AddLogTarget(logviewer);
	kt_alignment_messages->add(logviewer);
	kt_alignment_messages->show_all_children();
	SetupCallBacks();
	LoadDefaults();
	UpdateDialog();
	ToggleROI();
}

KipToolMainWindow::~KipToolMainWindow(void)
{

}

void KipToolMainWindow::SetupCallBacks()
{
	kt_menu_new->signal_activate().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_menu_new));
	kt_menu_open->signal_activate().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_menu_load));
	kt_menu_save->signal_activate().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_menu_save));
	kt_menu_saveas->signal_activate().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_menu_save_as));
	kt_menu_quit->signal_activate().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_menu_quit));
	kt_button_load_image->signal_clicked().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_button_load_image));
	kt_button_save_image->signal_clicked().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_button_save_image));
	kt_checkbutton_use_roi->signal_toggled().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_checkbutton_use_roi));
	kt_button_start_processing->signal_clicked().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_button_process));
	kt_hscale_images->signal_value_changed().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_image_slider));
	kt_checkbutton_originalhist->signal_toggled().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_checkbutton_show_original_histogram));
//	modulemanager.signal_module_selected().connect(sigc::mem_fun(*this,&KipToolMainWindow::on_module_selected));
}

void KipToolMainWindow::LoadDefaults()
{
	std::string tempdir = Glib::get_home_dir();

	kipl::strings::filenames::CheckPathSlashes(tempdir,true);

	std::string defaultsname=tempdir+"CurrentKIPscript.xml";

	try {
		m_Config.LoadConfigFile(defaultsname.c_str());
	}
	catch (kipl::base::KiplException & e) {
		logger(kipl::logging::Logger::LogWarning,"Failed to load previous process script.");
	}
}

void KipToolMainWindow::on_menu_new()
{

}

void KipToolMainWindow::on_menu_load()
{
	Gtk::FileChooserDialog dlg("Open reconstruction parameter file");

	dlg.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.set_current_folder(Glib::get_home_dir());
	
	int response=dlg.run();

	if (response==Gtk::RESPONSE_OK) {
		config_filename=dlg.get_filename();
		std::ostringstream msg;
		msg<<"File open: "<<config_filename;	
		logger(kipl::logging::Logger::LogVerbose,msg.str());

		// Read parameters
		Gtk::MessageDialog errdlg("No message",false,Gtk::MESSAGE_ERROR);
		msg.str("");
		try {
			m_Config.LoadConfigFile(config_filename.c_str());
			UpdateDialog();
		}
		catch (ModuleException &e) {
			msg<<"Failed to open parameter file "<<config_filename<<"\n"<<e.what();
			errdlg.set_message(msg.str());
			errdlg.run();
			return;
		}
		catch (KiplFrameworkException &e) {
			msg<<"Failed to open parameter file "<<config_filename<<"\n"<<e.what();
			errdlg.set_message(msg.str());
			errdlg.run();
			return;
		}
		catch (kipl::base::KiplException &e) {
			msg<<"Failed to open parameter file "<<config_filename<<"\n"<<e.what();
			errdlg.set_message(msg.str());
			errdlg.run();
			return;
		}
		catch (std::exception &e) {
			msg<<"Failed to open parameter file "<<config_filename<<"\n"<<e.what();
			errdlg.set_message(msg.str());
			errdlg.run();
			return;
		}
		catch (...) {
			msg<<"Failed to open parameter file "<<config_filename;
			errdlg.set_message(msg.str());
			errdlg.run();
			return;
		
		}
	}
	ToggleROI();
}

void KipToolMainWindow::on_menu_save()
{
	if (config_filename.empty())
		on_menu_save_as();
	else {
		UpdateConfig();
		std::ostringstream msg;
		msg<<"Saving current parameter file ("<<config_filename<<")";
		logger(kipl::logging::Logger::LogDebug,msg.str());
		std::ofstream of(config_filename.c_str());
		of<<m_Config.WriteXML();
	}
}

void KipToolMainWindow::on_menu_save_as()
{
	Gtk::FileChooserDialog dlg("Save reconstruction parameter file as",Gtk::FILE_CHOOSER_ACTION_SAVE);

	dlg.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.set_current_folder(Glib::get_home_dir());
	dlg.set_filename("recon.xml");
	
	int response=dlg.run();

	if (response==Gtk::RESPONSE_OK) {
		config_filename=dlg.get_filename();

		UpdateConfig();
		std::ostringstream msg;
		msg<<"Saving parameters as "<<config_filename;
		logger(kipl::logging::Logger::LogDebug,msg.str());

		std::ofstream of(config_filename.c_str());
		of<<m_Config.WriteXML();
	}
}

void KipToolMainWindow::on_menu_quit()
{
	main_window->hide();
}

void KipToolMainWindow::on_button_load_image()
{
	UpdateConfig();

	try {
		m_Original = LoadVolumeImage(m_Config); 

		kipl::base::Histogram(m_Original.GetDataPtr(),m_Original.Size(),
			m_OriginalHistogram.GetY(),m_OriginalHistogram.Size(),
			0.0f,0.0f,
			m_OriginalHistogram.GetX());
			m_OriginalHistogram.GetY()[0]=m_OriginalHistogram.GetY()[1];
			m_OriginalHistogram.GetY()[m_OriginalHistogram.Size()-1]=m_OriginalHistogram.GetY()[m_OriginalHistogram.Size()-2];
	}
	catch (KiplFrameworkException &e) 
	{
		Gtk::MessageDialog dlg("Failed to open the image",false,Gtk::MESSAGE_ERROR);
		logger(kipl::logging::Logger::LogError,e.what());
		return;
	}

	kt_hscale_images->set_range(0.0,m_Original.Size(2)-1);
	kt_hscale_images->set_value(0.0);
	size_t lo=0, hi=0;
	kipl::base::FindLimits(m_OriginalHistogram.GetY(),m_OriginalHistogram.Size(),95,&lo,&hi);
	imageviewer_original.set_levels(m_OriginalHistogram.GetX()[lo],m_OriginalHistogram.GetX()[hi]);
	UpdateImageDisplay(0);
	UpdateHistogramView();
}

void KipToolMainWindow::on_button_save_image()
{
	if (m_Engine!=NULL) {
		UpdateConfig();
		m_Engine->SaveImage(&m_Config.mOutImageInformation);
	}
}

void KipToolMainWindow::on_checkbutton_use_roi()
{
	ToggleROI();
}

void KipToolMainWindow::ToggleROI()
{
	bool active=kt_checkbutton_use_roi->get_active();

	kt_spinbutton_roi0->set_sensitive(active);
	kt_spinbutton_roi1->set_sensitive(active);
	kt_spinbutton_roi2->set_sensitive(active);
	kt_spinbutton_roi3->set_sensitive(active);
}

void KipToolMainWindow::on_button_process()
{
	UpdateConfig();

	std::ostringstream msg;
	string path=Glib::get_home_dir();
	kipl::strings::filenames::CheckPathSlashes(path,true);
	msg<<path<<"CurrentKIPscript.xml";

	ofstream of(msg.str().c_str());
	of<<m_Config.WriteXML();
	of.flush();

	if (m_Original.Size()==0) {
		Gtk::MessageDialog dlg("Please load an image before you start processing!",false,Gtk::MESSAGE_ERROR);
		dlg.run();
		return;
	}
	if (m_Config.modules.empty()) {
		Gtk::MessageDialog dlg("There are no modules in the process chain.",false,Gtk::MESSAGE_ERROR);
		dlg.run();
		return;
	}

	if (m_Engine) {
		delete m_Engine;
		m_Engine=NULL;
	}

	bool bBuildFailed=false;


	try {
		m_Engine=m_Factory.BuildEngine(m_Config);
	}
	catch (ModuleException &e) {
		bBuildFailed=true;
		msg<<"ModuleException: "<<e.what();
	}
	catch (KiplFrameworkException &e) {
		bBuildFailed=true;
		msg<<"KiplFrameworkException: "<<e.what();
	}
	catch (kipl::base::KiplException &e) {
		bBuildFailed=true;
		msg<<"KiplException: "<<e.what();
	}
	catch (std::exception &e) {
		bBuildFailed=true;
		msg<<"STL Exception: "<<e.what();
	}
	catch (...) {
		bBuildFailed=true;
		msg<<"Unhandled exception";
	}

	if (bBuildFailed) {
		logger(kipl::logging::Logger::LogError,msg.str());
		Gtk::MessageDialog dlg("Failed to build the process chain.",false,Gtk::MESSAGE_ERROR);
		dlg.run();
		return;
	}
	
	logger(kipl::logging::Logger::LogMessage,"The process chain construction was successful.");

	bool bExecutionFailed=false;
	m_PlotList.clear();
	m_HistogramList.clear();

	msg.str("");
	try {
		m_Engine->Run(&m_Original);

		m_PlotList=m_Engine->GetPlots();
		m_HistogramList=m_Engine->GetHistograms();
	}
	catch (ModuleException &e) {
		bBuildFailed=true;
		msg<<"ModuleException: "<<e.what();
	}
	catch (KiplFrameworkException &e) {
		bExecutionFailed=true;
		msg<<"KiplFrameworkException: "<<e.what();
	}
	catch (kipl::base::KiplException &e) {
		bExecutionFailed=true;
		msg<<"KiplException: "<<e.what();
	}
	catch (std::exception &e) {
		bExecutionFailed=true;
		msg<<"STL Exception: "<<e.what();
	}
	catch (...) {
		bExecutionFailed=true;
		msg<<"Unhandled exception";
	}

	if (bExecutionFailed) {
		logger(kipl::logging::Logger::LogError,msg.str());
		Gtk::MessageDialog dlg("The process chain exectution failed",false,Gtk::MESSAGE_ERROR);
		dlg.run();
		return;
	}

	logger(kipl::logging::Logger::LogMessage,"The process chain ended successfully");

	m_bRescaleViewers=true;
	UpdateImageDisplay(0);
	UpdatePlotView();
	UpdateHistogramView();
}

void KipToolMainWindow::on_module_selected()
{
	std::cout<<"selected module"<<std::endl;
}


void KipToolMainWindow::on_image_slider()
{
	int value=static_cast<int>(kt_hscale_images->get_value());
	UpdateImageDisplay(value);
}

void KipToolMainWindow::on_checkbutton_show_original_histogram()
{
	UpdateHistogramView();
}

void KipToolMainWindow::UpdatePlotView()
{
	std::map<std::string, kipl::containers::PlotData<float,float> >::iterator plot_it;
	std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > >::iterator module_it;
	int idx=0;

	if (!m_PlotList.empty()) {
		for (module_it=m_PlotList.begin(); module_it!=m_PlotList.end(); module_it++) {
			plotviewer_plots.ClearAll();
			idx=0;
			for (plot_it=module_it->second.begin(); plot_it!=module_it->second.end(); plot_it++,idx++) {
				plotviewer_plots.plot(plot_it->second.GetX(),plot_it->second.GetY(),plot_it->second.Size(),Gdk::Color("green"),idx);
			}
		}
	}

		plotviewer_plots.show_all();
}

void KipToolMainWindow::UpdateHistogramView()
{
	std::map<std::string, kipl::containers::PlotData<float,size_t> >::iterator it;
	int idx=1;

	if (!m_HistogramList.empty()) {

		for (it=m_HistogramList.begin(); it!=m_HistogramList.end(); it++, idx++) {
			plotviewer_histogram.plot(it->second.GetX(),it->second.GetY(),it->second.Size(),Gdk::Color("red"),idx);
		}
	}
	if (kt_checkbutton_originalhist->get_active()) {
		plotviewer_histogram.plot(m_OriginalHistogram.GetX(), m_OriginalHistogram.GetY(),m_OriginalHistogram.Size(),Gdk::Color("green"),0);
	}
	else {
		plotviewer_histogram.Clear(0);
	}
	plotviewer_histogram.show_all();
}

void KipToolMainWindow::UpdateImageDisplay(int idx)
{
	if ((m_Original.Size()!=0) && (idx<m_Original.Size(2)) && (0<=idx)) {
		float lo,hi;
		imageviewer_original.get_levels(&lo,&hi);
		imageviewer_original.set_image(m_Original.GetLinePtr(0,idx),m_Original.Dims(),lo,hi);
		if (m_Engine!=NULL) {
			kipl::base::TImage<float,3> &result=m_Engine->GetResultImage();

			if ((result.Size(0)==m_Original.Size(0)) &&
				(result.Size(1)==m_Original.Size(1)) &&
				(result.Size(2)==m_Original.Size(2))) 
			{
				if (m_bRescaleViewers) {
					imageviewer_result.set_image(result.GetLinePtr(0,idx),result.Dims());
					m_bRescaleViewers=false;
				}
				else {
					imageviewer_result.get_levels(&lo,&hi);
					imageviewer_result.set_image(result.GetLinePtr(0,idx),result.Dims(),lo,hi);
				}

				kipl::base::TImage<float,2> diff(result.Dims());
				float *pDiff=diff.GetDataPtr();
				float *pRes=result.GetLinePtr(0,idx);
				float *pImg=m_Original.GetLinePtr(0,idx);

				for (int i=0; i<diff.Size(); i++) {
						pDiff[i]=pRes[i]-pImg[i];
				}
				imageviewer_diff.set_image(pDiff,diff.Dims());
			}
		}
	}
}

void KipToolMainWindow::UpdateConfig()
{
	m_Config.mUserInformation.sProjectNumber = kt_entry_project->get_text();
	m_Config.mUserInformation.sSample        = kt_entry_sample->get_text();
	m_Config.mUserInformation.sOperator      = kt_entry_operator->get_text();
	m_Config.mUserInformation.sComment       = kt_entry_description->get_text();
	//m_Config.mImageInformation.sSourcePath   = kt_filechooserbutton_input_path->get_current_folder();
	m_Config.mImageInformation.sSourcePath   = kt_filechooserbutton_input_path->get_filename();
	kipl::strings::filenames::CheckPathSlashes(m_Config.mImageInformation.sSourcePath,true);
	m_Config.mImageInformation.sSourceFileMask      = kt_entry_filemaskin->get_text();  
	m_Config.mImageInformation.nFirstFileIndex      = static_cast<size_t>(kt_spinbutton_first_index->get_value());
	m_Config.mImageInformation.nLastFileIndex       = static_cast<size_t>(kt_spinbutton_last_index->get_value());
	//m_Config.mOutImageInformation.sDestinationPath     = kt_filechooserbutton_output_path->get_current_folder();
	m_Config.mOutImageInformation.sDestinationPath     = kt_filechooserbutton_output_path->get_filename();
	kipl::strings::filenames::CheckPathSlashes(m_Config.mOutImageInformation.sDestinationPath,true);
	m_Config.mOutImageInformation.sDestinationFileMask = kt_entry_filemask_out->get_text();
	m_Config.mImageInformation.bUseROI              = kt_checkbutton_use_roi->get_active();
	m_Config.mImageInformation.nROI[0]              = static_cast<size_t>(kt_spinbutton_roi0->get_value());
	m_Config.mImageInformation.nROI[1]              = static_cast<size_t>(kt_spinbutton_roi1->get_value());
	m_Config.mImageInformation.nROI[2]              = static_cast<size_t>(kt_spinbutton_roi2->get_value());
	m_Config.mImageInformation.nROI[3]              = static_cast<size_t>(kt_spinbutton_roi3->get_value());

	m_Config.modules = modulemanager.GetModules();
}

void KipToolMainWindow::UpdateDialog()
{
	kt_entry_project->set_text(m_Config.mUserInformation.sProjectNumber);
	kt_entry_sample->set_text(m_Config.mUserInformation.sSample);
	kt_entry_operator->set_text(m_Config.mUserInformation.sOperator);
	kt_entry_description->set_text(m_Config.mUserInformation.sComment);
	//kt_filechooserbutton_input_path->set_current_folder(m_Config.mImageInformation.sSourcePath);
	kt_filechooserbutton_input_path->set_filename(m_Config.mImageInformation.sSourcePath);
	kt_entry_filemaskin->set_text(m_Config.mImageInformation.sSourceFileMask);
	kt_spinbutton_first_index->set_value(m_Config.mImageInformation.nFirstFileIndex);
	kt_spinbutton_last_index->set_value(m_Config.mImageInformation.nLastFileIndex);
	//kt_filechooserbutton_output_path->set_current_folder(m_Config.mOutImageInformation.sDestinationPath);
	kt_filechooserbutton_output_path->set_filename(m_Config.mOutImageInformation.sDestinationPath);
	kt_entry_filemask_out->set_text(m_Config.mOutImageInformation.sDestinationFileMask);
	kt_checkbutton_use_roi->set_active(m_Config.mImageInformation.bUseROI);
	kt_spinbutton_roi0->set_value(m_Config.mImageInformation.nROI[0]);
	kt_spinbutton_roi1->set_value(m_Config.mImageInformation.nROI[1]);
	kt_spinbutton_roi2->set_value(m_Config.mImageInformation.nROI[2]);
	kt_spinbutton_roi3->set_value(m_Config.mImageInformation.nROI[3]);

	modulemanager.SetModules(m_Config.modules);
}

//while gtk.events_pending():
//    gtk.main_iteration()
