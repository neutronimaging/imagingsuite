#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QWidget>
#include <imagepainter.h>

namespace QtAddons {

class ImageViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewerWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    
protected:
    ImagePainter m_ImagePainter;
};

}

#endif // IMAGEVIEWERWIDGET_H
