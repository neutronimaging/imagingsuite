//<LICENSE>
#include "imageviewerwidget.h"

#include <iostream>
#include <sstream>
#include <cmath>

#include <QMenu>
#include <QMessageBox>
#include <QToolTip>
#include <QStylePainter>
#include <QSignalBlocker>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QDir>

#include <base/kiplenums.h>

#include "setgraylevelsdlg.h"
namespace QtAddons {

int ImageViewerWidget::m_nViewerCounter = -1;
//QList<ImageViewerWidget *> ImageViewerWidget::s_ViewerList = QList<ImageViewerWidget *>();

ImageViewerWidget::ImageViewerWidget(QWidget *parent) :
    QWidget(parent),
    logger("ImageViewerWidget"),
    m_ImagePainter(this),   
    m_rubberBandBox(QRubberBand::Rectangle, this),
    m_RubberBandStatus(RubberBandHide),
    m_MouseMode(ViewerROI),
    m_PressedButton(Qt::NoButton),
    m_mouseMoved(false),
    m_infoDialog(this),
    m_CurrentScale(1.0),
    saveImageAct(nullptr),
    copyImageAct(nullptr)
{
    QPalette palette;
    palette.setColor(QPalette::Window,Qt::black);
    setAutoFillBackground(true);
    setPalette(palette);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);
    setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    m_nViewerCounter++;
    m_sViewerName = QString("ImageViewer ")+QString::number(m_nViewerCounter);
    //s_ViewerList.push_back(this);
    m_infoDialog.setModal(false);

    connect(this,&ImageViewerWidget::levelsChanged,this,&ImageViewerWidget::on_levelsChanged);
    setupActions();
}

ImageViewerWidget::~ImageViewerWidget()
{
    //s_ViewerList.removeOne(this);
    m_infoDialog.close();
}

QString ImageViewerWidget::viewerName()
{
    return m_sViewerName;
}

void ImageViewerWidget::set_viewerName(QString &name)
{
    m_sViewerName = name;
}

QSize ImageViewerWidget::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize ImageViewerWidget::sizeHint() const
{
    return QSize(256, 256);
}

void ImageViewerWidget::ShowContextMenu(const QPoint& pos) // this is a slot
{
    logger(kipl::logging::Logger::LogMessage,"Context menu");
    // for most widgets
    QPoint globalPos = this->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu viewerMenu(this);

    viewerMenu.addAction(saveImageAct);
    viewerMenu.addAction(copyImageAct);
    viewerMenu.exec(globalPos);
//    viewerMenu.addAction("Full Image");
//    viewerMenu.addAction("Set Levels");
//    // ...

//    QAction* selectedItem = viewerMenu.exec(globalPos);
//    if (selectedItem)
//    {
//        if (selectedItem->text() == "Full Image") {
//            logger(kipl::logging::Logger::LogMessage,"Full Image");
//        }
//        if (selectedItem->text() == "Set Levels") {
//            logger(kipl::logging::Logger::LogMessage,"Set Levels");
//        }
//    }
//    else
//    {
//        // nothing was chosen
//        logger(kipl::logging::Logger::LogMessage,"Menu was canceled");
//    }
}

void ImageViewerWidget::on_levelsChanged(float lo, float hi)
{
    std::ignore = lo;
    std::ignore = hi;
}

void ImageViewerWidget::saveCurrentView()
{
    QString destname=QFileDialog::getSaveFileName(this,"Where should the image be saved?",QDir::homePath()+"/image.png");

    saveImage(destname);
}

void ImageViewerWidget::copyImage()
{
    QClipboard *clipboard = QApplication::clipboard();
    auto wsize = m_ImagePainter.zoomedImageSize();
    const int w=1024;
    if (wsize.width()<w)
    {
        wsize = QSize(w,static_cast<int>(w*static_cast<float>(wsize.height())/static_cast<float>(wsize.width())));
    }

    QPixmap p( wsize );
    QPainter painter(&p);
    painter.fillRect(QRect(0,0,wsize.width(),wsize.height()),QColor("lightgray"));
    m_ImagePainter.Render(painter,0,0,wsize.width(),wsize.height());

    clipboard->setPixmap(p);

}

void ImageViewerWidget::saveImage(const QString &fname)
{
    if (!fname.isEmpty())
    {
        auto wsize = m_ImagePainter.zoomedImageSize();
        const int w=1024;
        if (wsize.width()<w)
        {
            wsize = QSize(w,static_cast<int>(w*static_cast<float>(wsize.height())/static_cast<float>(wsize.width())));
        }

        QPixmap p( wsize );
        QPainter painter(&p);
        painter.fillRect(QRect(0,0,wsize.width(),wsize.height()),QColor("lightgray"));
        m_ImagePainter.Render(painter,0,0,wsize.width(),wsize.height());
        p.save(fname,"PNG");
    }
    else
    {
        QMessageBox::warning(this,"No file name","Please provide a file name to save the image.");
    }
}

void ImageViewerWidget::setupActions()
{
    copyImageAct = new QAction(tr("&Copy image"), this);
    copyImageAct->setShortcuts(QKeySequence::Copy);
    copyImageAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyImageAct, &QAction::triggered, this, &ImageViewerWidget::copyImage);

    saveImageAct = new QAction(tr("&Save image"), this);
    saveImageAct->setShortcuts(QKeySequence::Cut);
    saveImageAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));

    connect(saveImageAct,&QAction::triggered, this, &ImageViewerWidget::saveCurrentView );
}

void ImageViewerWidget::paintEvent(QPaintEvent * ) // event
{
    QPainter painter(this);
    QSize s=this->size();

    m_ImagePainter.Render(painter,0,0,s.width(),s.height());
}

void ImageViewerWidget::resizeEvent(QResizeEvent *event )
{
    widgetSize = event->size();
    // Call base class impl
    QWidget::resizeEvent(event);
}

void ImageViewerWidget::keyPressEvent(QKeyEvent *event)
{
    if (!this->hasFocus())
        return;
    char keyvalue=event->key();

    ostringstream msg;

    switch (keyvalue)
    {
    case 'm':
    case 'M':
        ShowContextMenu(QPoint(100,100));
        break;
//    case 'z':
//    case 'Z':
//        setCursor(Qt::SizeBDiagCursor);
//        m_MouseMode=ViewerZoom;
//        break;
    case 'i':
    case 'I':
        if (m_infoDialog.isVisible())
            m_infoDialog.close();
        else
            m_infoDialog.show();
        break;
    case 'p':
    case 'P':
        setCursor(Qt::OpenHandCursor);
        m_MouseMode=ViewerPan;
        break;
    case 'l':
    case 'L':
    {
        float lo;
        float hi;

        get_levels(&lo, &hi);

        SetGrayLevelsDlg dlg(this);
        int res=dlg.exec();

        if (res==QDialog::Rejected)
        {
            set_levels(lo,hi);
        }
        break;
    }
    case 'k':
    case 'K':
        setCursor(Qt::PointingHandCursor);
        m_MouseMode=ViewerProfile;
        break;
    case 'r':
    case 'R':
        setCursor(Qt::CrossCursor);
        m_MouseMode=ViewerROI;
        break;
    case '+':
        m_ImagePainter.zoomIn(nullptr);
        break;
    case '-':
        m_ImagePainter.zoomOut();
        break;
    }
}

void ImageViewerWidget::enterEvent(QEnterEvent *)
{
  // logger(kipl::logging::Logger::LogMessage,"Entered");

}

void ImageViewerWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint origin = event->pos();
    m_rubberBandOrigin = origin;

    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);

    if (event->button() == Qt::LeftButton)
    {
        if (m_MouseMode==ViewerROI)
        {
            if (rect.contains(origin))
            {
                m_RubberBandStatus = RubberBandDrag;
                m_rubberBandBox.setGeometry(QRect(origin, QSize()));
                rubberBandRect = m_rubberBandBox.geometry().normalized(); // This is fine for the visual
                m_rubberBandBox.show();
                setCursor(Qt::CrossCursor);
            }
        }
        if (m_MouseMode==ViewerPan)
        {

        }
    }
    if (event->button() == Qt::RightButton)
    {
        m_PressedButton=event->button();
    }



    m_LastMotionPosition=origin;

   QWidget::mousePressEvent(event);
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint origin=event->pos();
    rubberBandRect = m_rubberBandBox.rect().normalized();
    QSignalBlocker blocker(this);
    std::ostringstream msg;
    int x = origin.x();
    int y = origin.y();
    int dx= x - m_LastMotionPosition.x();
    int dy= y - m_LastMotionPosition.y();

    if (m_RubberBandStatus == RubberBandDrag)
    {
        m_rubberBandBox.setGeometry(QRect(m_rubberBandOrigin, origin).normalized());
        rubberBandRect = m_rubberBandBox.geometry().normalized();
    }

    if (m_MouseMode==ViewerPan)
    {
        m_ImagePainter.panImage(dx/m_ImagePainter.getScale(),dy/m_ImagePainter.getScale());
    }


    QPoint tooltipOffset(0,0);
    if (m_PressedButton == Qt::RightButton)
    {
        float minlevel, maxlevel;

        get_levels(&minlevel, &maxlevel);

        float fWindow=maxlevel-minlevel;
        float fLevel=minlevel+fWindow/2.0f;

        float fLevelStep  = fWindow/1000.0f;
        float fWindowStep = fWindow/1000.0f;
        msg.str("");

        if (std::abs(dx)<std::abs(dy))
            fLevel+=dy*fLevelStep;
        else
            fWindow+=dx*fWindowStep;

        msg<<"W="<<fWindow<<", L="<<fLevel;

        showToolTip(event->globalPosition().toPoint()+tooltipOffset,QString::fromStdString(msg.str()));
        set_levels(fLevel-fWindow/2.0f,fLevel+fWindow/2.0f);
    }
    else
    {
        QRect roi=m_ImagePainter.getCurrentZoomROI();

        QPoint pos(static_cast<int>( m_ImagePainter.globalPositionX((origin.x()-m_ImagePainter.getOffsetx())/m_ImagePainter.getScale()) ),
                   static_cast<int>( m_ImagePainter.globalPositionY((origin.y()-m_ImagePainter.getOffsety())/m_ImagePainter.getScale()) ) );

        if(roi.contains(pos)==true)
        {
            msg.str("");

            msg<<m_ImagePainter.getValue(pos.x(),pos.y())<<" @ ("<<pos.x()<<", "<<pos.y()<<")";

            showToolTip(event->globalPosition().toPoint()+tooltipOffset,QString::fromStdString(msg.str()));
        }
    }

    m_LastMotionPosition=origin;

    QWidget::mouseMoveEvent(event);
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint origin = event->pos();
    std::ostringstream msg;
    if ((event->button() == Qt::LeftButton))
    {
        if ((m_RubberBandStatus == RubberBandDrag) || (m_RubberBandStatus == RubberBandMove))
        {
            // rubberBandRect = m_rubberBandBox.geometry().normalized();
            rubberBandRect = QRect(m_rubberBandOrigin, origin).normalized();
            
            int xpos = m_ImagePainter.globalPositionX(floor((rubberBandRect.x()-m_ImagePainter.getOffsetx())/m_ImagePainter.getScale()));
            int ypos = m_ImagePainter.globalPositionY(floor((rubberBandRect.y()-m_ImagePainter.getOffsety())/m_ImagePainter.getScale()));

            int w    = floor(rubberBandRect.width()/m_ImagePainter.getScale());
            int h    = floor(rubberBandRect.height()/m_ImagePainter.getScale());

            xpos = xpos < 0 ? 0 : xpos;
            ypos = ypos < 0 ? 0 : ypos;
            auto currentZoomROI = m_ImagePainter.getCurrentZoomROI();
            w = currentZoomROI.width()+currentZoomROI.x()  <= xpos + w ? currentZoomROI.width()  + currentZoomROI.x() - xpos-1 : w;
            h = currentZoomROI.height()+currentZoomROI.y() <= ypos + h ? currentZoomROI.height() + currentZoomROI.y() - ypos-1 : h;

            roiRect.setRect(xpos, ypos, w, h);

            roiRect=roiRect.normalized();
            msg.str("");
            msg<<"mouse release roiRect="<<roiRect.x()<<", "<<roiRect.y()<<", h="<<roiRect.height()<<", w="<<roiRect.width();
            logger.message(msg.str());

            m_RubberBandStatus = RubberBandFreeze;

            m_infoDialog.updateInfo(m_ImagePainter.getImage(), roiRect);
        }
        if (m_MouseMode==ViewerPan)
        {
            m_MouseMode=ViewerROI;
        }
    }


    if (event->button() == Qt::RightButton )
    {
        m_PressedButton = Qt::NoButton;
    }


    QWidget::mouseReleaseEvent(event);
}


void ImageViewerWidget::showToolTip(QPoint position, QString message)
{
    QFont ttfont=this->font();
    ttfont.setPointSize(static_cast<int>(ttfont.pointSize()*0.9));
    QPalette color;

#if QT_VERSION < 0x050000
    color.setColor( QPalette::Active,QPalette::QPalette::ToolTipBase,Qt::yellow);
#else
//    color.setColor(QPalette::Active,QPalette::QPalette::ToolTipBase,QColor::yellow());
#endif

    QPoint ttpos=position;

    QToolTip::setPalette(color);
    QToolTip::setFont(ttfont);

    QToolTip::showText(ttpos, message,this);

}

void ImageViewerWidget::set_image(float const * const imgdata, const std::vector<size_t> & dims, bool keep_roi)
{
    QMutexLocker locker(&m_ImageMutex);
    std::ostringstream msg;
    m_ImagePainter.setImage(imgdata,dims);
    if (!keep_roi)
    {
        roiRect=QRect();
        rubberBandRect=QRect();
        m_rubberBandBox.hide();
    }
    else
    {
        paintEvent(nullptr);
        m_infoDialog.updateInfo(m_ImagePainter.getImage(), roiRect);
    }

    m_infoDialog.setHistogram(m_ImagePainter.getImageHistogram());
    float mi,ma;
    m_ImagePainter.getImageMinMax(&mi,&ma);

    QRect rect=QRect(0,0,static_cast<int>(dims[0]),static_cast<int>(dims[1]));

    emit newImageDims(rect);
}

QRect ImageViewerWidget::get_marked_roi()
{
    m_RubberBandStatus = RubberBandHide;
    //updateRubberBandRegion();
    m_rubberBandBox.hide();
    rubberBandRect=QRect();
    return roiRect;
}

void ImageViewerWidget::set_image(float const * const imgdata, const std::vector<size_t> &dims, float low, float high, bool keep_roi)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.setImage(imgdata,dims,low,high);

    m_infoDialog.setHistogram(m_ImagePainter.getImageHistogram());
    if (!keep_roi)
    {
        roiRect=QRect();
        rubberBandRect=QRect();
        m_rubberBandBox.hide();
    }
    else
    {
        paintEvent(nullptr);
        m_infoDialog.updateInfo(m_ImagePainter.getImage(), roiRect);
    }

    QRect rect=QRect(0,0,static_cast<int>(dims[0]),static_cast<int>(dims[1]));

    emit newImageDims(rect);
}

void ImageViewerWidget::image_dims(int &x, int &y)
{
    auto dims=m_ImagePainter.imageDims();
    x=dims[0];
    y=dims[1];
}

void ImageViewerWidget::set_plot(QVector<QPointF> datapoints, QColor color, int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.setPlot(datapoints,color,idx);
}

void ImageViewerWidget::clear_plot(int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.clearPlot(idx);
}

void ImageViewerWidget::set_rectangle(QRect rect, QColor color, int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.setRectangle(rect,color,idx);
}

void ImageViewerWidget::clear_rectangle(int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.clearRectangle(idx);
}

void ImageViewerWidget::set_marker(QMarker marker, int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.setMarker(marker,idx);
}

void ImageViewerWidget::clear_marker(int idx)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.clearMarker(idx);
}

void ImageViewerWidget::hold_annotations(bool hold)
{
    m_ImagePainter.holdAnnotations(hold);
}

void ImageViewerWidget::clear_viewer()
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.clear();
}

void ImageViewerWidget::set_levels(const float level_low, const float level_high, bool updatelinked)
{
    QMutexLocker locker(&m_ImageMutex);
    m_ImagePainter.setLevels(level_low,level_high);
    if (updatelinked)
    {
        UpdateLinkedViewers();
    }

    emit levelsChanged(level_low,level_high);
}

void ImageViewerWidget::set_levels(kipl::base::eQuantiles quantile, bool updatelinked)
{
    QMutexLocker locker(&m_ImageMutex);

    m_ImagePainter.setLevels(quantile);
    if (updatelinked)
    {
        UpdateLinkedViewers();
    }

    float level_low,level_high;

    m_ImagePainter.getLevels(&level_low,&level_high);

    emit levelsChanged(level_low,level_high);
}

void ImageViewerWidget::get_levels(float *level_low, float *level_high)
{
    m_ImagePainter.getLevels(level_low,level_high);
}

void ImageViewerWidget::get_minmax(float *level_low, float *level_high)
{
    m_ImagePainter.getImageMinMax(level_low,level_high);
}

void ImageViewerWidget::show_clamped(bool show)
{
    m_ImagePainter.showClamped(show);
}

const QVector<QPointF> & ImageViewerWidget::get_histogram()
{
    return m_ImagePainter.getImageHistogram();
}

void ImageViewerWidget::LinkImageViewer(QtAddons::ImageViewerWidget *w, bool connect)
{
    m_LinkedViewers.insert(w);
    if (connect)
    {
        w->LinkImageViewer(this,false);
    }
}

void ImageViewerWidget::ClearLinkedImageViewers(QtAddons::ImageViewerWidget *w)
{
    if (w)
    {
        QSet<ImageViewerWidget *>::iterator i=m_LinkedViewers.find(w);
        m_LinkedViewers.erase(i);
    }
    else
    {
        QSetIterator<ImageViewerWidget *> i(m_LinkedViewers);
        while (i.hasNext())
             i.next()->ClearLinkedImageViewers(this);

        m_LinkedViewers.clear();
    }
}

void ImageViewerWidget::UpdateFromLinkedViewer(QtAddons::ImageViewerWidget *w)
{
    float level_low, level_high;
    w->get_levels(&level_low, &level_high);
    set_levels(level_low, level_high,false);
}

void ImageViewerWidget::UpdateLinkedViewers()
{
    if (!m_LinkedViewers.empty())
    {
        QSetIterator<ImageViewerWidget *> i(m_LinkedViewers);
         while (i.hasNext())
             i.next()->UpdateFromLinkedViewer(this);
    }
}



}

