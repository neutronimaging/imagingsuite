//<LICENSE>

#include "niqamainwindow.h"
#include "ui_niqamainwindow.h"

#include <sstream>
#include <QSignalBlocker>

#include <base/index2coord.h>

#include <datasetbase.h>
#include <imagereader.h>

NIQAMainWindow::NIQAMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NIQAMainWindow)
{
    ui->setupUi(this);
}

NIQAMainWindow::~NIQAMainWindow()
{
    delete ui;
}

void NIQAMainWindow::on_button_bigball_load_clicked()
{
    ImageLoader loader=ui->ImageLoader_bigball->getReaderConfig();

    ImageReader reader;

    m_BigBall=reader.Read(loader,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->slider_bigball_slice->setMinimum(0);
    ui->slider_bigball_slice->setMaximum(m_BigBall.Size(2)-1);
    ui->spin_bigball_slice->setMinimum(0);
    ui->spin_bigball_slice->setMaximum(m_BigBall.Size(2)-1);
    ui->slider_bigball_slice->setValue(m_BigBall.Size(2)/2);
    on_slider_bigball_slice_sliderMoved(m_BigBall.Size(2)/2);

    m_BallAnalyzer.setImage(m_BigBall);
    ui->dspin_bigball_radius->setValue(m_BallAnalyzer.getRadius());

    kipl::base::coords3Df center=m_BallAnalyzer.getCenter();

    std::ostringstream msg;
    msg<<"["<<center.x<<", "<<center.y<<", "<<center.z<<"]";
    ui->label_bigball_center->setText(QString::fromStdString(msg.str()));

}

void NIQAMainWindow::on_slider_bigball_slice_sliderMoved(int position)
{
    QSignalBlocker blocker(ui->spin_bigball_slice);

    ui->viewer_bigball->set_image(m_BigBall.GetLinePtr(0,position),m_BigBall.Dims());
    ui->spin_bigball_slice->setValue(position);
}

void NIQAMainWindow::on_spin_bigball_slice_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->slider_bigball_slice);

    ui->slider_bigball_slice->setValue(arg1);
    on_slider_bigball_slice_sliderMoved(arg1);
}
