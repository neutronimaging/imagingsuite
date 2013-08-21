#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <logging/logger.h>

namespace painter {
class PlotWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit PlotWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    
};
}
#endif // PLOTWIDGET_H
