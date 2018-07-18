//<LICENSE>
#include "piercingpointdialog.h"
#include "ui_piercingpointdialog.h"

#include <ProjectionReader.h>

#include <qmarker.h>

#include <piercingpointestimator.h>

PiercingPointDialog::PiercingPointDialog(QWidget *parent) :
    QDialog(parent),
    logger("PiercingPointDialog"),
    ui(new Ui::PiercingPointDialog),
    correctGain(false),
    useROI(false)
{
    ui->setupUi(this);
    ui->widgetEstimationROI->setROIColor("limegreen");
    ui->widgetEstimationROI->setAutoHideROI(true);
    ui->widgetEstimationROI->registerViewer(ui->viewer);
}

PiercingPointDialog::~PiercingPointDialog()
{
    delete ui;
}

int PiercingPointDialog::exec(ReconConfig &config)
{
    std::copy(config.ProjectionInfo.projection_roi,config.ProjectionInfo.projection_roi+4,roi);
    position.first = config.ProjectionInfo.fpPoint[0];
    position.second = config.ProjectionInfo.fpPoint[1];

    ProjectionReader reader;

    ob=reader.Read(config.ProjectionInfo.sReferencePath,
                   config.ProjectionInfo.sOBFileMask,
                   config.ProjectionInfo.nOBFirstIndex,
                   config.ProjectionInfo.eFlip,
                   config.ProjectionInfo.eRotate,
                   1,
                   nullptr);

    dc=reader.Read(config.ProjectionInfo.sReferencePath,
                   config.ProjectionInfo.sDCFileMask,
                   config.ProjectionInfo.nDCFirstIndex,
                   config.ProjectionInfo.eFlip,
                   config.ProjectionInfo.eRotate,
                   1,
                   nullptr);

    UpdateDialog();
    ui->viewer->set_image(ob.GetDataPtr(),ob.Dims());

    int res=QDialog::exec();

    if (res==QDialog::Accepted) {
        UpdateConfig();
    }

    return res;
}

void PiercingPointDialog::on_checkBox_useROI_toggled(bool checked)
{
    ui->widgetEstimationROI->setVisible(checked);
    useROI=checked;
}

void PiercingPointDialog::UpdateConfig()
{
    useROI      = ui->checkBox_useROI->isChecked();
    correctGain = ui->checkBox_gainCorrection->isChecked();
    ui->widgetEstimationROI->getROI(roi);

    position.first  = ui->doubleSpinBox_posX->value();
    position.second = ui->doubleSpinBox_posY->value();
}

void PiercingPointDialog::UpdateDialog()
{
    ui->checkBox_useROI->setChecked(useROI);
    on_checkBox_useROI_toggled(useROI);
    ui->checkBox_gainCorrection->setChecked(correctGain);
    ui->widgetEstimationROI->setROI(roi);

    ui->doubleSpinBox_posX->setValue(position.first);
    ui->doubleSpinBox_posY->setValue(position.second);
}

void PiercingPointDialog::on_pushButton_estimate_clicked()
{
    ImagingAlgorithms::PiercingPointEstimator pe;

    UpdateConfig();
    logger(logger.LogMessage,"Pre estimate");
    position=pe(ob,dc,correctGain,useROI ? roi : nullptr );
    logger(logger.LogMessage,"post estimate");
    std::ostringstream msg;
    msg<<"Found pp at ["<<position.first<<", "<<position.second<<"]";
    logger(logger.LogMessage,msg.str());
    msg.str("");
    msg<<"Got pair "<<position.first<<", "<<position.second<<", ";
    logger(logger.LogMessage,msg.str());
    UpdateDialog();
    QPointF pos(position.first,position.second);

    if ((0.0f<pos.x()) && (pos.x()<ob.Size(0)) && (0.0f<pos.y()) && (pos.y()<ob.Size(1))) {
        ui->viewer->set_marker(QtAddons::QMarker(QtAddons::PlotGlyph_Plus,pos,QColor("red"),10),0);
    }
    else {
        logger(logger.LogWarning,"Estimated PP outside image, will not be marked.");
    }
}
