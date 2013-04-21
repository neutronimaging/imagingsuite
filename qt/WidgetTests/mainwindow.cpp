#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("MainWindow"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    kipl::logging::Logger::AddLogTarget(*ui->LogView);
    connect(ui->TestButton,SIGNAL(clicked()),this,SLOT(TestClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::TestClicked()
{
    logger(kipl::logging::Logger::LogMessage,"Testing");
}
