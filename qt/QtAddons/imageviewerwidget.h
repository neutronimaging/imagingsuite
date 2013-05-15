#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H
#include <QRect>
#include <QColor>
#include <QWidget>
#include <QResizeEvent>
#include <imagepainter.h>
#include <logging/logger.h>

namespace QtAddons {

class ImageViewerWidget : public QWidget
{
    Q_OBJECT

    kipl::logging::Logger logger;
public:
   // explicit ImageViewerWidget(QWidget *parent = 0);
    ImageViewerWidget(QWidget *parent = 0);
    //~ImageViewerWidget();

    void set_image(float const * const data, size_t const * const dims);
    void set_image(float const * const data, size_t const * const dims, const float low, const float high);
    void set_plot(QVector<QPointF> data, QColor color, int idx);
    void clear_plot(int idx=-1);
    void set_rectangle(QRect rect, QColor color, int idx);
    void clear_rectangle(int idx=-1);

    void clear_viewer();
    void set_levels(const float level_low, const float level_high);
    void get_levels(float *level_low, float *level_high);
    void show_clamped(bool show);
    QRect get_marked_roi();

 //   void set_interpolation(Gdk::InterpType interp) {m_Interpolation=interp; queue_draw(); }

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
//    void zoomIn();
//    void zoomOut();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void keyPressEvent(QKeyEvent *event);
//    void wheelEvent(QWheelEvent *event);
        
protected:
    ImagePainter m_ImagePainter;
    void updateRubberBandRegion();
    void refreshPixmap();
    QRect rubberBandRect;
    bool rubberBandIsShown;
    QRect roiRect;
    QSize widgetSize;
    enum { Margin = 5 };
};

}

#endif // IMAGEVIEWERWIDGET_H
