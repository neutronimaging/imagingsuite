#ifndef REPORTGENERATORBASE_H
#define REPORTGENERATORBASE_H
#include <logging/logger.h>

#include <string>
#include <QPainter>
#include <QColor>
#include <QPrinter>
#include <QString>
#include "imagepainter.h"
#include "plotpainter.h"

namespace QtAddons {
class ReportGeneratorBase
{
protected:
    kipl::logging::Logger logger;

public:
    ReportGeneratorBase(std::string name="ReportGenerator");

protected:
    void OpenDestination(QString fname);
    QPrinter m_Printer;
    QPainter m_Painter;
    ImagePainter m_ImagePainter;
    double m_fWidth;
    double m_fHeight;
    double m_fLine;
    double m_fMargin;
    double m_fLineHeight;
    QColor m_FontColor;
    int DrawImage(float *pImg, int *srcdims, int *dstdims, int x,int y, bool drawframe=false);
    virtual double MakeHeader(const std::string name);
    double Print(std::string text, double size=9.0);
    double Print(std::string text, double x, double y, double size=9.0);
};

}
#endif // REPORTGENERATORBASE_H

