#include "muhrecmainwindow.h"
#include "ui_muhrecmainwindow.h"

MuhRecMainWindow::MuhRecMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MuhRecMainWindow)
{
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*(ui->logviewer));
}

MuhRecMainWindow::~MuhRecMainWindow()
{
    delete ui;
}
