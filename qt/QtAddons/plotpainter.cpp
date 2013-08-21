#include "plotpainter.h"

namespace painter {
PlotPainter::PlotPainter(QWidget *parent):
    m_pParent(parent),
    logger("PlotPainter")
{
}

PlotPainter::~PlotPainter()
{

}

void PlotPainter::Render(QPainter &painter, int x, int y, int w, int h)
{

}

}
