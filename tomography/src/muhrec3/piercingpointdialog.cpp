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
    useROI(false),
    correctGain(false)
{
    ui->setupUi(this);
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
    ui->spinBox_roi0->setVisible(checked);
    ui->spinBox_roi0->setVisible(checked);
    ui->spinBox_roi1->setVisible(checked);
    ui->spinBox_roi2->setVisible(checked);
    ui->spinBox_roi3->setVisible(checked);
    ui->label_x0->setVisible(checked);
    ui->label_y0->setVisible(checked);
    ui->label_x1->setVisible(checked);
    ui->label_y1->setVisible(checked);
    ui->pushButton_getROI->setVisible(checked);
    useROI=checked;
}

void PiercingPointDialog::UpdateConfig()
{
    useROI      = ui->checkBox_useROI->isChecked();
    correctGain = ui->checkBox_gainCorrection->isChecked();
    roi[0]      = ui->spinBox_roi0->value();
    roi[1]      = ui->spinBox_roi1->value();
    roi[2]      = ui->spinBox_roi2->value();
    roi[3]      = ui->spinBox_roi3->value();

    position.first  = ui->doubleSpinBox_posX->value();
    position.second = ui->doubleSpinBox_posY->value();
}

void PiercingPointDialog::UpdateDialog()
{
    ui->checkBox_useROI->setChecked(useROI);
    on_checkBox_useROI_toggled(useROI);
    ui->checkBox_gainCorrection->setChecked(correctGain);
    ui->spinBox_roi0->setValue(roi[0]);
    ui->spinBox_roi1->setValue(roi[1]);
    ui->spinBox_roi2->setValue(roi[2]);
    ui->spinBox_roi3->setValue(roi[3]);

    ui->doubleSpinBox_posX->setValue(position.first);
    ui->doubleSpinBox_posY->setValue(position.second);
}

void PiercingPointDialog::on_pushButton_getROI_clicked()
{
    QRect rect=ui->viewer->get_marked_roi();
    ui->viewer->set_rectangle(rect,QColor("red"),0);

    ui->spinBox_roi0->setValue(rect.x());
    ui->spinBox_roi1->setValue(rect.y());
    ui->spinBox_roi2->setValue(rect.x()+rect.width());
    ui->spinBox_roi3->setValue(rect.y()+rect.height());

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


    ui->viewer->set_marker(QtAddons::QMarker(QtAddons::PlotGlyph_Plus,pos,QColor("red"),10),0);
}
