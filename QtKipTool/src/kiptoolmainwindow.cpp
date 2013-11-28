#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("KipToolMainWindow"),
    ui(new Ui::KipToolMainWindow)
{
    ui->setupUi(this);
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
