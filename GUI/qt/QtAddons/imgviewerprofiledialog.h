#ifndef IMGVIEWERPROFILEDIALOG_H
#define IMGVIEWERPROFILEDIALOG_H

#include <base/timage.h>
#include <logging/logger.h>

#include <QDialog>

namespace Ui {
class ImgViewerProfileDialog;
}

class ImgViewerProfileDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ImgViewerProfileDialog(QWidget *parent = nullptr);
    ~ImgViewerProfileDialog();

    void setImage(kipl::base::TImage<float,2> &img);
    void setROI(QRect &r);

private:
    Ui::ImgViewerProfileDialog *ui;
    kipl::base::TImage<float,2> currentImage;
    QRect currentROI;
};

#endif // IMGVIEWERPROFILEDIALOG_H
