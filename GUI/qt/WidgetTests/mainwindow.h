#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <logging/logger.h>

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

};

#endif // MAINWINDOW_H
