#include <QDir>
#include <QFileDialog>

#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"

#include "ImageIO.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow)
{
    ui->setupUi(this);
    logger.AddLogTarget(*(ui->widget_logviewer));

    ui->widget_moduleconfigurator->configure("kiptool",QDir::currentPath().toStdString());

    SetupCallbacks();
}

KipToolMainWindow::~KipToolMainWindow()
{
    delete ui;
}

void KipToolMainWindow::UpdateDialog()
{

}

void KipToolMainWindow::UpdateConfig()
{

}

void KipToolMainWindow::SetupCallbacks()
{
    connect(ui->button_browsedatapath,    SIGNAL(clicked()), this, SLOT(on_button_browsedatapath_clicked()));
    connect(ui->button_browsedestination, SIGNAL(clicked()), this, SLOT(on_button_browsedestination_clicked()));
    connect(ui->button_getROI,            SIGNAL(clicked()), this, SLOT(on_button_getROI_clicked()));
    connect(ui->button_loaddata,          SIGNAL(clicked()), this, SLOT(on_button_loaddata_clicked()));
    connect(ui->button_savedata,          SIGNAL(clicked()), this, SLOT(on_button_savedata_clicked()));

    //connect(ui->spinDoseROIy1,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));

    connect(ui->slider_images,      SIGNAL(sliderMoved(int)),this,SLOT(on_slider_images_sliderMoved(int)));
    connect(ui->check_crop,         SIGNAL(stateChanged(int)),this,SLOT(on_check_crop_stateChanged(int)));
    connect(ui->check_showorighist, SIGNAL(stateChanged(int)),this,SLOT(on_check_showorighist_stateChanged(int)));

    //void on_combo_plotselector_currentIndexChanged(int index);
    // Menus
    connect(ui->actionNew,     SIGNAL(triggered()), this, SLOT(on_actionNew_triggered()));
    connect(ui->actionOpen,    SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
    connect(ui->actionSave,    SIGNAL(triggered()), this, SLOT(on_actionSave_triggered()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(on_actionSave_as_triggered()));
    connect(ui->actionQuit,    SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
    connect(ui->actionAbout,   SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()));
    connect(ui->actionStart_processing, SIGNAL(triggered()), this,SLOT(on_actionStart_processing_triggered()));
}

void KipToolMainWindow::on_button_browsedatapath_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location of the image data",
                                      ui->edit_datapath->text());

    if (!projdir.isEmpty())
        ui->edit_datapath->setText(projdir);
}

void KipToolMainWindow::on_button_getROI_clicked()
{
    QRect rect=ui->imageviewer_original->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spin_crop0->blockSignals(true);
        ui->spin_crop1->blockSignals(true);
        ui->spin_crop2->blockSignals(true);
        ui->spin_crop3->blockSignals(true);
        ui->spin_crop0->setValue(rect.x());
        ui->spin_crop1->setValue(rect.y());
        ui->spin_crop2->setValue(rect.x()+rect.width());
        ui->spin_crop3->setValue(rect.y()+rect.height());
        ui->spin_crop0->blockSignals(false);
        ui->spin_crop1->blockSignals(false);
        ui->spin_crop2->blockSignals(false);
        ui->spin_crop3->blockSignals(false);

        UpdateMatrixROI();
    }
}

void KipToolMainWindow::UpdateMatrixROI()
{
    logger(kipl::logging::Logger::LogMessage,"Update MatrixROI");
    QRect rect;

    rect.setCoords(ui->spin_crop0->value(),
                   ui->spin_crop1->value(),
                   ui->spin_crop2->value(),
                   ui->spin_crop3->value());

    ui->imageviewer_original->set_rectangle(rect,QColor("green"),0);
}

void KipToolMainWindow::on_button_loaddata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Load image data");
}

void KipToolMainWindow::on_button_browsedestination_clicked()
{
    QString projdir=QFileDialog::getExistingDirectory(this,
                                      "Select location to save the processed image data",
                                      ui->edit_destinationpath->text());

    if (!projdir.isEmpty())
        ui->edit_destinationpath->setText(projdir);
}

void KipToolMainWindow::on_check_crop_stateChanged(int arg1)
{

}

void KipToolMainWindow::on_button_savedata_clicked()
{
    logger(kipl::logging::Logger::LogMessage,"Save processed data");
}

void KipToolMainWindow::on_check_showorighist_stateChanged(int arg1)
{

}

void KipToolMainWindow::on_combo_plotselector_currentIndexChanged(int index)
{

}

void KipToolMainWindow::on_slider_images_sliderMoved(int position)
{
logger(kipl::logging::Logger::LogMessage,"New slider position");
}

void KipToolMainWindow::on_actionNew_triggered()
{}

void KipToolMainWindow::on_actionOpen_triggered()
{}


void KipToolMainWindow::on_actionSave_triggered()
{}

void KipToolMainWindow::on_actionSave_as_triggered()
{}

void KipToolMainWindow::on_actionQuit_triggered()
{}

void KipToolMainWindow::on_actionStart_processing_triggered()
{}

void KipToolMainWindow::on_actionAbout_triggered()
{}
