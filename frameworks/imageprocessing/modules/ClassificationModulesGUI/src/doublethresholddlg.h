#ifndef DOUBLETHRESHOLDDLG_H
#define DOUBLETHRESHOLDDLG_H

#include <QDialog>

namespace Ui {
class DoubleThresholdDlg;
}

class DoubleThresholdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DoubleThresholdDlg(QWidget *parent = nullptr);
    ~DoubleThresholdDlg();

private:
    Ui::DoubleThresholdDlg *ui;
};

#endif // DOUBLETHRESHOLDDLG_H
