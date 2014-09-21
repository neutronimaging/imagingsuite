#include "morphspotcleandlg.h"
#include "ui_morphspotcleandlg.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>

MorphSpotCleanDlg::MorphSpotCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("MorphSpotCleanDlg",true,true,parent),
    ui(new Ui::MorphSpotCleanDlg),
    m_eConnectivity(kipl::morphology::conn8),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanBoth),
    m_fThreshold(0.05f)
{
    ui->setupUi(this);
    float data[16]={0,1,2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15 };
    size_t dims[2]={4,4};

}

MorphSpotCleanDlg::~MorphSpotCleanDlg()
{
    delete ui;
}

void MorphSpotCleanDlg::ApplyParameters()
{
    kipl::base::TImage<float,2> img(m_Projections.Dims());
    memcpy(img.GetDataPtr(),m_Projections.GetDataPtr(),img.Size()*sizeof(float));

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;
    kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewerOrignal->set_image(img.GetDataPtr(),img.Dims(),axis[nLo],axis[nHi]);

//    std::map<std::string, std::string> parameters;
//    UpdateParameters();
//    UpdateParameterList(parameters);

//    m_Cleaner.Configure(*m_Config, parameters);

//    kipl::base::TImage<float,2> det=m_Cleaner.DetectionImage(img);
//    size_t cumhist[N];
//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(det.GetDataPtr(), det.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::math::cumsum(hist,cumhist,N);

//    float fcumhist[N];
//    size_t ii=0;
//    for (ii=0;ii<N;ii++) {
//        fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
//        if (0.99f<fcumhist[ii])
//            break;
//    }

//    size_t N99=ii;
//    m_plot.setCurveData(0,axis,fcumhist,N99);
//    float threshold[N];
//    // In case of sigmoid mixing
////    for (size_t i=0; i<N; i++) {
////        threshold[i]=kipl::math::Sigmoid(axis[i], m_fThreshold, m_fSigma);
////    }
////    m_plot.setCurveData(1,axis,threshold,N99);
//    m_plot.setPlotCursor(0,QtAddons::PlotCursor(m_fThreshold,Qt::red,QtAddons::PlotCursor::Vertical));

//    std::map<std::string,std::string> pars;
//    kipl::base::TImage<float,2> pimg=img;
//    pimg.Clone();
//    m_Cleaner.Process(pimg, pars);

//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(pimg.GetDataPtr(), pimg.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
//    m_viewer_processed.set_image(pimg.GetDataPtr(), pimg.Dims(),axis[nLo],axis[nHi]);

//    kipl::base::TImage<float,2> diff=pimg-img;
//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
//    for (size_t i=0; i<diff.Size(); i++)
//        diff[i]=diff[i]!=0.0;
//    m_viewer_difference.set_image(diff.GetDataPtr(), diff.Dims());
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
//    m_spin_threshold.setValue(m_fThreshold);
//    m_combo_CleanMethod.setCurrentIndex(m_eCleanMethod);
//    m_combo_Connectivity.setCurrentIndex(m_eConnectivity);
}

void MorphSpotCleanDlg::UpdateParameters()
{
    //m_eDetectionMethod =
//    m_fThreshold  = m_spin_threshold.value();
//    m_eCleanMethod = static_cast<ImagingAlgorithms::eMorphCleanMethod>(m_combo_CleanMethod.currentIndex());
//    m_eConnectivity = static_cast<kipl::morphology::MorphConnect>(m_combo_Connectivity.currentIndex());

}

void MorphSpotCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["cleanmethod"]  = enum2string(m_eCleanMethod);
    parameters["threshold"]    = kipl::strings::value2string(m_fThreshold);
    parameters["connectivity"] = enum2string(m_eConnectivity);
}
