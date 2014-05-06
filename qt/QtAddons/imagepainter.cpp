#include "imagepainter.h"
#include <math/image_statistics.h>
#include <sstream>
#include <base/thistogram.h>

namespace QtAddons {

ImagePainter::ImagePainter(QWidget * parent) :
    m_pParent(parent),
    logger("ImagePainter"),
    m_bHold_annotations(false),
    m_data(NULL),
    m_cdata(NULL)
{
      const int N=16;
      size_t dims[2]={N,N};
      float data[N*N];

      for (int i=0; i<N*N; i++)
          data[i]=static_cast<float>(i);

      set_image(data,dims);
}

ImagePainter::~ImagePainter()
{
    if (m_data!=NULL)
        delete [] m_data;

    if (m_cdata!=NULL)
        delete [] m_cdata;
}

void ImagePainter::Render(QPainter &painter, int x, int y, int w, int h)
{
    float img_ratio        = static_cast<float>(m_dims[0])/static_cast<float>(m_dims[1]);
    float allocation_ratio = static_cast<float>(w)/static_cast<float>(h);

    if (!m_pixmap_full.isNull())
    {
     //   logger(kipl::logging::Logger::LogMessage,"Drawing the image");
        QSize widgetSize(w,h);
        QPoint centerPoint(0,0);
        // Scale new image which size is widgetSize
        QPixmap scaledPixmap = m_pixmap_full.scaled(widgetSize, Qt::KeepAspectRatio);
        // Calculate image center position into screen
        centerPoint.setX(x+(widgetSize.width()-scaledPixmap.width())/2);
        centerPoint.setY(y+(widgetSize.height()-scaledPixmap.height())/2);
        // Draw image

        painter.drawPixmap(centerPoint,scaledPixmap);


        offset_x = (w-scaledPixmap.width())/2+x;
        offset_y = (h-scaledPixmap.height())/2+y;

        scaled_width = scaledPixmap.width();
        scaled_height = scaledPixmap.height();

        m_fScale=scaledPixmap.width()/static_cast<float>(m_dims[0]);

        if (!m_BoxList.empty()) {

            QMap<int,QPair<QRect, QColor> >::iterator it;

            for (it=m_BoxList.begin(); it!=m_BoxList.end(); it++) {
                QRect rect=it->first;

                painter.setPen(QPen(it->second));
                painter.drawRect(offset_x+rect.x()*m_fScale,offset_y+rect.y()*m_fScale,rect.width()*m_fScale,rect.height()*m_fScale);
            }
        }

        if (!m_PlotList.empty()) {

            QMap<int,QPair<QVector<QPointF>, QColor> >::iterator it;
            for (it=m_PlotList.begin(); it!=m_PlotList.end(); it++) {

                painter.setPen(QPen(it->second));
                QPoint offset(offset_x,offset_y);

                for (int i=1; i<it->first.size(); i++) {
                    painter.drawLine(offset+m_fScale*it->first[i-1],offset+m_fScale*it->first[i]);
                }
            }
        }


        painter.setPen(QColor(Qt::black));
    }
}

void ImagePainter::set_image(float const * const data, size_t const * const dims)
{
    float mi,ma;
    kipl::math::minmax(data,dims[0]*dims[1],&mi, &ma);
    set_image(data,dims,mi,ma);
}

void ImagePainter::set_image(float const * const data, size_t const * const dims, const float low, const float high)
{
    if (m_data!=NULL) {
        delete [] m_data;
    }
    m_dims[0]=dims[0];
    m_dims[1]=dims[1];
    m_NData=m_dims[0]*m_dims[1];
    m_data=new float[m_NData];

    memcpy(m_data,data,sizeof(float)*m_NData);
    kipl::math::minmax(m_data,m_NData,&m_ImageMin, &m_ImageMax);
    m_MinVal=low;
    m_MaxVal=high;
    const size_t nHistSize=256;
    float haxis[nHistSize];
    size_t hist[nHistSize];
    kipl::base::Histogram(m_data,m_NData,hist,nHistSize,m_ImageMin,m_ImageMax,haxis);
    m_Histogram.clear();
    for (int i=0; i<nHistSize; i++) {
        m_Histogram.append(QPointF(haxis[i],static_cast<float>(hist[i])));
    }
    ostringstream msg;

    if (!m_bHold_annotations) {
        m_BoxList.clear();
        m_PlotList.clear();
    }

    prepare_pixbuf();
}

void ImagePainter::set_plot(QVector<QPointF> data, QColor color, int idx)
{
    m_PlotList[idx]=qMakePair<QVector<QPointF>, QColor>(data,color);
    m_pParent->update();
}

void ImagePainter::set_rectangle(QRect rect, QColor color, int idx)
{
    m_BoxList[idx]=qMakePair<QRect,QColor>(rect,color);
    m_pParent->update();
}

void ImagePainter::hold_annotations(bool hold)
{
    m_bHold_annotations=hold;
}

int ImagePainter::clear_plot(int idx)
{
    QMap<int,QPair<QVector<QPointF>, QColor> >::iterator it;

    if (!m_PlotList.empty()) {
        if (idx<0)  {
            m_PlotList.clear();
        }

        else {
            it=m_PlotList.find(idx);
            if (it!=m_PlotList.end()) {
                m_PlotList.erase (it);
            }
        }
        m_pParent->update();
        return 1;
    }
    return 0;
}

int ImagePainter::clear_rectangle(int idx)
{
    QMap<int,QPair<QRect, QColor> >::iterator it;

    if (!m_BoxList.empty()) {
        if (idx<0) {
            m_BoxList.clear();
        }
        else
        {
            it=m_BoxList.find(idx);

            if (it!=m_BoxList.end()) {
                m_BoxList.erase (it);
            }
        }
        m_pParent->update();
        return 1;
    }

    return 0;
}

int ImagePainter::clear()
{
    m_BoxList.clear();
    m_PlotList.clear();

    return 1;
}

void ImagePainter::set_levels(const float level_low, const float level_high)
{
    m_MinVal = level_low;
    m_MaxVal = level_high;
    prepare_pixbuf();
}

float ImagePainter::getValue(int x, int y)
{
    return m_data[x+m_dims[0]*y];
}

void ImagePainter::get_levels(float *level_low, float *level_high)
{
    *level_low  = m_MinVal;
    *level_high = m_MaxVal;
}

void ImagePainter::get_image_minmax(float *level_low, float *level_high)
{
    *level_low  = m_ImageMin;
    *level_high = m_ImageMax;
}

void ImagePainter::show_clamped(bool show)
{

}

void ImagePainter::prepare_pixbuf()
{
    ostringstream msg;
    if (m_cdata!=NULL)
        delete [] m_cdata;

    m_cdata=new uchar[m_NData*3];
    memset(m_cdata,0,3*sizeof(uchar)*m_NData);

    const float scale=255.0f/(m_MaxVal-m_MinVal);
    float val=0.0;
    int idx=0;
    for (int i=0; i<m_NData; i++) {
        idx=3*i;
        if (m_data[i]==m_data[i]) {
            val=(m_data[i]-m_MinVal)*scale;
            if (255.0f<val)
                m_cdata[idx]=255;
            else if (val<0.0f)
                m_cdata[idx]=0;
            else
                m_cdata[idx]=static_cast<uchar>(val);

            m_cdata[idx+2]=m_cdata[idx+1]=m_cdata[idx];
        }
        else {
            m_cdata[idx]=255;
            m_cdata[idx+1]=0;
            m_cdata[idx+2]=0;
        }
    }

    QImage qimg(m_cdata,m_dims[0],m_dims[1],3*m_dims[0],QImage::Format_RGB888);
    QSize imgdims=qimg.size();

    m_pixmap_full=QPixmap::fromImage(qimg);

    if (m_pixmap_full.isNull()) {
        logger(kipl::logging::Logger::LogMessage,"Pixmap failed");
    }

    if (m_pParent!=NULL) {
        m_pParent->update();
    }

}

const QVector<QPointF> & ImagePainter::get_image_histogram()
{
    return m_Histogram;
}
}
