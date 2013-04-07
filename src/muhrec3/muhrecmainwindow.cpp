#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"

MuhRecMainWindow::MuhRecMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MuhRecMainWindow)
{
    ui->setupUi(this);
}

MuhRecMainWindow::~MuhRecMainWindow()
{
    delete ui;
}
