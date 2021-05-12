//#include <QtGui/QApplication>
#include <QApplication>
#include "mainwindow.h"

#include <QRect>
#include <QPoint>
#include <iostream>

void testQRect();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    testQRect();
    MainWindow w;
    w.show();
    
    return a.exec();
}

void testQRect()
{
    using namespace std;
    QRect rubberBandRect(100,90,80,70);

    const int w=10;
    QRect r0 = QRect(rubberBandRect.x()-w,
                     rubberBandRect.y()-w,
                     rubberBandRect.width()+2*w,
                     rubberBandRect.height()+2*w);
    QRect r1 = QRect(rubberBandRect.x()+w,
                     rubberBandRect.y()+w,
                     rubberBandRect.width()-2*w,
                     rubberBandRect.height()-2*w);

    QPoint q0=QPoint(20,20);
    QPoint q1=QPoint(100,90);
    QPoint q2=QPoint(120,120);

    std::cout<<r0.contains(q0)<<", "<<r1.contains(q0)<<std::endl;
    std::cout<<r0.contains(q1)<<", "<<r1.contains(q1)<<std::endl;
    std::cout<<r0.contains(q2)<<", "<<r1.contains(q2)<<std::endl;

}
