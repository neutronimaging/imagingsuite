#ifndef IMAGINGTOOLMAIN_H
#define IMAGINGTOOLMAIN_H

#include <QMainWindow>

namespace Ui {
class ImagingToolMain;
}

class ImagingToolMain : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ImagingToolMain(QWidget *parent = 0);
    ~ImagingToolMain();
    
private:
    Ui::ImagingToolMain *ui;
};

#endif // IMAGINGTOOLMAIN_H
