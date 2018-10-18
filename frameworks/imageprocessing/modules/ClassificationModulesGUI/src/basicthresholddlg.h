#ifndef BASICTHRESHOLDDLG_H
#define BASICTHRESHOLDDLG_H

#include <QDialog>

namespace Ui {
class BasicThresholdDlg;
}

class BasicThresholdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit BasicThresholdDlg(QWidget *parent = nullptr);
    ~BasicThresholdDlg();

private:
    Ui::BasicThresholdDlg *ui;
};

#endif // BASICTHRESHOLDDLG_H
