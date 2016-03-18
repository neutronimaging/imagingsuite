#ifndef VIEWERMAINWINDOW_H
#define VIEWERMAINWINDOW_H

#include <string>
#include <QMainWindow>

#include <base/timage.h>

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

private:
    void LoadImage(std::string fname,kipl::base::TImage<float,2> &img);
    Ui::ViewerMainWindow *ui;
};

#endif // VIEWERMAINWINDOW_H
