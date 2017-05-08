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
    m_eCleaningMethod(ImagingAlgorithms::VerticalComponentFFT)
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

    m_Config=dynamic_cast<ReconConfig *>(config);

    try {
        m_nLevels = GetFloatParameter(parameters,"decnum");
        m_fSigma  = GetFloatParameter(parameters,"sigma");
        m_sWaveletName = GetStringParameter(parameters, "wname");
        string2enum(GetStringParameter(parameters,"method"),m_eCleaningMethod);
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
    size_t dims[3]={m_Projections.Size(0), 1,m_Projections.Size(2)};
    kipl::base::TImage<float,3> img(dims);

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;

    m_OriginalSino=GetSinogram(m_Projections,m_Projections.Size(1)>>1);

    memcpy(img.GetDataPtr(),m_OriginalSino.GetDataPtr(),m_OriginalSino.Size()*sizeof(float));

    kipl::base::Histogram(m_OriginalSino.GetDataPtr(), m_OriginalSino.Size(), hist, N, 0.0f, 0.0f, axis);

    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewer_original->set_image(m_OriginalSino.GetDataPtr(),m_OriginalSino.Dims(),axis[nLo],axis[nHi]);

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    std::map<std::string,std::string> pars;

    try {
        m_Cleaner.Configure(*m_Config, parameters);
        m_Cleaner.Process(img, pars);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox errdlg(this);
        errdlg.setText("Failed to process sinogram.");

        logger(kipl::logging::Logger::LogWarning,e.what());
        return ;
    }

    m_ProcessedSino.Resize(m_OriginalSino.Dims());
    memcpy(m_ProcessedSino.GetDataPtr(),img.GetDataPtr(), m_ProcessedSino.Size()*sizeof(float));

    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(m_ProcessedSino.GetDataPtr(), m_ProcessedSino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewer_result->set_image(m_ProcessedSino.GetDataPtr(), m_ProcessedSino.Dims(),axis[nLo],axis[nHi]);

    m_DifferenceSino=m_ProcessedSino-m_OriginalSino;
    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 95.0, &nLo, &nHi);
    ui->viewer_difference->set_image(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.Dims());
    double sum2=0.0;
    float *pDiff=m_DifferenceSino.GetDataPtr();
    for (size_t i=0; i<m_DifferenceSino.Size(); i++)
        sum2+=pDiff[i]*pDiff[i];

    std::ostringstream msg;
    msg<<"(MSE = "<<sum2/m_DifferenceSino.Size();
    ui->label_mse->setText(QString::fromStdString(msg.str()));
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
    ui->combo_filtertype->setCurrentIndex(static_cast<int>(m_eCleaningMethod));
}

void WaveletRingCleanDlg::UpdateParameters()
{
    m_sWaveletName    = ui->combo_wavelets->currentText().toStdString();
    m_nLevels         = ui->entry_levels->value();
    m_fSigma          = ui->entry_cutoff->value();
    m_bParallel       = false;
    m_eCleaningMethod = static_cast<ImagingAlgorithms::StripeFilterOperation>(ui->combo_filtertype->currentIndex());
}

void WaveletRingCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["decnum"]   = kipl::strings::value2string(m_nLevels);
    parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
    parameters["wname"]    = m_sWaveletName;
    parameters["parallel"] = m_bParallel ? "true" : "false";
    parameters["method"]   = enum2string(m_eCleaningMethod);
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
