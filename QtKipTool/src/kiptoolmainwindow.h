#ifndef KIPTOOLMAINWINDOW_H
#define KIPTOOLMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class KipToolMainWindow;
}

class KipToolMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit KipToolMainWindow(QWidget *parent = 0);
    ~KipToolMainWindow();
    
private:
    Ui::KipToolMainWindow *ui;
};

#endif // KIPTOOLMAINWINDOW_H
