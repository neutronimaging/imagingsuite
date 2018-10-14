//<LICENSE>
#include "issfilterdlg.h"
#include "ui_issfilterdlg.h"

#include <QDebug>
#include <QSignalBlocker>
#include <QFileDialog>

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <scalespace/ISSfilter.h>
#include <scalespace/filterenums.h>
#include <math/image_statistics.h>
#include <base/thistogram.h>

#include <ISSfilterModule.h>

ISSFilterDlg::ISSFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("ISSFilter", true, false, true,parent),
    ui(new Ui::ISSFilterDlg)
{
    ui->setupUi(this);
}

ISSFilterDlg::~ISSFilterDlg()
{
    delete ui;
}

int ISSFilterDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_fSlope     = static_cast<double>(GetFloatParameter(parameters,"slope")) ;
    m_fIntercept = static_cast<double>(GetFloatParameter(parameters,"intercept")) ;
    m_bAutoScale = kipl::strings::string2bool(GetStringParameter(parameters,"autoscale")) ;

    m_fTau        = static_cast<double>(GetFloatParameter(parameters,"tau"));
    m_fLambda     = static_cast<double>(GetFloatParameter(parameters,"lambda"));
    m_fAlpha      = static_cast<double>(GetFloatParameter(parameters,"alpha"));
    m_nIterations = GetIntParameter(parameters,"iterations");

    m_bSaveIterations = kipl::strings::string2bool(GetStringParameter(parameters,"saveiterations")) ;
    m_sIterationPath  = GetStringParameter(parameters,"iterationpath");
    string2enum(GetStringParameter(parameters,"regularization"), m_eRegularization );
    string2enum(GetStringParameter(parameters,"initialimage"),m_eInitialImage);

    pOriginal=&img;
    UpdateDialog();

    ui->volumesViewer->setImages(pOriginal,nullptr);
    const size_t N=512;
    float axis[N];
    size_t hist[N];
    kipl::base::Histogram(pOriginal->GetDataPtr(),pOriginal->Size(),hist,N,0.0f,0.0f,axis);
    ui->plotHistogram->setCurveData(0,axis,hist,N,"Original");

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

void ISSFilterDlg::UpdateDialog()
{
    ui->groupBox_normalization->setChecked(!m_bAutoScale);
    ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    ui->doubleSpinBox_slope->setValue(m_fSlope);
    ui->doubleSpinBox_alpha->setValue(m_fAlpha);
    ui->doubleSpinBox_lambda->setValue(m_fLambda);
    ui->doubleSpinBox_tau->setValue(m_fTau);
    ui->spinBox_iterations->setValue(m_nIterations);

    ui->groupBox_saveIterations->setChecked(m_bSaveIterations);
    ui->lineEdit_iterationPath->setText(QString::fromStdString(m_sIterationPath));
    ui->comboBox_initialImage->setCurrentIndex(static_cast<int>(m_eInitialImage));
    ui->comboBox_regularization->setCurrentIndex(static_cast<int>(m_eRegularization));
}

void ISSFilterDlg::UpdateParameters()
{
    m_bAutoScale=!ui->groupBox_normalization->isChecked();
    m_fSlope=ui->doubleSpinBox_slope->value();
    m_fIntercept=ui->doubleSpinBox_intercept->value();

    m_fTau=ui->doubleSpinBox_tau->value();
    m_fLambda=ui->doubleSpinBox_lambda->value();
    m_fAlpha=ui->doubleSpinBox_alpha->value();
    m_nIterations=ui->spinBox_iterations->value();
    m_sIterationPath = ui->lineEdit_iterationPath->text().toStdString();
    m_bSaveIterations = ui->groupBox_saveIterations->isChecked();

    m_eRegularization=static_cast<akipl::scalespace::eRegularizationType>(ui->comboBox_regularization->currentIndex());
    m_eInitialImage=static_cast<akipl::scalespace::eInitialImageType>(ui->comboBox_initialImage->currentIndex());
}

void ISSFilterDlg::ApplyParameters()
{
    logger.message("Applying current parameters");
    UpdateParameters();
    std::map<string, string> parameters;
    UpdateParameterList(parameters);


    akipl::scalespace::ISSfilterQ3D<float> iss(nullptr);
    filtered.Clone(*pOriginal);

    normalizeImage(filtered,true);
    iss.eInitialImage = m_eInitialImage;
    iss.m_eRegularization = m_eRegularization;

    iss.Process(    filtered,
                    m_fTau,
                    m_fLambda,
                    m_fAlpha,
                    m_nIterations,
                    m_bSaveIterations,
                    m_sIterationPath);
    if (m_bAutoScale) {
        ui->doubleSpinBox_slope->setValue(m_fSlope);
        ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    }
    normalizeImage(filtered,false);

    ui->volumesViewer->setImages(pOriginal,&filtered);

    const size_t N=512;
    float axis[N];
    size_t hist[N];
    kipl::base::Histogram(filtered.GetDataPtr(),filtered.Size(),hist,N,0.0f,0.0f,axis);
    ui->plotHistogram->setCurveData(1,axis,hist,N,"Filtered");

    float x[4096];

    for (int i=0; i<m_nIterations; ++i)
        x[i]=i+1;
    ui->plotError->hideLegend();
    ui->plotError->setCurveData(0,x,iss.GetErrorArray(), m_nIterations);
}

void ISSFilterDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters["slope"]     = kipl::strings::value2string(m_fSlope);
    parameters["intercept"] = kipl::strings::value2string(m_fIntercept);
    parameters["autoscale"] = kipl::strings::bool2string(m_bAutoScale);

    parameters["tau"]         = kipl::strings::value2string(m_fTau);
    parameters["lambda"]      = kipl::strings::value2string(m_fLambda);
    parameters["alpha"]       = kipl::strings::value2string(m_fAlpha);
    parameters["iterations"]  = kipl::strings::value2string(m_nIterations);

    parameters["saveiterations"] = kipl::strings::bool2string(m_bSaveIterations);
    parameters["iterationpath"]  = m_sIterationPath;
    parameters["regularization"] = enum2string(m_eRegularization);
    parameters["initialimage"]   = enum2string(m_eInitialImage);
}

void ISSFilterDlg::on_pushButton_browseIterationPath_clicked()
{
    QString fname=QFileDialog::getSaveFileName(this,"Select iteration file name",QDir::homePath(),"*.tif");

    if (fname.contains('#')==false) {
        int pos=fname.lastIndexOf('.');
        fname=fname.insert(pos,"_####");
    }

    ui->lineEdit_iterationPath->setText(fname);
}


void ISSFilterDlg::on_pushButton_Apply_clicked()
{
    ApplyParameters();
}

void ISSFilterDlg::normalizeImage(kipl::base::TImage<float,3> & img, bool forward)
{
    double slope=1.0, intercept=0.0;

    std::ostringstream msg;

    if (forward) {
        if (m_bAutoScale) {
            std::pair<double,double> stats=kipl::math::statistics(img.GetDataPtr(),img.Size());

            m_fIntercept=stats.first;
            m_fSlope=1.0f/stats.second;
        }

        msg<<"Scaling image with slope="<<m_fSlope<<" and intercept="<<m_fIntercept;
        logger(kipl::logging::Logger::LogMessage,msg.str());
        #pragma omp parallel
        {
            float *pImg = img.GetDataPtr();
            ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());

            #pragma omp for
            for (ptrdiff_t i=0; i<N; i++) {
                pImg[i]=m_fSlope*(pImg[i]-m_fIntercept);
            }
        }

    }
    else {
        slope=1.0/m_fSlope;
        intercept=m_fIntercept;

        #pragma omp parallel
        {
            float *pImg = img.GetDataPtr();
            ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());

            #pragma omp for
            for (ptrdiff_t i=0; i<N; i++) {
                pImg[i]=slope*pImg[i]+intercept;
            }
        }

    }


}
