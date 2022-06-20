#include "ReplaceUnderexposeddlg.h"
#include "ui_ReplaceUnderexposeddlg.h"

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/thistogram.h>
#include <math/sums.h>
#include <math/mathfunctions.h>
#include <base/KiplException.h>
#include <stltools/stlvecmath.h>

#include <ParameterHandling.h>
#include <ModuleException.h>
#include <ProjectionReader.h>
#include <ReplaceUnderexposed.h>

#include <plotcursor.h>

#include <sstream>
#include <numeric>
#include <tuple>

ReplaceUnderexposedDlg::ReplaceUnderexposedDlg(QWidget *parent) :
    ConfiguratorDialogBase("ReplaceUnderexposedDlg",true,true,true,parent),
    ui(new Ui::ReplaceUnderexposedDlg)
{
    ui->setupUi(this);
}

ReplaceUnderexposedDlg::~ReplaceUnderexposedDlg()
{
    delete ui;
}

int ReplaceUnderexposedDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{
    std::ignore = img;

    std::ostringstream msg;

    m_Config = dynamic_cast<ReconConfig *>(config);

    msg.str("");
    try {
        threshold = GetFloatParameter(parameters,"threshold");
    }
    catch (ModuleException &e) {
        msg<<"Module exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Kipl Exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg<<"STL exception: Failed to get parameters: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }

    try {
        LoadDoseList(*m_Config);
    }
    catch (ModuleException &e) {
        msg<<"Module exception: Failed to load dose list: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Kipl Exception: Failed to load dose list: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg<<"STL exception: Failed to load dose list: "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
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

void ReplaceUnderexposedDlg::on_doubleSpinBox_threshold_valueChanged(double arg1)
{
    std::ignore = arg1;
    ApplyParameters();
}


void ReplaceUnderexposedDlg::on_doubleSpinBox_threshold_editingFinished()
{
    ApplyParameters();
}

void ReplaceUnderexposedDlg::ApplyParameters()
{
    UpdateParameters();

    ui->plot->setCursor(0,new QtAddons::PlotCursor(threshold,
                                                   QColor(Qt::green),
                                                   QtAddons::PlotCursor::Horizontal,
                                                   "Threshold"));
    ImagingAlgorithms::ReplaceUnderexposed psc;

    auto res = psc.detectUnderExposure(doselist,threshold);
    QString label;
    label.setNum(res.size());
    ui->label_projcount->setText(label);
}

void ReplaceUnderexposedDlg::UpdateDialog()
{
    ui->doubleSpinBox_threshold->setValue(threshold);
    std::vector<float> x(doselist.size());
    std::iota(x.begin(), x.end(), 0);
    ui->plot->setCurveData(0,x,doselist,"Dose");
}

void ReplaceUnderexposedDlg::UpdateParameters()
{
    threshold = ui->doubleSpinBox_threshold->value();
}

void ReplaceUnderexposedDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters["threshold"] = std::to_string(threshold);
}

void ReplaceUnderexposedDlg::LoadDoseList(ReconConfig &config)
{
    ostringstream    msg;
    ProjectionReader reader;
    float            fDose=0.0f;

    doselist.clear();

    for (size_t i=config.ProjectionInfo.nFirstIndex; i<=config.ProjectionInfo.nLastIndex; ++i)
    {
               fDose=reader.GetProjectionDose(config.ProjectionInfo.sPath,
                                    config.ProjectionInfo.sFileMask,
                                    i,
                                    config.ProjectionInfo.eFlip,
                                    config.ProjectionInfo.eRotate,
                                    config.ProjectionInfo.fBinning,
                                    config.ProjectionInfo.dose_roi);

               doselist.push_back(fDose);
    }
}
