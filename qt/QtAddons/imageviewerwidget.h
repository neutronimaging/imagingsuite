#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H
#include <QRect>
#include <QColor>
#include <QWidget>
#include <QResizeEvent>
#include <imagepainter.h>
#include <logging/logger.h>
#include <plotter.h>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace QtAddons {

class ImageViewerWidget : public QWidget
{
    Q_OBJECT

    kipl::logging::Logger logger;
public:
    enum eViewerMouseModes {
        ViewerROI=0,
        ViewerZoom=1,
        ViewerPan=2
    };
   // explicit ImageViewerWidget(QWidget *parent = 0);
    ImageViewerWidget(QWidget *parent = 0);
    //~ImageViewerWidget();

    void set_image(float const * const data, size_t const * const dims);
    void set_image(float const * const data, size_t const * const dims, const float low, const float high);
    void set_plot(QVector<QPointF> data, QColor color, int idx);
    void clear_plot(int idx=-1);
    void set_rectangle(QRect rect, QColor color, int idx);
    void clear_rectangle(int idx=-1);
    void hold_annotations(bool hold);

    void clear_viewer();
    void set_levels(const float level_low, const float level_high);
    void get_levels(float *level_low, float *level_high);
    void get_minmax(float *level_low, float *level_high);
    void show_clamped(bool show);
    QRect get_marked_roi();
    const QVector<QPointF> & get_histogram();

 //   void set_interpolation(Gdk::InterpType interp) {m_Interpolation=interp; queue_draw(); }

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
//    void zoomIn();
//    void zoomOut();
    void ShowContextMenu(const QPoint& pos);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *);
    virtual void keyPressEvent(QKeyEvent *event);
//    void wheelEvent(QWheelEvent *event);
        
protected:

    ImagePainter m_ImagePainter;
    void updateRubberBandRegion();
    void showToolTip(QPoint position, QString message);
    void refreshPixmap();
    QRect rubberBandRect;
    bool rubberBandIsShown;
    QRect roiRect;
    eViewerMouseModes m_MouseMode;
    Qt::MouseButton m_PressedButton;
    QPoint m_LastMotionPosition;
    QSize widgetSize;
    enum { Margin = 5 };
};

class SetGrayLevelsDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    SetGrayLevelsDialog(QWidget *parent);

protected slots:
    void GrayLevelsChanged(double x);

protected:
    QtAddons::Plotter m_Plotter;
    QDoubleSpinBox m_spinLow;
    QDoubleSpinBox m_spinHigh;
    QLabel m_label1;
    QLabel m_label2;
    QVBoxLayout m_VerticalLayout;
    QHBoxLayout m_HorizontalLayout;

    ImageViewerWidget *m_pParent;

};

}

#endif // IMAGEVIEWERWIDGET_H
