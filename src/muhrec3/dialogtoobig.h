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
    void SetFields(QString path, QString mask);
    QString GetPath();
    QString GetMask();


private slots:
    void on_button_browse_clicked();

private:
    Ui::DialogTooBig *ui;

};

#endif // DIALOGTOOBIG_H
