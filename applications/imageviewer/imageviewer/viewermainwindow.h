#ifndef VIEWERMAINWINDOW_H
#define VIEWERMAINWINDOW_H

#include <string>
#include <QMainWindow>

#include <base/timage.h>
#include <io/analyzefileext.h>
#include <logging/logger.h>
#include <loggingdialog.h>

namespace Ui {
class ViewerMainWindow;
}

class ViewerMainWindow : public QMainWindow
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ViewerMainWindow(QWidget *parent = 0);
    ~ViewerMainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_horizontalSlider_sliderMoved(int position);

    void on_spinBox_valueChanged(int arg1);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    void on_actionSave_as_triggered();

    void on_pushButton_showLog_clicked();

    void on_actionMeasure_pixel_size_triggered();

private:
    void LoadImage(std::string fname,kipl::base::TImage<float,2> &img);
    void updateView(const std::string &fname);
    Ui::ViewerMainWindow *ui;
    QtAddons::LoggingDialog logdlg;

    std::string m_fname;
    kipl::base::TImage<float,2> currentImage;
    kipl::io::eExtensionTypes m_ext;
    bool isMultiFrame;
};

#endif // VIEWERMAINWINDOW_H
