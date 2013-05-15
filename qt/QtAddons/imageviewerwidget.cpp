#include "imageviewerwidget.h"
#include <QStylePainter>
#include <sstream>
#include <cmath>


namespace QtAddons {

ImageViewerWidget::ImageViewerWidget(QWidget *parent) :
    QWidget(parent),
    logger("ImageViewerWidget"),
    m_ImagePainter(this),
    rubberBandIsShown(false)
{
    QPalette palette;
    palette.setColor(QPalette::Background,Qt::black);
    setAutoFillBackground(true);
    setPalette(palette);
}

//ImageViewerWidget::~ImageViewerWidget()
//{
//}

QSize ImageViewerWidget::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize ImageViewerWidget::sizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}

void ImageViewerWidget::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    QSize s=this->size();

    m_ImagePainter.Render(painter,0,0,s.width(),s.height());

    if (rubberBandIsShown) {
        painter.setPen(palette().light().color());
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }
}

void ImageViewerWidget::resizeEvent(QResizeEvent *event )
{
    widgetSize = event->size();
    // Call base class impl
    QWidget::resizeEvent(event);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent *event)
{
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);

    if (rubberBandIsShown) {
        rubberBandIsShown = false;
        updateRubberBandRegion();
    }

    if (event->button() == Qt::LeftButton) {
        if (rect.contains(event->pos())) {
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
    }
    if (event->button() == Qt::RightButton) {
        logger(kipl::logging::Logger::LogMessage,"Right");

    }
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBandIsShown) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    std::ostringstream msg;
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown) {
        updateRubberBandRegion();

        QRect r=rubberBandRect.normalized();
        roiRect.setRect(floor((r.x()-m_ImagePainter.get_offsetX())/m_ImagePainter.get_scale()),
                        floor((r.y()-m_ImagePainter.get_offsetY())/m_ImagePainter.get_scale()),
                        floor(r.width()/m_ImagePainter.get_scale()),
                        floor(r.height()/m_ImagePainter.get_scale())
                        );

//        msg<<"ROI selected @"<<roiRect.x()<<", "<<roiRect.y()<<" w="<<roiRect.width()<<", h="<<roiRect.height();
//        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
}

void ImageViewerWidget::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();

    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void ImageViewerWidget::set_image(float const * const data, size_t const * const dims)
{
    m_ImagePainter.set_image(data,dims);
}

QRect ImageViewerWidget::get_marked_roi()
{
    rubberBandIsShown=false;
    updateRubberBandRegion();
    return roiRect;
}

void ImageViewerWidget::set_image(float const * const data, size_t const * const dims, const float low, const float high)
{
    m_ImagePainter.set_image(data,dims,low,high);
}

void ImageViewerWidget::set_plot(QVector<QPointF> data, QColor color, int idx)
{
    m_ImagePainter.set_plot(data,color,idx);
}

void ImageViewerWidget::clear_plot(int idx)
{
    m_ImagePainter.clear_plot(idx);
}

void ImageViewerWidget::set_rectangle(QRect rect, QColor color, int idx)
{
    m_ImagePainter.set_rectangle(rect,color,idx);
}

void ImageViewerWidget::clear_rectangle(int idx)
{
    m_ImagePainter.clear_rectangle(idx);
}

void ImageViewerWidget::clear_viewer()
{
    m_ImagePainter.clear();
}

void ImageViewerWidget::set_levels(const float level_low, const float level_high)
{
    m_ImagePainter.set_levels(level_low,level_high);
}

void ImageViewerWidget::get_levels(float *level_low, float *level_high)
{
    m_ImagePainter.get_levels(level_low,level_high);
}

void ImageViewerWidget::show_clamped(bool show)
{
    m_ImagePainter.show_clamped(show);
}

}
