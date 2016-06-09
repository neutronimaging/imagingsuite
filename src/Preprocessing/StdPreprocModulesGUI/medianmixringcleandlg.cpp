#include "medianmixringcleandlg.h"
#include "ui_medianmixringclean.h"

#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <ModuleException.h>
#include <base/KiplException.h>

MedianMixRingCleanDlg::MedianMixRingCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("MedianMixRingCleanDlg",true,true,true,parent),
    ui(new Ui::MedianMixRingClean)
{
    ui->setupUi(this);
}

MedianMixRingCleanDlg::~MedianMixRingCleanDlg()
{
    delete ui;
}

int MedianMixRingCleanDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img)
{
    m_fSigma=GetFloatParameter(parameters,"width");
    m_fLambda=GetFloatParameter(parameters,"threshold");

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

void MedianMixRingCleanDlg::ApplyParameters()
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

    std::map<std::string,std::string> pars;
    m_ProcessedImage=m_OriginalImage;
    m_ProcessedImage.Clone();
    m_Cleaner.Process(m_ProcessedImage, pars);

    std::ostringstream msg;
    msg.str(""); msg<<"Processed image"<<m_ProcessedImage;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    m_DifferenceImage = m_OriginalImage - m_ProcessedImage;
    ui->viewer_difference->set_image(m_DifferenceImage.GetDataPtr(),m_DifferenceImage.Dims());
}

void MedianMixRingCleanDlg::UpdateDialog()
{
    ui->entry_lambda->setValue(m_fLambda);
    ui->entry_sigma->setValue(m_fSigma);
}

void MedianMixRingCleanDlg::UpdateParameters()
{
    m_fSigma=ui->entry_sigma->value();
    m_fLambda=ui->entry_lambda->value();
}

void MedianMixRingCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["width"]     =   kipl::strings::value2string(m_fSigma);
    parameters["threshold"] =   kipl::strings::value2string(m_fLambda);
}
