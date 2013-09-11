#include "stdafx.h"
#include "SpotClean2Dlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>

SpotClean2Dlg::SpotClean2Dlg(QWidget *parent) :
    ConfiguratorDialogBase("SpotClean2Dlg", true, parent),
	lbl_gamma("Gamma"),
	lbl_sigma("Sigma"),
	lbl_iterations("Iterations"),
	lbl_maxarea("Max spot area"),
	lbl_min("Min value"),
	lbl_max("Max value"),
	m_eDetectionMethod(ImagingAlgorithms::Detection_Median),
	m_fGamma(0.05f),
	m_fSigma(0.005f),
	m_nIterations(1),
	m_fMaxLevel(-0.1f),
	m_fMinLevel(12.0f),
	m_nMaxArea(100)
{
    float data[16]={0,1,2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15 };
	size_t dims[2]={4,4};
    m_FrameMain.setLayout(&m_vbox_main);
    m_vbox_main.addLayout(&m_hbox_viewers);
    m_hbox_viewers.addWidget(&m_viewer_original);
    m_hbox_viewers.addWidget(&m_viewer_processed);
    m_hbox_viewers.addWidget(&m_viewer_difference);
    m_hbox_viewers.addWidget(&m_plot);
    m_viewer_difference.set_image(data,dims);
	m_viewer_original.set_image(data,dims);
	m_viewer_processed.set_image(data,dims);

	const int nDigits=4;

    m_entry_gamma.setDecimals(nDigits);
    m_entry_gamma.setRange(0.0,10.0);
    m_entry_gamma.setSingleStep(0.01);

    m_entry_sigma.setDecimals(nDigits);
    m_entry_sigma.setRange(0.0,10.0);
    m_entry_sigma.setSingleStep(0.001);

    m_entry_iterations.setRange(1,10);
    m_entry_maxarea.setRange(1,500);

    m_entry_min.setDecimals(nDigits);
    m_entry_min.setRange(-1.0e6,1.0e6);
    m_entry_min.setSingleStep(0.1);

    m_entry_max.setDecimals(nDigits);
    m_entry_max.setRange(-1.0e6,1.0e6);
    m_entry_max.setSingleStep(0.1);

    m_hbox_parameters.addWidget(&lbl_gamma);
    m_hbox_parameters.addWidget(&m_entry_gamma);
    m_hbox_parameters.addWidget(&lbl_sigma);
    m_hbox_parameters.addWidget(&m_entry_sigma);
    m_hbox_parameters.addWidget(&lbl_iterations);
    m_hbox_parameters.addWidget(&m_entry_iterations);
    m_hbox_parameters.addWidget(&lbl_maxarea);
    m_hbox_parameters.addWidget(&m_entry_maxarea);
    m_vbox_parameters.addLayout(&m_hbox_parameters);

    m_hbox_minmax.addWidget(&lbl_min);
    m_hbox_minmax.addWidget(&m_entry_min);
    m_hbox_minmax.addWidget(&lbl_max);
    m_hbox_minmax.addWidget(&m_entry_max);
    m_vbox_parameters.addLayout(&m_hbox_minmax);

    m_hbox_params_plot.addLayout(&m_vbox_parameters);
    m_vbox_main.addLayout(&m_hbox_params_plot);

    setWindowTitle("Configure spot clean");

    show();
}

SpotClean2Dlg::~SpotClean2Dlg(void)
{

}

void SpotClean2Dlg::ApplyParameters()
{
    logger(kipl::logging::Logger::LogMessage,"Apply paramters");
	kipl::base::TImage<float,2> img(m_Projections.Dims());
	memcpy(img.GetDataPtr(),m_Projections.GetDataPtr(),img.Size()*sizeof(float));

	const size_t N=512;
	size_t hist[N];
	float axis[N];
	size_t nLo, nHi;
	kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, N, 0.0f, 0.0f, axis);
	kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
	m_viewer_original.set_image(img.GetDataPtr(),img.Dims(),axis[nLo],axis[nHi]);

	std::map<std::string, std::string> parameters;
	UpdateParameters();
	UpdateParameterList(parameters);

	cleaner.Configure(*m_Config, parameters);


	kipl::base::TImage<float,2> det=cleaner.DetectionImage(img);
	size_t cumhist[N];
	memset(hist,0,N*sizeof(size_t));
	memset(axis,0,N*sizeof(float));
	kipl::base::Histogram(det.GetDataPtr(), det.Size(), hist, N, 0.0f, 0.0f, axis);
	kipl::math::cumsum(hist,cumhist,N);

	float fcumhist[N];
	size_t ii=0;
	for (ii=0;ii<N;ii++) {
		fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
		if (0.99f<fcumhist[ii])
			break;
	}

	size_t N99=ii;
    m_plot.setCurveData(0,axis,fcumhist,N99);
	float threshold[N];
	for (size_t i=0; i<N; i++) {
		threshold[i]=kipl::math::Sigmoid(axis[i], m_fGamma, m_fSigma);
	}
    m_plot.setCurveData(1,axis,threshold,N99);

	std::map<std::string,std::string> pars;
	kipl::base::TImage<float,2> pimg=img;
	pimg.Clone();
	cleaner.Process(pimg, pars);

	memset(hist,0,N*sizeof(size_t));
	memset(axis,0,N*sizeof(float));
	kipl::base::Histogram(pimg.GetDataPtr(), pimg.Size(), hist, N, 0.0f, 0.0f, axis);
	kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
	m_viewer_processed.set_image(pimg.GetDataPtr(), pimg.Dims(),axis[nLo],axis[nHi]);

	kipl::base::TImage<float,2> diff=pimg-img;
    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
	for (size_t i=0; i<diff.Size(); i++)
		diff[i]=diff[i]!=0.0;
	m_viewer_difference.set_image(diff.GetDataPtr(), diff.Dims());
}

int SpotClean2Dlg::exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
{
	m_Projections=img;

	m_Config=dynamic_cast<ReconConfig *>(config);

	string2enum(GetStringParameter(parameters,"detectionmethod"), m_eDetectionMethod);
	m_fGamma=GetFloatParameter(parameters,"gamma");
	m_fSigma=GetFloatParameter(parameters,"sigma");
	m_nIterations=GetIntParameter(parameters,"iterations");
	m_fMaxLevel=GetFloatParameter(parameters,"maxlevel");
	m_fMinLevel=GetFloatParameter(parameters,"minlevel");
	m_nMaxArea=GetIntParameter(parameters,"maxarea");

	UpdateDialog();
    ApplyParameters();

    int res=QDialog::exec();
    
    if (res==QDialog::Accepted) {
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

void SpotClean2Dlg::UpdateDialog()
{
	// m_combobox_method. ...
    m_entry_gamma.setValue(m_fGamma);
    m_entry_sigma.setValue(m_fSigma);
    m_entry_iterations.setValue(m_nIterations);
    m_entry_max.setValue(m_fMaxLevel);
    m_entry_min.setValue(m_fMinLevel);
    m_entry_maxarea.setValue(m_nMaxArea);
}

void SpotClean2Dlg::UpdateParameters()
{
	//m_eDetectionMethod =
    m_fGamma      = m_entry_gamma.value();
    m_fSigma      = m_entry_sigma.value();
    m_nIterations = m_entry_iterations.value();
    m_fMaxLevel   = m_entry_max.value();
    m_fMinLevel   = m_entry_min.value();
    m_nMaxArea	  = m_entry_maxarea.value();
}

void SpotClean2Dlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
	parameters["detectionmethod"] = enum2string(m_eDetectionMethod);
	parameters["gamma"]      = kipl::strings::value2string(m_fGamma);
	parameters["sigma"]      = kipl::strings::value2string(m_fSigma);
	parameters["iterations"] = kipl::strings::value2string(m_nIterations);
	parameters["maxlevel"]   = kipl::strings::value2string(m_fMaxLevel);
	parameters["minlevel"]   = kipl::strings::value2string(m_fMinLevel);
	parameters["maxarea"]   = kipl::strings::value2string(m_nMaxArea);
}
