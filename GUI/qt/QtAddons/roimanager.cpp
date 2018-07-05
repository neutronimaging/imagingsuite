//<LICENSE>

#include <QDebug>

#include "roimanager.h"
#include "ui_roimanager.h"
#include <base/roi.h>
namespace QtAddons {

class ROIListItem : public QListWidgetItem
{
public:
    ROIListItem() : roi(0,0,1,1) {}
    ROIListItem(const ROIListItem &item) :
        QListWidgetItem(item),
        roi(item.roi)
    {}

    kipl::base::RectROI roi;
};

ROIManager::ROIManager(QWidget *parent) :
    QWidget(parent),
    logger("ROIManager"),
    ui(new Ui::ROIManager),
    viewer(nullptr)
{
    ui->setupUi(this);
}

ROIManager::~ROIManager()
{
    delete ui;
}

void ROIManager::setViewer(QtAddons::ImageViewerWidget *v)
{
    viewer=v;
}

void ROIManager::setROIs(std::list<kipl::base::RectROI> &rois)
{
    ostringstream msg;
    size_t roi[4];
    //todo implement set rois
    for (auto it = rois.begin(); it!=rois.end(); ++it) {
        ROIListItem *item = new ROIListItem;

        QRect rect=viewer->get_marked_roi();

        item->roi=*it;
        it->getBox(roi);
        msg.str("");
        msg<<item->roi.getName()<<" ("<<item->roi.getID()
          <<"): [x0: "<<roi[0]<<", y0: "<<roi[1]
          <<", x1: "<<roi[2]<<", y1: "<<roi[3]<<"]";

        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);

        item->setCheckState(Qt::CheckState::Checked);
        ui->listROI->addItem(item);
        viewer->set_rectangle(rect,QColor("green"),item->roi.getID());
    }
}

std::list<kipl::base::RectROI> ROIManager::getROIs()
{
    std::list<kipl::base::RectROI> roiList;
    qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    for (int i=0; i<ui->listROI->count(); ++i)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));
        roiList.push_back(roiItem->roi);
    }
    qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    return roiList;
}

std::list<kipl::base::RectROI> ROIManager::getSelectedROIs()
{
    std::list<kipl::base::RectROI> roiList;
    qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    for (int i=0; i<ui->listROI->count(); ++i)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));
        if (roiItem->checkState()==Qt::Checked)
            roiList.push_back(roiItem->roi);
    }
    qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    return roiList;
}

void ROIManager::updateViewer()
{
    if (viewer==nullptr) {
        logger(logger.LogError,"Can't update. The ROI manager is not connected with a viewer.");
        return ;
    }
    QRect rect;
    size_t roi[4];

    for (int i=0; i<ui->listROI->count(); ++i)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));

        roiItem->roi.getBox(roi);

        rect.setCoords(roi[0],roi[1],roi[2],roi[3]);
        viewer->set_rectangle(rect,QColor("green"),roiItem->roi.getID());
    }
}

void ROIManager::on_button_addROI_clicked()
{
    if (viewer==nullptr) {
        logger(logger.LogError,"Can't add ROI. The ROI manager is not connected with a viewer.");
        return ;
    }

    std::ostringstream msg;

    ROIListItem *item = new ROIListItem;

    QRect rect=viewer->get_marked_roi();

    item->roi=kipl::base::RectROI(rect.x(),rect.y(),rect.x()+rect.width(),rect.y()+rect.height());

    msg<<item->roi.getName()<<" ("<<item->roi.getID()
      <<"): [x0: "<<rect.x()<<", y0: "<<rect.y()
      <<", x1: "<<rect.x()+rect.width()<<", y1: "<<rect.y()+rect.height()<<"]";
    item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
    item->setData(Qt::CheckStateRole,Qt::Unchecked);

    item->setCheckState(Qt::CheckState::Checked);
    ui->listROI->addItem(item);
    viewer->set_rectangle(rect,QColor("green"),item->roi.getID());
}

void ROIManager::on_button_deleteROI_clicked()
{
    QList<QListWidgetItem*> items = ui->listROI->selectedItems();
    if (items.isEmpty()) {
        logger(logger.LogWarning,"Can't delete ROIs, unless they are selected");
        return ;
    }

    foreach(QListWidgetItem * item, items)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(item);
        viewer->clear_rectangle(roiItem->roi.getID());
        delete ui->listROI->takeItem(ui->listROI->row(item));
    }

}
}
