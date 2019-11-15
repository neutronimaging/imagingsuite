//<LICENSE>

#include "dosecorrectiondlg.h"
#include "ui_dosecorrectiondlg.h"

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <math/basicprojector.h>

DoseCorrectionDlg::DoseCorrectionDlg(QWidget *parent) :
    ConfiguratorDialogBase("ClampData", true, false, true,parent),
    ui(new Ui::DoseCorrectionDlg)
{
    ui->setupUi(this);
    ui->roi_widget->registerViewer(ui->imageviewer);
    ui->roi_widget->setROIColor("limegreen");
    ui->roi_widget->setTitle("Dose ROI");
}

DoseCorrectionDlg::~DoseCorrectionDlg()
{
    delete ui;
}

int DoseCorrectionDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fSlope = static_cast<double>(GetFloatParameter(parameters,"slope"));
    m_fIntercept = static_cast<double>(GetFloatParameter(parameters,"intercept"));
    kipl::strings::String2Array(GetStringParameter(parameters,"doseroi"),m_nROI,4);

    UpdateDialog();
    updateImage(img);

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

    return 0;

}

void DoseCorrectionDlg::UpdateDialog()
{
    ui->doubleSpinBox_slope->setValue(m_fSlope);
    ui->doubleSpinBox_intercept->setValue(m_fIntercept);

    ui->roi_widget->setROI(m_nROI);
}

void DoseCorrectionDlg::UpdateParameters()
{
    m_fSlope = ui->doubleSpinBox_slope->value();
    m_fIntercept = ui->doubleSpinBox_intercept->value();

    ui->roi_widget->getROI(m_nROI);
}

void DoseCorrectionDlg::ApplyParameters()
{

}

void DoseCorrectionDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters.clear();

    parameters["slope"]     = kipl::strings::value2string(m_fSlope);
    parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
    std::ostringstream arraystring;
    arraystring<<m_nROI[0]<<" "<<m_nROI[1]<<" "<<m_nROI[2]<<" "<<m_nROI[3];
    parameters["doseroi"]   = arraystring.str();
}

void DoseCorrectionDlg::updateImage(kipl::base::TImage<float, 3> &img)
{
    kipl::base::TImage<float,2> proj=kipl::math::BasicProjector<float>::project(img,kipl::base::ImagePlaneXY);

    ui->imageviewer->set_image(proj.GetDataPtr(),proj.Dims());

}
