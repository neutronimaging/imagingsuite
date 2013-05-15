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
    int flip=static_cast<int>(m_fScale) & 1;
    if (flip) {
        ui->ImageView->set_rectangle(QRect(10,10,30,40),QColor(Qt::red),0);
    }
    else {
        QVector<QPointF> data;

        for (int i=0;i<img.Size(0); i++)
            data.append(QPointF(i,20+50*sin(2*3.14*i/100.0f)));
        ui->ImageView->set_plot(data,QColor(Qt::green),0);
    }

}

void MainWindow::on_GetROIButton_clicked()
{
    QRect rect=ui->ImageView->get_marked_roi();

    ui->roi_x0->setValue(rect.x());
    ui->roi_y0->setValue(rect.y());
    ui->roi_x1->setValue(rect.x()+rect.width());
    ui->roi_y1->setValue(rect.x()+rect.height());
}
