#include "loggingdialog.h"
#include "ui_loggingdialog.h"

namespace QtAddons {


LoggingDialog::LoggingDialog(QWidget *parent) :
    QDialog(parent),
    logger("LoggingDialog"),
    ui(new Ui::LoggingDialog)
{
    ui->setupUi(this);
    setModal(false);
}

LoggingDialog::~LoggingDialog()
{
    delete ui;
}

size_t LoggingDialog::write(const std::string &str)
{
    return ui->widget_logger->write(str);
}

QString LoggingDialog::serialize()
{
    return ui->widget_logger->serialize();
}

void LoggingDialog::setLogLevel(kipl::logging::Logger::LogLevel level)
{
    return ui->widget_logger->setLogLevel(level);

}

void LoggingDialog::clear()
{
    ui->widget_logger->clear();
}

}
