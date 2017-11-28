//<LICENSE>

#include "medianmixringcleandlg.h"
#undef None
#include "ui_medianmixringclean.h"
#define None 0L
#include <QMessageBox>

#include <strings/miscstring.h>
#include <base/KiplException.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>

#include <ParameterHandling.h>
#include <ModuleException.h>

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
    m_Projections=img;

    m_Config=dynamic_cast<ReconConfig *>(config);

    try {
        m_fSigma=GetFloatParameter(parameters,"width");
        m_fLambda=GetFloatParameter(parameters,"threshold");
    }
    catch (kipl::base::KiplException & e) {
        logger(kipl::logging::Logger::LogWarning,e.what());
        return false;
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
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }

    return false;
}

void MedianMixRingCleanDlg::ApplyParameters()
{
    m_OriginalImage=GetSinogram(m_Projections,m_Projections.Size(1)/2);

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


    std::map<std::string,std::string> pars;

    size_t dims[3]={m_OriginalImage.Size(0), 1,m_OriginalImage.Size(1)};
    kipl::base::TImage<float,3> img(dims);

    memcpy(img.GetDataPtr(),m_OriginalImage.GetDataPtr(),m_OriginalImage.Size()*sizeof(float));

    try {
        m_Cleaner.Configure(*m_Config, parameters);
        m_Cleaner.Process(img, pars);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox errdlg(this);
        errdlg.setText("MedianMix ring clean failed to process sinogram.");
        errdlg.setDetailedText(QString::fromStdString(e.what()));
        errdlg.exec();

        logger(kipl::logging::Logger::LogWarning,e.what());
        return ;
    }

    m_ProcessedImage.Resize(m_OriginalImage.Dims());
    memcpy(m_ProcessedImage.GetDataPtr(),img.GetDataPtr(),m_ProcessedImage.Size()*sizeof(float));
    ui->viewer_result->set_image(m_ProcessedImage.GetDataPtr(),m_ProcessedImage.Dims());

    std::ostringstream msg;
    msg.str(""); msg<<"Processed image"<<m_ProcessedImage;
    logger(kipl::logging::Logger::LogMessage,msg.str());

    m_DifferenceImage = m_OriginalImage - m_ProcessedImage;
    ui->viewer_difference->set_image(m_DifferenceImage.GetDataPtr(),m_DifferenceImage.Dims());

    size_t Nwidth=m_Cleaner.profile.Size(0);
    float *xaxis=new float[Nwidth];
    for (size_t i=0; i<Nwidth; i++)
        xaxis[i]=static_cast<float>(i);

    ui->plot_decision->setCurveData(0,xaxis,m_Cleaner.profile.GetLinePtr(m_Cleaner.profile.Size(1)/2),m_Cleaner.profile.Size(0));

    DrawCursors(m_fLambda,m_fSigma);

    delete [] xaxis;
}

void MedianMixRingCleanDlg::DrawCursors(float lambda, float sigma)
{
    float dy_low=-log(1.0f/0.025f-1.0f)*sigma;
    float dy_high=-log(1.0f/0.975f-1.0f)*sigma;

    ui->plot_decision->setPlotCursor(0,QtAddons::PlotCursor(lambda+dy_low, QColor("green"), QtAddons::PlotCursor::Horizontal));
    ui->plot_decision->setPlotCursor(1,QtAddons::PlotCursor(lambda+dy_high, QColor("red"), QtAddons::PlotCursor::Horizontal));
    ui->plot_decision->setPlotCursor(2,QtAddons::PlotCursor(lambda, QColor("lightblue"), QtAddons::PlotCursor::Horizontal));
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

void MedianMixRingCleanDlg::on_entry_lambda_valueChanged(double arg1)
{
    DrawCursors(ui->entry_lambda->value(), ui->entry_sigma->value());
}

void MedianMixRingCleanDlg::on_entry_sigma_valueChanged(double arg1)
{
    DrawCursors(ui->entry_lambda->value(), ui->entry_sigma->value());
}

void MedianMixRingCleanDlg::on_button_apply_clicked()
{
    ApplyParameters();
}
