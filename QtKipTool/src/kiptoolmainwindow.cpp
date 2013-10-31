#include "kiptoolmainwindow.h"
#include "ui_kiptoolmainwindow.h"

KipToolMainWindow::KipToolMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KipToolMainWindow)
{
    ui->setupUi(this);
}

KipToolMainWindow::~KipToolMainWindow()
{
    delete ui;
}
