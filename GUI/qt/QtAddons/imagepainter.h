#ifndef IMAGEPAINTER_H
#define IMAGEPAINTER_H
#include "QtAddons_global.h"

#include <cstdlib>
#include <map>
#include <vector>

#include <QPixmap>
#include <QWidget>
#include <QMap>
#include <QColor>
#include <QRect>
#include <QPainter>

#include <logging/logger.h>
#include <base/timage.h>
#include <base/kiplenums.h>

#include "qmarker.h"

class QPainter;


namespace QtAddons {

/// \brief Back-end class that manages the painting of images onto a widget.
///
/// Instances of this class are usually not used directly but are embedded into a widget class.
class QTADDONSSHARED_EXPORT ImagePainter
{
    QWidget * m_pParent;
    kipl::logging::Logger logger;
public:
    ImagePainter(QWidget *parent=nullptr);
    ~ImagePainter();
    void Render(QPainter &painter, int x, int y, int w, int h);

    void setImage(kipl::base::TImage<float,2> &img);
    kipl::base::TImage<float,2> getImage();
    void setImage(float const * const data, const std::vector<size_t> &dims);
    void setImage(float const * const data, const std::vector<size_t> &dims, const float low, const float high);
    void setPlot(QVector<QPointF> data, QColor color, int idx);
    int clearPlot(int idx=-1);
    void setRectangle(QRect rect, QColor color, int idx);
    int  clearRectangle(int idx=-1);
    void setMarker(QMarker marker, int idx);
    int clearMarker(int idx=-1);
    void holdAnnotations(bool hold);

    int  clear();
    void setLevels(const float level_low, const float level_high);
    void setLevels(kipl::base::eQuantiles quantile);
    void getLevels(float *level_low, float *level_high);
    void getImageMinMax(float *level_low, float *level_high);
    const QVector<QPointF> &getImageHistogram();
    const std::vector<size_t> & imageDims() {return m_dims;}
    void showClamped(bool show);
    float getScale();
    int getOffsetx() {return offset_x;}
    int getOffsety() {return offset_y;}
    float getValue(int x, int y);
    void globalPosition(int &x,int &y);
    int globalPositionX(int x);
    int globalPositionY(int y);
    int zoomIn(QRect *zoomROI=nullptr);
    int zoomOut();
    int panImage(int dx, int dy);
    QRect getCurrentZoomROI();
    QSize zoomedImageSize();
    QSize imageSize();
    //void set_interpolation(Gdk::InterpType interp) {m_Interpolation=interp;}
protected:
    void preparePixbuf();
    void createZoomImage(QRect roi);

    std::vector<size_t> m_dims;
    size_t m_NData;

    float m_ImageMin;
    float m_ImageMax;

    float m_MinVal;
    float m_MaxVal;

    float m_WindowScale;
    int offset_x;
    int offset_y;
    int scaled_width;
    int scaled_height;
    float m_fScale;
    bool m_bHold_annotations;

    float * m_data;  ///<! float pixel buffer
    kipl::base::TImage<float,2> m_OriginalImage;
    kipl::base::TImage<float,2> m_ZoomedImage;
    uchar * m_cdata;   ///<! RGB Pixel buffer

    QVector<QRect> m_ZoomList; ///<! Stack of zoom ROIs
    QMap<int,std::pair<QRect, QColor> > m_BoxList; ///<! List of Rectangles to draw on the image
    QMap<int,std::pair<QVector<QPointF>, QColor> > m_PlotList; ///<! List of plot data to draw on the image
    QMap<int,QMarker > m_MarkerList;
    QVector<QPointF> m_Histogram; ///<! Histogram of the full image

    QPixmap m_pixmap_full;
    QRect m_currentROI;
    QRect m_globalROI;
};

}

#endif // IMAGEPAINTER_H
