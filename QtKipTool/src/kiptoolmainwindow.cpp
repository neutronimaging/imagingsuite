#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"

#include "ImageIO.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow)
{
    ui->setupUi(this);
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
    connect(ui->button_browsedatapath, SIGNAL(clicked()), this, SLOT(on_button_browsedatapath_clicked()));
    connect(ui->button_browsedestination, SIGNAL(clicked()), this, SLOT(on_button_browsedestination_clicked()));
    connect(ui->button_getROI, SIGNAL(clicked()), this, SLOT(on_button_getROI_clicked()));
    connect(ui->button_loaddata, SIGNAL(clicked()), this, SLOT(on_button_loaddata_clicked()));
    connect(ui->button_savedata, SIGNAL(clicked()), this, SLOT(on_button_savedata_clicked()));

    //connect(ui->spinDoseROIy1,SIGNAL(valueChanged(int)),this,SLOT(DoseROIChanged(int)));

    connect(ui->slider_images,SIGNAL(sliderMoved(int)),this,SLOT(on_slider_images_sliderMoved(int)));
    connect(ui->check_crop,SIGNAL(stateChanged(int)),this,SLOT(on_check_crop_stateChanged(int)));
    connect(ui->check_showorighist,SIGNAL(stateChanged(int)),this,SLOT(on_check_showorighist_stateChanged(int)));

    //void on_combo_plotselector_currentIndexChanged(int index);
    // Menus
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(on_MenuFileNew()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(on_MenuFileOpen()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(on_MenuFileSave()));
    connect(ui->actionSave_as,SIGNAL(triggered()),this,SLOT(on_MenuFileSaveAs()));
    connect(ui->actionQuit,SIGNAL(triggered()),this,SLOT(on_MenuFileQuit()));
    connect(ui->actionStart_processing,SIGNAL(triggered()),this,SLOT(on_MenuStartProcessing()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(on_MenuHelpAbout()));
}

void KipToolMainWindow::on_button_browsedatapath_clicked()
{

}

void KipToolMainWindow::on_button_getROI_clicked()
{

}

void KipToolMainWindow::on_button_loaddata_clicked()
{

}

void KipToolMainWindow::on_button_browsedestination_clicked()
{

}

void KipToolMainWindow::on_check_crop_stateChanged(int arg1)
{

}

void KipToolMainWindow::on_button_savedata_clicked()
{

}

void KipToolMainWindow::on_check_showorighist_stateChanged(int arg1)
{

}

void KipToolMainWindow::on_combo_plotselector_currentIndexChanged(int index)
{

}

void KipToolMainWindow::on_slider_images_sliderMoved(int position)
{

}

void KipToolMainWindow::on_MenuFileNew()
{}

void KipToolMainWindow::on_MenuFileOpen()
{}

void KipToolMainWindow::on_MenuFileSave()
{}

void KipToolMainWindow::on_MenuFileSaveAs()
{}

void KipToolMainWindow::on_MenuFileQuit()
{}

void KipToolMainWindow::on_MenuStartProcessing()
{}

void KipToolMainWindow::on_MenuHelpAbout()
{}
