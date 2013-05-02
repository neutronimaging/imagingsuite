#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <base/timage.h>
#include <generators/Sine2D.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("MainWindow"),
    ui(new Ui::MainWindow),
    m_fScale(1.0)
{
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*ui->LogView);
    connect(ui->TestButton,SIGNAL(clicked()),this,SLOT(TestClicked()));
    connect(ui->PlotButton,SIGNAL(clicked()),this,SLOT(PlotClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::TestClicked()
{
    logger(kipl::logging::Logger::LogMessage,"Testing");
}

void MainWindow::PlotClicked()
{
    logger(kipl::logging::Logger::LogMessage,"Plotting");
    QVector<QPointF> data;

    int N=100;
    for (int i=0; i<N; i++) {
        float x=i/float(N);
        data.append(QPointF(x,sin(3.1415*x)));
    }

    ui->CurvePlotter->setCurveData(0,data);
}

void MainWindow::on_ShowImageButton_clicked()
{
    m_fScale=fmod(m_fScale+1.0,10.0);
    kipl::base::TImage<float,2> img=kipl::generators::Sine2D::JaehneRings(100,m_fScale);

    ui->ImageView->set_image(img.GetDataPtr(),img.Dims());

}
