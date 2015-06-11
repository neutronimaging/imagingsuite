#ifndef SAVEPROJECTIONSDLG_H
#define SAVEPROJECTIONSDLG_H

#include <QDialog>

namespace Ui {
class SaveProjectionsDlg;
}

class SaveProjectionsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SaveProjectionsDlg(QWidget *parent = 0);
    ~SaveProjectionsDlg();

private slots:
    void on_buttonBrowse_clicked();

private:
    Ui::SaveProjectionsDlg *ui;
};

#endif // SAVEPROJECTIONSDLG_H
