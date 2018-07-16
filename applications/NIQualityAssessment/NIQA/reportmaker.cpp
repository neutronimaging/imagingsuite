#include "reportmaker.h"

#include <QDebug>
#include <sstream>

#include <qtextdocument.h>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QMarginsF>
#endif

ReportMaker::ReportMaker()
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

//   QString svgfname="/Users/kaestner/test.svg";
//   QSvgGenerator generator;
//   generator.setFileName(svgfname);
//   generator.setSize(QSize(1024, 1024));
//   generator.setViewBox(QRect(0, 0, 1024, 1024));
//   generator.setTitle(tr("SVG Generator Example Drawing"));

//   QPainter painter;
//   painter.begin(&generator);
//   ui->widget->render(&painter);
//   painter.end();

    std::ostringstream text;


   text<<makeInfoSection();
   text<<makeContrastSection(mConfig.contrastAnalysis.makeReport);
   text<<makeEdge2DSection(mConfig.edgeAnalysis2D.makeReport);
   text<<makeEdge3DSection(mConfig.edgeAnalysis3D.makeReport);
   text<<makeBallsSection(mConfig.ballPackingAnalysis.makeReport);

//   doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
   QTextDocument doc;
   doc.setHtml(QString::fromStdString(text.str()));
   doc.print(&printer);
}

std::string ReportMaker::makeInfoSection()
{
    std::ostringstream text;

    text<<"<H1>NIQA Report</H1>"<<std::endl;
    text<<"Report date: "<<mConfig.userInformation.analysisDate[0]<<"-"<<mConfig.userInformation.analysisDate[1]<<"-"<<mConfig.userInformation.analysisDate[2]<<"<br/>";
    text<<"<H2>User information</H2>"<<std::endl;

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

    return text.str();
}

std::string ReportMaker::makeContrastSection(bool active)
{
    std::ostringstream text;

    if (active==true) {
        text<<"<p style=\"page-break-after: always;\">&nbsp;</p>"<<std::endl;
        text<<"<h2>Contrast sample</h2>"<<std::endl;
        text<<"<h3>Image description</h3>";
        text<<"<table style=\"padding: 10px\">"
             <<"<tr><td>File mask:</td><td>"<<mConfig.contrastAnalysis.fileMask<<"</td></tr>"
             <<"<tr><td>First:</td><td>"<<mConfig.contrastAnalysis.first<<"</td></tr>"
             <<"<tr><td>Last:</td><td>"<<mConfig.contrastAnalysis.last<<"</td></tr>"
             <<"<tr><td>Step:</td><td>"<<mConfig.contrastAnalysis.step<<"</td></tr>"
             <<"<tr><td>Pixel size:</td><td>"<<mConfig.contrastAnalysis.pixelSize<<" mm</td></tr>"
             <<"</table>"<<std::endl;

        text<<"<h3>Analysis results</h3>";

    }

    return text.str();
}

std::string ReportMaker::makeEdge2DSection(bool active)
{
    std::ostringstream text;

//    text<<"<H1>Neutron Imaging Quality Asssessment Report</H1>"<<std::endl;
//    text<<"<H2>User information</H2>"<<std::endl;
//    text<<"<p>";
//    text<<"User"<<mConfig.userInformation.userName<<std::endl;
//    text<<"</p>";

    return text.str();
}

std::string ReportMaker::makeEdge3DSection(bool active)
{
    std::ostringstream text;

//    text<<"<H1>Neutron Imaging Quality Asssessment Report</H1>"<<std::endl;
//    text<<"<H2>User information</H2>"<<std::endl;
//    text<<"<p>";
//    text<<"User"<<mConfig.userInformation.userName<<std::endl;
//    text<<"</p>";

    return text.str();
}

std::string ReportMaker::makeBallsSection(bool active)
{
    std::ostringstream text;

//    text<<"<H1>Neutron Imaging Quality Asssessment Report</H1>"<<std::endl;
//    text<<"<H2>User information</H2>"<<std::endl;
//    text<<"<p>";
//    text<<"User"<<mConfig.userInformation.userName<<std::endl;
//    text<<"</p>";

    return text.str();
}
