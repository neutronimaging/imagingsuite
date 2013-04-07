#ifndef MUHRECMAINWINDOW_H
#define MUHRECMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MuhRecMainWindow;
}

class MuhRecMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MuhRecMainWindow(QWidget *parent = 0);
    ~MuhRecMainWindow();
    
private:
    Ui::MuhRecMainWindow *ui;
};

#endif // MUHRECMAINWINDOW_H
