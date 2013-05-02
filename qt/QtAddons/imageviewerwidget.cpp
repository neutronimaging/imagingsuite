#include "imageviewerwidget.h"
#include <QStylePainter>


namespace QtAddons {

ImageViewerWidget::ImageViewerWidget(QWidget *parent) :
    QWidget(parent),
    logger("ImageViewerWidget"),
    m_ImagePainter(this)
{
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
    //logger(kipl::logging::Logger::LogMessage,"paint event");
    QPainter painter(this);
    QSize s=this->size();

    m_ImagePainter.Render(painter,0,0,s.width(),s.height());

    //   painter.drawPixmap(0, 0, pixmap);

//    if (rubberBandIsShown) {
//        painter.setPen(palette().light().color());
//        painter.drawRect(rubberBandRect.normalized()
//                                       .adjusted(0, 0, -1, -1));
//    }

//    if (hasFocus()) {
//        QStyleOptionFocusRect option;
//        option.initFrom(this);
//        option.backgroundColor = palette().dark().color();
//        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
//    }
}

void ImageViewerWidget::resizeEvent(QResizeEvent *event )
{
    widgetSize = event->size();
    // Call base class impl
    QWidget::resizeEvent(event);
}

void ImageViewerWidget::set_image(float const * const data, size_t const * const dims)
{
    m_ImagePainter.set_image(data,dims);
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
