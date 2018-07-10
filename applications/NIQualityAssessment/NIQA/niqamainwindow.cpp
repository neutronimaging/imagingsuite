//<LICENSE>

#include "niqamainwindow.h"
#include "ui_niqamainwindow.h"

#include <sstream>
#include <array>

#include <QSignalBlocker>
#include <QLineSeries>
#include <QPointF>
#include <QChart>
#include <QListWidgetItem>

#include <base/index2coord.h>

#include <datasetbase.h>
#include <imagereader.h>
#include <base/tprofile.h>
#include <math/basicprojector.h>
#include <pixelsizedlg.h>
#include <strings/filenames.h>

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
    ui(new Ui::NIQAMainWindow),
    logdlg(new QtAddons::LoggingDialog(this)),
    configFileName("niqaconfig.xml")
{
    ui->setupUi(this);

    // Setup logging dialog
    logdlg->setModal(false);
    kipl::logging::Logger::AddLogTarget(*logdlg);

    ui->widget_roiEdge2D->setAllowUpdateImageDims(false);
    ui->widget_roiEdge2D->registerViewer(ui->viewer_edgeimages);
    ui->widget_roiEdge2D->setROIColor("red");

    ui->groupBox_2DCrop->setChecked(false);
    //on_check_edge2dcrop_toggled(false);

    ui->widget_openBeamReader->setLabel("Open beam mask");
    ui->widget_darkCurrentReader->setLabel("Dark current mask");

    ui->widget_roi3DBalls->registerViewer(ui->viewer_Packing);

    ui->check_3DBallsCrop->setChecked(false);
    on_check_3DBallsCrop_toggled(false);
    ui->widget_bundleroi->setViewer(ui->viewer_Packing);
    loadCurrent();
    updateDialog();
}

NIQAMainWindow::~NIQAMainWindow()
{
    delete ui;
}

void NIQAMainWindow::on_button_bigball_load_clicked()
{
    saveCurrent();
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
    saveCurrent();
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
    saveCurrent();
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

        EdgeFileItemDialog dlg;

        dlg.setInfo(item->distance,item->filename);
        int res=dlg.exec();

        if (res==dlg.Accepted) {
            dlg.getInfo(item->distance,item->filename);
        }

        msg<<it->toStdString()<<std::endl<<"Edge distance="<<item->distance;
        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);

        item->setCheckState(Qt::CheckState::Checked);
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

    img=reader.Read(item->filename.toStdString(),kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->viewer_edgeimages->set_image(img.GetDataPtr(),img.Dims());
   // on_check_edge2dcrop_toggled(ui->check_edge2dcrop->isEnabled());

}

void NIQAMainWindow::on_button_LoadPacking_clicked()
{
    saveCurrent();
    ImageLoader loader=ui->imageloader_packing->getReaderConfig();

    ImageReader reader;

    size_t crop[4];
    size_t *pCrop = nullptr;

    if (ui->check_3DBallsCrop->isChecked()) {
        ui->widget_roi3DBalls->getROI(crop);
        pCrop=crop;
    }
     m_BallAssembly=reader.Read(loader,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,pCrop);

     QSignalBlocker blocker(ui->slider_PackingImages);
     ui->slider_PackingImages->setMinimum(0);
     ui->slider_PackingImages->setMaximum(static_cast<int>(m_BallAssembly.Size(2))-1);
     ui->slider_PackingImages->setValue(m_BallAssembly.Size(2)/2);
     on_slider_PackingImages_sliderMoved(m_BallAssembly.Size(2)/2);
     m_BallAssemblyProjection=kipl::math::BasicProjector<float>::project(m_BallAssembly,kipl::base::ImagePlaneXY);
}

void NIQAMainWindow::on_button_AnalyzePacking_clicked()
{
    saveCurrent();
    std::ostringstream msg;
    std::list<kipl::base::RectROI> roiList=ui->widget_bundleroi->getSelectedROIs();

    msg<<"Have "<<roiList.size()<<" ROIs";
    logger(logger.LogMessage,msg.str());

    if (roiList.empty())
        return ;

    try
    {
        m_BallAssemblyAnalyzer.analyzeImage(m_BallAssembly,roiList);
    }
    catch (kipl::base::KiplException &e)
    {
        logger(logger.LogError, e.what());
        return;
    }

    std::list<kipl::math::Statistics> roiStats=m_BallAssemblyAnalyzer.getStatistics();
    plotPackingStatistics(roiStats);
}

void NIQAMainWindow::plotPackingStatistics(std::list<kipl::math::Statistics> &roiStats)
{
    std::vector<float> points;

    foreach(auto stats, roiStats) {
        points.push_back(stats.s());
    }

    std::sort(points.begin(),points.end());

    QLineSeries *series0 = new QLineSeries(); //Life time
    float pos=0;
    foreach (auto point, points) {
        series0->append(QPointF(++pos,point));
    }

    QChart *chart = new QChart(); // Life time


    chart->addSeries(series0);
    chart->legend()->hide();
    chart->createDefaultAxes();
//    chart->axisX()->setRange(0, 20);
//    chart->axisY()->setRange(0, 10);

    ui->chart_packing->setChart(chart);


}

void NIQAMainWindow::saveCurrent()
{
    ostringstream confpath;
    ostringstream msg;
    // Save current recon settings
    QDir dir;

    QString path=dir.homePath()+"/.imagingtools";

    logger.message(path.toStdString());
    if (!dir.exists(path)) {
        dir.mkdir(path);
    }
    std::string sPath=path.toStdString();
    kipl::strings::filenames::CheckPathSlashes(sPath,true);
    confpath<<sPath<<"CurrentNIQA.xml";

    try {
        updateConfig();
        ofstream of(confpath.str().c_str());
        if (!of.is_open()) {
            msg.str("");
            msg<<"Failed to open config file: "<<confpath.str()<<" for writing.";
            logger.error(msg.str());
            return ;
        }

        of<<config.WriteXML();
        of.flush();
        logger.message("Saved current recon config");
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Saving current config failed with exception: "<<e.what();
        logger.error(msg.str());
        return;
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Saving current config failed with exception: "<<e.what();
        logger.error(msg.str());
        return;
    }

}

void NIQAMainWindow::loadCurrent()
{
    std::ostringstream msg;

    QDir dir;

    std::string dname=dir.homePath().toStdString()+"/.imagingtools/CurrentNIQA.xml";
    kipl::strings::filenames::CheckPathSlashes(dname,false);

    QString defaultsname=QString::fromStdString(dname);
    msg.str("");
    msg<<"The config file "<<(dir.exists(defaultsname)==true ? "exists." : "doesn't exist.");
    logger(logger.LogMessage,msg.str());
    bool bUseDefaults=true;
    if (dir.exists(defaultsname)==true) { // is there a previous recon?
        bUseDefaults=false;

        try {
            config.loadConfigFile(dname.c_str(),"niqa");
            msg.str("");
            msg<<config.WriteXML();
            logger.message(msg.str());
        }
        catch (kipl::base::KiplException &e) {
            msg.str("");
            msg<<"Loading defaults failed :\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
        }
        catch (std::exception &e) {
            msg.str("");
            msg<<"Loading defaults failed :\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());

        }
    }
}

void NIQAMainWindow::updateConfig()
{
    // User information
    config.userInformation.userName  = ui->lineEdit_userName->text().toStdString();
    config.userInformation.institute = ui->lineEdit_institute->text().toStdString();
    config.userInformation.instrument = ui->lineEdit_instrument->text().toStdString();
    QDate date;
    date = ui->dateEdit_experimentDate->date();
    config.userInformation.experimentDate[0] = date.year();
    config.userInformation.experimentDate[1] = date.month();
    config.userInformation.experimentDate[2] = date.day();
    config.userInformation.comment = ui->plainTextEdit_comment->toPlainText().toStdString();

    date = ui->dateEdit_analysisDate->date();
    config.userInformation.analysisDate[0] = date.year();
    config.userInformation.analysisDate[1] = date.month();
    config.userInformation.analysisDate[2] = date.day();

    config.userInformation.country = ui->lineEdit_country->text().toStdString();

    // Contrast analysis
    ImageLoader loader;
    loader=ui->ImageLoader_contrast->getReaderConfig();
    config.contrastAnalysis.fileMask = loader.m_sFilemask;
    config.contrastAnalysis.first    = loader.m_nFirst;
    config.contrastAnalysis.last     = loader.m_nLast;
    config.contrastAnalysis.step     = loader.m_nStep;
    config.contrastAnalysis.intensitySlope     = ui->spin_contrast_slope->value();
    config.contrastAnalysis.intensityIntercept = ui->spin_contrast_intercept->value();
    config.contrastAnalysis.pixelSize          = ui->spin_contrast_pixelsize->value();
    config.contrastAnalysis.makeReport         = ui->checkBox_reportContrast->isChecked();

    //todo implement list transfer
    config.edgeAnalysis2D.multiImageList.clear();
    for(int i = 0; i < ui->listEdgeFiles->count(); ++i)
    {
        EdgeFileListItem *item = dynamic_cast<EdgeFileListItem *>(ui->listEdgeFiles->item(i));

        config.edgeAnalysis2D.multiImageList.insert(make_pair(item->distance,item->filename.toStdString()));
    }

    config.edgeAnalysis2D.normalize = ui->groupBox_2Dreferences->isChecked();

    loader = ui->widget_openBeamReader->getReaderConfig();
    config.edgeAnalysis2D.obMask = loader.m_sFilemask;
    config.edgeAnalysis2D.obFirst = loader.m_nFirst;
    config.edgeAnalysis2D.obLast = loader.m_nLast;
    config.edgeAnalysis2D.obStep = loader.m_nStep;

    loader = ui->widget_darkCurrentReader->getReaderConfig();
    config.edgeAnalysis2D.dcMask = loader.m_sFilemask;
    config.edgeAnalysis2D.dcFirst = loader.m_nFirst;
    config.edgeAnalysis2D.dcLast = loader.m_nLast;
    config.edgeAnalysis2D.dcStep = loader.m_nStep;
    config.edgeAnalysis2D.pixelSize = ui->doubleSpinBox_2dEdge_pixelSize->value();
    config.edgeAnalysis2D.useROI = ui->groupBox_2DCrop->isChecked();
    config.edgeAnalysis2D.fitFunction = ui->comboBox_edgeFitFunction->currentIndex();
    ui->widget_roiEdge2D->getROI(config.edgeAnalysis2D.roi);
    config.edgeAnalysis2D.makeReport = ui->checkBox_report2DEdge->isChecked();

    loader = ui->ImageLoader_bigball->getReaderConfig();
    config.edgeAnalysis3D.fileMask = loader.m_sFilemask;
    config.edgeAnalysis3D.first = loader.m_nFirst;
    config.edgeAnalysis3D.last  = loader.m_nLast;
    config.edgeAnalysis3D.step  = loader.m_nStep;
    config.edgeAnalysis3D.pixelSize = ui->dspin_bigball_pixelsize->value();
    config.edgeAnalysis3D.precision = ui->spin_bigball_precision->value();
    config.edgeAnalysis3D.radius = ui->dspin_bigball_radius->value();
    config.edgeAnalysis3D.makeReport =ui->checkBox_report3DEdge->isChecked();

    loader = ui->imageloader_packing->getReaderConfig();
    config.ballPackingAnalysis.fileMask = loader.m_sFilemask;
    config.ballPackingAnalysis.first    = loader.m_nFirst;
    config.ballPackingAnalysis.last     = loader.m_nLast;
    config.ballPackingAnalysis.step     = loader.m_nStep;
    config.ballPackingAnalysis.useCrop  = ui->check_3DBallsCrop->isChecked();
    config.ballPackingAnalysis.analysisROIs = ui->widget_bundleroi->getROIs();
    ui->widget_roi3DBalls->getROI(config.ballPackingAnalysis.roi);
    config.ballPackingAnalysis.makeReport = ui->checkBox_reportBallPacking->isChecked();

}

void NIQAMainWindow::updateDialog()
{
    std::ostringstream msg;
    // User information
    ui->lineEdit_userName->setText(QString::fromStdString(config.userInformation.userName));
    ui->lineEdit_institute->setText(QString::fromStdString(config.userInformation.institute));
    ui->lineEdit_instrument->setText(QString::fromStdString(config.userInformation.instrument));
    ui->dateEdit_experimentDate->setDate(QDate(config.userInformation.experimentDate[0],config.userInformation.experimentDate[1],config.userInformation.experimentDate[2]));
    ui->dateEdit_analysisDate->setDate(QDate(config.userInformation.analysisDate[0],config.userInformation.analysisDate[1],config.userInformation.analysisDate[2]));
    ui->label_version->setText(QString::fromStdString(config.userInformation.softwareVersion));
    ui->lineEdit_country->setText(QString::fromStdString(config.userInformation.country));

    // Contrast analysis
    ImageLoader loader;
    loader=ui->ImageLoader_contrast->getReaderConfig();
    loader.m_sFilemask = config.contrastAnalysis.fileMask;
    loader.m_nFirst    = config.contrastAnalysis.first;
    loader.m_nLast     = config.contrastAnalysis.last;
    loader.m_nStep     = config.contrastAnalysis.step;
    ui->ImageLoader_contrast->setReaderConfig(loader);

    ui->spin_contrast_slope->setValue(config.contrastAnalysis.intensitySlope);
    ui->spin_contrast_intercept->setValue(config.contrastAnalysis.intensityIntercept);
    ui->spin_contrast_pixelsize->setValue(config.contrastAnalysis.pixelSize);
    ui->checkBox_reportContrast->setChecked(config.contrastAnalysis.makeReport);

    //todo implement list transfer
//    config.edgeAnalysis2D.multiImageList = ui->;

    if (config.edgeAnalysis2D.multiImageList.empty()==false) {
        for (auto it=config.edgeAnalysis2D.multiImageList.begin(); it!=config.edgeAnalysis2D.multiImageList.end(); ++it) {
            EdgeFileListItem *item = new EdgeFileListItem;

            item->distance  = it->first;
            item->filename  = QString::fromStdString(it->second);
            msg.str("");
            msg<<it->second<<std::endl<<"Edge distance="<<it->first;
            item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
            item->setData(Qt::CheckStateRole,Qt::Unchecked);

            item->setCheckState(Qt::CheckState::Checked);
            ui->listEdgeFiles->addItem(item);
        }
    }

    ui->groupBox_2Dreferences->setChecked(config.edgeAnalysis2D.normalize);

    loader.m_sFilemask = config.edgeAnalysis2D.obMask;
    loader.m_nFirst    = config.edgeAnalysis2D.obFirst;
    loader.m_nLast     = config.edgeAnalysis2D.obLast;
    loader.m_nStep     = config.edgeAnalysis2D.obStep;
    ui->widget_openBeamReader->setReaderConfig(loader);


    loader.m_sFilemask = config.edgeAnalysis2D.dcMask;
    loader.m_nFirst    = config.edgeAnalysis2D.dcFirst;
    loader.m_nLast     = config.edgeAnalysis2D.dcLast;
    loader.m_nStep     = config.edgeAnalysis2D.dcStep;
    ui->widget_darkCurrentReader->setReaderConfig(loader);

    ui->doubleSpinBox_2dEdge_pixelSize->setValue(config.edgeAnalysis2D.pixelSize);
    ui->groupBox_2DCrop->setChecked(config.edgeAnalysis2D.useROI);
    ui->widget_roiEdge2D->setROI(config.edgeAnalysis2D.roi);
    ui->comboBox_edgeFitFunction->setCurrentIndex(config.edgeAnalysis2D.fitFunction);
    ui->checkBox_report2DEdge->setChecked(config.edgeAnalysis2D.makeReport);

    loader.m_sFilemask = config.edgeAnalysis3D.fileMask;
    loader.m_nFirst = config.edgeAnalysis3D.first;
    loader.m_nLast = config.edgeAnalysis3D.last;
    loader.m_nStep = config.edgeAnalysis3D.step;
    ui->ImageLoader_bigball->setReaderConfig(loader);

    ui->dspin_bigball_pixelsize->setValue(config.edgeAnalysis3D.pixelSize);
    ui->spin_bigball_precision->setValue(config.edgeAnalysis3D.precision);
    ui->dspin_bigball_radius->setValue(config.edgeAnalysis3D.radius);
    ui->checkBox_report3DEdge->setChecked(config.edgeAnalysis3D.makeReport);

    loader.m_sFilemask = config.ballPackingAnalysis.fileMask;
    loader.m_nFirst = config.ballPackingAnalysis.first;
    loader.m_nLast = config.ballPackingAnalysis.last;
    loader.m_nStep = config.ballPackingAnalysis.step;
    ui->imageloader_packing->setReaderConfig(loader);
    ui->check_3DBallsCrop->setChecked(config.ballPackingAnalysis.useCrop);
    ui->widget_bundleroi->setROIs(config.ballPackingAnalysis.analysisROIs);
    ui->widget_roi3DBalls->setROI(config.ballPackingAnalysis.roi);
    ui->checkBox_reportBallPacking->setChecked(config.ballPackingAnalysis.makeReport);
}

void NIQAMainWindow::saveConfig(std::string fname)
{
    updateConfig();
    configFileName=fname;
    std::ofstream conffile(configFileName.c_str());

    conffile<<config.WriteXML();
}

void NIQAMainWindow::on_slider_PackingImages_sliderMoved(int position)
{

    switch (ui->combo_PackingImage->currentIndex()) {
        case 0: ui->slider_PackingImages->setEnabled(true);
                ui->viewer_Packing->set_image(m_BallAssembly.GetLinePtr(0,position),
                                              m_BallAssembly.Dims());
                if (ui->check_3DBallsCrop->isChecked()) {
                    QRect roi;
                    ui->widget_roi3DBalls->getROI(roi);
                    ui->viewer_Packing->set_rectangle(roi,QColor("red"),0);
                }

                break;

        case 1:
                ui->slider_PackingImages->setEnabled(false);
                ui->viewer_Packing->set_image(m_BallAssemblyProjection.GetDataPtr(),
                                      m_BallAssemblyProjection.Dims());

                break;
        case 2: ui->slider_PackingImages->setEnabled(true);
                ui->viewer_Packing->set_image(m_BallAssemblyAnalyzer.getMask().GetLinePtr(0,position),
                                              m_BallAssemblyAnalyzer.getMask().Dims());
                break;
        case 3:
                {
                    ui->slider_PackingImages->setEnabled(true);
                    kipl::base::TImage<float,2> slice(m_BallAssembly.Dims());
                    int *pSlice=m_BallAssemblyAnalyzer.getLabels().GetLinePtr(0,position);
                    std::copy(pSlice,pSlice+slice.Size(),slice.GetDataPtr());
                    ui->viewer_Packing->set_image(slice.GetDataPtr(),slice.Dims());
                    break;
                }
        case 4:
                ui->slider_PackingImages->setEnabled(true);
                ui->viewer_Packing->set_image(m_BallAssemblyAnalyzer.getDist().GetLinePtr(0,position),
                                                     m_BallAssemblyAnalyzer.getDist().Dims());
                       break;

    }


}

void NIQAMainWindow::on_combo_PackingImage_currentIndexChanged(int index)
{
    (void) index;
    on_slider_PackingImages_sliderMoved(ui->slider_PackingImages->value());
}

void NIQAMainWindow::on_widget_roiEdge2D_getROIclicked()
{
    QRect rect=ui->viewer_edgeimages->get_marked_roi();

    ui->widget_roiEdge2D->setROI(rect);
    ui->viewer_edgeimages->set_rectangle(rect,QColor("red"),0);
}

void NIQAMainWindow::on_widget_roiEdge2D_valueChanged(int x0, int y0, int x1, int y1)
{
    QRect rect(x0,y0,x1-x0+1,y1-y0+1);
    ui->viewer_edgeimages->set_rectangle(rect,QColor("red"),0);
}

void NIQAMainWindow::on_button_get2Dedges_clicked()
{
    saveCurrent();
    getEdge2Dprofiles();
    plotEdgeProfiles();

    ui->tabWidget_edge2D->setCurrentIndex(1);
}

void NIQAMainWindow::on_button_estimateCollimation_clicked()
{

}

void NIQAMainWindow::getEdge2Dprofiles()
{
    kipl::base::TImage<float,2> img;
    EdgeFileListItem *item = nullptr;

    saveCurrent();
    ImageReader reader;
    size_t crop[4];
    ui->widget_roiEdge2D->getROI(crop);
    m_Edges.clear();
    size_t *pCrop= ui->groupBox_2DCrop->isChecked() ? crop : nullptr;

    float *profile=nullptr;
    for (int i=0; i<ui->listEdgeFiles->count(); ++i) {
        item = dynamic_cast<EdgeFileListItem *>(ui->listEdgeFiles->item(i));
        if (item->checkState()==Qt::CheckState::Unchecked)
            continue;

        qDebug() <<item->filename;
        try {
            img=reader.Read(item->filename.toStdString(),kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,pCrop);
        }
        catch (kipl::base::KiplException &e) {
            qDebug() << QString::fromStdString(e.what());
            return ;
        }

        if (profile==nullptr)
            profile=new float[img.Size(0)];

        kipl::base::HorizontalProjection2D(img.GetDataPtr(), img.Dims(), profile, true);
        std::vector<float> pvec;
        std::copy(profile,profile+img.Size(0),std::back_inserter(pvec));
        m_Edges[item->distance]=pvec;
    }

    delete [] profile;
}

void NIQAMainWindow::estimateResolutions()
{

}

void NIQAMainWindow::plotEdgeProfiles()
{
    QChart *chart = new QChart(); // Life time

    for (auto it = m_Edges.begin(); it!=m_Edges.end(); ++it) {
        QLineSeries *series = new QLineSeries(); //Life time
        auto edge=it->second;
        for (size_t i=0; i<edge.size(); ++i) {
            series->append(QPointF(i,float(edge[i])));
        }

        chart->addSeries(series);
    }
    chart->legend()->hide();
    chart->createDefaultAxes();

    ui->chart_2Dedges->setChart(chart);
}

void NIQAMainWindow::on_check_3DBallsCrop_toggled(bool checked)
{
    if (checked) {
        QRect roi;
        ui->widget_roi3DBalls->getROI(roi);
        ui->viewer_Packing->set_rectangle(roi,QColor("red"),0);
        ui->widget_roi3DBalls->show();
    }
    else {
        ui->viewer_Packing->clear_rectangle(0);
        ui->widget_roi3DBalls->hide();
    }
}

void NIQAMainWindow::on_widget_roi3DBalls_getROIclicked()
{
   QRect rect=ui->viewer_Packing->get_marked_roi();
   ui->widget_roi3DBalls->setROI(rect);
   ui->viewer_Packing->set_rectangle(rect,QColor("red"),0);
}

void NIQAMainWindow::on_widget_roi3DBalls_valueChanged(int x0, int y0, int x1, int y1)
{
    (void) x0;
    (void) y0;
    (void) x1;
    (void) y1;
    QRect roi;
    ui->widget_roi3DBalls->getROI(roi);

    ui->viewer_Packing->set_rectangle(roi,QColor("red"),0);
}

void NIQAMainWindow::on_pushButton_contrast_pixelSize_clicked()
{
    PixelSizeDlg dlg(this);

    int res=0;
    try {
        res=dlg.exec();
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox::warning(this,"Warning","Image could not be loaded",QMessageBox::Ok);

        logger.warning(e.what());
        return;
    }

    if (res==dlg.Accepted) {
        ui->spin_contrast_pixelsize->setValue(dlg.getPixelSize());
    }


}

void NIQAMainWindow::on_pushButton_logging_clicked()
{
    if (logdlg->isHidden()) {

        logdlg->show();
    }
    else {
        logdlg->hide();
    }
}

void NIQAMainWindow::on_pushButton_2dEdge_pixelSize_clicked()
{
    PixelSizeDlg dlg(this);

    int res=0;
    try {
        res=dlg.exec();
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox::warning(this,"Warning","Image could not be loaded",QMessageBox::Ok);

        logger.warning(e.what());
        return;
    }

    if (res==dlg.Accepted) {
        ui->doubleSpinBox_2dEdge_pixelSize->setValue(dlg.getPixelSize());
    }
}

void NIQAMainWindow::on_actionNew_triggered()
{
    NIQAConfig conf;
    config = conf;
    updateDialog();
}

void NIQAMainWindow::on_actionLoad_triggered()
{
    logger.message("Load config");
    QString fname=QFileDialog::getOpenFileName(this,"Load configuration file",QDir::homePath());

    bool fail=false;
    std::string failmessage;

    try {
        config.loadConfigFile(fname.toStdString(),"niqa");
    }
    catch (kipl::base::KiplException &e) {
        fail=true;
        failmessage=e.what();
    }
    catch (std::exception &e) {
        fail=true;
        failmessage=e.what();
    }

    if (fail==true) {
        std::ostringstream msg;
        msg<<"Could not load config file\n"<<failmessage;
        logger.warning(msg.str());
        QMessageBox::warning(this,"Warning","Could not open specified config file",QMessageBox::Ok);
        return;
    }

    logger.message(config.WriteXML());
    updateDialog();
}

void NIQAMainWindow::on_actionSave_triggered()
{
    if (configFileName=="niqaconfig.xml") {
        on_actionSave_as_triggered();
    }
    else {
        saveConfig(configFileName);
    }
}

void NIQAMainWindow::on_actionSave_as_triggered()
{
    QString fname=QFileDialog::getSaveFileName(this,"Save configuration file",QDir::homePath());

    saveConfig(fname.toStdString());
}

void NIQAMainWindow::on_actionQuit_triggered()
{

}


void NIQAMainWindow::on_pushButton_createReport_clicked()
{
    saveCurrent();
}

void NIQAMainWindow::on_comboBox_edgeFitFunction_currentIndexChanged(int index)
{

}

void NIQAMainWindow::on_comboBox_edgePlotType_currentIndexChanged(int index)
{

}
