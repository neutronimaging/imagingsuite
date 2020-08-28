//<LICENSE>
#include "nonlindiffusiondlg.h"
#include "ui_nonlindiffusiondlg.h"
#include <QDebug>
#include <QSignalBlocker>
#include <QFileDialog>

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <scalespace/filterenums.h>
#include <math/image_statistics.h>
#include <math/normalizeimage.h>
#include <base/thistogram.h>
#include <scalespace/NonLinDiffAOS.h>

NonLinDiffusionDlg::NonLinDiffusionDlg(QWidget *parent) :
    ConfiguratorDialogBase("NonLinDiffusion", true, false, true,parent),
    ui(new Ui::NonLinDiffusionDlg)
{
    ui->setupUi(this);
    ui->plot_histogram->hideLegend();
}

NonLinDiffusionDlg::~NonLinDiffusionDlg()
{
    delete ui;
}

int NonLinDiffusionDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fSlope     = static_cast<double>(GetFloatParameter(parameters,"slope")) ;
    m_fIntercept = static_cast<double>(GetFloatParameter(parameters,"intercept")) ;
    m_bAutoScale = kipl::strings::string2bool(GetStringParameter(parameters,"autoscale")) ;

    m_fTau        = static_cast<double>(GetFloatParameter(parameters,"tau"));
    m_fLambda     = static_cast<double>(GetFloatParameter(parameters,"lambda"));
    m_fSigma      = static_cast<double>(GetFloatParameter(parameters,"sigma"));
    m_nIterations = GetIntParameter(parameters,"iterations");

    m_bSaveIterations = kipl::strings::string2bool(GetStringParameter(parameters,"saveiterations")) ;
    m_sIterationPath  = GetStringParameter(parameters,"iterationpath");

    pOriginal=&img;
    UpdateDialog();

    ui->viewer->setImages(pOriginal,nullptr);
    const size_t N=512;
    float axis[N];
    size_t hist[N];
    kipl::base::Histogram(pOriginal->GetDataPtr(),pOriginal->Size(),hist,N,0.0f,0.0f,axis);
    ui->plot_histogram->setCurveData(0,axis,hist,N,"Original");

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

void NonLinDiffusionDlg::UpdateDialog()
{
    ui->groupBox_normalize->setChecked(!m_bAutoScale);
    ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    ui->doubleSpinBox_slope->setValue(m_fSlope);
    ui->doubleSpinBox_sigma->setValue(m_fSigma);
    ui->doubleSpinBox_lambda->setValue(m_fLambda);
    ui->doubleSpinBox_tau->setValue(m_fTau);
    ui->spinBox_iterations->setValue(m_nIterations);

    ui->groupBox_saveIterations->setChecked(m_bSaveIterations);
    ui->lineEdit_iterationPath->setText(QString::fromStdString(m_sIterationPath));

}

void NonLinDiffusionDlg::UpdateParameters()
{
    m_bAutoScale=!ui->groupBox_normalize->isChecked();
    m_fSlope=ui->doubleSpinBox_slope->value();
    m_fIntercept=ui->doubleSpinBox_intercept->value();

    m_fTau=ui->doubleSpinBox_tau->value();
    m_fLambda=ui->doubleSpinBox_lambda->value();
    m_fSigma=ui->doubleSpinBox_sigma->value();
    m_nIterations=ui->spinBox_iterations->value();
    m_sIterationPath = ui->lineEdit_iterationPath->text().toStdString();
    m_bSaveIterations = ui->groupBox_saveIterations->isChecked();
}

void NonLinDiffusionDlg::ApplyParameters()
{
    logger.message("Applying current parameters");
    UpdateParameters();
    std::map<string, string> parameters;
    UpdateParameterList(parameters);


    filtered.Clone(*pOriginal);

    normalizeImage(filtered,true);

    if (m_bAutoScale) {
        ui->doubleSpinBox_slope->setValue(m_fSlope);
        ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    }

    akipl::scalespace::NonLinDiffusionFilter<float,3> nld(m_fSigma, m_fTau, m_fLambda, m_nIterations,nullptr);

    nld(filtered);
    normalizeImage(filtered,false);

    ui->viewer->setImages(pOriginal,&filtered);

    const size_t N=512;
    float axis[N];
    size_t hist[N];
    kipl::base::Histogram(filtered.GetDataPtr(),filtered.Size(),hist,N,0.0f,0.0f,axis);
    ui->plot_histogram->setCurveData(1,axis,hist,N,"Filtered");

    float x[4096];

    for (int i=0; i<m_nIterations; ++i)
        x[i]=i+1;
}

void NonLinDiffusionDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters["slope"]     = kipl::strings::value2string(m_fSlope);
    parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
    parameters["autoscale"] = kipl::strings::bool2string(m_bAutoScale);

    parameters["tau"]         = kipl::strings::value2string(m_fTau);
    parameters["lambda"]      = kipl::strings::value2string(m_fLambda);
    parameters["sigma"]       = kipl::strings::value2string(m_fSigma);
    parameters["iterations"]  = kipl::strings::value2string(m_nIterations);

    parameters["saveiterations"] = kipl::strings::bool2string(m_bSaveIterations);
    parameters["iterationpath"]  = m_sIterationPath;
}

void NonLinDiffusionDlg::normalizeImage(kipl::base::TImage<float,3> & img, bool forward)
{
    kipl::math::normalizeData(img.GetDataPtr(),img.Size(),m_fSlope,m_fIntercept,forward,m_bAutoScale);
}

void NonLinDiffusionDlg::on_pushButton_apply_clicked()
{
    ApplyParameters();
}

void NonLinDiffusionDlg::on_pushButton_browseIterations_clicked()
{
    QString fname=QFileDialog::getSaveFileName(this,"Select iteration file name",QDir::homePath(),"*.tif");

    if (fname.contains('#')==false) {
        int pos=fname.lastIndexOf('.');
        fname=fname.insert(pos,"_####");
    }

    ui->lineEdit_iterationPath->setText(fname);
}
