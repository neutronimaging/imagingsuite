#include "adaptivefilterdlg.h"
#include "ui_adaptivefilterdlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <AdaptiveFilter.h>
#include <math/mathfunctions.h>
#include <base/thistogram.h>
#include <base/tprofile.h>
#include <plotter.h>
#include <map>
#include <string>

AdaptiveFilterDlg::AdaptiveFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("AdaptiveFilter",true,false,true,parent),
    ui(new Ui::AdaptiveFilterDlg)
{
    ui->setupUi(this);
}

AdaptiveFilterDlg::~AdaptiveFilterDlg()
{
    delete ui;
}

int AdaptiveFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_Projections=img;

    m_Config=config;
    m_fLambda     = GetFloatParameter(parameters,"lambda");
    m_fSigma      = GetFloatParameter(parameters,"sigma");
    m_nFilterSize = GetIntParameter(parameters,"size");

    UpdateDialog();
    ApplyParameters();

    int res=QDialog::exec();

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

void AdaptiveFilterDlg::ApplyParameters()
{
    kipl::base::TImage<float,3> img=m_Projections;
    img.Clone();

    m_Sino=GetSinogram(img,img.Size(1)/2);

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;
    kipl::base::Histogram(m_Sino.GetDataPtr(), m_Sino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerOriginal->set_image(m_Sino.GetDataPtr(),m_Sino.Dims(),axis[nLo],axis[nHi]);

    float *sinoprofile=new float[m_Sino.Size(1)];
    kipl::base::verticalprofile2D(m_Sino.GetDataPtr(),m_Sino.Dims(),sinoprofile,true);
    float maxprof=*std::max_element(sinoprofile,sinoprofile+m_Sino.Size(1));
    float minprof=*std::min_element(sinoprofile,sinoprofile+m_Sino.Size(1));

    float *weightprofile=new float[m_Sino.Size(1)];
    float *sinoangles=new float[m_Sino.Size(1)];

    ReconConfig *rc=dynamic_cast<ReconConfig *>(m_Config);
    float da=(rc->ProjectionInfo.fScanArc[1]-rc->ProjectionInfo.fScanArc[0])/(m_Sino.Size(1)-1);
    float wscale=maxprof-minprof;
    for (size_t i=0; i<m_Sino.Size(1); i++) {
        sinoangles[i]=rc->ProjectionInfo.fScanArc[0]+i*da;
        weightprofile[i]=minprof+0.1*wscale+0.8*wscale*kipl::math::Sigmoid(static_cast<float>(i),m_fLambda,m_fSigma);
    }

    ui->plotHistogram->setCurveData(0,sinoprofile,sinoangles,m_Sino.Size(1),Qt::blue);
    ui->plotHistogram->setCurveData(1,weightprofile,sinoangles,m_Sino.Size(1),Qt::red);

    AdaptiveFilter module;

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    module.Configure(*(dynamic_cast<ReconConfig *>(m_Config)),parameters);
    module.Process(img,parameters);

    m_ProcessedSino=GetSinogram(img,img.Size(1)/2);

    kipl::base::Histogram(m_ProcessedSino.GetDataPtr(), m_ProcessedSino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerProcessed->set_image(m_ProcessedSino.GetDataPtr(),m_ProcessedSino.Dims(),axis[nLo],axis[nHi]);

    on_comboCompare_currentIndexChanged(ui->comboCompare->currentIndex());

    delete [] sinoangles;
    delete [] sinoprofile;
    delete [] weightprofile;
}

void AdaptiveFilterDlg::UpdateDialog()
{
    ui->spinLambda->setValue(m_fLambda);
    ui->spinSigma->setValue(m_fSigma);
    ui->spinSize->setValue(m_nFilterSize);
}

void AdaptiveFilterDlg::UpdateParameters()
{
    m_fLambda     = ui->spinLambda->value();
    m_fSigma      = ui->spinSigma->value();
    m_nFilterSize = ui->spinSize->value();
}

void AdaptiveFilterDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["lambda"] = kipl::strings::value2string(m_fLambda);
    parameters["sigma"]  = kipl::strings::value2string(m_fSigma);
    parameters["size"]   = kipl::strings::value2string(m_nFilterSize);
}

void AdaptiveFilterDlg::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text()=="Apply")
        ApplyParameters();
}

void AdaptiveFilterDlg::on_comboCompare_currentIndexChanged(int index)
{
    kipl::base::TImage<float,2> diff=m_Sino-m_ProcessedSino;

    switch (index) {
    case 0: break;
    case 1:
        for (int i=0; diff.Size(); i++)
            diff[i]= diff[i]!=0.0f;
        break;
    }

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;

    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerDifference->set_image(diff.GetDataPtr(),diff.Dims(),axis[nLo],axis[nHi]);
}
