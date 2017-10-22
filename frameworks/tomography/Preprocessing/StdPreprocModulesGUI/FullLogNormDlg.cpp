//<LICENSE>

#include <sstream>

#include "FullLogNormDlg.h"
#include "ui_FullLogNormDlg.h"

#include <ParameterHandling.h>
#include <strings/miscstring.h>
#include <ReconException.h>
#include <ModuleException.h>

FullLogNormDlg::FullLogNormDlg(QWidget *parent) :
    ConfiguratorDialogBase("FullLogNormDlg", true,false, false, parent),
    ui(new Ui::FullLogNormDlg),
    m_bUseDose(true),
    m_bUseLUT(false),
    m_eAverageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage)
{
    ui->setupUi(this);
    UpdateDialog();
}

FullLogNormDlg::~FullLogNormDlg()
{
    delete ui;
}

int FullLogNormDlg::exec(ConfigBase * UNUSED(config),
                         std::map<std::string,
                         std::string> &parameters,
                         kipl::base::TImage<float, 3> & UNUSED(img))
{
    UpdateDialogFromParameterList(parameters);
    UpdateDialog();
    std::ostringstream msg;

    int res=QDialog::exec();

    if (res == QDialog::Accepted) {
        UpdateParameterList(parameters);
        msg.str("");
        msg<<"Accepted and got "<<parameters.size()<<" parameters";
        logger(kipl::logging::Logger::LogMessage,msg.str());
        }

    return res;
}

void FullLogNormDlg::UpdateDialog()
{
    ui->check_usedose->setChecked(m_bUseDose);
    ui->check_useLut->setChecked(m_bUseLUT);
    ui->combo_averagemethod->setCurrentIndex(static_cast<int>(m_eAverageMethod));
}

void FullLogNormDlg::UpdateParameters()
{
    m_bUseDose = ui->check_usedose->checkState();
    m_bUseLUT = ui->check_useLut->checkState();
    m_eAverageMethod = static_cast<ImagingAlgorithms::AverageImage::eAverageMethod>(ui->combo_averagemethod->currentIndex());
}

void FullLogNormDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
        parameters.clear();
        UpdateParameters();

        parameters["usenormregion"]=kipl::strings::bool2string(m_bUseDose);
        parameters["uselut"]=kipl::strings::bool2string(m_bUseLUT);
        parameters["referenceaverage"]=enum2string(m_eAverageMethod);
}

void FullLogNormDlg::UpdateDialogFromParameterList(std::map<std::string, std::string> &parameters)
{
    try {
        m_bUseDose       = kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));
        m_bUseLUT        = kipl::strings::string2bool(GetStringParameter(parameters,"uselut"));
        string2enum(GetStringParameter(parameters,"referenceaverage"),m_eAverageMethod);
    }
    catch (ModuleException & e) {
        logger(logger.LogError,e.what());
    }

    UpdateDialog();
}

void FullLogNormDlg::ApplyParameters()
{}
