#ifndef CONFIGUREGEOMETRYDIALOG_H
#define CONFIGUREGEOMETRYDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigureGeometryDialog;
}

class ConfigureGeometryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigureGeometryDialog(QWidget *parent = 0);
    ~ConfigureGeometryDialog();
    
private:
    Ui::ConfigureGeometryDialog *ui;
};

#endif // CONFIGUREGEOMETRYDIALOG_H
