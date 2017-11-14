//<LICENSE>

#include "niqamainwindow.h"
#include "ui_niqamainwindow.h"

#include <sstream>

#include <QSignalBlocker>
#include <QLineSeries>
#include <QPointF>
#include <QChart>
#include <QListWidgetItem>

#include <base/index2coord.h>

#include <datasetbase.h>
#include <imagereader.h>

#include "edgefileitemdialog.h"


class EdgeFileListItem : public QListWidgetItem
{
public:
    EdgeFileListItem() {}
    EdgeFileListItem(const EdgeFileListItem &item) : QListWidgetItem(item) {}

    float distance;
    QString filename;
};

NIQAMainWindow::NIQAMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("NIQAMainWindow"),
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
    float R=m_BallAnalyzer.getRadius();

    ui->dspin_bigball_radius->setValue(R);

    kipl::base::coords3Df center=m_BallAnalyzer.getCenter();

    std::ostringstream msg;
    msg<<"["<<center.x<<", "<<center.y<<", "<<center.z<<"]";
    ui->label_bigball_center->setText(QString::fromStdString(msg.str()));

    std::vector<float> distance;
    std::vector<float> profile;
    std::vector<float> stddev;
    m_BallAnalyzer.getEdgeProfile(R-20.0f,R+20.0f,distance,profile,stddev);

    QLineSeries *series0 = new QLineSeries(); //Life time

    for (auto dit=distance.begin(), pit=profile.begin(); dit!=distance.end(); ++dit, ++pit) {
        series0->append(QPointF(*dit,*pit));
    }

    QChart *chart = new QChart(); // Life time


    chart->addSeries(series0);
    chart->legend()->hide();
    chart->createDefaultAxes();
//    chart->axisX()->setRange(0, 20);
//    chart->axisY()->setRange(0, 10);

    ui->chart_bigball->setChart(chart);
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

void NIQAMainWindow::on_button_contrast_load_clicked()
{
    std::ostringstream msg;

    ImageLoader loader=ui->ImageLoader_contrast->getReaderConfig();

    ImageReader reader;

    msg<<loader.m_sFilemask<<loader.m_nFirst<<", "<<loader.m_nLast;
    logger(logger.LogMessage,msg.str());
    m_Contrast=reader.Read(loader,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->slider_contrast_images->setMinimum(0);
    ui->slider_contrast_images->setMaximum(m_Contrast.Size(2)-1);
    ui->slider_contrast_images->setValue((m_Contrast.Size(2)-1)/2);

    ui->slider_contrast_images->setMinimum(0);
    ui->spin_contrast_images->setMaximum(m_Contrast.Size(2)-1);

    m_ContrastSampleAnalyzer.setImage(m_Contrast);


}

void NIQAMainWindow::on_slider_contrast_images_sliderMoved(int position)
{
    QSignalBlocker blocker(ui->spin_contrast_images);

    ui->spin_contrast_images->setValue(position);

    ui->viewer_contrast->set_image(m_Contrast.GetLinePtr(0,position),m_Contrast.Dims());

}

void NIQAMainWindow::on_spin_contrast_images_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->slider_contrast_images);

    ui->slider_contrast_images->setValue(arg1);

    on_slider_contrast_images_sliderMoved(arg1);
}

void NIQAMainWindow::on_combo_contrastplots_currentIndexChanged(int index)
{
    switch (index) {
        case 0: showContrastHistogram(); break;
        case 1: showContrastBoxPlot(); break;
    }
}

void NIQAMainWindow::showContrastBoxPlot()
{
    std::ostringstream msg;
    QChart *chart = new QChart(); // Life time
    std::vector<kipl::math::Statistics> stats=m_ContrastSampleAnalyzer.getStatistics();

    for (int i=0; i<6; ++i) {
        QBoxPlotSeries *insetSeries = new QBoxPlotSeries();
        msg.str("");
        msg<<"Inset "<<i+1;
        QBoxSet *set = new QBoxSet(QString::fromStdString(msg.str()));

        set->setValue(QBoxSet::LowerExtreme,stats[i].Min());
        set->setValue(QBoxSet::UpperExtreme,stats[i].Max());
        set->setValue(QBoxSet::LowerQuartile,stats[i].E()-stats[i].s()*1.96f);
        set->setValue(QBoxSet::UpperQuartile,stats[i].E()+stats[i].s()*1.96f);
        set->setValue(QBoxSet::Median,stats[i].E());
        insetSeries->append(set);

        chart->addSeries(insetSeries);
    }

    chart->legend()->hide();
    chart->createDefaultAxes();

    chart->legend()->hide();
    chart->createDefaultAxes();

    ui->chart_contrast->setChart(chart);
}

void NIQAMainWindow::showContrastHistogram()
{
    size_t bins[16000];
    float axis[16000];
    int histsize=m_ContrastSampleAnalyzer.getHistogram(axis,bins);

    QLineSeries *series0 = new QLineSeries(); //Life time

    for (int i=0; i<histsize; ++i) {
        series0->append(QPointF(axis[i],float(bins[i])));
    }

    QChart *chart = new QChart(); // Life time

    chart->addSeries(series0);
    chart->legend()->hide();
    chart->createDefaultAxes();
//    chart->axisX()->setRange(0, 20);
//    chart->axisY()->setRange(0, 10);

    ui->chart_contrast->setChart(chart);
}

void NIQAMainWindow::on_button_AnalyzeContrast_clicked()
{
    m_ContrastSampleAnalyzer.analyzeContrast(ui->spin_contrast_pixelsize->value());
}

void NIQAMainWindow::on_button_addEdgeFile_clicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,"Select files to open",QDir::homePath());

    std::ostringstream msg;

    for (auto it=filenames.begin(); it!=filenames.end(); it++) {

        msg.str("");

        EdgeFileListItem *item = new EdgeFileListItem;

        item->distance  = 0.0f;
        item->filename  = *it;

        msg<<it->toStdString()<<std::endl<<"Edge distance="<<item->distance;
        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);

        ui->listEdgeFiles->addItem(item);
    }
}



void NIQAMainWindow::on_button_deleteEdgeFile_clicked()
{
    QList<QListWidgetItem*> items = ui->listEdgeFiles->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listEdgeFiles->takeItem(ui->listEdgeFiles->row(item));
    }
}

void NIQAMainWindow::on_listEdgeFiles_doubleClicked(const QModelIndex &index)
{
    EdgeFileListItem *item = dynamic_cast<EdgeFileListItem *>(ui->listEdgeFiles->item(index.row()));

    EdgeFileItemDialog dlg;

    dlg.setInfo(item->distance,item->filename);
    int res=dlg.exec();

    if (res==dlg.Accepted) {
        dlg.getInfo(item->distance,item->filename);
        std::ostringstream msg;
        msg<<item->filename.toStdString()<<std::endl<<"Edge distance="<<item->distance;
        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
    }


}

void NIQAMainWindow::on_listEdgeFiles_clicked(const QModelIndex &index)
{
    EdgeFileListItem *item = dynamic_cast<EdgeFileListItem *>(ui->listEdgeFiles->item(index.row()));

    kipl::base::TImage<float,2> img;

    ImageReader reader;

//    msg<<loader.m_sFilemask<<loader.m_nFirst<<", "<<loader.m_nLast;
//    logger(logger.LogMessage,msg.str());
    img=reader.Read(item->filename.toStdString(),kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->viewer_edgeimages->set_image(img.GetDataPtr(),img.Dims());
}
