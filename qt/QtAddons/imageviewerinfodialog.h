#ifndef IMAGEVIEWERINFODIALOG_H
#define IMAGEVIEWERINFODIALOG_H

#include <QDialog>
#include <QRect>

#include <base/timage.h>

namespace Ui {
class ImageViewerInfoDialog;
}

class ImageViewerInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageViewerInfoDialog(QWidget *parent = 0);
    ~ImageViewerInfoDialog();
    void UpdateInfo(kipl::base::TImage<float, 2> img, QRect roi);

private:
    Ui::ImageViewerInfoDialog *ui;
    kipl::base::TImage<float,2> *m_CurrentImage;
    kipl::base::TImage<float,2> m_roiImage;
};

#endif // IMAGEVIEWERINFODIALOG_H
