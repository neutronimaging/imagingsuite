#include "qtlogviewer.h"
#include <sstream>
#include <QFileDialog>
#include <QTextStream>

namespace QtAddons {
QtLogViewer::QtLogViewer(QWidget *parent) :
    QWidget(parent),
    logger("QtLogViewer"),
    save_button("Save"),
    clear_button("Clear"),
    m_LogFile((QDir::homePath()+"/muhlog.txt").toStdString().c_str()),
    m_CurrentLoglevel(kipl::logging::Logger::LogError)
{
    this->setLayout(&vbox);
    vbox.addWidget(&textedit);
    vbox.addLayout(&hbox);

    hbox.addWidget(&save_button);
    hbox.addWidget(&clear_button);
    hbox.addWidget(&loglevel_combo);

    QStringList loglevels;

    loglevels.append("Error");
    loglevels.append("Warning");
    loglevels.append("Message");
    loglevels.append("Verbose");
    loglevels.append("Debug");

    loglevel_combo.addItems(loglevels);
    SetLogLevel(kipl::logging::Logger::LogMessage);

    connect(&save_button,SIGNAL(clicked()),this,SLOT(save_clicked()));
    connect(&clear_button,SIGNAL(clicked()),this,SLOT(clear_clicked()));
    connect(&loglevel_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(loglevel_changed(int)));

    this->show();
}

QtLogViewer::~QtLogViewer()
{
}

size_t QtLogViewer::Write(std::string str)
{
    QMutexLocker locker(&m_Mutex);
    textedit.append(QString(str.c_str()));
    m_LogFile<<str<<std::endl;

    return 0;
}

QString QtLogViewer::serialize()
{
    QString str;

    QMutexLocker locker(&m_Mutex);

    str=textedit.toPlainText();

    return str;

}

void QtLogViewer::SetLogLevel(kipl::logging::Logger::LogLevel level)
{
        m_CurrentLoglevel=level;
        kipl::logging::Logger::SetLogLevel(level);
        loglevel_combo.setCurrentIndex(static_cast<int>(level));
        std::ostringstream msg;
        msg<<"Changed log-level to "<<kipl::logging::Logger::GetLogLevel();
        logger(kipl::logging::Logger::LogMessage,msg.str());
}

void QtLogViewer::clear()
{
    QMutexLocker locker(&m_Mutex);
    textedit.clear();
}

void QtLogViewer::save_clicked()
{
    logger(kipl::logging::Logger::LogVerbose,"Save the log buffer");

    QString filename = QFileDialog::getSaveFileName(this,
         tr("Save log file"), "/home", tr("log Files (*.log)"));
    if (filename.isEmpty()==false)
    {
        QFile f( filename );
        f.open( QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&f);
        out<<serialize();
        f.close();
    }
    else {
        logger(logger.LogWarning,"Cancelled the dialog, no file saved.");
    }
}

void QtLogViewer::clear_clicked()
{
    clear();
    logger(kipl::logging::Logger::LogVerbose,"Cleared the log buffer");
}

void QtLogViewer::loglevel_changed(int level)
{
    QMutexLocker locker(&m_Mutex);
    if (level!=m_CurrentLoglevel)
        SetLogLevel(static_cast<kipl::logging::Logger::LogLevel>(level));
}

}
