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
#include <segmentation/thresholds.h>

DoubleThresholdDlg::DoubleThresholdDlg(QWidget *parent) :
    ConfiguratorDialogBase("DoubleThreshold", true, false, true,parent),
    ui(new Ui::DoubleThresholdDlg),
    pOriginal(nullptr),
    m_fHighThreshold(1.0),
    m_fLowThreshold(0.0),
    m_compare(kipl::segmentation::cmp_greatereq)
{
    ui->setupUi(this);
    ui->viewer->showDifference(false);
    ui->plot_histogram->hideLegend();
    ui->plot_histogram->setPointsVisible(0);
}

DoubleThresholdDlg::~DoubleThresholdDlg()
{
    delete ui;
}

void DoubleThresholdDlg::UpdateDialog()
{
    ui->doubleSpinBox_lower->setValue(static_cast<double>(m_fLowThreshold));
    ui->doubleSpinBox_upper->setValue(static_cast<double>(m_fHighThreshold));
    ui->comboBox->setCurrentIndex(static_cast<int>(m_compare));

}

void DoubleThresholdDlg::UpdateParameters()
{
    m_fLowThreshold  = static_cast<float>(ui->doubleSpinBox_lower->value());
    m_fHighThreshold = static_cast<float>(ui->doubleSpinBox_upper->value());

    m_compare = static_cast<kipl::segmentation::CmpType>(ui->comboBox->currentIndex());
}

void DoubleThresholdDlg::ApplyParameters()
{
    UpdateParameters();

    kipl::base::TImage<char,3> res(pOriginal->Dims());
    kipl::segmentation::DoubleThreshold(*pOriginal,res,
            m_fLowThreshold,m_fHighThreshold,
            m_compare,
            kipl::base::conn6);

    std::copy_n(res.GetDataPtr(),res.Size(),bilevelImg.GetDataPtr());
    ui->viewer->setImages(pOriginal,&bilevelImg);
    QString status;

    std::ostringstream msg;

    msg<<"Low: "<<m_fLowThreshold<<"/High: "<<m_fHighThreshold<<", Compare :"<<m_compare;

    ui->label_currentSettings->setText(QString::fromStdString(msg.str()));

}

void DoubleThresholdDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["highthreshold"]      = kipl::strings::value2string(m_fHighThreshold);
    parameters["lowthreshold"]       = kipl::strings::value2string(m_fLowThreshold);
    parameters["compare"]            = enum2string(m_compare);
}

int DoubleThresholdDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fHighThreshold      = GetFloatParameter(parameters,"highthreshold");
    m_fLowThreshold       = GetFloatParameter(parameters,"lowthreshold");
    if (m_fHighThreshold<m_fLowThreshold)
        std::swap(m_fLowThreshold,m_fHighThreshold);

    string2enum(GetStringParameter(parameters,"compare"),m_compare);
    pOriginal = &img;
    bilevelImg.Resize(img.Dims());

    const size_t N=512;
    size_t bins[N];
    float axis[N];

    kipl::base::Histogram(img.GetDataPtr(),img.Size(),bins,N,0.0f,0.0f,axis);
    m_nHistMax = *std::max_element(bins,bins+N);

    ui->viewer->setImages(pOriginal,nullptr);

    try {
        ui->plot_histogram->setCurveData(0,axis,bins,N,"Histogram");
    }
    catch (std::bad_alloc & e) {
        QString msg="Failed to allocate series: ";
        msg=msg+QString::fromStdString(e.what());
        QMessageBox::warning(this,"Exception",msg);
        reject();
    }
    UpdateDialog();

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

void DoubleThresholdDlg::on_doubleSpinBox_upper_valueChanged(double arg1)
{
    QtCharts::QLineSeries *series=nullptr;
    try {
        series=new QtCharts::QLineSeries();
    }
    catch (std::bad_alloc & e) {
        QString msg="Failed to allocate series: ";
        msg=msg+QString::fromStdString(e.what());
        QMessageBox::warning(this,"Exception",msg);
        return;
    }

    series->append(static_cast<qreal>(arg1),static_cast<qreal>(0));
    series->append(static_cast<qreal>(arg1),static_cast<qreal>(m_nHistMax));
    series->setName("Upper threshold");

    ui->plot_histogram->setCurveData(2,series);
    ui->plot_histogram->updateAxes();
}

void DoubleThresholdDlg::on_doubleSpinBox_lower_valueChanged(double arg1)
{
    QtCharts::QLineSeries *series=nullptr;
    try {
        series=new QtCharts::QLineSeries();
    }
    catch (std::bad_alloc & e) {
        QString msg="Failed to allocate series: ";
        msg=msg+QString::fromStdString(e.what());
        QMessageBox::warning(this,"Exception",msg);
        return;
    }

    series->append(static_cast<qreal>(arg1),static_cast<qreal>(0));
    series->append(static_cast<qreal>(arg1),static_cast<qreal>(m_nHistMax));
    series->setName("Upper threshold");


    ui->plot_histogram->setCurveData(1,series);
    ui->plot_histogram->updateAxes();
}


void DoubleThresholdDlg::on_comboBox_currentIndexChanged(int index)
{
   // ApplyParameters();
}

void DoubleThresholdDlg::on_pushButton_apply_clicked()
{
    ApplyParameters();
}
