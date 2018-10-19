//<LICENSE>
#include "doublethresholddlg.h"
#include "ui_doublethresholddlg.h"

#include <QDebug>
#include <QLineSeries>
#include <QMessageBox>

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <math/image_statistics.h>
#include <base/thistogram.h>

DoubleThresholdDlg::DoubleThresholdDlg(QWidget *parent) :
    ConfiguratorDialogBase("DoubleThreshold", true, false, true,parent),
    ui(new Ui::DoubleThresholdDlg)
{
    ui->setupUi(this);
}

DoubleThresholdDlg::~DoubleThresholdDlg()
{
    delete ui;
}

void DoubleThresholdDlg::UpdateDialog()
{

}

void DoubleThresholdDlg::UpdateParameters()
{

}

void DoubleThresholdDlg::ApplyParameters()
{

}

void DoubleThresholdDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["highthreshold"]      = kipl::strings::value2string(m_fHighThreshold);
    parameters["lowthreshold"]       = kipl::strings::value2string(m_fLowThreshold);
    parameters["backgroundvalue"]    = kipl::strings::value2string(m_fBackgroundValue);
    parameters["usebackgroundvalue"] = kipl::strings::bool2string(m_bUseBackgroundValue);
}

int DoubleThresholdDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fHighThreshold      = GetFloatParameter(parameters,"highthreshold");
    m_fLowThreshold       = GetFloatParameter(parameters,"lowthreshold");
    if (m_fHighThreshold<m_fLowThreshold)
        std::swap(m_fLowThreshold,m_fHighThreshold);

    m_fBackgroundValue    = GetFloatParameter(parameters,"backgroundvalue");
    m_bUseBackgroundValue = kipl::strings::string2bool(GetStringParameter(parameters,"usebackgroundvalue"));

    pOriginal = &img;
    bilevelImg.Resize(img.Dims());

    const size_t N=512;
    size_t bins[N];
    float axis[N];

    kipl::base::Histogram(img.GetDataPtr(),img.Size(),bins,N,0.0f,0.0f,axis);
    m_nHistMax = *std::max_element(bins,bins+N);

    ui->doubleSpinBox_lower->setMaximum(m_fHighThreshold);
    ui->doubleSpinBox_upper->setMinimum(m_fLowThreshold);

    try {
 //       qDebug() << "Plotting treshold";
        ui->plot_histogram->setCurveData(0,axis,bins,N,"Histogram");
 //       qDebug() <<"Histogram plotted";
    }
    catch (std::bad_alloc & e) {
        QString msg="Failed to allocate series: ";
        msg=msg+QString::fromStdString(e.what());
        QMessageBox::warning(this,"Exception",msg);
        reject();
    }
    UpdateDialog();

  //  ApplyParameters();

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

void DoubleThresholdDlg::on_checkBox_background_toggled(bool checked)
{
    ui->doubleSpinBox_background->setVisible(checked);
}
