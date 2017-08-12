#ifndef NIQAMAINWINDOW_H
#define NIQAMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class NIQAMainWindow;
}

class NIQAMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NIQAMainWindow(QWidget *parent = 0);
    ~NIQAMainWindow();

private:
    Ui::NIQAMainWindow *ui;
};

#endif // NIQAMAINWINDOW_H
