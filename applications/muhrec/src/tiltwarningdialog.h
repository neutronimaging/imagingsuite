#ifndef TILTWARNINGDIALOG_H
#define TILTWARNINGDIALOG_H

#include <QDialog>

namespace Ui {
class TiltWarningDialog;
}

class TiltWarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TiltWarningDialog(QWidget *parent = nullptr);
    ~TiltWarningDialog();

private:
    Ui::TiltWarningDialog *ui;
};

#endif // TILTWARNINGDIALOG_H
