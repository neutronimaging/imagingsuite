#ifndef EDGEFITTINGDIALOG_H
#define EDGEFITTINGDIALOG_H

#include <QDialog>

namespace Ui {
class edgefittingdialog;
}

class edgefittingdialog : public QDialog
{
    Q_OBJECT

public:
    explicit edgefittingdialog(QWidget *parent = 0);
    ~edgefittingdialog();

private:
    Ui::edgefittingdialog *ui;
};

#endif // EDGEFITTINGDIALOG_H
