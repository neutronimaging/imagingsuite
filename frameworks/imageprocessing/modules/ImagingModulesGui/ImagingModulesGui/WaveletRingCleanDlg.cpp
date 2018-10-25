//<LICENSE>

#include "WaveletRingCleanDlg.h"
#undef None
#include "ui_WaveletRingCleanDlg.h"
#define None 0L

#include <list>

#include <QMessageBox>

#include <strings/miscstring.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <wavelets/wavelets.h>

#include <ParameterHandling.h>
#include <ModuleException.h>

#include <StripeFilter.h>

WaveletRingCleanDlg::WaveletRingCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("WaveletRingCleanDlg",true,true,true,parent),
    ui(new Ui::WaveletRingCleanDlg),
    m_nLevels(3),
    m_fSigma(0.05f),
    m_sWaveletName("daub15"),
    op(ImagingAlgorithms::VerticalComponentFFT),
    plane(kipl::base::ImagePlaneXY)
{
    ui->setupUi(this);
    PrepareWaveletComboBox();
}

WaveletRingCleanDlg::~WaveletRingCleanDlg()
{
    delete ui;
}

int WaveletRingCleanDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img)
{
    m_Projections=img;
//    memcpy(m_Projections.GetDataPtr(), img.GetDataPtr(), img.Dims()*sizeof(float));

//    ui->volumeviewer->setImages(&m_Projections, nullptr);

    m_Config=dynamic_cast<KiplProcessConfig *>(config);

    try {
        m_nLevels = GetFloatParameter(parameters,"decnum");
        m_fSigma  = GetFloatParameter(parameters,"sigma");
        m_sWaveletName = GetStringParameter(parameters, "waveletname");
        string2enum(GetStringParameter(parameters,"filtertype"),op);
        string2enum(GetStringParameter(parameters,"plane"), plane);
        m_bParallel = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));
    }
    catch (ModuleException & e)
    {
        logger(kipl::logging::Logger::LogWarning,e.what());
        return false;
    }

    UpdateDialog();


    try {
        ApplyParameters();
    }
    catch (kipl::base::KiplException &e) {
        logger(kipl::logging::Logger::LogError,e.what());
        return false;
    }

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        logger(kipl::logging::Logger::LogMessage,"Use settings");
        UpdateParameters();
        UpdateParameterList(parameters);
    }
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Discard settings");
    }
    return res;

}

void WaveletRingCleanDlg::ApplyParameters()
{

    logger.message("Applying current parameters");
    UpdateParameters();
    std::map<string, string> parameters;
    UpdateParameterList(parameters);

    kipl::base::TImage<float,3> img(m_Projections.Dims());


    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;


    m_OriginalImg.Clone(m_Projections);
    img.Clone(m_Projections);

    std::map<std::string,std::string> pars;


    try {
        m_Cleaner.Configure(*m_Config, parameters);
        m_Cleaner.ProcessCore(img, pars);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox errdlg(this);
        errdlg.setText("Failed to process image.");

        logger(kipl::logging::Logger::LogWarning,e.what());
        return ;
    }


    m_ProcessedImg.Resize(m_OriginalImg.Dims());
    memcpy(m_ProcessedImg.GetDataPtr(),img.GetDataPtr(), img.Size()*sizeof(float));


    ui->volumeviewer->setImages(&m_OriginalImg, &m_ProcessedImg);

    m_DifferenceSino=m_ProcessedImg-m_OriginalImg;
    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
//    kipl::base::Histogram(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.Size(), hist, N, 0.0f, 0.0f, axis);
//    kipl::base::FindLimits(hist, N, 95.0, &nLo, &nHi);
//    ui->viewer_difference->set_image(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.Dims());
    double sum2=0.0;
    float *pDiff=m_DifferenceSino.GetDataPtr();
    for (size_t i=0; i<m_DifferenceSino.Size(); i++)
        sum2+=pDiff[i]*pDiff[i];

    std::ostringstream msg;
    msg<<"(MSE = "<<sum2/m_DifferenceSino.Size();
//    ui->label_mse->setText(QString::fromStdString(msg.str()));
}

void WaveletRingCleanDlg::UpdateDialog()
{
    ui->entry_levels->setValue(m_nLevels);
    ui->entry_cutoff->setValue(m_fSigma);
    int default_wavelet, idx;

    kipl::wavelets::WaveletKernel<float> wk("daub4");

    std::list<std::string> wlist=wk.NameList();
    std::list<std::string>::iterator it;

    for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
        if (*it==m_sWaveletName)
            default_wavelet=idx;
    }

    ui->combo_wavelets->setCurrentIndex(default_wavelet);
    ui->combo_filtertype->setCurrentIndex(static_cast<int>(op));
}

void WaveletRingCleanDlg::UpdateParameters()
{
    m_sWaveletName    = ui->combo_wavelets->currentText().toStdString();
    m_nLevels         = ui->entry_levels->value();
    m_fSigma          = ui->entry_cutoff->value();
    m_bParallel       = false;
    op = static_cast<ImagingAlgorithms::StripeFilterOperation>(ui->combo_filtertype->currentIndex());
    plane = ui->volumeviewer->getImagePlane();
}

void WaveletRingCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["decnum"]   = kipl::strings::value2string(m_nLevels);
    parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
    parameters["waveletname"]    = m_sWaveletName;
    parameters["parallel"] = m_bParallel ? "true" : "false";
    parameters["filtertype"]   = enum2string(op);
    parameters["plane"] = enum2string(plane);
}

void WaveletRingCleanDlg::PrepareWaveletComboBox()
{
    std::ostringstream msg;

    kipl::wavelets::WaveletKernel<float> wk("daub4");
    std::list<std::string> wlist=wk.NameList();
    std::list<std::string>::iterator it;

    ui->combo_wavelets->clear();

    int default_wavelet, idx;
    for (idx=0, it=wlist.begin(); it!=wlist.end(); it++,idx++) {
        QString str=QString::fromStdString(*it);
        if (*it==m_sWaveletName)
            default_wavelet=idx;
        ui->combo_wavelets->addItem(str);
    }

    ui->combo_wavelets->setCurrentIndex(default_wavelet);
}

void WaveletRingCleanDlg::on_button_apply_clicked()
{
    ApplyParameters();
}
