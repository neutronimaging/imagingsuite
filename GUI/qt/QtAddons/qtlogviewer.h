#ifndef QTLOGVIEWER_H
#define QTLOGVIEWER_H

#include <QWidget>
#include <QMutex>

#include <logging/logger.h>

namespace Ui {
class QtLogViewer;
}

namespace QtAddons {

class QtLogViewer : public QWidget, public kipl::logging::LogWriter
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit QtLogViewer(QWidget *parent = 0);
    ~QtLogViewer();

    virtual size_t Write(std::string str);

    QString serialize();
    void setLogLevel(kipl::logging::Logger::LogLevel level);
    void clear();

private slots:
    void on_buttonSave_clicked();

    void on_buttonClear_clicked();

    void on_comboLogLevel_currentIndexChanged(int index);

    void updateText(QString msg);

private:
    Ui::QtLogViewer *ui;

    std::ofstream m_LogFile;

    QMutex m_Mutex;
    kipl::logging::Logger::LogLevel m_CurrentLoglevel;

signals:
    void newText(QString msg);
};
}

#endif // QTLOGVIEWER_H
