#include "reportgeneratorbase.h"
#include <sstream>
#include <ctime>

namespace QtAddons {
ReportGeneratorBase::ReportGeneratorBase(std::string name) :
    logger(name),
    m_fLine(0.0),
    m_fMargin(10.0f),
    m_fLineHeight(14.0),
    m_FontColor(QColor("black"))
{

}

void ReportGeneratorBase::OpenDestination(QString fname)
{
    m_Printer.setOutputFormat(QPrinter::PdfFormat);
    m_Printer.setPageSize(QPageSize(QPageSize::A4));
    m_Printer.setPageMargins(QMarginsF(m_fMargin,m_fMargin,m_fMargin,m_fMargin),QPageLayout::Millimeter);
    m_Printer.setOutputFileName(fname);

   m_fWidth  = m_Printer.width();
   m_fHeight = m_Printer.width();
}

int ReportGeneratorBase::DrawImage(float * /*pImg*/, int * /*srcdims*/, int * /*dstdims*/, int /*x*/,int /*y*/, bool /*drawframe*/)
{
//    size_t imgdims[4]={size_t(srcdims[0]),size_t(srcdims[1]),size_t(srcdims[2]),size_t(srcdims[3])};

//    m_ImagePainter.setImage(pImg,imgdims);

//    m_ImagePainter.Render(m_Painter,x,y,dstdims[0],dstdims[1]);
    return 0;
}

double ReportGeneratorBase::MakeHeader(const std::string name)
{

    std::ostringstream msg;

    QFont font("helvetic",10);
    QFontMetrics fm(font);
    msg<<"Reporting "<<name;
    m_Painter.drawText(0,fm.height(),QString::fromStdString(msg.str()));

    char timestr[256];
    time_t currenttime=0;
    time(&currenttime);
    struct tm* timeinfo;
    timeinfo=localtime(&currenttime);

    strftime(timestr,256,"%Y-%m-%d, %X",timeinfo);
    //msg.str(""); msg<<"Processing date: "<<timestr;
    msg.str(""); msg<<timestr;
    QString str=QString::fromStdString(msg.str());
//    int w=fm.horizontalAdvance(str);

    auto br=fm.boundingRect("-0.0000");
    int w = br.width();
    m_Painter.drawText(m_fWidth-w-2*m_fMargin,fm.height(),str);
    m_Painter.drawLine(0,fm.height()*1.5,m_fWidth, fm.height()*1.5);

    m_fLine+=fm.height()*2.5;
    return 0;
}

double ReportGeneratorBase::Print(std::string text, double size)
{
    size_t p1=text.find('\n');
    std::string t0;
    std::string t1=text;

    m_Painter.setPen(m_FontColor);

    QFont font("helvetic",size);
    m_Painter.setFont(font);

    QFontMetrics fm(font);

    double height=fm.height();

    do {
        if (p1==std::string::npos) {
            t0=t1;
            t1.clear();
        }
        else {
            t0=t1.substr(0,p1);
            t1=t1.substr(p1+1);
        }

        Print(t0,0,m_fLine,size);
        p1=t1.find('\n');
        m_fLine+=1.25*height;

    } while (!t1.empty());

    return m_fLine;
}

double ReportGeneratorBase::Print(std::string text, double x, double y, double size)
{
    m_Painter.setPen(m_FontColor);
    m_Painter.setFont(QFont("helvetic",size));
    m_Painter.drawText(x,y,QString::fromStdString(text));

    return 0.0;
}

}
