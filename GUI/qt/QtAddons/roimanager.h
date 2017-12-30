#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include <QWidget>

namespace Ui {
class ROIManager;
}

class ROIManager : public QWidget
{
    Q_OBJECT

public:
    explicit ROIManager(QWidget *parent = 0);
    ~ROIManager();

private:
    Ui::ROIManager *ui;
};

#endif // ROIMANAGER_H
