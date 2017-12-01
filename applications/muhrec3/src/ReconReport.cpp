
#include "stdafx.h"

#include <glibmm/date.h>
#include <glibmm/timeval.h>
#include <cstdlib>
#include <DrawItems.h>
#include <ProjectionReader.h>
#include "ReconReport.h"

ReconReport::ReconReport(int *dims) : 
	ReportGenerator(dims,"ReconReport")
{
}

void ReconReport::CreateReport(std::string filename, ReconConfig * config,
		kipl::base::TImage<float,2> * xy,
		kipl::base::TImage<float,2> * xz,
		kipl::base::TImage<float,2> * yz,
		size_t *hist,
		float *haxis,
		int Nbins)
{

#ifdef CAIRO_HAS_PDF_SURFACE
    Cairo::RefPtr<Cairo::PdfSurface> surface =
        Cairo::PdfSurface::create(filename, width, height);

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

	cr->save(); // save the state of the context
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();    // fill image with the color
    cr->restore();  // color is back to black now
    cr->save();

    MakeHeader(cr,config->UserInformation.sProjectNumber);
	ostringstream msg;

	msg.str(""); msg<<"Operator:   "<<config->UserInformation.sOperator;
	Print(cr,msg.str());
	msg.str(""); msg<<"Instrument: "<<config->UserInformation.sInstrument;
	Print(cr,msg.str());
	msg.str(""); msg<<"Sample:     "<<config->UserInformation.sSample;
	Print(cr,msg.str());

	m_fLine+=0.5*m_fLineHeight;
	Print(cr,"Input files",11.0);

	msg.str(""); msg<<"Projections: "<<config->ProjectionInfo.sPath<<config->ProjectionInfo.sFileMask
					<<", ["<<config->ProjectionInfo.nFirstIndex<<" - "<<config->ProjectionInfo.nLastIndex
					<<", step "<<config->ProjectionInfo.nProjectionStep<<"]";
	Print(cr,msg.str());

	if (config->ProjectionInfo.nOBCount!=0) {
		msg.str(""); msg<<"Open beam: "<<config->ProjectionInfo.sReferencePath<<config->ProjectionInfo.sOBFileMask<<", "<<config->ProjectionInfo.nOBCount<<" images";
		Print(cr,msg.str());
	}
	else {
		Print(cr,"No open beam images were used");
	}

	if (config->ProjectionInfo.nDCCount!=0) {
		msg.str(""); msg<<"Dark current: "<<config->ProjectionInfo.sReferencePath<<config->ProjectionInfo.sDCFileMask
						<<", "<<config->ProjectionInfo.nDCCount<<" images";
		Print(cr,msg.str());
	}
	else {
		Print(cr,"No dark current images were used");
	}
	m_fLine+=0.5*m_fLineHeight;
	Print(cr,"Geometry",11.0);

	msg.str(""); msg<<"Projection dimensions: "<<config->ProjectionInfo.nDims[0]<<"x"<<config->ProjectionInfo.nDims[1]<<" pixels";
	Print(cr,msg.str());

	msg.str(""); msg<<"Projection pixel size: "<<config->ProjectionInfo.fResolution[0]<<" mm/pixel";
	Print(cr,msg.str());

	msg.str(""); msg<<"Scan arc "<<config->ProjectionInfo.fScanArc[0]
	                <<" - "<<config->ProjectionInfo.fScanArc[1]
	                <<" ("<<config->ProjectionInfo.scantype<<")";
	Print(cr,msg.str());


	msg.str(""); msg<<"Dose ROI ("<<config->ProjectionInfo.dose_roi[0]<<", "<<config->ProjectionInfo.dose_roi[1]<<") - ("
					<<config->ProjectionInfo.dose_roi[2]<<", "<<config->ProjectionInfo.dose_roi[3]<<")";
	Print(cr,msg.str());

	msg.str(""); msg<<"Center of rotation: "<<config->ProjectionInfo.fCenter;
	if (config->ProjectionInfo.bCorrectTilt) {
		msg<<", axis tilt corrected by "<<config->ProjectionInfo.fTiltAngle<<" degrees, pivot at "<<config->ProjectionInfo.fTiltPivotPosition;
	}
	Print(cr,msg.str());

	if (config->ProjectionInfo.bTranslate) {
		Print(cr,"The axis was translated to the margin.");
	}

	msg.str(""); msg<<"Reconstructed slices "<<config->ProjectionInfo.roi[1]<<" - "<<config->ProjectionInfo.roi[3];
	Print(cr,msg.str());

	m_fLine+=0.5*m_fLineHeight;
	Print(cr,"Matrix",11.0);
	msg.str(""); msg<<"Matrix dimensions "<<config->MatrixInfo.nDims[0]<<"x"<<config->MatrixInfo.nDims[1]<<"x"<<config->MatrixInfo.nDims[2];
	Print(cr,msg.str());

	msg.str(""); msg<<"Destination path "<<config->MatrixInfo.sDestinationPath<<config->MatrixInfo.sFileMask;
	Print(cr,msg.str());

	msg.str(""); msg<<"The result was rotated by "<<config->MatrixInfo.fRotation<<" degrees";
	Print(cr,msg.str());
	if (config->MatrixInfo.bUseROI) {
		msg.str(""); msg<<"The matrix was cropped to the region ("<<config->MatrixInfo.roi[0]<<", "<<config->MatrixInfo.roi[1]<<") - ("<<config->MatrixInfo.roi[2]<<", "<<config->MatrixInfo.roi[3]<<")";
		Print(cr,msg.str());
	}
	msg.str(""); msg<<"Slices saved as "<<config->MatrixInfo.FileType;
	if (config->MatrixInfo.FileType!=kipl::io::TIFFfloat) {
		msg<<" with gray level interval ["<<config->MatrixInfo.fGrayInterval[0]<<", "<<config->MatrixInfo.fGrayInterval[1]<<"]";
	}
	Print(cr,msg.str());

	ProcessingModules(cr,config);

//	// todo


//	if (hist!=NULL) {
//		linepos+=10;
//		cr->set_font_size(14.0);
//		cr->move_to(margin,linepos);
//		cr->show_text("Histogram");
//		cr->move_to(margin,linepos+5); cr->line_to(width/2,linepos+5);
//		cr->stroke();
//		linepos+=textheight+3;
//		this->Plot(cr,hist,haxis,Nbins,static_cast<int>(margin),static_cast<int>(linepos),static_cast<int>(width-2*margin),150);
//		linepos+=160;
//	}
//
	int imgcnt=0;
	int w3=static_cast<int>((width-2*m_fMargin)/3);
	int dstdims[2]={w3-15,w3-15};


	if (xy!=NULL || xz!=NULL || yz!=NULL) {
		m_fLine+=m_fLineHeight;
		Print(cr,"Images",11.0);
		cr->move_to(m_fMargin,m_fLine-0.5*m_fLineHeight); cr->line_to(width/2,m_fLine-0.5*m_fLineHeight);
		m_fLine+=5;
		cr->stroke();
	}

	if (xy!=NULL) {
		Print(cr,"XY slice",m_fMargin,m_fLine);

		m_ImageRenderer.set_image(xy->GetDataPtr(),xy->Dims());
		m_ImageRenderer.Render(cr,static_cast<int>(m_fMargin),static_cast<int>(m_fLine+0.5*m_fLineHeight),dstdims[0],dstdims[1]);
		cr->stroke();
		imgcnt++;
	}

	Print(cr,"Projection",m_fMargin+imgcnt*w3,m_fLine);
	kipl::base::TImage<float,2> img=LoadProjection(config);
	m_ImageRenderer.set_image(img.GetDataPtr(),img.Dims(),0.0f,1.5f);
	Gtk_addons::ImageViewerRectangle rect(config->ProjectionInfo.roi[0], config->ProjectionInfo.roi[1],
			config->ProjectionInfo.roi[2]-config->ProjectionInfo.roi[0],
			config->ProjectionInfo.roi[3]-config->ProjectionInfo.roi[1],	Gdk::Color("green"));
	m_ImageRenderer.set_rectangle(rect, 0);
//	cr->line_width(0.3);
	m_ImageRenderer.Render(cr,static_cast<int>(m_fMargin)+imgcnt*w3,static_cast<int>(m_fLine+0.5*m_fLineHeight),dstdims[0],dstdims[1]);
	cr->stroke();
	imgcnt++;


	cr->show_page();
#else

    msg.str("");
    msg<< "The report generator was not compiled with Cairo support.";
    logger(kipl::logging::Logger::LogWarning,msg.str());

#endif

}

kipl::base::TImage<float,2> ReconReport::LoadProjection(ReconConfig * config)
{
	ProjectionReader reader;
	kipl::base::TImage<float,2> img,ob;

	img=reader.Read(config->ProjectionInfo.sPath,
			config->ProjectionInfo.sFileMask,
			config->ProjectionInfo.nFirstIndex,
			config->ProjectionInfo.eFlip,
			config->ProjectionInfo.eRotate,
			config->ProjectionInfo.fBinning,
			NULL);

	if (config->ProjectionInfo.nOBCount) {
		ob=reader.Read(config->ProjectionInfo.sReferencePath,
				config->ProjectionInfo.sOBFileMask,
				config->ProjectionInfo.nOBFirstIndex,
				config->ProjectionInfo.eFlip,
				config->ProjectionInfo.eRotate,
				config->ProjectionInfo.fBinning,
				NULL);

		float *pImg=img.GetDataPtr();
		float *pOb=ob.GetDataPtr();

		for (size_t i=0; i<img.Size(); i++)
			pImg[i]/=pOb[i];
	}

	return img;
}

void ReconReport::ProcessingModules(Cairo::RefPtr<Cairo::Context> cr,ReconConfig * config)
{
	ostringstream msg;
	msg.str("");
	m_fLine+=0.5*m_fLineHeight;
	Print(cr,"Preprocessing modules:",11.0);
	if (config->modules.empty()) {
		Print(cr,"No preprocessing modules were used.");
	}
	else {
		std::list<ModuleConfig>::iterator it;
		for (it=config->modules.begin(); it!=config->modules.end(); it++) {
			if (it->m_bActive) {
				msg.str(""); msg<<it->m_sModule<<" ["<<it->m_sSharedObject<<"]";
				Print(cr,msg.str());
				msg.str(""); msg<<it->PrintParameters()<<"\n ";
				Print(cr,msg.str(),6);
			}
		}
	}
	m_fLine+=0.5*m_fLineHeight;
	Print(cr,"Backprojector",11.0);
	msg.str(""); msg<<config->backprojector.m_sModule<<" ["<<config->backprojector.m_sSharedObject<<"]";
	Print(cr,msg.str());
	msg.str(""); msg<<config->backprojector.PrintParameters()<<"\n ";
	Print(cr,msg.str(),6);

}
