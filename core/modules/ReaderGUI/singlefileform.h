#ifndef SINGLEFILEFORM_H
#define SINGLEFILEFORM_H

#include <QWidget>

namespace Ui {
class SingleFileForm;
}

class SingleFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit SingleFileForm(QWidget *parent = 0);
    ~SingleFileForm();

    void setFileName(QString fn);
    void setFileName(std::string fn);

    std::string getFileName();

    void setLabel(QString str);
private slots:
    void on_pushButton_browse_clicked();

private:
    Ui::SingleFileForm *ui;
};

#endif // SINGLEFILEFORM_H
