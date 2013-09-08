#include "findskiplistdialog.h"
#include "ui_findskiplistdialog.h"

FindSkipListDialog::FindSkipListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindSkipListDialog)
{
    ui->setupUi(this);
}

FindSkipListDialog::~FindSkipListDialog()
{
    delete ui;
}
