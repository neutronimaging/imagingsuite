#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <map>
#include <QWidget>
#include <QLineSeries>



namespace Ui {
class PlotWidget;
}
namespace QtAddons {
class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();

    void setCurveData(int id, const QVector<QPointF> &data, QString name = "");
    void setCurveData(int id, float const * const x, float const * const y, const int N, QString name = "");
    void setCurveData(int id, float const * const x, size_t const * const y, const int N, QString name ="");
    void setCurveData(int id, QtCharts::QLineSeries *series);
    void clearCurve(int id);
    void clearAllCurves();
    void showLegend();
    void hideLegend();
    void setTitle(QString title);
    void hideTitle();

//    void setPlotCursor(int id, PlotCursor c);
//    void clearPlotCursor(int id);
//    void clearAllPlotCursors();

private:
    Ui::PlotWidget *ui;
    std::map<int, QtCharts::QAbstractSeries *> seriesmap;
};

}
#endif // PLOTWIDGET_H
