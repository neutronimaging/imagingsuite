#ifndef VIEWERMAINWINDOW_H
#define VIEWERMAINWINDOW_H

#include <string>
#include <QMainWindow>

#include <base/timage.h>
#include <io/analyzefileext.h>

namespace Ui {
class ViewerMainWindow;
}

class ViewerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewerMainWindow(QWidget *parent = 0);
    ~ViewerMainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_horizontalSlider_sliderMoved(int position);

    void on_spinBox_valueChanged(int arg1);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    void LoadImage(std::string fname,kipl::base::TImage<float,2> &img);
    Ui::ViewerMainWindow *ui;

    std::string m_fname;
    kipl::io::eExtensionTypes m_ext;
};

#endif // VIEWERMAINWINDOW_H
