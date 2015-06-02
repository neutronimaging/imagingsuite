
#include "morphspotcleandlg.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <ModuleException.h>
#include <base/KiplException.h>

MorphSpotCleanDlg::MorphSpotCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("MorphSpotCleanDlg",true,true,true,parent),
    ui(new Ui::MorphSpotCleanDlg),
    m_eConnectivity(kipl::morphology::conn8),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectBoth),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
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
    memcpy(img.GetLinePtr(0,m_Projections.Size(2)/2),m_Projections.GetDataPtr(),img.Size()*sizeof(float));

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;
    kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerOrignal->set_image(img.GetDataPtr(),img.Dims(),axis[nLo],axis[nHi]);
 //      ui->viewerOrignal->set_image(img.GetDataPtr(),img.Dims(),0.0f,1.5f);

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

//    m_Cleaner.Configure(*m_Config, parameters);

//    kipl::base::TImage<float,2> det=m_Cleaner.DetectionImage(img);
//    for (int i=0; i<img.Size(); i++)
//        if (det[i]!=det[i]) det[i]=0;

//    size_t cumhist[N];
//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//  //  kipl::base::Histogram(det.GetDataPtr(), det.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::math::cumsum(hist,cumhist,N);

//    float fcumhist[N];
//    size_t ii=0;
//    for (ii=0;ii<N;ii++) {
//        fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
//        if (0.99f<fcumhist[ii])
//            break;
//    }

//    size_t N99=ii;
//    ui->plotDetection->setCurveData(0,axis,hist,N99);
//    float threshold[N];
//    // In case of sigmoid mixing
////    for (size_t i=0; i<N; i++) {
////        threshold[i]=kipl::math::Sigmoid(axis[i], m_fThreshold, m_fSigma);
////    }
////    m_plot.setCurveData(1,axis,threshold,N99);
//    ui->plotDetection->setPlotCursor(0,QtAddons::PlotCursor(m_fThreshold,Qt::red,QtAddons::PlotCursor::Vertical));

//    std::map<std::string,std::string> pars;
//    kipl::base::TImage<float,2> pimg=img;
//    pimg.Clone();
//    m_Cleaner.Process(pimg, pars);

//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(pimg.GetDataPtr(), pimg.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
//    ui->viewerProcessed->set_image(pimg.GetDataPtr(), pimg.Dims(),axis[nLo],axis[nHi]);

//    kipl::base::TImage<float,2> diff=pimg-img;
//    memset(hist,0,N*sizeof(size_t));
//    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
////    for (size_t i=0; i<diff.Size(); i++)
////        diff[i]=(diff[i]!=0.0 ? 1.0f : 0.0f);
//    ui->viewerDifference->set_image(diff.GetDataPtr(), diff.Dims());
//   //  ui->viewerDifference->set_image(det.GetDataPtr(), det.Dims());
}

int MorphSpotCleanDlg::exec(ConfigBase *config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
{

    std::ostringstream msg;

    m_Projections=img;

    m_Config=dynamic_cast<ReconConfig *>(config);

    msg.str("");
    try {
        string2enum(GetStringParameter(parameters,"cleanmethod"), m_eCleanMethod);
        string2enum(GetStringParameter(parameters,"connectivity"), m_eConnectivity);

        m_fThreshold        = GetFloatParameter(parameters,"threshold");
        m_fSigma            = GetFloatParameter(parameters,"sigma");
        m_nEdgeSmoothLength = GetIntParameter(parameters,"edgesmooth");
        m_nMaxArea          = GetIntParameter(parameters,"maxarea");
        m_fMinLevel         = GetFloatParameter(parameters,"minlevel");
        m_fMaxLevel         = GetFloatParameter(parameters,"maxlevel");
    }
    catch (ModuleException &e) {
        msg<<"Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg<<"Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }


    UpdateDialog();
    ApplyParameters();

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
        return true;
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }

    return false;
}

void MorphSpotCleanDlg::UpdateDialog()
{
    ui->spinThreshold->setValue(m_fThreshold);
    ui->spinSigma->setValue(m_fSigma);
    ui->comboMethod->setCurrentIndex(m_eCleanMethod);
    ui->comboConnectivity->setCurrentIndex(m_eConnectivity);
    ui->spinArea->setValue(m_nMaxArea);
    ui->spinMinValue->setValue(m_fMinLevel);
    ui->spinMaxValue->setValue(m_fMaxLevel);
    ui->spinEdgeLenght->setValue(m_nEdgeSmoothLength);
}

void MorphSpotCleanDlg::UpdateParameters()
{
    m_eCleanMethod = static_cast<ImagingAlgorithms::eMorphCleanMethod>(ui->comboMethod->currentIndex());
    m_eConnectivity = static_cast<kipl::morphology::MorphConnect>(ui->comboConnectivity->currentIndex());
    m_fThreshold = ui->spinThreshold->value();
    m_fSigma     = ui->spinSigma->value();
    m_fMinLevel = ui->spinMinValue->value();
    m_fMaxLevel = ui->spinMaxValue->value();
    m_nMaxArea  = ui->spinArea->value();
    m_nEdgeSmoothLength = ui->spinEdgeLenght->value();
}

void MorphSpotCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{    
    parameters["connectivity"] = enum2string(m_eConnectivity);
    parameters["cleanmethod"]  = enum2string(m_eCleanMethod);
    parameters["threshold"]    = kipl::strings::value2string(m_fThreshold);
    parameters["sigma"]        = kipl::strings::value2string(m_fSigma);
    parameters["edgesmooth"]   = kipl::strings::value2string(m_nEdgeSmoothLength);
    parameters["maxarea"]      = kipl::strings::value2string(m_nMaxArea);
    parameters["minlevel"]     = kipl::strings::value2string(m_fMinLevel);
    parameters["maxlevel"]     = kipl::strings::value2string(m_fMaxLevel);
}


void MorphSpotCleanDlg::on_buttonApply_clicked()
{
    ApplyParameters();
}

