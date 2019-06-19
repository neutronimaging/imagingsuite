//<LICENSE>

#include "morphspotcleandlg.h"
#undef None
#include "ui_morphspotcleandlg.h"
#define None 0L

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <base/KiplException.h>

#include <ParameterHandling.h>
#include <ModuleException.h>

#include <plotcursor.h>

MorphSpotCleanDlg::MorphSpotCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("MorphSpotCleanDlg",true,true,true,parent),
    ui(new Ui::MorphSpotCleanDlg),
    m_eConnectivity(kipl::morphology::conn4),
    m_eDetectionMethod(ImagingAlgorithms::MorphDetectBoth),
    m_eCleanMethod(ImagingAlgorithms::MorphCleanReplace),
    m_fThreshold{0.1f,0.1f},
    m_fSigma{0.01f,0.01f},
    m_bRemoveInfNaN(false),
    m_bClampData(false),
    m_bThreading(false)
{
    ui->setupUi(this);
    ui->spinArea->hide();
    ui->label_area->hide();
}

MorphSpotCleanDlg::~MorphSpotCleanDlg()
{
    delete ui;
}

void MorphSpotCleanDlg::ApplyParameters()
{
    //kipl::base::TImage<float,2> img(m_Projections.Dims());
    m_OriginalImage=GetProjection(m_Projections,m_Projections.Size(2)/2);

    const size_t N=1024;
    size_t hist[N];
    float axis[N];

    size_t nLo, nHi;

    kipl::base::Histogram(m_OriginalImage.GetDataPtr(), m_OriginalImage.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerOrignal->set_image(m_OriginalImage.GetDataPtr(),m_OriginalImage.Dims(),axis[nLo],axis[nHi]);

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    ui->plotDetection->clearAllCurves();
    ui->plotDetection->clearAllCursors();

    m_Cleaner.Configure(*config, parameters);

    m_DetectionImageHoles=m_Cleaner.DetectionImage(m_OriginalImage, ImagingAlgorithms::MorphDetectHoles);
    for (size_t i=0; i<m_DetectionImageHoles.Size(); i++) // Fix nans
        if (!std::isfinite(m_DetectionImageHoles[i])) m_DetectionImageHoles[i]=0;

    if ((m_eDetectionMethod==ImagingAlgorithms::MorphDetectHoles) || (m_eDetectionMethod==ImagingAlgorithms::MorphDetectBoth))
        prepareDetectionPlot(m_DetectionImageHoles,0,N,"Detection Holes","Threshold Holes");

    m_DetectionImagePeaks=m_Cleaner.DetectionImage(m_OriginalImage, ImagingAlgorithms::MorphDetectPeaks);
    for (size_t i=0; i<m_DetectionImagePeaks.Size(); i++) // Fix nans
        if (!std::isfinite(m_DetectionImagePeaks[i])) m_DetectionImagePeaks[i]=0;

    if ((m_eDetectionMethod==ImagingAlgorithms::MorphDetectPeaks) || (m_eDetectionMethod==ImagingAlgorithms::MorphDetectBoth))
        prepareDetectionPlot(m_DetectionImagePeaks,1,N,"Detection Peaks","Threshold Peaks");

    std::map<std::string,std::string> pars;
    m_ProcessedImage=m_OriginalImage;
    m_ProcessedImage.Clone();
    m_Cleaner.Process(m_ProcessedImage, pars);

    std::ostringstream msg;
    msg.str(""); msg<<"Processed image"<<m_ProcessedImage;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(m_ProcessedImage.GetDataPtr(), m_ProcessedImage.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewerProcessed->set_image(m_ProcessedImage.GetDataPtr(), m_ProcessedImage.Dims(),axis[nLo],axis[nHi]);

    on_comboDetectionDisplay_currentIndexChanged(ui->comboDetectionDisplay->currentIndex());
}

void MorphSpotCleanDlg::prepareDetectionPlot(kipl::base::TImage<float,2> &img,int det,size_t N,std::string curvelabel, std::string threslabel)
{

    size_t *hist = new size_t[N];
    float  *axis = new float[N];

    std::fill_n(hist,N,0);
    std::fill_n(axis,N,0);

    size_t *cumhist = new size_t[N];

    kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, N, 0.0f, 0.0f, axis);

    kipl::math::cumsum(hist,cumhist,N);

    float *fcumhist=new float[N];
    size_t ii=0;

    for (ii=0;ii<N;ii++) {
        fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
        if (0.999f<fcumhist[ii])
            break;
    }

    size_t N99=ii;

    ui->plotDetection->setCurveData(0+2*det,axis,fcumhist,static_cast<int>(N99),QString::fromStdString(curvelabel));
    float *threshold = new float[N];
    float *thaxis    = new float[N];
    if (m_fSigma[det]!=0.0f)
    { // In case of sigmoid mixing
        float endPoint = axis[N99] < m_fThreshold[det]+5*m_fSigma[det] ? m_fThreshold[det]+5*m_fSigma[det] : axis[N99];
        qDebug() <<"axis0"<<axis[0]<< "N99=" <<axis[N99]<<",endPoint="<<endPoint<<", diff="<<endPoint-axis[0];

        for (size_t i=0; i<N; i++) {
            thaxis[i]=axis[0]+i*(endPoint-axis[0])/N;
            threshold[i]=kipl::math::Sigmoid(thaxis[i], m_fThreshold[det], m_fSigma[det]);
        }
        ui->plotDetection->setCurveData(1+2*det,thaxis,threshold,static_cast<int>(N),QString::fromStdString(threslabel));
    }
    else
    {
        ui->plotDetection->setCursor(0,new QtAddons::PlotCursor(static_cast<double>(m_fThreshold[det]),
                                                                Qt::red,
                                                                QtAddons::PlotCursor::Vertical));
    }



    delete [] hist;
    delete [] axis;
    delete [] cumhist;
    delete [] fcumhist;
    delete [] threshold;
    delete [] thaxis;
}

int MorphSpotCleanDlg::exec(ConfigBase *_config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> &img)
{

    std::ostringstream msg;

    m_Projections=img;

    config=dynamic_cast<ReconConfig *>(_config);

    msg.str("");
    try {
        string2enum(GetStringParameter(parameters,"cleanmethod"), m_eCleanMethod);
        string2enum(GetStringParameter(parameters,"detectionmethod"), m_eDetectionMethod);
        string2enum(GetStringParameter(parameters,"connectivity"), m_eConnectivity);

        kipl::strings::String2Array(GetStringParameter(parameters,"threshold"),m_fThreshold,2);
        kipl::strings::String2Array(GetStringParameter(parameters,"sigma"),m_fSigma,2);
        m_nEdgeSmoothLength = GetIntParameter(parameters,"edgesmooth");
        m_nMaxArea          = GetIntParameter(parameters,"maxarea");
        m_bClampData        = kipl::strings::string2bool(GetStringParameter(parameters,"clampdata"));
        m_fMinLevel         = GetFloatParameter(parameters,"minlevel");
        m_fMaxLevel         = GetFloatParameter(parameters,"maxlevel");
        m_bThreading        = kipl::strings::string2bool(GetStringParameter(parameters,"threading"));
    }
    catch (ModuleException &e) {
        msg<<"Module exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Kipl Exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg<<"STL exception: Failed to get parameters: "<<e.what();
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
    ui->spinThresholdHoles->setValue(m_fThreshold[0]);
    ui->spinSigmaHoles->setValue(m_fSigma[0]);
    ui->spinThresholdPeaks->setValue(m_fThreshold[1]);
    ui->spinSigmaPeaks->setValue(m_fSigma[1]);
    ui->comboCleanMethod->setCurrentIndex(m_eCleanMethod);
    ui->comboDetectionMethod->setCurrentIndex(m_eDetectionMethod);
    on_comboDetectionMethod_currentIndexChanged(m_eDetectionMethod);

    ui->comboConnectivity->setCurrentIndex(m_eConnectivity);
    ui->spinArea->setValue(m_nMaxArea);
    ui->checkBoxRemoveInfNan->setChecked(m_bRemoveInfNaN);
    ui->groupBox_clampData->setChecked(m_bClampData);
    ui->spinMinValue->setValue(m_fMinLevel);
    ui->spinMaxValue->setValue(m_fMaxLevel);
    ui->spinEdgeLenght->setValue(m_nEdgeSmoothLength);
}

void MorphSpotCleanDlg::UpdateParameters()
{
    m_eCleanMethod      = static_cast<ImagingAlgorithms::eMorphCleanMethod>(ui->comboCleanMethod->currentIndex());
    m_eDetectionMethod  = static_cast<ImagingAlgorithms::eMorphDetectionMethod>(ui->comboDetectionMethod->currentIndex());
    m_eConnectivity     = static_cast<kipl::morphology::MorphConnect>(ui->comboConnectivity->currentIndex());
    m_fThreshold[0]     = static_cast<float>(ui->spinThresholdHoles->value());
    m_fSigma[0]         = static_cast<float>(ui->spinSigmaHoles->value());
    m_fThreshold[1]     = static_cast<float>(ui->spinThresholdPeaks->value());
    m_fSigma[1]         = static_cast<float>(ui->spinSigmaPeaks->value());
    m_bRemoveInfNaN     = ui->checkBoxRemoveInfNan->isChecked();
    m_bClampData        = ui->groupBox_clampData->isChecked();
    m_fMinLevel         = static_cast<float>(ui->spinMinValue->value());
    m_fMaxLevel         = static_cast<float>(ui->spinMaxValue->value());
    m_nMaxArea          = ui->spinArea->value();
    m_nEdgeSmoothLength = ui->spinEdgeLenght->value();
}

void MorphSpotCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{    
    parameters["connectivity"]    = enum2string(m_eConnectivity);
    parameters["cleanmethod"]     = enum2string(m_eCleanMethod);
    parameters["detectionmethod"] = enum2string(m_eDetectionMethod);
    parameters["threshold"]       = kipl::strings::Array2String(m_fThreshold,2);
    parameters["sigma"]           = kipl::strings::Array2String(m_fSigma,2);
    parameters["edgesmooth"]      = kipl::strings::value2string(m_nEdgeSmoothLength);
    parameters["maxarea"]         = kipl::strings::value2string(m_nMaxArea);
    parameters["removeinfnan"]    = kipl::strings::bool2string(m_bRemoveInfNaN);
    parameters["clampdata"]       = kipl::strings::bool2string(m_bClampData);
    parameters["minlevel"]        = kipl::strings::value2string(m_fMinLevel);
    parameters["maxlevel"]        = kipl::strings::value2string(m_fMaxLevel);
    parameters["threading"]       = kipl::strings::bool2string(m_bThreading);
}


void MorphSpotCleanDlg::on_buttonApply_clicked()
{
    ApplyParameters();
}


void MorphSpotCleanDlg::on_comboDetectionDisplay_currentIndexChanged(int index)
{
    std::ostringstream msg;

    msg<<"Change display index to "<<index;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    if (m_OriginalImage.Size()==0) {
        msg.str(""); msg<<"Original image void";
        logger(kipl::logging::Logger::LogMessage,msg.str());

        return;
    }
    if (m_OriginalImage.Size()!=m_ProcessedImage.Size()) {
        msg.str(""); msg<<"Original image: "<<m_OriginalImage<<std::endl<<"Processed image: "<<m_ProcessedImage;
        logger(kipl::logging::Logger::LogMessage,msg.str());

        return;
    }

    kipl::base::TImage<float,2> dimg=m_ProcessedImage;

    logger(kipl::logging::Logger::LogMessage,"Start switching");
    switch (index) {
    case 0: // Difference
        dimg=m_OriginalImage-m_ProcessedImage;
        break;
    case 1:
        for (size_t i=0; i<dimg.Size(); i++) {
            if (m_OriginalImage[i]!=0.0f)
                dimg[i] = 100*(m_OriginalImage[i]-m_ProcessedImage[i])/m_OriginalImage[i];
            else
                dimg[i] = 0.0f;
        }
        break;
    case 2: // Detection image
        switch (m_eDetectionMethod)
        {
        case ImagingAlgorithms::MorphDetectHoles :
            dimg=m_DetectionImageHoles;
            break;
        case ImagingAlgorithms::MorphDetectPeaks :
            dimg=m_DetectionImagePeaks;
            break;
        case ImagingAlgorithms::MorphDetectBoth :
            dimg=m_DetectionImagePeaks - m_DetectionImageHoles;
            break;
        }

        break;
    case 3: // Hit map
        dimg=m_OriginalImage-m_ProcessedImage;
        for (size_t i=0; i<dimg.Size(); i++)
            dimg[i] = dimg[i]!=0.0f ? 1.0f : 0.0f;
        break;
    }
    size_t nLo,nHi;

    const size_t N=512;
    float axis[N];
    size_t hist[N];

    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));

    kipl::base::Histogram(dimg.GetDataPtr(), dimg.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    logger(kipl::logging::Logger::LogMessage,"Start display");
    ui->viewerDifference->set_image(dimg.GetDataPtr(),dimg.Dims(),axis[nLo],axis[nHi]);
}

void MorphSpotCleanDlg::on_comboDetectionMethod_currentIndexChanged(int index)
{
    ImagingAlgorithms::eMorphDetectionMethod method = static_cast<ImagingAlgorithms::eMorphDetectionMethod>(index);

    switch (method)
    {
    case ImagingAlgorithms::MorphDetectHoles :
        ui->groupBoxPeaks->hide();
        ui->groupBoxHoles->show();
        break;

    case ImagingAlgorithms::MorphDetectPeaks :
        ui->groupBoxPeaks->show();
        ui->groupBoxHoles->hide();
        break;
    case ImagingAlgorithms::MorphDetectBoth :
        ui->groupBoxPeaks->show();
        ui->groupBoxHoles->show();
        break;

    }
}
