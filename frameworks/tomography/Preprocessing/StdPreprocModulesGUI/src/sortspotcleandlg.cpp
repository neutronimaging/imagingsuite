#include "sortspotcleandlg.h"
#include "ui_sortspotcleandlg.h"

#include <algorithm>

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/KiplException.h>
#include <ParameterHandling.h>
#include <ModuleException.h>

namespace
{
const std::vector<kipl::base::eConnectivity> kDilationOptions = {
    kipl::base::eConnectivity::none,
    kipl::base::eConnectivity::conn4,
    kipl::base::eConnectivity::conn8
};

const std::vector<ImagingAlgorithms::eSortSpotQuantile> kQuantileMethods = {
    ImagingAlgorithms::eSortSpotQuantile::DarkSpots,
    ImagingAlgorithms::eSortSpotQuantile::BrightSpots,
    ImagingAlgorithms::eSortSpotQuantile::Both
};

const std::vector<ImagingAlgorithms::eSortSpotThresholdMethod> kThresholdMethods = {
    ImagingAlgorithms::eSortSpotThresholdMethod::ConfidenceInterval,
    ImagingAlgorithms::eSortSpotThresholdMethod::Absolute
};

const std::vector<kipl::base::eConnectivity> kFillConnectivityOptions = {
    kipl::base::eConnectivity::conn4,
    kipl::base::eConnectivity::conn8
};

template <typename T>
int indexOf(const std::vector<T> &values, const T &value)
{
    const auto it = std::find(values.begin(), values.end(), value);
    return it == values.end() ? 0 : static_cast<int>(std::distance(values.begin(), it));
}
}

SortSpotCleanDlg::SortSpotCleanDlg(QWidget *parent)
    : ConfiguratorDialogBase("SortSpotCleanDlg",true,true,true,parent),
    ui(new Ui::SortSpotCleanDlg)
{
    ui->setupUi(this);
    ui->label_detectionplot->hide();
    ui->detection_Plot->hide();
}

SortSpotCleanDlg::~SortSpotCleanDlg()
{
    delete ui;
}

int SortSpotCleanDlg::exec( ConfigBase * _config, 
                            std::map<std::string, std::string> &parameters, 
                            kipl::base::TImage<float,3> & img)
{
    std::ostringstream msg;
    config=dynamic_cast<ReconConfig *>(_config);
    m_Projections = img;

    try {
        string2enum(GetStringParameter(parameters,"thresholdmethod"), m_thresholdMethod);
        string2enum(GetStringParameter(parameters,"connectivity"), m_connectivity);
        string2enum(GetStringParameter(parameters,"dilation"), m_dilation);
        string2enum(GetStringParameter(parameters,"quantilemethod"), m_quantileMethod);

        m_fThreshold    = GetFloatParameter(parameters,"threshold");
        m_fQuantile     = GetFloatParameter(parameters,"quantile");

        m_bClampData    = kipl::strings::string2bool(GetStringParameter(parameters,"clampdata"));
        m_bRemoveInfNaN = kipl::strings::string2bool(GetStringParameter(parameters,"removeinfnan"));
        m_fMinLevel     = GetFloatParameter(parameters,"minlevel");
        m_fMaxLevel     = GetFloatParameter(parameters,"maxlevel");
        m_nPatchSize    = GetIntParameter(parameters,"patchsize");
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
    UpdateParameters();
    
    auto res=QDialog::exec();

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

void SortSpotCleanDlg::ApplyParameters()
{
    // Implementation for ApplyParameters method
    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);
    m_Cleaner.Configure(*config, parameters);

    m_CleanedImage.Clone(m_OriginalImage); // Clone the original image to the cleaned image

    std::map<std::string,std::string> pars;
    
    m_Cleaner.Process(m_CleanedImage, pars);

    std::ostringstream msg;

    ui->viewer_Cleaned->set_image(m_CleanedImage.GetDataPtr(), m_CleanedImage.dims());
    ui->viewer_Cleaned->set_levels(kipl::base::quantile99);

    on_comboBox_Difference_currentIndexChanged(ui->comboBox_Difference->currentIndex());
}

void SortSpotCleanDlg::UpdateDialog()
{
    // Implementation for UpdateDialog method
    ui->checkBox_RemoveInfNan    ->setChecked(m_bRemoveInfNaN);
    
    ui->comboBox_Dilation        ->setCurrentIndex(indexOf(kDilationOptions, m_dilation));
    ui->comboBox_Difference      ->setCurrentIndex(0);
    ui->comboBox_Quantiles       ->setCurrentIndex(indexOf(kQuantileMethods, m_quantileMethod));
    ui->comboBox_ThresholdType   ->setCurrentIndex(indexOf(kThresholdMethods, m_thresholdMethod));
    ui->comboBox_FillConnectivity->setCurrentIndex(indexOf(kFillConnectivityOptions, m_connectivity));
    ui->doubleSpinBox_Quantile   ->setValue(m_fQuantile);
    ui->doubleSpinBox_threshold  ->setValue(m_fThreshold);
    ui->spinBox_PatchSize        ->setValue(m_nPatchSize);
    ui->groupBox_Clamping        ->setChecked(m_bClampData);
    ui->doubleSpinBox_minLevel   ->setValue(m_fMinLevel);
    ui->doubleSpinBox_maxLevel   ->setValue(m_fMaxLevel);
    ui->spinBox_imgidx           ->setValue(static_cast<int>(m_Projections.Size(2))/2);
    ui->spinBox_imgidx           ->setMaximum(static_cast<int>(m_Projections.Size(2))-1);
    ui->spinBox_imgidx           ->setMinimum(0);
}

void SortSpotCleanDlg::UpdateParameters()
{
    // Implementation for UpdateParameters method
    m_dilation         = kDilationOptions.at(ui->comboBox_Dilation->currentIndex());
    m_quantileMethod   = kQuantileMethods.at(ui->comboBox_Quantiles->currentIndex());

    m_thresholdMethod = kThresholdMethods.at(ui->comboBox_ThresholdType->currentIndex());

    m_connectivity    = kFillConnectivityOptions.at(ui->comboBox_FillConnectivity->currentIndex());
    
    m_fQuantile        = ui->doubleSpinBox_Quantile  ->value();
    m_fThreshold       = ui->doubleSpinBox_threshold ->value();
    m_nPatchSize       = ui->spinBox_PatchSize       ->value();
    m_bClampData       = ui->groupBox_Clamping       ->isChecked();
    m_fMinLevel        = ui->doubleSpinBox_minLevel  ->value();
    m_fMaxLevel        = ui->doubleSpinBox_maxLevel  ->value();
    m_bRemoveInfNaN    = ui->checkBox_RemoveInfNan   ->isChecked();

}

void SortSpotCleanDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{    
    parameters["thresholdmethod"]= enum2string(m_thresholdMethod);
    parameters["connectivity"]   = enum2string(m_connectivity);
    parameters["dilation"]       = enum2string(m_dilation);
    parameters["quantilemethod"] = enum2string(m_quantileMethod);

    parameters["threshold"]    = std::to_string(m_fThreshold);
    parameters["quantile"]     = std::to_string(m_fQuantile);

    parameters["clampdata"]    = kipl::strings::bool2string(m_bClampData);
    parameters["removeinfnan"] = kipl::strings::bool2string(m_bRemoveInfNaN);
    parameters["minlevel"]     = std::to_string(m_fMinLevel);
    parameters["maxlevel"]     = std::to_string(m_fMaxLevel);
    parameters["patchsize"]    = std::to_string(m_nPatchSize);
}

void SortSpotCleanDlg::on_comboBox_Difference_currentIndexChanged(int index)
{
    logger.message("Changed difference display: "+std::to_string(index));
    
    auto diffImage = m_OriginalImage - m_CleanedImage; // Default to original image
    switch (index) {
    case 0: // Original - cleaned
        ui->viewer_Difference->set_image(diffImage.GetDataPtr(), diffImage.dims());
        ui->viewer_Difference->set_levels(kipl::base::quantile99);
        break;
    case 1: // Difference precentage 
        diffImage = diffImage / m_OriginalImage; // Normalize by original image to get percentage difference
        ui->viewer_Difference->set_image(diffImage.GetDataPtr(), diffImage.dims());
        ui->viewer_Difference->set_levels(kipl::base::quantile99);
        break;
    case 2: // Detection image
        break;
    case 3: // Hit map
        for (size_t i = 0; i < diffImage.Size(); ++i) 
        {
            diffImage[i] = (diffImage[i] != 0.0f) ? 1.0f : 0.0f; // Binary hit map    
        }
        ui->viewer_Difference->set_image(diffImage.GetDataPtr(), diffImage.dims(),0.0f,1.0f);
        break;
    default:
        break;
    }

}

void SortSpotCleanDlg::on_comboBox_ThresholdType_currentIndexChanged(int index)
{
    logger.message("Threshold type changed: "+std::to_string(index));
    switch (index) {
    case 0: // Confidence Interval
        break;
    case 1: // Absolute
        break;
    default:
        break;
    }
}



void SortSpotCleanDlg::on_pushButton_Apply_clicked()
{
    logger.message("Apply button clicked");
    ApplyParameters();
}

void SortSpotCleanDlg::on_spinBox_imgidx_valueChanged(int arg1)
{
    logger.message("Image index changed: "+std::to_string(arg1));
    // Update the displayed image based on the new index   
    m_OriginalImage.resize(m_Projections.dims());
    std::copy_n(m_Projections.GetLinePtr(0,arg1), m_OriginalImage.Size(), m_OriginalImage.GetDataPtr());
    
    ui->viewer_Original->set_image(m_OriginalImage.GetDataPtr(), m_OriginalImage.dims());
    ui->viewer_Original->set_levels(kipl::base::quantile99);
}

