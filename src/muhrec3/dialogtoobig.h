#ifndef DIALOGTOOBIG_H
#define DIALOGTOOBIG_H

#include <QDialog>

namespace Ui {
class DialogTooBig;
}

class DialogTooBig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTooBig(QWidget *parent = 0);
    ~DialogTooBig();
    void SetFields(QString path, QString mask, bool reconAll, int first, int last, int roiMin, int roiMax);
    QString GetPath();
    QString GetMask();
    int GetFirst();
    int GetLast();


private slots:
    void on_button_browse_clicked();

    void on_check_reconAll_toggled(bool checked);

private:
    Ui::DialogTooBig *ui;

};

#endif // DIALOGTOOBIG_H
