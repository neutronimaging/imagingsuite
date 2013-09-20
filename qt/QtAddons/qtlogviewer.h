#ifndef QTLOGVIEWER_H
#define QTLOGVIEWER_H

#include "QtAddons_global.h"
#include <qwidget.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <qmutex.h>
#include <logging/logger.h>
#include <fstream>

namespace QtAddons {
class QTADDONSSHARED_EXPORT QtLogViewer : public QWidget, public kipl::logging::LogWriter{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    QtLogViewer(QWidget *parent=NULL);
    ~QtLogViewer();

    virtual size_t Write(std::string str);

    bool isContainer() { return false;}
    QString serialize();
    void SetLogLevel(kipl::logging::Logger::LogLevel level);
    void clear();

private slots:
    void save_clicked();
    void clear_clicked();
    void loglevel_changed(int level);

protected:
    QVBoxLayout vbox;
    QHBoxLayout hbox;

    QTextEdit textedit;


    QPushButton save_button;
    QPushButton clear_button;
    QComboBox   loglevel_combo;

    std::ofstream m_LogFile;

    QMutex m_Mutex;
    kipl::logging::Logger::LogLevel m_CurrentLoglevel;
};

}
#endif // QTLOGVIEWER_H
