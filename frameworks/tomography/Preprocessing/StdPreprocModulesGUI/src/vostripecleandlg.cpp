//<LICENSE>

#include "vostripecleandlg.h"
#include "ui_vostripecleandlg.h"

#include <ParameterHandling.h>
#include <strings/miscstring.h>
#include <base/thistogram.h>

#include <ModuleException.h>

#include <QMessageBox>

VoStripeCleanDlg::VoStripeCleanDlg(QWidget *parent) :
    ConfiguratorDialogBase("VoStripeCleanDlg",true,true,true,parent),
    ui(new Ui::VoStripeCleanDlg)
{
    ui->setupUi(this);
}

VoStripeCleanDlg::~VoStripeCleanDlg()
{
    delete ui;
}

int VoStripeCleanDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_Projections=img;

    m_Config=dynamic_cast<ReconConfig *>(config);

    try
    {
        useUnresponsiveStripes  = kipl::strings::string2bool(GetStringParameter(parameters,"useUnresponsiveStripes"));
        useStripeSorting        = kipl::strings::string2bool(GetStringParameter(parameters,"useStripeSorting"));
        useLargeStripes         = kipl::strings::string2bool(GetStringParameter(parameters,"useLargeStripes"));
        filterSize_unresponsive = GetIntParameter(parameters,"filterSize_unresponsive");
        filterSize_sorting      = GetIntParameter(parameters,"filterSize_sorting");
        filterSize_large        = GetIntParameter(parameters,"filterSize_large");
        snr_unresponsive        = GetIntParameter(parameters,"snr_unresponsive");
        snr_large               = GetFloatParameter(parameters,"snr_large");
    }
    catch (ModuleException & e)
    {
        logger(kipl::logging::Logger::LogWarning,e.what());
        return false;
    }

    UpdateDialog();

    try
    {
        ApplyParameters();
    }
    catch (kipl::base::KiplException &e)
    {
        logger(kipl::logging::Logger::LogError,e.what());
        return false;
    }

    int res=QDialog::exec();

    if (res==QDialog::Accepted)
    {
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

void VoStripeCleanDlg::ApplyParameters()
{
    kipl::base::TImage<float,3> img({m_Projections.Size(0), 1,m_Projections.Size(2)});

    const size_t N=512;
    std::vector<size_t> hist(N,0UL);
    std::vector<float>  axis(N,0.0f);

    size_t nLo, nHi;

    m_OriginalSino=GetSinogram(m_Projections,m_Projections.Size(1)>>1);

    std::copy_n(m_OriginalSino.GetDataPtr(),m_OriginalSino.Size(),img.GetDataPtr());

    kipl::base::Histogram(m_OriginalSino.GetDataPtr(), m_OriginalSino.Size(), N, hist, axis, 0.0f, 0.0f,true);

    kipl::base::FindLimits(hist, 97.5, nLo, nHi);
    ui->viewer_original->set_image(m_OriginalSino.GetDataPtr(),m_OriginalSino.dims(),axis[nLo],axis[nHi]);

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

    m_ProcessedSino.resize(m_OriginalSino.dims());

    std::copy_n(img.GetDataPtr(),img.Size(),m_ProcessedSino.GetDataPtr());

    std::fill(axis.begin(),axis.end(),0.0f);
    std::fill(hist.begin(),hist.end(),0UL);

    kipl::base::Histogram(m_ProcessedSino.GetDataPtr(), m_ProcessedSino.Size(), N, hist, axis, 0.0f, 0.0f, true);
    kipl::base::FindLimits(hist, 97.5, nLo, nHi);
    ui->viewer_processed->set_image(m_ProcessedSino.GetDataPtr(), m_ProcessedSino.dims(),axis[nLo],axis[nHi]);

    m_DifferenceSino=m_ProcessedSino-m_OriginalSino;

    std::fill(axis.begin(),axis.end(),0.0f);
    std::fill(hist.begin(),hist.end(),0UL);
    kipl::base::Histogram(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.Size(), N, hist, axis, 0.0f, 0.0f, true);
    kipl::base::FindLimits(hist, 97.5, nLo, nHi);
    ui->viewer_difference->set_image(m_DifferenceSino.GetDataPtr(), m_DifferenceSino.dims(),axis[nLo],axis[nHi]);

//    double sum2=0.0;
//    float *pDiff=m_DifferenceSino.GetDataPtr();
//    for (size_t i=0; i<m_DifferenceSino.Size(); i++)
//        sum2+=pDiff[i]*pDiff[i];

//    std::ostringstream msg;
//    msg<<"(MSE = "<<sum2/m_DifferenceSino.Size();
//    ui->label_mse->setText(QString::fromStdString(msg.str()));

}

void VoStripeCleanDlg::UpdateDialog()
{
    ui->groupBox_unresponsive -> setChecked(useUnresponsiveStripes);
    ui->groupBox_sorting      -> setChecked(useStripeSorting);
    ui->groupBox_large        -> setChecked(useLargeStripes);

    ui->spinBox_unresponsiveFilterSize -> setValue(filterSize_unresponsive);
    ui->spinBox_sortingFilterSize      -> setValue(filterSize_sorting);
    ui->spinBox_largeFilterSize        -> setValue(filterSize_large);
    ui->doubleSpinBox_unresponsiveSNR  -> setValue(snr_unresponsive);
    ui->doubleSpinBox_largeSNR         -> setValue(snr_large);
}

void VoStripeCleanDlg::UpdateParameters()
{
    useUnresponsiveStripes  = ui->groupBox_unresponsive->isChecked();
    useStripeSorting        = ui->groupBox_sorting->isChecked();
    useLargeStripes         = ui->groupBox_large->isChecked();

    filterSize_unresponsive = ui->spinBox_unresponsiveFilterSize->value();
    filterSize_sorting      = ui->spinBox_sortingFilterSize->value();
    filterSize_large        = ui->spinBox_largeFilterSize->value();
    snr_unresponsive        = ui->doubleSpinBox_unresponsiveSNR->value();
    snr_large               = ui->doubleSpinBox_largeSNR->value();
}

void VoStripeCleanDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters["useUnresponsiveStripes"]  = kipl::strings::bool2string(useUnresponsiveStripes);
    parameters["useStripeSorting"]        = kipl::strings::bool2string(useStripeSorting);
    parameters["useLargeStripes"]         = kipl::strings::bool2string(useLargeStripes);
    parameters["filterSize_unresponsive"] = std::to_string(filterSize_unresponsive);
    parameters["filterSize_sorting"]      = std::to_string(filterSize_sorting);
    parameters["filterSize_large"]        = std::to_string(filterSize_large);
    parameters["snr_unresponsive"]        = std::to_string(snr_unresponsive);
    parameters["snr_large"]               = std::to_string(snr_large);
}
