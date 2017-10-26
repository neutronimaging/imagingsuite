#ifndef NIQAMAINWINDOW_H
#define NIQAMAINWINDOW_H

#include <QMainWindow>

#include <base/timage.h>

#include <ballanalysis.h>

namespace Ui {
class NIQAMainWindow;
}

class NIQAMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NIQAMainWindow(QWidget *parent = 0);
    ~NIQAMainWindow();

private slots:
    void on_button_bigball_load_clicked();

    void on_slider_bigball_slice_sliderMoved(int position);

    void on_spin_bigball_slice_valueChanged(int arg1);

private:
    Ui::NIQAMainWindow *ui;

    kipl::base::TImage<float,3> m_BigBall;
    kipl::base::TImage<float,3> m_BallAssembly;

    std::map<float, kipl::base::TImage<float,2>> m_Edges;

    ImagingQAAlgorithms::BallAnalysis m_BallAnalyzer;

};

#endif // NIQAMAINWINDOW_H
