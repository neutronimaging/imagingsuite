#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <logging/logger.h>
#include <loggingdialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QtAddons::LoggingDialog *logdlg;

    float m_fScale;

        void on_roiwidget_getROIclicked();
        void on_roiwidget_valueChanged(int x0, int y0, int x1, int y1);

private slots:
    void TestClicked();
    void PlotClicked();


    void on_ShowImageButton_clicked();
    void on_GetROIButton_clicked();
    void GetModulesClicked();
    void on_check_linkimages_toggled(bool checked);
    void on_pushButton_listdata_clicked();

    void on_button_ListAllROIs_clicked();
    void on_button_ListSelectedROIs_clicked();
    void on_pushButton_showLogger_clicked();
    void on_pushButton_clicked();
    void on_checkBox_stride_toggled(bool checked);
    void on_checkBox_repeat_toggled(bool checked);
    void on_checkBox_roi_toggled(bool checked);
    void on_checkBox_fliprot_toggled(bool checked);
    void on_pushButton_getFormInfo_clicked();
};

#endif // MAINWINDOW_H
