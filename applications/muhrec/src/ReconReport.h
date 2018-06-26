#ifndef __RECONREPORT_H
#define __RECONREPORT_H

#include <ReportGenerator.h>
#include <base/timage.h>
#include <ReconConfig.h>
#include <ContextPlotter.h>
#include <ContextImageViewer.h>

class ReconReport : public Gtk_addons::ReportGenerator
{
public:
	ReconReport(int *dims);

	void CreateReport(std::string filename, 
		ReconConfig * config,
		kipl::base::TImage<float,2> * xy=NULL,
		kipl::base::TImage<float,2> * xz=NULL,
		kipl::base::TImage<float,2> * yz=NULL,
		size_t *hist=NULL,
		float *haxis=NULL,
		int Nbins=0);
private:
	void ProcessingModules(Cairo::RefPtr<Cairo::Context> cr,ReconConfig * config);
	kipl::base::TImage<float,2> LoadProjection(ReconConfig * config);
	Gtk_addons::ContextPlotter m_PlotRenderer;
	Gtk_addons::ContextImageViewer m_ImageRenderer;
};

#endif
