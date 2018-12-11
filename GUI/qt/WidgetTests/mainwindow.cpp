#include <cmath>

#include <QListWidgetItem>
#include <QDebug>


#include <base/timage.h>
#include <generators/Sine2D.h>

#include <buildfilelist.h>

#include <loggingdialog.h>
#include <datasetbase.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("MainWindow"),
    ui(new Ui::MainWindow),
    logdlg(new QtAddons::LoggingDialog(this)),
    m_fScale(1.0)
{
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*logdlg);
    connect(ui->TestButton,SIGNAL(clicked()),this,SLOT(TestClicked()));
    connect(ui->PlotButton,SIGNAL(clicked()),this,SLOT(PlotClicked()));
    connect(ui->GetModulesButton,SIGNAL(clicked()),this,SLOT(GetModulesClicked()));

    connect(ui->roiWidget,&QtAddons::ROIWidget::getROIClicked,this,&MainWindow::on_roiwidget_getROIclicked);
    connect(ui->roiWidget,&QtAddons::ROIWidget::valueChanged,this,&MainWindow::on_roiwidget_valueChanged);
    ui->singlemodule->Configure("muhrec");
    ui->widgetROImanager->setViewer(ui->ImageView_2);
    ui->uxROI1->registerViewer(ui->ImageView);
    ui->uxROI1->setROIColor("red");
    ui->uxROI2->registerViewer(ui->ImageView);
    ui->uxROI2->setROIColor("green");

    ui->uxROI3->registerViewer(ui->ImageView);
    ui->uxROI3->setROIColor("deepskyblue");
    ui->uxROI3->setTitle("My ROI");
    ui->uxROI3->setCheckable(true);

}

MainWindow::~MainWindow()
{
    delete logdlg;
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

    int N=10;
    for (int i=0; i<N; i++) {
        float x=i/float(N-1);
        data.append(QPointF(x,sin(2*3.1415*x)));
    }

    ui->CurvePlotter->setCurveData(0,data);
    ui->CurvePlotter->setPlotCursor(0,QtAddons::PlotCursor(0.5,QColor("blue"),QtAddons::PlotCursor::Vertical));
    ui->CurvePlotter->setPlotCursor(1,QtAddons::PlotCursor(0.25,QColor("red"),QtAddons::PlotCursor::Horizontal));
}

void MainWindow::on_ShowImageButton_clicked()
{
    m_fScale=fmod(m_fScale+1.0,10.0);
    kipl::base::TImage<float,2> img=kipl::generators::Sine2D::JaehneRings(100,m_fScale);

    ui->ImageView->set_image(img.GetDataPtr(),img.Dims());
    ui->ImageView_2->set_image(img.GetDataPtr(),img.Dims());
    int flip=static_cast<int>(m_fScale) & 1;
    if (flip) {
        ui->ImageView->set_rectangle(QRect(10,10,30,40),QColor(Qt::red),0);
    }
    else {
        QVector<QPointF> data;

        for (int i=0;i<static_cast<int>(img.Size(0)); i++)
            data.append(QPointF(i,20+50*sin(2*3.14*i/100.0f)));
        ui->ImageView->set_plot(data,QColor(Qt::green),0);
        QtAddons::QMarker marker(QtAddons::PlotGlyph_Plus,QPoint(20,30),QColor("blue"),10);
        ui->ImageView->set_marker(marker,0);
    }

}

void MainWindow::GetModulesClicked()
{
    std::list<ModuleConfig> modules=ui->ModuleConf->GetModules();

    std::list<ModuleConfig>::iterator it;
    std::ostringstream msg;
    msg<<"Got "<<modules.size()<<" modules from the widget\n";
    for (it=modules.begin(); it!=modules.end(); it++) {
        msg<<(it->m_sModule)<<"("<<(it->m_bActive ? "Active": "Disabled")<<"), has "<<(it->parameters.size())<<" parameters\n";
    }

    logger(kipl::logging::Logger::LogMessage,msg.str());
}

void MainWindow::on_GetROIButton_clicked()
{
    QRect rect=ui->ImageView->get_marked_roi();

    ui->roi_x0->setValue(rect.x());
    ui->roi_y0->setValue(rect.y());
    ui->roi_x1->setValue(rect.x()+rect.width());
    ui->roi_y1->setValue(rect.x()+rect.height());
}

void MainWindow::on_check_linkimages_toggled(bool checked)
{
    QtAddons::ImageViewerWidget *v1=dynamic_cast<QtAddons::ImageViewerWidget *>(ui->ImageView);
    QtAddons::ImageViewerWidget *v2=dynamic_cast<QtAddons::ImageViewerWidget *>(ui->ImageView_2);

    if (checked)
        v1->LinkImageViewer(v2);
    else
       v1->ClearLinkedImageViewers();
}

void MainWindow::on_pushButton_listdata_clicked()
{
    std::list<ImageLoader> loaderlist;

    loaderlist=ui->ImageLoaders->GetList();
    for (auto it=loaderlist.begin(); it!=loaderlist.end(); it++) {
        std::cout<<*it<<std::endl;
    }

    std::list<std::string> flist=BuildFileList(loaderlist);

    for (auto it=flist.begin(); it!=flist.end(); it++) {
        std::cout<<*it<<std::endl;
    }


}

void MainWindow::on_roiwidget_getROIclicked()
{
    logger(logger.LogMessage,"Get roi clicked");
    QRect rect=ui->ImageView_2->get_marked_roi();

    ui->roiWidget->setROI(rect);
    ui->ImageView_2->set_rectangle(rect,QColor("red"),0);
}


void MainWindow::on_roiwidget_valueChanged(int x0, int y0, int x1, int y1)
{
    QRect rect(x0,y0,x1-x0,y1-y0);
    ui->ImageView_2->set_rectangle(rect,QColor("red"),0);
}

void MainWindow::on_button_ListAllROIs_clicked()
{
    qDebug() << "List all rois";
    std::list<kipl::base::RectROI> roilist=ui->widgetROImanager->getROIs();
    qDebug() << "got list "<<roilist.size();
    std::ostringstream msg;
    msg<<"All ROIs";
    size_t coord[4];
    foreach (kipl::base::RectROI roi, roilist) {
        msg<<std::endl;
        roi.getBox(coord);
        msg<<roi.getName()<<" ("<<roi.getID()
          <<"): [x0: "<<coord[0]<<", y0: "<<coord[1]
          <<", x1: "<<coord[2]<<", y1: "<<coord[3]<<"]";
    }
    logger(logger.LogMessage,msg.str());

}

void MainWindow::on_button_ListSelectedROIs_clicked()
{
    qDebug() << "List some rois";
    std::list<kipl::base::RectROI> roilist=ui->widgetROImanager->getSelectedROIs();
    qDebug() << "got list "<<roilist.size();
    std::ostringstream msg;
    msg<<"Selected ROIs";
    size_t coord[4];
    foreach (kipl::base::RectROI roi, roilist) {
        msg<<std::endl;
        roi.getBox(coord);
        msg<<roi.getName()<<" ("<<roi.getID()
          <<"): [x0: "<<coord[0]<<", y0: "<<coord[1]
          <<", x1: "<<coord[2]<<", y1: "<<coord[3]<<"]";
    }
    logger(logger.LogMessage,msg.str());
}

void MainWindow::on_pushButton_showLogger_clicked()
{
    if (logdlg->isHidden()) {

        logdlg->show();
    }
    else {
        logdlg->hide();
    }
}

void MainWindow::on_pushButton_clicked()
{

}



void MainWindow::on_checkBox_stride_toggled(bool checked)
{
    ui->widget_readerform->setHideStride(!checked);
}

void MainWindow::on_checkBox_repeat_toggled(bool checked)
{
    ui->widget_readerform->setHideRepeat(!checked);
}

void MainWindow::on_checkBox_roi_toggled(bool checked)
{
    ui->widget_readerform->setHideROI(!checked);
}

void MainWindow::on_checkBox_fliprot_toggled(bool checked)
{
    ui->widget_readerform->setHideMirrorRotate(!checked);
}

void MainWindow::on_pushButton_getFormInfo_clicked()
{
    ImageLoader readerInfo=ui->widget_readerform->getReaderConfig();
    std::ostringstream msg;
    msg<<readerInfo;
    logger.message(msg.str());
}
