#include "reportmaker.h"

#include <QDebug>
#include <sstream>

#include <QTextDocument>
#include <QTextCursor>
#include <QPixmap>
#include <QImage>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QMarginsF>
#endif

ReportMaker::ReportMaker() :
    edgechart(nullptr),
    cursor(&doc)
{

}

void ReportMaker::makeReport(QString reportName, NIQAConfig &config)
{
   mConfig=config;
   QPrinter printer(QPrinter::PrinterResolution);
   printer.setOutputFormat(QPrinter::PdfFormat);
   printer.setPaperSize(QPrinter::A4);
   printer.setOutputFileName(reportName);
   printer.setPageMargins(QMarginsF(25.0,25.0,25.0,25.0));

   doc.clear();
   doc.setDefaultFont(QFont("Helvetica",11));

   makeInfoSection();
   makeContrastSection(mConfig.contrastAnalysis.makeReport);
   makeEdge2DSection(mConfig.edgeAnalysis2D.makeReport);
   makeEdge3DSection(mConfig.edgeAnalysis3D.makeReport);
   makeBallsSection(mConfig.ballPackingAnalysis.makeReport);

   doc.print(&printer);
}

void ReportMaker::addEdge2DInfo(QtCharts::QChartView *c)
{
    edgechart=c;

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

    qDebug() << QString::fromStdString(mConfig.userInformation.comment);
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
             <<"<tr><td>File mask:</td><td>"<<mConfig.contrastAnalysis.fileMask<<"</td></tr>"
             <<"<tr><td>First:</td><td>"<<mConfig.contrastAnalysis.first<<"</td></tr>"
             <<"<tr><td>Last:</td><td>"<<mConfig.contrastAnalysis.last<<"</td></tr>"
             <<"<tr><td>Step:</td><td>"<<mConfig.contrastAnalysis.step<<"</td></tr>"
             <<"<tr><td>Pixel size:</td><td>"<<mConfig.contrastAnalysis.pixelSize<<" mm</td></tr>"
             <<"</table>"<<std::endl;

        text<<"<h3>Analysis results</h3><br/>";
        cursor.insertHtml(QString::fromStdString(text.str()));
        cursor.insertText("\n");
        insertFigure(edgechart,400,true);

    }
}

void ReportMaker::makeEdge3DSection(bool active)
{

}

void ReportMaker::makeBallsSection(bool active)
{

}

void ReportMaker::insertFigure(QtCharts::QChartView *cv, int width, bool nl)
{
    QImage img(1024,768,QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&img);
    cv->render(&painter);
    painter.end();

    if (0<width)
        img = img.scaledToWidth(width, Qt::SmoothTransformation );

    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertImage(img);
    if (nl==true)
        cursor.insertHtml("<br>");
}

void ReportMaker::insertPageBreak()
{
     cursor.insertHtml("<div class=\"page-break\"></div> ");
  //  cursor.movePosition(QTextCursor::End);
 //   cursor.insertHtml("<p style=\"page-break-after: always;\"> .</p>");
    cursor.insertText("\n");
}
