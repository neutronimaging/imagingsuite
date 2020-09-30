//<LICENSE>

#ifndef REMOVEBACKGROUNDDLG_H
#define REMOVEBACKGROUNDDLG_H

#include <QDialog>

namespace Ui {
class RemoveBackgroundDlg;
}

class RemoveBackgroundDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveBackgroundDlg(QWidget *parent = nullptr);
    ~RemoveBackgroundDlg();

private:
    Ui::RemoveBackgroundDlg *ui;
};

#endif // REMOVEBACKGROUNDDLG_H
