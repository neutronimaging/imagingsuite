#ifndef REPORTMAKER_H
#define REPORTMAKER_H

#include <string>
#include <map>
#include <vector>

#include <QString>
#include <QChartView>
#include <QTextDocument>
#include <QTextCursor>

#include <math/statistics.h>

#include "niqaconfig.h"

class ReportMaker
{
public:
    ReportMaker();
    void makeReport(QString reportName, NIQAConfig &config);
    void addContrastInfo(QtCharts::QChartView *c, std::vector<kipl::math::Statistics> insets);
    void addEdge2DInfo(QtCharts::QChartView *c, QtCharts::QChartView *d, std::map<double, double> edges);
    void addEdge3DInfo(QtCharts::QChartView *c, double FWHM, double radius);
    void addBallsInfo();
private:
    void makeInfoSection();
    void makeContrastSection(bool active);
    void makeEdge2DSection(bool active);
    void makeEdge3DSection(bool active);
    void makeBallsSection(bool active);
    void insertFigure(QtCharts::QChartView *cv, QString imgname, int width, bool nl);
    void insertPageBreak();



    NIQAConfig mConfig;
    QtCharts::QChartView *contrast_plot;
    std::vector<kipl::math::Statistics> contrast_insets;

    QtCharts::QChartView *edge2d_edgeplots;
    QtCharts::QChartView *edge2d_collimation;
    std::map<double,double> edge2d_edges;

    QtCharts::QChartView *edge3d_edgeplot;
    double edge3d_FWHM;
    double edge3d_radius;

    QTextDocument doc;
    QTextCursor cursor;

};

#endif // REPORTMAKER_H
