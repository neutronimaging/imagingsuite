#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include "QtAddons_global.h"

#include <QRect>
#include <QColor>
#include <QWidget>
#include <QRubberBand>
#include <QResizeEvent>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QList>
#include <QSet>
#include <QMutex>

#include <logging/logger.h>
#include <base/kiplenums.h>
#include "imagepainter.h"
#include "plotter.h"
#include "imageviewerinfodialog.h"
#include "qglyphs.h"
#include "qmarker.h"



namespace QtAddons {

class QTADDONSSHARED_EXPORT ImageViewerWidget : public QWidget
{
    Q_OBJECT

    kipl::logging::Logger logger;
 //   static QList<ImageViewerWidget *> s_ViewerList;
    static int m_nViewerCounter;
    QString m_sViewerName;
public:
    enum eViewerMouseModes {
        ViewerROI=0,
        ViewerZoom=1,
        ViewerPan=2,
        ViewerProfile=3
    };

    enum eRubberBandStatus
    {
        RubberBandHide=0,
        RubberBandFreeze=1,
        RubberBandDrag=2,
        RubberBandMove=3
    };

   // explicit ImageViewerWidget(QWidget *parent = 0);
    ImageViewerWidget(QWidget *parent = nullptr);
    ~ImageViewerWidget();

    void set_image(float const * const data, const std::vector<size_t> &dims);
    void set_image(float const * const data, const std::vector<size_t> & dims, float low, float high);
    void getImageDims(int &x, int &y);
    void set_plot(QVector<QPointF> data, QColor color, int idx);
    void clear_plot(int idx=-1);
    void set_rectangle(QRect rect, QColor color, int idx);
    void clear_rectangle(int idx=-1);
    void set_marker(QtAddons::QMarker marker, int idx);
    void clear_marker(int idx=-1);
    void hold_annotations(bool hold);

    void clear_viewer();
    void set_levels(const float level_low, const float level_high, bool updatelinked=true);
    void set_levels(kipl::base::eQuantiles quantile=kipl::base::allData, bool updatelinked=true);
    void get_levels(float *level_low, float *level_high);
    void get_minmax(float *level_low, float *level_high);
    void show_clamped(bool show);
    QString viewerName();
    void set_viewerName(QString &name);
    QRect get_marked_roi();
    const QVector<QPointF> & get_histogram();

 //   void set_interpolation(Gdk::InterpType interp) {m_Interpolation=interp; queue_draw(); }

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void LinkImageViewer(QtAddons::ImageViewerWidget *w, bool connect=true);
    void ClearLinkedImageViewers(QtAddons::ImageViewerWidget *w=nullptr);
public slots:
//    void zoomIn();
//    void zoomOut();
    void ShowContextMenu(const QPoint& pos);
    void on_levelsChanged(float lo, float hi);

private slots:
    void saveCurrentView();
    void copyImage();

protected:
    void setupActions();
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEnterEvent *) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
//    void wheelEvent(QWheelEvent *event);
        
protected:
    void UpdateFromLinkedViewer(QtAddons::ImageViewerWidget *w);
    void UpdateLinkedViewers();
    void saveImage(const QString &fname);

    ImagePainter m_ImagePainter;
    void updateRubberBandRegion();
    void showToolTip(QPoint position, QString message);
    void refreshPixmap();
    QRect rubberBandRect;
    QRubberBand m_rubberBandLine;
    QPoint m_rubberBandOrigin;
    eRubberBandStatus m_RubberBandStatus;
    QRect roiRect;
    eViewerMouseModes m_MouseMode;
    Qt::MouseButton m_PressedButton;
    bool m_mouseMoved;
    QPoint m_LastMotionPosition;
    QSize widgetSize;
    QSet<ImageViewerWidget *> m_LinkedViewers;
    enum { Margin = 5 };
    ImageViewerInfoDialog m_infoDialog;
    QMutex m_MouseMoveMutex;
    QMutex m_ImageMutex;
    double m_CurrentScale;
    QAction *saveImageAct;
    QAction *copyImageAct;

signals:
    void newImageDims(const QRect &rect);
    void levelsChanged(float lo, float hi);
};


}

#endif // IMAGEVIEWERWIDGET_H
