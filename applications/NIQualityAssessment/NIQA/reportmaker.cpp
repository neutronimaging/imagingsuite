#include "reportmaker.h"

#include <QDebug>
#include <sstream>

#include <QTextDocument>
#include <QTextCursor>
#include <QPixmap>
#include <QImage>
#include <QSvgGenerator>
#include <QDir>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QMarginsF>
#endif

ReportMaker::ReportMaker() :
    edge2d_edgeplots(nullptr),
    cursor(&doc)
{

}

void ReportMaker::makeReport(QString reportName, NIQAConfig &config)
{
   mConfig=config;
   QPrinter printer(QPrinter::HighResolution);
   printer.setOutputFormat(QPrinter::PdfFormat);
   printer.setResolution(1200);
   printer.setPaperSize(QPrinter::A4);
   printer.setOutputFileName(reportName);
   printer.setPageMargins(QMarginsF(10.0,10.0,10.0,10.0));

   doc.clear();
   doc.setDefaultFont(QFont("Helvetica",10));

   makeInfoSection();
   makeContrastSection(mConfig.contrastAnalysis.makeReport);
   makeEdge2DSection(mConfig.edgeAnalysis2D.makeReport);
   makeEdge3DSection(mConfig.edgeAnalysis3D.makeReport);
   makeBallsSection(mConfig.ballPackingAnalysis.makeReport);

   doc.print(&printer);
}

void ReportMaker::addContrastInfo(QtCharts::QChartView *c, std::vector<kipl::math::Statistics> insets)
{
    contrast_plot=c;
    contrast_insets=insets;
}

void ReportMaker::addEdge2DInfo(QtCharts::QChartView *c, QtCharts::QChartView *d,std::map<double,double> edges)
{
    edge2d_edgeplots=c;
    edge2d_collimation=d;
    edge2d_edges=edges;
}

void ReportMaker::addEdge3DInfo(QtCharts::QChartView *c, double FWHM, double radius)
{
    edge3d_edgeplot=c;
    edge3d_FWHM=FWHM;
    edge3d_radius=radius;
}


void ReportMaker::makeInfoSection()
{
    std::ostringstream text;

    text<<"<h1>NIQA Report</h1>"<<std::endl;
    text<<"Report date: "<<mConfig.userInformation.analysisDate[0]<<"-"<<mConfig.userInformation.analysisDate[1]<<"-"<<mConfig.userInformation.analysisDate[2]<<"<br/>";
    text<<"<h2>User information</h2>"<<std::endl;

    text<<"<table style=\"padding: 5px\">"
        <<"<tr><td>Operator</td><td>"<<mConfig.userInformation.userName<<"</td></tr>"
        <<"<tr><td>Instrument</td><td>"<<mConfig.userInformation.instrument<<"</td></tr>"
        <<"<tr><td>Institute</td><td>"<<mConfig.userInformation.institute<<"</td></tr>"
        <<"<tr><td>Country</td><td>"<<mConfig.userInformation.country<<"</td></tr>"
        <<"<tr><td>Experiment date</td><td>"<<mConfig.userInformation.experimentDate[0]<<"-"<<mConfig.userInformation.experimentDate[1]<<"-"<<mConfig.userInformation.experimentDate[2]<<"</td></tr>"
        <<"<tr><td>Software version</td><td>"<<mConfig.userInformation.softwareVersion<<"</td></tr>"
        <<"</table>"<<std::endl;

    if (mConfig.userInformation.comment.empty()==false) {

        text<<"<h3>Commment</h3>"
            <<"<p>"<<mConfig.userInformation.comment<<"</p>"<<std::endl;
    }

    cursor.insertHtml(QString::fromStdString(text.str()));
}

void ReportMaker::makeContrastSection(bool active)
{


    if (active==true) {
        std::ostringstream text;
        insertPageBreak();
        text<<"<h2 style=\"page-break-before: always;\">Contrast sample</h2>"<<std::endl;
        text<<"<h3>Image description</h3>";
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><td>File mask:</td><td>"<<mConfig.contrastAnalysis.fileMask<<"</td></tr>"
             <<"<tr><td>First:</td><td>"<<mConfig.contrastAnalysis.first<<"</td></tr>"
             <<"<tr><td>Last:</td><td>"<<mConfig.contrastAnalysis.last<<"</td></tr>"
             <<"<tr><td>Step:</td><td>"<<mConfig.contrastAnalysis.step<<"</td></tr>"
             <<"<tr><td>Pixel size:</td><td>"<<mConfig.contrastAnalysis.pixelSize<<" mm</td></tr>"
             <<"</table>"<<std::endl;

        text<<"<h3>Analysis results</h3>";
        cursor.insertHtml(QString::fromStdString(text.str()));
        insertFigure(contrast_plot,"/Users/kaestner/niqacontrast.svg",200,true);
        text.str("");
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><th>Average</th><th>StdDev</th></tr>"<<std::endl;
        for (auto item : contrast_insets) {
            text<<"<tr><td>"<<item.E()<<"</td><td>"<<item.s()<<"</td></tr>"<<std::endl;
        }
        text<<"</table><br/>"<<std::endl;
        cursor.insertHtml(QString::fromStdString(text.str()));
    }
}

void ReportMaker::makeEdge2DSection(bool active)
{


    if (active==true) {
        std::ostringstream text;
        insertPageBreak();
        text<<"<h2>2D edge sample</h2>"<<std::endl;
        text<<"<h3>Image description</h3>";
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><th>Distance</th><th>File name</th></tr>"<<std::endl;
        for (auto item : mConfig.edgeAnalysis2D.multiImageList) {
            text<<"<tr><td>"<<item.first<<"</td><td>"<<item.second<<"</td></tr>"<<std::endl;
        }
        text<<"</table><br/>"<<std::endl;

        text<<"<table style=\"padding: 10px\">"
             <<"<tr><td>Pixel size:</td><td>"<<mConfig.contrastAnalysis.pixelSize<<" mm</td></tr>"
             <<"</table>"<<std::endl;

        text<<"<h3>Analysis results</h3><br/>";
        cursor.insertHtml(QString::fromStdString(text.str()));
        cursor.insertText("\n");
        insertFigure(edge2d_edgeplots,"/Users/kaestner/niqaedge2da.svg",200,true);
        insertFigure(edge2d_collimation,"/Users/kaestner/niqaedge2db.svg",200,true);
        text.str("");
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><th>Distance [mm]</th><th>FWHM [mm]</th></tr>"<<std::endl;
        for (auto item : edge2d_edges) {
            text<<"<tr><td>"<<item.first<<"</td><td>"<<item.second<<"</td></tr>"<<std::endl;
        }
        text<<"</table><br/>"<<std::endl;
        cursor.insertHtml(QString::fromStdString(text.str()));


    }
}

void ReportMaker::makeEdge3DSection(bool active)
{
    if (active==true) {
        std::ostringstream text;
        insertPageBreak();
        text<<"<h2 style=\"page-break-before: always;\">3D edge sample</h2>"<<std::endl;
        text<<"<h3>Image description</h3>";
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><td>File mask:</td><td>"<<mConfig.edgeAnalysis3D.fileMask<<"</td></tr>"
             <<"<tr><td>First:</td><td>"<<mConfig.edgeAnalysis3D.first<<"</td></tr>"
             <<"<tr><td>Last:</td><td>"<<mConfig.edgeAnalysis3D.last<<"</td></tr>"
             <<"<tr><td>Step:</td><td>"<<mConfig.edgeAnalysis3D.step<<"</td></tr>"
             <<"<tr><td>Pixel size:</td><td>"<<mConfig.edgeAnalysis3D.pixelSize<<" mm</td></tr>"
             <<"<tr><td>Sampling precision:</td><td>"<<mConfig.edgeAnalysis3D.precision<<" pixels</td></tr>"
             <<"</table>"<<std::endl;

        text<<"<h3>Analysis results</h3>";
        cursor.insertHtml(QString::fromStdString(text.str()));

        insertFigure(edge3d_edgeplot,"/Users/kaestner/niqaedge3d.svg",200,true);
        text.str("");
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><td>Ball radius:</td><td>"<<edge3d_radius<<" mm</td></tr>"
             <<"<tr><td>FWHM:</td><td>"<<edge3d_FWHM<<" mm</td></tr>"
             <<"</table>"<<std::endl;
        cursor.insertHtml(QString::fromStdString(text.str()));
    }
}

void ReportMaker::makeBallsSection(bool active)
{

}

void ReportMaker::insertFigure(QtCharts::QChartView *cv, QString imgname, int width, bool nl)
{
    QString svgfname=imgname;
    QSvgGenerator generator;
    generator.setFileName(svgfname);
    float aspectratio=float(cv->geometry().height())/float(cv->geometry().width());
    generator.setSize(QSize(1024, int(1024*aspectratio)));
    generator.setViewBox(QRect(0, 0, 1024, int(1024*aspectratio)));
    generator.setTitle("SVG Generator Example Drawing");

    QPainter painter(&generator);
    cv->render(&painter);
    painter.end();

    cursor.insertHtml("<img src=\""+imgname+"\" width=\""+QString::number(width)+"\" alt=\"Kiwi standing on oval\" />");
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

}

void ReportMaker::insertPageBreak()
{
     cursor.insertHtml("<div class=\"page-break\"></div> ");
  //  cursor.movePosition(QTextCursor::End);
 //   cursor.insertHtml("<p style=\"page-break-after: always;\"> .</p>");
    cursor.insertText("\n");
}
