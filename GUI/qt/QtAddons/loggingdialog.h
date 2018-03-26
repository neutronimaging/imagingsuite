#ifndef LOGGINGDIALOG_H
#define LOGGINGDIALOG_H

#include <QDialog>

#include <logging/logger.h>

namespace Ui {
class LoggingDialog;
}

namespace QtAddons {
class LoggingDialog : public QDialog, public kipl::logging::LogWriter
{
    Q_OBJECT
    kipl::logging::Logger logger;



public:
    explicit LoggingDialog(QWidget *parent = 0);
    ~LoggingDialog();

    virtual size_t Write(std::string str);

    QString serialize();
    void setLogLevel(kipl::logging::Logger::LogLevel level);
    void clear();

private:
    Ui::LoggingDialog *ui;
};

}
#endif // LOGGINGDIALOG_H
