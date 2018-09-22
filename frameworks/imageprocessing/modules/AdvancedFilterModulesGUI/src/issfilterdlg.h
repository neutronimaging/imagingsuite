#ifndef ISSFILTERDLG_H
#define ISSFILTERDLG_H

#include <QDialog>

#include <ConfiguratorDialogBase.h>

namespace Ui {
class ISSFilterDlg;
}

class ISSFilterDlg : public ConfiguratorDialogBase
{
    Q_OBJECT

public:
    explicit ISSFilterDlg(QWidget *parent = nullptr);
    ~ISSFilterDlg();

private:
    Ui::ISSFilterDlg *ui;
};

#endif // ISSFILTERDLG_H
