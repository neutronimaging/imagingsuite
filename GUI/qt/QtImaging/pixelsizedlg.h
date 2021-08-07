#ifndef PIXELSIZEDLG_H
#define PIXELSIZEDLG_H

#include <qtimaging_global.h>
#include <string>
#include <vector>
#include <QDialog>

#include <logging/logger.h>
#include <base/timage.h>
#include <base/kiplenums.h>

namespace Ui {
class PixelSizeDlg;
}

class QTIMAGINGSHARED_EXPORT PixelSizeDlg : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit PixelSizeDlg(QWidget *parent = nullptr);
    ~PixelSizeDlg();

    float pixelSize();
    float getDistance(kipl::base::TImage<float,2> &im, size_t *roi);
    float getDistance2(kipl::base::TImage<float,2> &im, size_t *roi);

private slots:

    void on_lineEdit_FileName_returnPressed();

    void on_pushButton_Browse_clicked();

    void on_pushButton_Analyze_clicked();

    void on_pushButton_level95p_clicked();

    void on_pushButton_level99p_clicked();

private:
    void loadImage(QString fn);
    void plotEdge(std::vector<std::pair<float,float>> &ep, QColor c, int idx);

    kipl::base::TImage<float,2> diffEdge(kipl::base::TImage<float,2> &img);
    std::vector<std::pair<float,float>> edgePositions(kipl::base::TImage<float,2> &img, size_t *roi, kipl::base::eImageAxes axis);
    void computeEdgeEquation(std::vector<std::pair<float,float>> edgePosition);

    float distanceToLine(float x, float y);
    Ui::PixelSizeDlg *ui;

    float mPixelSize;
    float pixelDistance;
    float lineCoeffs[2];
    std::vector<std::pair<float,float>> edgePositions0;
    std::vector<std::pair<float,float>> edgePositions1;

    std::string fname;
    kipl::base::TImage<float,2> img;
};

#endif // PIXELSIZEDLG_H
