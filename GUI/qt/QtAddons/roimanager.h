#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include "QtAddons_global.h"
#include <list>
#include <QWidget>
#include <QListWidgetItem>

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
    explicit ROIManager(QWidget *parent = nullptr);
    ~ROIManager();

    void setViewer(QtAddons::ImageViewerWidget *v);
    void setROIs(std::list<kipl::base::RectROI> &rois);
    std::list<kipl::base::RectROI> getROIs();
    std::list<kipl::base::RectROI> getSelectedROIs();
    void setLabelVisible(bool show);
    bool labelIsVisible();
    void setRequireLabel(bool require);
    bool requireLabel();
    void updateViewer();
private slots:
    void on_button_addROI_clicked();

    void on_button_deleteROI_clicked();

    void on_listROI_itemDoubleClicked(QListWidgetItem *item);

    void on_button_save_clicked();

    void on_button_load_clicked();

    void on_listROI_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::ROIManager *ui;
    QtAddons::ImageViewerWidget *viewer;
    bool bVisibleLabels;
    bool bRequireLabel;
};


}
#endif // ROIMANAGER_H
