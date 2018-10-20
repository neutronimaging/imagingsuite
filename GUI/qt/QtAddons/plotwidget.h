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
    void setTitle(QString title);
    void hideTitle();
    void updateAxes();

//    void setPlotCursor(int id, PlotCursor c);
//    void clearPlotCursor(int id);
//    void clearAllPlotCursors();

private:
    Ui::PlotWidget *ui;
    std::map<int, QtCharts::QAbstractSeries *> seriesmap;
    int m_nPointsVisible;
};

}
#endif // PLOTWIDGET_H
