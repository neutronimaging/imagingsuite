#include "issfilterdlg.h"
#include "ui_issfilterdlg.h"

ISSFilterDlg::ISSFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("ISSFilter", true, true, true,parent),
    ui(new Ui::ISSFilterDlg)
{
    ui->setupUi(this);
}

ISSFilterDlg::~ISSFilterDlg()
{
    delete ui;
}
