#ifndef PIXELSIZEDLG_H
#define PIXELSIZEDLG_H

#include <qtimaging_global.h>
#include <string>

#include <QDialog>

#include <logging/logger.h>
#include <base/timage.h>

namespace Ui {
class PixelSizeDlg;
}

class QTIMAGINGSHARED_EXPORT PixelSizeDlg : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit PixelSizeDlg(QWidget *parent = 0);
    ~PixelSizeDlg();

    float getPixelSize();
    float getDistance(kipl::base::TImage<float,2> &im, size_t *roi);

private slots:

    void on_lineEdit_FileName_returnPressed();

    void on_pushButton_Browse_clicked();

    void on_pushButton_Analyze_clicked();

private:
    void loadImage(QString fn);
    Ui::PixelSizeDlg *ui;

    float pixelSize;
    float pixelDistance;
    std::string fname;
    kipl::base::TImage<float,2> img;
};

#endif // PIXELSIZEDLG_H
