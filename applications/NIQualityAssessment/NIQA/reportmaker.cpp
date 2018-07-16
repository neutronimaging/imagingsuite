#include "reportmaker.h"

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

    text<<"<H1>Neutron Imaging Quality Asssessment Report</H1>"<<std::endl;
    text<<"<H2>User information</H2>"<<std::endl;
    text<<"<p>";
    text<<"User"<<mConfig.userInformation.userName<<std::endl;
    text<<"</p>";
}

std::string ReportMaker::makeContrastSection(bool active)
{

}

std::string ReportMaker::makeEdge2DSection(bool active)
{

}

std::string ReportMaker::makeEdge3DSection(bool active)
{

}

std::string ReportMaker::makeBallsSection(bool active)
{

}
