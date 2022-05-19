#include "projectionseriescorrectiondlg.h"
#include "ui_projectionseriescorrectiondlg.h"

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
#include <projectionseriescorrection.h>

#include <plotcursor.h>

#include <sstream>
#include <numeric>
#include <tuple>

ProjectionSeriesCorrectionDlg::ProjectionSeriesCorrectionDlg(QWidget *parent) :
    ConfiguratorDialogBase("ProjectionSeriesCorrectionDlg",true,true,true,parent),
    ui(new Ui::ProjectionSeriesCorrectionDlg)
{
    ui->setupUi(this);
}

ProjectionSeriesCorrectionDlg::~ProjectionSeriesCorrectionDlg()
{
    delete ui;
}

int ProjectionSeriesCorrectionDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
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

void ProjectionSeriesCorrectionDlg::on_doubleSpinBox_threshold_valueChanged(double arg1)
{
    std::ignore = arg1;
    ApplyParameters();
}


void ProjectionSeriesCorrectionDlg::on_doubleSpinBox_threshold_editingFinished()
{
    ApplyParameters();
}

void ProjectionSeriesCorrectionDlg::ApplyParameters()
{
    UpdateParameters();

    ui->plot->setCursor(0,new QtAddons::PlotCursor(threshold,
                                                   QColor(Qt::green),
                                                   QtAddons::PlotCursor::Horizontal,
                                                   "Threshold"));
    ImagingAlgorithms::ProjectionSeriesCorrection psc;

    auto res = psc.detectUnderExposure(doselist,threshold);
    QString label;
    label.setNum(res.size());
    ui->label_projcount->setText(label);
}

void ProjectionSeriesCorrectionDlg::UpdateDialog()
{
    ui->doubleSpinBox_threshold->setValue(threshold);
    std::vector<float> x(doselist.size());
    std::iota(x.begin(), x.end(), 0);
    ui->plot->setCurveData(0,x,doselist,"Dose");
}

void ProjectionSeriesCorrectionDlg::UpdateParameters()
{
    threshold = ui->doubleSpinBox_threshold->value();
}

void ProjectionSeriesCorrectionDlg::UpdateParameterList(std::map<string, string> &parameters)
{
    parameters["threshold"] = std::to_string(threshold);
}

void ProjectionSeriesCorrectionDlg::LoadDoseList(ReconConfig &config)
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
