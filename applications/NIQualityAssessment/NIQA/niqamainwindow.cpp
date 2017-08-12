#include "niqamainwindow.h"
#include "ui_niqamainwindow.h"

NIQAMainWindow::NIQAMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NIQAMainWindow)
{
    ui->setupUi(this);
}

NIQAMainWindow::~NIQAMainWindow()
{
    delete ui;
}
