#ifndef ROIDIALOG_H
#define ROIDIALOG_H

#include <string>

#include <QDialog>
#include <QRect>

#include <base/timage.h>

namespace Ui {
class ROIDialog;
}

namespace QtAddons {
class ROIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ROIDialog(QWidget *parent = nullptr);
    ~ROIDialog();
    void setROI(QRect r);
    QRect ROI();
    void setImage(kipl::base::TImage<float,2> &img);

private:
    Ui::ROIDialog *ui;
};
}

#endif // ROIDIALOG_H
