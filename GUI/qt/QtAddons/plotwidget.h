#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "QtAddons_global.h"

#include <map>
#include <QWidget>
#include <QLineSeries>
#include <QAction>

#include "callout.h"
#include "plotcursor.h"


namespace Ui {
class PlotWidget;
}
namespace QtAddons {
class QTADDONSSHARED_EXPORT PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();

    void setCurveData(int id, const QVector<QPointF> &data, QString name = "");
    void setCurveData(int id, float const * const x, float const * const y, const int N, QString name = "");
    void setCurveData(int id, float const * const x, size_t const * const y, const int N, QString name ="");
    /// \brief Sets a plot line using the QLineSeries object
    /// \param id The index number of the plot to set
    /// \param series the plot data
    /// \param deleteData A flag to indicate that the series container shall be deleted in the method. This is in particular necessary for the update case.
    void setCurveData(int id, QtCharts::QLineSeries *series, bool deleteData=true);
    void clearCurve(int id);
    void clearAllCurves();
    void setPointsVisible(int n=-1);
    void showLegend();
    void hideLegend();
    void showCurve(int n);
    void hideCurve(int n);
    void setTitle(QString title);
    void hideTitle();
    void updateAxes();

    void setCursor(int id, PlotCursor *c);
    void clearCursors(int id);
    void clearAllCursors();

//    void setPlotCursor(int id, PlotCursor c);
//    void clearPlotCursor(int id);
//    void clearAllPlotCursors();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void setupActions();

private slots:
    void savePlot();
    void copy();
    void saveCurveData();

private:
    void findMinMax();
    void keepCallout();
    void tooltip(QPointF point, bool state);

    Ui::PlotWidget *ui;
    std::map<int, QtCharts::QAbstractSeries *> seriesmap;
    std::map<int, PlotCursor *> cursors;

    int m_nPointsVisible;
    double minX=std::numeric_limits<double>::max();
    double maxX=-std::numeric_limits<double>::max();
    double minY=std::numeric_limits<double>::max();
    double maxY=-std::numeric_limits<double>::max();

    QAction *savePlotAct;
    QAction *copyAct;
    QAction *savePlotDataAct;

    QList<Callout *> m_callouts;
    Callout *m_tooltip;
};

}
#endif // PLOTWIDGET_H
