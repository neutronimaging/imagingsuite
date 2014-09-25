#include "adaptivefilterdlg.h"
#include "ui_adaptivefilterdlg.h"
#include <strings/miscstring.h>
#include <ParameterHandling.h>
#include <AdaptiveFilter.h>
#include <base/thistogram.h>
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

int AdaptiveFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img)
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
    kipl::base::TImage<float,2> sino;
    kipl::base::TImage<float,3> img=m_Projections;
    img.Clone();

    sino=GetSinogram(img,img.Size(2)/2);

    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;
    kipl::base::Histogram(sino.GetDataPtr(), sino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerOriginal->set_image(sino.GetDataPtr(),sino.Dims(),axis[nLo],axis[nHi]);
    ui->plotHistogram->setCurveData(0,axis,hist,N);
    ui->plotHistogram->setPlotCursor(0,QtAddons::PlotCursor(m_fLambda,Qt::red,QtAddons::PlotCursor::Vertical));

    AdaptiveFilter module;

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    module.Configure(*(dynamic_cast<ReconConfig *>(m_Config)),parameters);
    module.Process(img,parameters);

    kipl::base::TImage<float,2> psino=GetSinogram(img,img.Size(2)/2);

    kipl::base::Histogram(psino.GetDataPtr(), psino.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerProcessed->set_image(psino.GetDataPtr(),psino.Dims(),axis[nLo],axis[nHi]);

    kipl::base::TImage<float,2> diff=sino-psino;
    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5f, &nLo, &nHi);
    ui->viewerProcessed->set_image(diff.GetDataPtr(),diff.Dims(),axis[nLo],axis[nHi]);
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
