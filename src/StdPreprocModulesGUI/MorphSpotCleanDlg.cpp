#include "stdafx.h"
#include "MorphSpotCleanDlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>

MorphSpotCleanDlg::MorphSpotCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("MorphSpotCleanDlg", true, true, parent),
    lbl_threshold("Threshold"),
    lbl_Method("Clean method"),
    lbl_Connectivity("Connectivity"),
    m_eConnectivity(kipl::morphology::conn8),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanBoth),
    m_fThreshold(0.05f)
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

    m_spin_threshold.setDecimals(nDigits);
    m_spin_threshold.setRange(0.0,1e5);
    m_spin_threshold.setSingleStep(0.01);

    m_hbox_parameters.addWidget(&lbl_threshold);
    m_hbox_parameters.addWidget(&m_spin_threshold);
    m_hbox_parameters.addWidget(&lbl_Method);
    m_hbox_parameters.addWidget(&m_combo_CleanMethod);
    m_hbox_parameters.addWidget(&lbl_Connectivity);
    m_hbox_parameters.addWidget(&m_combo_Connectivity);

    m_combo_CleanMethod.addItem(QString::fromStdString(enum2string(ImagingAlgorithms::MorphCleanHoles)));
    m_combo_CleanMethod.addItem(QString::fromStdString(enum2string(ImagingAlgorithms::MorphCleanPeaks)));
    m_combo_CleanMethod.addItem(QString::fromStdString(enum2string(ImagingAlgorithms::MorphCleanBoth)));

    m_combo_Connectivity.addItem(QString::fromStdString(enum2string(kipl::morphology::conn4)));
    m_combo_Connectivity.addItem(QString::fromStdString(enum2string(kipl::morphology::conn8)));

    m_hbox_params_plot.addLayout(&m_vbox_parameters);
    m_vbox_main.addLayout(&m_hbox_params_plot);

    setWindowTitle("Configure spot clean");

    show();
}

MorphSpotCleanDlg::~MorphSpotCleanDlg(void)
{

}

void MorphSpotCleanDlg::ApplyParameters()
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


//	kipl::base::TImage<float,2> det=cleaner.DetectionImage(img);
//	size_t cumhist[N];
//	memset(hist,0,N*sizeof(size_t));
//	memset(axis,0,N*sizeof(float));
//	kipl::base::Histogram(det.GetDataPtr(), det.Size(), hist, N, 0.0f, 0.0f, axis);
//	kipl::math::cumsum(hist,cumhist,N);

//	float fcumhist[N];
//	size_t ii=0;
//	for (ii=0;ii<N;ii++) {
//		fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
//		if (0.99f<fcumhist[ii])
//			break;
//	}

//	size_t N99=ii;
//    m_plot.setCurveData(0,axis,fcumhist,N99);
//	float threshold[N];
//	for (size_t i=0; i<N; i++) {
//		threshold[i]=kipl::math::Sigmoid(axis[i], m_fGamma, m_fSigma);
//	}
//    m_plot.setCurveData(1,axis,threshold,N99);

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

int MorphSpotCleanDlg::exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
{
	m_Projections=img;

	m_Config=dynamic_cast<ReconConfig *>(config);

    string2enum(GetStringParameter(parameters,"cleanmethod"), m_eCleanMethod);
    string2enum(GetStringParameter(parameters,"connectivity"), m_eConnectivity);
    m_fThreshold=GetFloatParameter(parameters,"threshold");

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

void MorphSpotCleanDlg::UpdateDialog()
{
    m_spin_threshold.setValue(m_fThreshold);
    m_combo_CleanMethod.setCurrentIndex(m_eCleanMethod);
    m_combo_Connectivity.setCurrentIndex(m_eConnectivity);
}

void MorphSpotCleanDlg::UpdateParameters()
{
	//m_eDetectionMethod =
    m_fThreshold  = m_spin_threshold.value();
    m_eCleanMethod = static_cast<ImagingAlgorithms::eMorphCleanMethod>(m_combo_CleanMethod.currentIndex());
    m_eConnectivity = static_cast<kipl::morphology::MorphConnect>(m_combo_Connectivity.currentIndex());

}

void MorphSpotCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["cleanmethod"]  = enum2string(m_eCleanMethod);
    parameters["threshold"]    = kipl::strings::value2string(m_fThreshold);
    parameters["connectivity"] = enum2string(m_eConnectivity);
}
