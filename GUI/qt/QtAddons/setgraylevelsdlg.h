#ifndef SETGRAYLEVELSDLG_H
#define SETGRAYLEVELSDLG_H

#include <QDialog>

namespace Ui {
class SetGrayLevelsDlg;
}

namespace QtAddons {


class ImageViewerWidget;


class SetGrayLevelsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetGrayLevelsDlg(QWidget *parent = nullptr);
    ~SetGrayLevelsDlg();


private slots:
    void on_doubleSpinBox_levelLow_valueChanged(double arg1);

    void on_doubleSpinBox_levelHigh_valueChanged(double arg1);

    void on_doubleSpinBox_levelCenter_valueChanged(double arg1);

    void on_doubleSpinBox_levelWindow_valueChanged(double arg1);

private:
    void updateLevels(bool interval, double a, double b);
    Ui::SetGrayLevelsDlg *ui;
    ImageViewerWidget * m_parent;
    double histMin;
    double histMax;

};
}
#endif // SETGRAYLEVELSDLG_H
