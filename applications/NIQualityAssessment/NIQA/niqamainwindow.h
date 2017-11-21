#ifndef NIQAMAINWINDOW_H
#define NIQAMAINWINDOW_H

#include <QMainWindow>

#include <base/timage.h>
#include <logging/logger.h>

#include <ballanalysis.h>
#include <contrastsampleanalysis.h>
#include <ballassemblyanalysis.h>

namespace Ui {
class NIQAMainWindow;
}

class NIQAMainWindow : public QMainWindow
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit NIQAMainWindow(QWidget *parent = 0);
    ~NIQAMainWindow();

private slots:
    void on_button_bigball_load_clicked();

    void on_slider_bigball_slice_sliderMoved(int position);

    void on_spin_bigball_slice_valueChanged(int arg1);

    void on_button_contrast_load_clicked();

    void on_slider_contrast_images_sliderMoved(int position);

    void on_spin_contrast_images_valueChanged(int arg1);

    void on_combo_contrastplots_currentIndexChanged(int index);

    void on_button_AnalyzeContrast_clicked();

    void on_button_addEdgeFile_clicked();

    void on_button_deleteEdgeFile_clicked();

    void on_listEdgeFiles_doubleClicked(const QModelIndex &index);

    void on_listEdgeFiles_clicked(const QModelIndex &index);

    void on_button_LoadPacking_clicked();

    void on_button_AnalyzePacking_clicked();

    void on_slider_PackingImages_sliderMoved(int position);

    void on_combo_PackingImage_currentIndexChanged(int index);

private:
    void showContrastBoxPlot();
    void showContrastHistogram();

    Ui::NIQAMainWindow *ui;

    kipl::base::TImage<float,3> m_BigBall;
    kipl::base::TImage<float,3> m_BallAssembly;
    kipl::base::TImage<float,3> m_Contrast;

    std::map<float, kipl::base::TImage<float,2>> m_Edges;

    ImagingQAAlgorithms::BallAnalysis m_BallAnalyzer;
    ImagingQAAlgorithms::ContrastSampleAnalysis m_ContrastSampleAnalyzer;
    ImagingQAAlgorithms::BallAssemblyAnalysis m_BallAssemblyAnalyzer;

};

#endif // NIQAMAINWINDOW_H
