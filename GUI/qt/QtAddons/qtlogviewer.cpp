#include "qtlogviewer.h"
#include "ui_qtlogviewer.h"

#include <QFileDialog>
#include <QTextStream>
#include <QSignalBlocker>
#include <QDir>

namespace QtAddons {


QtLogViewer::QtLogViewer(QWidget *parent) :
    QWidget(parent),
    LogWriter("QtLogViewerWriter"),
    logger("QtLogViewer"),
    ui(new Ui::QtLogViewer),
    m_CurrentLoglevel(kipl::logging::Logger::LogMessage)
{
    ui->setupUi(this);
    setLogLevel(m_CurrentLoglevel);
    connect(this,&QtLogViewer::newText,this,&QtLogViewer::updateText);
}

QtLogViewer::~QtLogViewer()
{
    delete ui;
}

void QtLogViewer::on_buttonSave_clicked()
{
    logger(kipl::logging::Logger::LogVerbose,"Save the log buffer");

    QString filename = QFileDialog::getSaveFileName(this,
         tr("Save log file"), QDir::homePath(), tr("log Files (*.log)"));
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

void QtLogViewer::on_buttonClear_clicked()
{
    clear();
    logger(kipl::logging::Logger::LogVerbose,"Cleared the log buffer");
}

void QtLogViewer::on_comboLogLevel_currentIndexChanged(int index)
{
    if (index!=m_CurrentLoglevel)
        setLogLevel(static_cast<kipl::logging::Logger::LogLevel>(index));
}

size_t QtLogViewer::write(const std::string &str)
{
    emit newText(QString::fromStdString(str));

    m_LogFile<<str<<std::endl;
    return 0;
}

QString QtLogViewer::serialize()
{
    QString str;

    QMutexLocker locker(&m_Mutex);

    str=ui->textEdit->toPlainText();

    return str;
}

void QtLogViewer::setLogLevel(kipl::logging::Logger::LogLevel level)
{
    QSignalBlocker blocker(ui->comboLogLevel);

    m_CurrentLoglevel=level;
    kipl::logging::Logger::SetLogLevel(level);
    ui->comboLogLevel->setCurrentIndex(static_cast<int>(level));
    std::ostringstream msg;
    msg<<"Changed log-level to "<<kipl::logging::Logger::GetLogLevel();
    logger(level,msg.str());
}

void QtLogViewer::clear()
{
    QMutexLocker locker(&m_Mutex);
    ui->textEdit->clear();
}

void QtLogViewer::updateText(QString msg) // Update text slot
{
    ui->textEdit->append(msg);
}

}
