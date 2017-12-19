#ifndef ROIWIDGET_H
#define ROIWIDGET_H

#include <QWidget>

#include <logging/logger.h>

namespace Ui {
class ROIWidget;
}

class ROIWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit ROIWidget(QWidget *parent = 0);
    ~ROIWidget();

    void setROI(int x0,int y0, int x1, int y1);
    void setROI(int *roi);
    void setROI(size_t *roi);


private slots:
    void on_spinX0_valueChanged(int arg1);

    void on_spinY0_valueChanged(int arg1);

    void on_spinX1_valueChanged(int arg1);

    void on_spinY1_valueChanged(int arg1);

    void on_buttonGetROI_clicked();

private:
    Ui::ROIWidget *ui;
    int roi[4];

signals:
    void getROIClicked(void);
    void valueChanged(int x0,int y0, int x1, int y1);
};

#endif // ROIWIDGET_H
