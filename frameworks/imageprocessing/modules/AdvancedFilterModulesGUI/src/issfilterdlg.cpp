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
    ui->spinBox_slice->setMinimum(0);
    ui->spinBox_slice->setMaximum(static_cast<int>(img.Size(2))-1);
    ui->horizontalSlider_slice->setMinimum(0);
    ui->horizontalSlider_slice->setMaximum(static_cast<int>(img.Size(2))-1);
    ui->horizontalSlider_slice->setValue(static_cast<int>((img.Size(2)-1)/2));
    on_horizontalSlider_slice_sliderMoved(static_cast<int>((img.Size(2)-1)/2));

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

    on_horizontalSlider_slice_sliderMoved(ui->horizontalSlider_slice->value());

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

void ISSFilterDlg::on_spinBox_slice_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->horizontalSlider_slice);
    ui->horizontalSlider_slice->setValue(arg1);
    on_horizontalSlider_slice_sliderMoved(arg1);
}

void ISSFilterDlg::on_horizontalSlider_slice_sliderMoved(int position)
{
    QSignalBlocker blocker(ui->spinBox_slice);

    ui->spinBox_slice->setValue(position);
    size_t lPos=static_cast<size_t>(position);

    float *pOrig=pOriginal->GetLinePtr(0UL,lPos);
    ui->image_original->set_image(pOrig,pOriginal->Dims());

    if (filtered.Size() == pOriginal->Size()) {

        float *pFilt=filtered.GetLinePtr(0,lPos);
        ui->image_filtered->set_image(pFilt,filtered.Dims());

        kipl::base::TImage<float,2> diff(pOriginal->Dims());

        for (size_t i=0; i<diff.Size(); ++i) {
            diff[i]=pFilt[i]-pOrig[i];
        }
        ui->image_diff->set_image(diff.GetDataPtr(),diff.Dims());
    }

}

void ISSFilterDlg::on_buttonBox_clicked(QAbstractButton *button)
{

}

void ISSFilterDlg::on_pushButton_Apply_clicked()
{
    ApplyParameters();
}

void ISSFilterDlg::on_checkBox_linkLevels_toggled(bool checked)
{
    if (checked)
        ui->image_original->LinkImageViewer(ui->image_filtered);
    else
        ui->image_original->ClearLinkedImageViewers();
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
