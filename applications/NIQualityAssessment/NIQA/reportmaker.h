#ifndef REPORTMAKER_H
#define REPORTMAKER_H

#include <string>

#include <QString>
#include <QChartView>
#include <QTextDocument>
#include <QTextCursor>


#include "niqaconfig.h"

class ReportMaker
{
public:
    ReportMaker();
    void makeReport(QString reportName, NIQAConfig &config);
    void addContrastInfo();
    void addEdge2DInfo(QtCharts::QChartView *c);
    void addEdge3DInfo();
    void addBallsInfo();
private:
    void makeInfoSection();
    void makeContrastSection(bool active);
    void makeEdge2DSection(bool active);
    void makeEdge3DSection(bool active);
    void makeBallsSection(bool active);
    void insertFigure(QtCharts::QChartView *cv,int width,bool nl);
    void insertPageBreak();



    NIQAConfig mConfig;
    QtCharts::QChartView *edgechart;
    QTextDocument doc;
    QTextCursor cursor;

};

#endif // REPORTMAKER_H
