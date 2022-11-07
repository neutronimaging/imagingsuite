#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <stltools/stlvecmath.h>

class LoggingTests : public QObject
{
    Q_OBJECT

public:
    LoggingTests();
    ~LoggingTests();

private slots:


};




LoggingTests::LoggingTests()
{

}

LoggingTests::~LoggingTests()
{

}

QTEST_APPLESS_MAIN(LoggingTests)

#include "include/tst_logging.moc"
