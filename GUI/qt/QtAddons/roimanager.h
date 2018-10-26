#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "QtAddons_global.h"
#include <list>
#include <QWidget>

#include <base/roi.h>
#include <logging/logger.h>

#include "imageviewerwidget.h"


namespace Ui {
class ROIManager;
}

namespace QtAddons {

class QTADDONSSHARED_EXPORT ROIManager : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ROIManager(QWidget *parent = 0);
    ~ROIManager();

    void setViewer(QtAddons::ImageViewerWidget *v);
    void setROIs(std::list<kipl::base::RectROI> &rois);
    std::list<kipl::base::RectROI> getROIs();
    std::list<kipl::base::RectROI> getSelectedROIs();

    void updateViewer();
private slots:
    void on_button_addROI_clicked();

    void on_button_deleteROI_clicked();

private:
    Ui::ROIManager *ui;
    QtAddons::ImageViewerWidget *viewer;
};

}
#endif // ROIMANAGER_H
