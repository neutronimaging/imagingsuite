#ifndef NIQAMAINWINDOW_H
#define NIQAMAINWINDOW_H

#include <QMainWindow>

#include <tnt.h>
#include <base/timage.h>
#include <logging/logger.h>

#include <loggingdialog.h>

#include <ballanalysis.h>
#include <contrastsampleanalysis.h>
#include <ballassemblyanalysis.h>
#include <math/nonlinfit.h>

#include "niqaconfig.h"

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

    void on_button_get2Dedges_clicked();

    void on_button_estimateCollimation_clicked();

    void on_check_3DBallsCrop_toggled(bool checked);

    void on_pushButton_contrast_pixelSize_clicked();

    void on_pushButton_logging_clicked();

    void on_pushButton_2dEdge_pixelSize_clicked();

    void on_actionNew_triggered();

    void on_actionLoad_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionQuit_triggered();

    void on_pushButton_createReport_clicked();

    void on_comboBox_edgeFitFunction_currentIndexChanged(int index);

    void on_comboBox_edgePlotType_currentIndexChanged(int index);

    void on_radioButton_contrast_interval_toggled(bool checked);

    void on_radioButton_contrast_scaling_toggled(bool checked);

    void on_spin_contrast_intensity0_valueChanged(double arg1);

    void on_spin_contrast_intensity1_valueChanged(double arg1);

    void on_actionAbout_triggered();

    void on_actionUser_manual_triggered();

    void on_actionReport_a_bug_triggered();

    void on_pushButton_bigball_pixelsize_clicked();

    void on_comboBox_bigball_plotinformation_currentIndexChanged(int index);

private:
    void showContrastBoxPlot();
    void showContrastHistogram();

    void on_widget_roiEdge2D_getROIclicked();
    void on_widget_roiEdge2D_valueChanged(int x0, int y0, int x1, int y1);
    void on_widget_roi3DBalls_getROIclicked();
    void on_widget_roi3DBalls_valueChanged(int x0, int y0, int x1, int y1);
    void getEdge2Dprofiles();
    void estimateResolutions();
    void fitEdgeProfile(std::vector<float> &dataX, std::vector<float> &dataY, std::vector<float> &dataSig, Nonlinear::FitFunctionBase &fitFunction);
    void fitEdgeProfile(TNT::Array1D<double> &dataX, TNT::Array1D<double> &dataY, TNT::Array1D<double> &dataSig, Nonlinear::FitFunctionBase &fitFunction);
    void plotEdgeProfiles();
    void plotPackingStatistics(std::list<kipl::math::Statistics> &roiStats);
    void plot3DEdgeProfiles(int index);
    void saveCurrent();
    void loadCurrent();

    void updateConfig();
    void updateDialog();
    void saveConfig(std::string fname);

    Ui::NIQAMainWindow *ui;
    QtAddons::LoggingDialog *logdlg;

    std::string configFileName;

    NIQAConfig config;

    kipl::base::TImage<float,3> m_BigBall;
    kipl::base::TImage<float,3> m_BallAssembly;
    kipl::base::TImage<float,2> m_BallAssemblyProjection;
    kipl::base::TImage<float,3> m_Contrast;

    map<float,std::vector<float>> m_Edges2D;
    // 3D Edge
    std::vector<float> m_edge3DDistance;
    std::vector<float> m_edge3DProfile;
    std::vector<float> m_edge3DDprofile;
    std::vector<float> m_edge3DStdDev;

    ImagingQAAlgorithms::BallAnalysis m_BallAnalyzer;
    ImagingQAAlgorithms::ContrastSampleAnalysis m_ContrastSampleAnalyzer;
    ImagingQAAlgorithms::BallAssemblyAnalysis m_BallAssemblyAnalyzer;

};

#endif // NIQAMAINWINDOW_H
