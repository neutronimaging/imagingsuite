//<LICENSE>

#include "SpotClean2Dlg.h"
#undef None
#include "ui_SpotClean2Dlg.h"
#define None 0L

#include <base/thistogram.h>
#include <base/KiplException.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <strings/miscstring.h>

#include <ModuleException.h>
#include <ParameterHandling.h>

SpotClean2Dlg::SpotClean2Dlg(QWidget *parent) :
    ConfiguratorDialogBase("SpotClean2Dlg",true,true,true,parent),
    ui(new Ui::SpotClean2Dlg)
{
    ui->setupUi(this);
}

SpotClean2Dlg::~SpotClean2Dlg()
{
    delete ui;
}

int SpotClean2Dlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img)
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

void SpotClean2Dlg::ApplyParameters()
{
    m_OriginalImage=GetProjection(m_Projections,m_Projections.Size(2)/2);

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;

    kipl::base::Histogram(m_OriginalImage.GetDataPtr(), m_OriginalImage.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);

    ui->viewer_original->set_image(m_OriginalImage.GetDataPtr(),m_OriginalImage.Dims(),axis[nLo],axis[nHi]);

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    m_Cleaner.Configure(*m_Config, parameters);

    m_DetectionImage=m_Cleaner.DetectionImage(m_OriginalImage);
    for (size_t i=0; i<m_DetectionImage.Size(); i++)
        if (m_DetectionImage[i]!=m_DetectionImage[i]) m_DetectionImage[i]=0;

    size_t cumhist[N];
    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));

    kipl::base::Histogram(m_DetectionImage.GetDataPtr(), m_DetectionImage.Size(), hist, N, 0.0f, 0.0f, axis);

    kipl::math::cumsum(hist,cumhist,N);

    float fcumhist[N];
    size_t ii=0;
    for (ii=0;ii<N;ii++) {
        fcumhist[ii]=static_cast<float>(cumhist[ii])/static_cast<float>(cumhist[N-1]);
        if (0.99f<fcumhist[ii])
            break;
    }

    size_t N99=ii;
    ui->plot_detection->setCurveData(0,axis,fcumhist,N99);
    float threshold[N];

    for (size_t i=0; i<N; i++) {
        threshold[i]=kipl::math::Sigmoid(axis[i], m_fGamma, m_fSigma);
    }
    ui->plot_detection->setCurveData(1,axis,threshold,N99);

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
    ui->viewer_result->set_image(m_ProcessedImage.GetDataPtr(), m_ProcessedImage.Dims(),axis[nLo],axis[nHi]);

    on_combo_detection_display_currentIndexChanged(ui->combo_detection_display->currentIndex());
}

void SpotClean2Dlg::UpdateDialog()
{
    ui->entry_gamma->setValue(m_fGamma);
    ui->entry_sigma->setValue(m_fSigma);
    ui->entry_iterations->setValue(m_nIterations);
    ui->entry_max->setValue(m_fMaxLevel);
    ui->entry_min->setValue(m_fMinLevel);
    ui->entry_maxarea->setValue(m_nMaxArea);
    ui->combo_detectionmethod->setCurrentIndex(m_eDetectionMethod);
}

void SpotClean2Dlg::UpdateParameters()
{
    m_fGamma            = ui->entry_gamma->value();
    m_fSigma            = ui->entry_sigma->value();
    m_nIterations       = ui->entry_iterations->value();
    m_fMaxLevel         = ui->entry_max->value();
    m_fMinLevel         = ui->entry_min->value();
    m_nMaxArea          = ui->entry_maxarea->value();
    m_eDetectionMethod  = static_cast<ImagingAlgorithms::DetectionMethod>(ui->combo_detectionmethod->currentIndex());
}

void SpotClean2Dlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["detectionmethod"] = enum2string(m_eDetectionMethod);
    parameters["gamma"]           = kipl::strings::value2string(m_fGamma);
    parameters["sigma"]           = kipl::strings::value2string(m_fSigma);
    parameters["iterations"]      = kipl::strings::value2string(m_nIterations);
    parameters["maxlevel"]        = kipl::strings::value2string(m_fMaxLevel);
    parameters["minlevel"]        = kipl::strings::value2string(m_fMinLevel);
    parameters["maxarea"]         = kipl::strings::value2string(m_nMaxArea);
}

void SpotClean2Dlg::on_combo_detection_display_currentIndexChanged(int index)
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
                dimg[i]=0.0f;
        }
    case 2: // Detection image
        dimg=m_DetectionImage;
        break;
    case 3: // Hit map
        dimg=m_OriginalImage-m_ProcessedImage;
        for (size_t i=0; i<dimg.Size(); i++)
            dimg[i] = dimg[i]!=0 ? 1.0f : 0.0f;
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
    ui->viewer_difference->set_image(dimg.GetDataPtr(),dimg.Dims(),axis[nLo],axis[nHi]);
}

void SpotClean2Dlg::on_buttonBox_clicked(QAbstractButton *button)
{
        std::ostringstream msg;
        QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

        logger(kipl::logging::Logger::LogMessage,"Clicked on button box");
        switch(standardButton) {
            // Standard buttons:
            case QDialogButtonBox::Ok:
                logger(kipl::logging::Logger::LogMessage,"Ok");
                UpdateParameters();
                accept();
                break;
            case QDialogButtonBox::Cancel:
            logger(kipl::logging::Logger::LogMessage,"Cancel");
                reject();
                break;
            case QDialogButtonBox::Apply:
            logger(kipl::logging::Logger::LogMessage,"Apply");
                ApplyParameters();
                break;
            }
}


void SpotClean2Dlg::on_button_apply_clicked()
{
    ApplyParameters();
}
