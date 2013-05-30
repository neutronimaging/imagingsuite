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

private slots:
    void TestClicked();
    void PlotClicked();


    void on_ShowImageButton_clicked();
    void on_GetROIButton_clicked();
    void GetModulesClicked();
};

#endif // MAINWINDOW_H
