//<LICENSE>

#include "polynomialcorrectiondlg.h"
#include "ui_polynomialcorrectiondlg.h"
#include <base/thistogram.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <ParameterHandling.h>

PolynomialCorrectionDlg::PolynomialCorrectionDlg(QWidget *parent) :
    ConfiguratorDialogBase("PolynomialCorrectionDlg",true,false,true,parent),
    ui(new Ui::PolynomialCorrectionDlg),
    m_nPolyOrder(1)
{
    memset(m_fCoefs,0,sizeof(float)*10);
    m_fCoefs[1]=1.0f;

    ui->setupUi(this);
    UpdateDialog();
}

PolynomialCorrectionDlg::~PolynomialCorrectionDlg()
{
    delete ui;
}

int PolynomialCorrectionDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float, 3> &img)
{
    m_Projections=img;

    m_Config=dynamic_cast<ReconConfig *>(config);

    m_nPolyOrder = GetFloatParameter(parameters,"order");
    kipl::strings::String2Array(GetStringParameter(parameters,"coefficents"),m_fCoefs,m_nPolyOrder+1);
    ui->spinOrder->setValue(m_nPolyOrder);
    on_spinOrder_valueChanged(m_nPolyOrder);
    UpdateDialog();
    ApplyParameters();

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
    return false;
}

void PolynomialCorrectionDlg::ApplyParameters()
{
    const size_t N=512;
    size_t hist[N];
    float axis[N];
    size_t nLo, nHi;

    kipl::base::TImage<float,2> origimg(m_Projections.Dims());
    memcpy(origimg.GetDataPtr(), m_Projections.GetLinePtr(0,m_Projections.Size(2)/2), sizeof(float)*origimg.Size());
    kipl::base::TImage<float,2> resimg=origimg; resimg.Clone();
    kipl::base::TImage<float,2> procimg=origimg; procimg.Clone();

    kipl::base::Histogram(origimg.GetDataPtr(), origimg.Size(), hist, N, 0.0f, 0.0f, axis);

    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewerOriginal->set_image(origimg.GetDataPtr(),origimg.Dims(),axis[nLo],axis[nHi]);

    std::map<std::string, std::string> parameters;
    UpdateParameters();
    UpdateParameterList(parameters);

    corrector.Configure(*m_Config, parameters);

    std::map<std::string,std::string> pars;

    corrector.Process(procimg, pars);

    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(procimg.GetDataPtr(), procimg.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 97.5, &nLo, &nHi);
    ui->viewerProcessed->set_image(procimg.GetDataPtr(), procimg.Dims(),axis[nLo],axis[nHi]);

    kipl::base::TImage<float,2> diff=procimg-origimg;
    memset(hist,0,N*sizeof(size_t));
    memset(axis,0,N*sizeof(float));
    kipl::base::Histogram(diff.GetDataPtr(), diff.Size(), hist, N, 0.0f, 0.0f, axis);
    kipl::base::FindLimits(hist, N, 95.0, &nLo, &nHi);
    float maxintense=axis[nHi];
    ui->viewerDifference->set_image(diff.GetDataPtr(), diff.Dims());

    float poly[N];
    corrector.PlotPolynomial(axis,poly,N,0.0f,maxintense);

    ui->plotCurve->setCurveData(1,axis,poly,N,Qt::red);
    axis[0]=0.0f;
    axis[1]=maxintense;
    poly[0]=0.0f;
    poly[1]=maxintense;
    ui->plotCurve->setCurveData(0,axis,poly,2,Qt::green);
}

void PolynomialCorrectionDlg::UpdateDialog()
{
    m_nPolyOrder = ui->spinOrder->value();
    ui->spinA0->setValue(m_fCoefs[0]);
    ui->spinA1->setValue(m_fCoefs[1]);
    ui->spinA2->setValue(m_fCoefs[2]);
    ui->spinA3->setValue(m_fCoefs[3]);
    ui->spinA4->setValue(m_fCoefs[4]);
}

void PolynomialCorrectionDlg::UpdateParameters()
{
    m_nPolyOrder = ui->spinOrder->value();
    m_fCoefs[0]  = ui->spinA0->value();
    m_fCoefs[1]  = ui->spinA1->value();
    m_fCoefs[2]  = ui->spinA2->value();
    m_fCoefs[3]  = ui->spinA3->value();
    m_fCoefs[4]  = ui->spinA4->value();
}

void PolynomialCorrectionDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters["order"]       = kipl::strings::value2string(m_nPolyOrder);
    parameters["coefficents"] = kipl::strings::Array2String(m_fCoefs,m_nPolyOrder+1);
}

void PolynomialCorrectionDlg::on_spinOrder_valueChanged(int arg1)
{
    if (arg1<4) {
        ui->spinA4->hide();
        ui->labelA4->hide();
    }
    else {
        ui->spinA4->show();
        ui->labelA4->show();
    }

    if (arg1<3) {
        ui->spinA3->hide();
        ui->labelA3->hide();
    }
    else {
        ui->spinA3->show();
        ui->labelA3->show();
    }

    if (arg1<2) {
        ui->spinA2->hide();
        ui->labelA2->hide();
    }
    else {
        ui->spinA2->show();
        ui->labelA2->show();
    }

    if (arg1<1) {
        ui->spinA1->hide();
        ui->labelA1->hide();
    }
    else {
        ui->spinA1->show();
        ui->labelA1->show();
    }

}

void PolynomialCorrectionDlg::on_button_apply_clicked()
{
    ApplyParameters();
}
