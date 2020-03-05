//<LICENSE>
#include <sstream>

#include <QLine>

#include <math/image_statistics.h>
#include <base/thistogram.h>

#include "imagepainter.h"


namespace QtAddons {

ImagePainter::ImagePainter(QWidget * parent) :
    m_pParent(parent),
    logger("ImagePainter"),
    m_ImageMin(0.0f),
    m_ImageMax(1.0f),
    m_MinVal(0.0f),
    m_MaxVal(1.0f),
    m_bHold_annotations(false),
    m_data(nullptr),
    m_cdata(nullptr),
    m_currentROI(0,0,0,0)
{
      const int N=16;
      size_t dims[2]={N,N};
      float data[N*N];

      for (int i=0; i<N*N; i++)
          data[i]=static_cast<float>(i);

      setImage(data,dims);
}

ImagePainter::~ImagePainter()
{
    if (m_data!=nullptr)
        delete [] m_data;

    if (m_cdata!=nullptr)
        delete [] m_cdata;
}

void ImagePainter::Render(QPainter &painter, int x, int y, int w, int h)
{
    if (!m_pixmap_full.isNull())
    {
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
                QRect drawrect=rect.intersected(m_currentROI);
                //QRect drawrect=rect.translated(-m_currentROI.topLeft());

                if (drawrect.isEmpty()==false) {
                    drawrect.translate(-m_currentROI.topLeft());
                    rect.setRect(drawrect.x()*m_fScale,
                                 drawrect.y()*m_fScale,
                                 drawrect.width()*m_fScale,
                                 drawrect.height()*m_fScale);
                    rect=rect.normalized();
                    rect.translate(offset_x,offset_y);

                    painter.setPen(QPen(it->second));
                    painter.drawRect(rect);
                }
            }
        }

        if (!m_PlotList.empty()) {

            QMap<int,QPair<QVector<QPointF>, QColor> >::iterator it;
            for (it=m_PlotList.begin(); it!=m_PlotList.end(); it++) {

                painter.setPen(QPen(it->second));
                QPoint offset(offset_x,offset_y);
                QPointF pointA, pointB;
                QLine line;
                for (int i=1; i<it->first.size(); i++) {
                    pointA=offset+m_fScale*it->first[i-1];
                    pointB=offset+m_fScale*it->first[i];

                    painter.drawLine(pointA,pointB);
                }
            }
        }

        if (!m_MarkerList.empty()) {
            QMap<int,QMarker>::iterator it;
            for (it=m_MarkerList.begin(); it!=m_MarkerList.end(); it++)
            {
                QPoint offset(offset_x,offset_y);

                it->Draw(painter,m_fScale,offset);

            }
        }

        painter.setPen(QColor(Qt::black));
    }
}

void ImagePainter::setImage(kipl::base::TImage<float, 2> &img)
{
    setImage(img.GetDataPtr(),img.Dims());
}

kipl::base::TImage<float,2> ImagePainter::getImage()
{
    return m_OriginalImage;
}

void ImagePainter::setImage(float const * const data, size_t const * const dims)
{
    setImage(data,dims,0.0f,0.0f);
}

void ImagePainter::setImage(float const * const data, size_t const * const dims, const float low, const float high)
{
    m_dims[0]=static_cast<int>(dims[0]);
    m_dims[1]=static_cast<int>(dims[1]);
    m_globalROI.setRect(0,0,dims[0],dims[1]);

    m_OriginalImage.Resize(dims);
    std::copy_n(data,m_OriginalImage.Size(),m_OriginalImage.GetDataPtr());

    kipl::math::minmax(data,dims[0]*dims[1],&m_ImageMin, &m_ImageMax,true);
    if (low==high) {
        m_MinVal=m_ImageMin;
        m_MaxVal=m_ImageMax;
    }
    else {
        m_MinVal=low;
        m_MaxVal=high;
    }


    const size_t nHistSize=1024;
    float haxis[nHistSize];
    size_t hist[nHistSize];
    kipl::base::Histogram(m_OriginalImage.GetDataPtr(),
                          m_OriginalImage.Size(),
                          hist,
                          nHistSize,
                          m_ImageMin,
                          m_ImageMax,
                          haxis,
                          true);

    m_Histogram.clear();
    for (size_t i=0; i<nHistSize; ++i) {
        m_Histogram.append(QPointF(static_cast<qreal>(haxis[i]),static_cast<qreal>(hist[i])));
    }

    if (!m_bHold_annotations) {
        m_BoxList.clear();
        m_PlotList.clear();
    }

    m_ZoomList.clear();
    createZoomImage(m_globalROI);
}

void ImagePainter::setPlot(QVector<QPointF> data, QColor color, int idx)
{
    m_PlotList[idx]=qMakePair<QVector<QPointF>, QColor>(data,color);
    m_pParent->update();
}

void ImagePainter::setRectangle(QRect rect, QColor color, int idx)
{
    m_BoxList[idx]=qMakePair<QRect,QColor>(rect,color);
    m_pParent->update();
}

void ImagePainter::holdAnnotations(bool hold)
{
    m_bHold_annotations=hold;
}

int ImagePainter::clearPlot(int idx)
{
    QMap<int,QPair<QVector<QPointF>, QColor> >::iterator it;

    if (!m_PlotList.empty())
    {
        if (idx<0)
        {
            m_PlotList.clear();
        }
        else
        {
            it=m_PlotList.find(idx);
            if (it!=m_PlotList.end())
            {
                m_PlotList.erase (it);
            }
        }
        m_pParent->update();
        return 1;
    }
    return 0;
}

int ImagePainter::clearRectangle(int idx)
{
    QMap<int,QPair<QRect, QColor> >::iterator it;

    if (!m_BoxList.empty())
    {
        if (idx<0)
        {
            m_BoxList.clear();
        }
        else
        {
            it=m_BoxList.find(idx);

            if (it!=m_BoxList.end())
            {
                m_BoxList.erase (it);
            }
        }
        m_pParent->update();
        return 1;
    }

    return 0;
}

void ImagePainter::setMarker(QtAddons::QMarker marker, int idx)
{
    m_MarkerList.insert(idx,marker);
    m_pParent->update();
}

int ImagePainter::clearMarker(int idx)
{
    QMap<int,QMarker >::iterator it;

    if (!m_MarkerList.empty())
    {
        if (idx<0)
        {
            m_MarkerList.clear();
        }
        else
        {
            it=m_MarkerList.find(idx);

            if (it!=m_MarkerList.end())
            {
                m_MarkerList.erase (it);
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

void ImagePainter::setLevels(const float level_low, const float level_high)
{
    m_MinVal = level_low;
    m_MaxVal = level_high;
    preparePixbuf();
}

float ImagePainter::getValue(int x, int y)
{
    return m_OriginalImage[x+m_dims[0]*y];
}

void ImagePainter::globalPosition(int &x, int &y)
{
    x += m_currentROI.x();
    y += m_currentROI.y();
}

int ImagePainter::globalPositionX(int x)
{
    return x+m_currentROI.x();
}

int ImagePainter::globalPositionY(int y)
{
    return y+m_currentROI.y();
}

void ImagePainter::getLevels(float *level_low, float *level_high)
{
    *level_low  = m_MinVal;
    *level_high = m_MaxVal;
}

void ImagePainter::getImageMinMax(float *level_low, float *level_high)
{
    *level_low  = m_ImageMin;
    *level_high = m_ImageMax;
}

void ImagePainter::showClamped(bool show)
{
    (void) show;
}

void ImagePainter::preparePixbuf()
{
    ostringstream msg;
    if (m_cdata!=nullptr)
        delete [] m_cdata;

    m_NData=m_ZoomedImage.Size();
    m_cdata=new uchar[m_NData*3];
    std::fill_n(m_cdata,m_NData*3,0);

    const float scale=255.0f/(m_MaxVal-m_MinVal);
    float val=0.0f;
    int idx=0;
    for (size_t i=0; i<m_NData; i++) {
        idx=static_cast<int>(3*i);
        if (std::isfinite(m_ZoomedImage[i]))
        {
            val=(m_ZoomedImage[i]-m_MinVal)*scale;
            if (255.0f<val)
                m_cdata[idx]=255;
            else if (val<0.0f)
                m_cdata[idx]=0;
            else
                m_cdata[idx]=static_cast<uchar>(val);

            m_cdata[idx+2]=m_cdata[idx+1]=m_cdata[idx];
        }
        else {
            if (std::isinf(m_ZoomedImage[i]))
            {
                m_cdata[idx]   = 255;
                m_cdata[idx+1] = 0;
                m_cdata[idx+2] = 0;
            }

            if (std::isnan(m_ZoomedImage[i]))
            {
                m_cdata[idx]   = 0;
                m_cdata[idx+1] = 255;
                m_cdata[idx+2] = 0;
            }

        }
    }

    QImage qimg(m_cdata,
                m_ZoomedImage.Size(0),
                m_ZoomedImage.Size(1),
                3*m_ZoomedImage.Size(0),
                QImage::Format_RGB888);

    m_pixmap_full=QPixmap::fromImage(qimg);

    if (m_pixmap_full.isNull())
    {
        logger(kipl::logging::Logger::LogMessage,"Pixmap failed");
    }

    if (m_pParent!=nullptr)
    {
        m_pParent->update();
    }

}

const QVector<QPointF> & ImagePainter::getImageHistogram()
{
    return m_Histogram;
}

void ImagePainter::createZoomImage(QRect roi)
{
    size_t dims[2]={static_cast<size_t>(roi.width()),static_cast<size_t>(roi.height())};
    m_ZoomedImage.Resize(dims);
    m_currentROI=roi;

    for (size_t y=0; y<m_ZoomedImage.Size(1); y++)
    {
        memcpy(m_ZoomedImage.GetLinePtr(y),m_OriginalImage.GetLinePtr(y+roi.y())+roi.x(), m_ZoomedImage.Size(0)*sizeof(float));
    }
    preparePixbuf();
}

int ImagePainter::zoomIn(QRect *zoomROI)
{
    int x=0;
    int y=0;
    int w=0;
    int h=0;

    QRect roi;
    if (zoomROI==nullptr) // Automatic zooming
    {
       if (m_ZoomList.empty())
       {
           x = static_cast<int>(m_OriginalImage.Size(0)/4);
           y = static_cast<int>(m_OriginalImage.Size(1)/4);
           w = static_cast<int>(m_OriginalImage.Size(0)/2);
           h = static_cast<int>(m_OriginalImage.Size(1)/2);
       }
       else
       {
           //QRect currentROI=m_ZoomList.last();

           x = m_currentROI.x() + m_currentROI.width()/4;
           y = m_currentROI.y() + m_currentROI.height()/4;
           w = m_currentROI.width()/2;
           h = m_currentROI.height()/2;
       }

       roi.setRect(x,y,w,h);
    }
    else
    {
        roi=*zoomROI;
    }

    m_ZoomList.push_back(roi);
    createZoomImage(roi);
    std::ostringstream msg;

    msg<<": zoomed image: "<<m_ZoomedImage<<", zoom stack size="<<m_ZoomList.size();
    logger(kipl::logging::Logger::LogMessage, msg.str());

    return m_ZoomList.size();
}

int ImagePainter::zoomOut()
{
    QRect roi;
    if (!m_ZoomList.empty()) {
        roi=m_ZoomList.last();
        m_ZoomList.pop_back();
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Zoom list is empty");
        roi.setRect(0,0,m_OriginalImage.Size(0),m_OriginalImage.Size(1));

        m_ZoomedImage=m_OriginalImage;
        m_ZoomedImage.Clone();
    }

    createZoomImage(roi);

    std::ostringstream msg;

    msg<<": zoomed image: "<<m_ZoomedImage<<", zoom stack size="<<m_ZoomList.size();
    logger(kipl::logging::Logger::LogMessage, msg.str());

    return m_ZoomList.size();
}

QRect ImagePainter::getCurrentZoomROI()
{
    return m_currentROI;
}

int ImagePainter::panImage(int dx, int dy)
{

    if (!m_ZoomList.empty()) {
        std::ostringstream msg;
        QRect roi=m_ZoomList.last();

        roi.translate(dx,dy);
        roi=roi.normalized();

        if (((roi.x()+roi.width())<m_globalROI.width()) &&
                (0<=roi.x()) &&
                ((roi.y()+roi.height())<m_globalROI.height()) &&
                (0<=roi.y())) {
            createZoomImage(roi);
            m_ZoomList.last()=roi;
        }
    }

    return 0;
}

}
