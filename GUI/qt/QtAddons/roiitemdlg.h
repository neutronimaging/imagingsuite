#ifndef ROIITEMDLG_H
#define ROIITEMDLG_H

#include <QDialog>
#include <base/roi.h>

namespace Ui {
class ROIItemDlg;
}

namespace QtAddons {

class ROIItemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ROIItemDlg(QWidget *parent = nullptr);
    ~ROIItemDlg();
    void setROIItem(kipl::base::RectROI &roi);
    kipl::base::RectROI roiItem();

private:
    Ui::ROIItemDlg *ui;
};
}
#endif // ROIITEMDLG_H
