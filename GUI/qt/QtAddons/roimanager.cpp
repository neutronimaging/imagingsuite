//<LICENSE>

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMessageBox>

#include "roimanager.h"
#include "ui_roimanager.h"
#include <base/roi.h>
#include "roiitemdlg.h"

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
    viewer(nullptr),
    bVisibleLabels(false),
    bRequireLabel(false)
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

    ui->listROI->clear();
    for (auto it = rois.begin(); it!=rois.end(); ++it) {
        ROIListItem *item = new ROIListItem;

        item->roi=*it;
        it->getBox(roi);
        msg.str("");
        if (bVisibleLabels)
            msg<<item->roi.label()<<": ";

        msg<<"[x0: "<<roi[0]<<", y0: "<<roi[1]
           <<", x1: "<<roi[2]<<", y1: "<<roi[3]<<"]";

        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
        item->setData(Qt::CheckStateRole,Qt::Unchecked);

        item->setCheckState(Qt::CheckState::Checked);
        ui->listROI->addItem(item);
        QRect rect=QRect(roi[0],roi[1],roi[0]-roi[2],roi[3]-roi[1]);
        viewer->set_rectangle(rect,QColor("green"),item->roi.getID());
    }
    updateViewer();
}

std::list<kipl::base::RectROI> ROIManager::getROIs()
{
    std::list<kipl::base::RectROI> roiList;
   // qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    for (int i=0; i<ui->listROI->count(); ++i)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));
        roiList.push_back(roiItem->roi);
    }
    //qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    return roiList;
}

std::list<kipl::base::RectROI> ROIManager::getSelectedROIs()
{
    std::list<kipl::base::RectROI> roiList;
  //  qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    for (int i=0; i<ui->listROI->count(); ++i)
    {
        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));
        if (roiItem->checkState()==Qt::Checked)
            roiList.push_back(roiItem->roi);
    }
 //   qDebug() << "Items ="<<ui->listROI->count()<<", "<<roiList.size();
    return roiList;
}

void ROIManager::setLabelVisible(bool show)
{
    bVisibleLabels = show;
}

bool ROIManager::labelIsVisible()
{
    return bVisibleLabels;
}

bool ROIManager::requireLabel()
{
    return bRequireLabel;
}

void ROIManager::setRequireLabel(bool require)
{
    bRequireLabel = require;
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

    if (bRequireLabel)
    {
        QtAddons::ROIItemDlg dlg(this);
        dlg.setROIItem(item->roi);
        int res=dlg.exec();
        if (res==QDialog::Accepted)
        {
            item->roi = dlg.roiItem();
        }
    }

    if (bVisibleLabels)
        msg<<item->roi.label()<<": ";

    msg<<"[x0: "<<rect.x()<<", y0: "<<rect.y()
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

void QtAddons::ROIManager::on_listROI_itemDoubleClicked(QListWidgetItem *item)
{
    QtAddons::ROIItemDlg dlg(this);

    ROIListItem *roiItem = dynamic_cast<ROIListItem *>(item);
    dlg.setROIItem(roiItem->roi);

    int res = dlg.exec();

    if (res == dlg.Accepted) {
        kipl::base::RectROI roi = dlg.roiItem();
        roiItem->roi = roi;

        auto box = roi.box();
        QRect rect(box[0],box[1],box[2]-box[0],box[3]-box[1]);
        viewer->set_rectangle(rect,QColor("green"),roiItem->roi.getID());
        std::ostringstream msg;

        if (bVisibleLabels)
            msg<<roi.label()<<": ";

        msg<<"[x0: "<<rect.x()<<", y0: "<<rect.y()
          <<", x1: "<<rect.x()+rect.width()<<", y1: "<<rect.y()+rect.height()<<"]";
        item->setData(Qt::DisplayRole,QString::fromStdString(msg.str()));
    }

}

}

void QtAddons::ROIManager::on_button_save_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save ROI list",QDir::homePath());
    QFile f( filename );
    f.open( QIODevice::WriteOnly );
    QTextStream out(&f);
    // store data in f
    out<<"{\n";
    out<<"  \"rois\" : [\n";
    size_t roi[4];
    for (int i=0; i<ui->listROI->count(); ++i)
    {

        ROIListItem * roiItem = dynamic_cast<ROIListItem *>(ui->listROI->item(i));
        out<<"    {\n";
        out<<"      \"label\" : \""<<roiItem->roi.label().c_str()<<"\",\n";
        roiItem->roi.getBox(roi);
        out<<"      \"roi\" : \""<< roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3] <<"\"\n    }";
        if (i<ui->listROI->count()-1)
            out<<",\n";
        else
            out<<"\n";
    }
    out<<"  ]\n}\n";
    f.close();
}

void QtAddons::ROIManager::on_button_load_clicked()
{
    QMessageBox::warning(this,"Not available","It is not possible to load a ROI list. This is a place holder for future implementation");

}
