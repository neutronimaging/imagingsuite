#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <logging/logger.h>

class LoggingTests : public QObject
{
    Q_OBJECT

public:
    LoggingTests();
    ~LoggingTests();

private slots:
    void testLoggingEnums();
    void testLogWriter();
    void testLogStreamWriter();
    void testLogger();
    void testMultiTargetLogger();

};




LoggingTests::LoggingTests()
{

}

LoggingTests::~LoggingTests()
{

}

void LoggingTests::testLoggingEnums()
{
//    enum2string();
    QCOMPARE(enum2string(kipl::logging::Logger::LogDebug).c_str(),   "debug");
    QCOMPARE(enum2string(kipl::logging::Logger::LogVerbose).c_str(), "verbose");
    QCOMPARE(enum2string(kipl::logging::Logger::LogMessage).c_str(), "message");
    QCOMPARE(enum2string(kipl::logging::Logger::LogWarning).c_str(), "warning");
    QCOMPARE(enum2string(kipl::logging::Logger::LogError).c_str(),   "error");

    //    string2enum();
    kipl::logging::Logger::LogLevel level;
    string2enum("debug",level);
    QCOMPARE(level,kipl::logging::Logger::LogDebug);
    string2enum("verbose",level);
    QCOMPARE(level,kipl::logging::Logger::LogVerbose);
    string2enum("message",level);
    QCOMPARE(level,kipl::logging::Logger::LogMessage);
    string2enum("warning",level);
    QCOMPARE(level,kipl::logging::Logger::LogWarning);
    string2enum("error",level);
    QCOMPARE(level,kipl::logging::Logger::LogError);
}

void LoggingTests::testLogWriter()
{
    kipl::logging::LogWriter lw("lw");

    QCOMPARE(lw.isValid(),true);

    QCOMPARE(lw.write("test"),0);
    QCOMPARE(lw.loggerName(),std::string("lw"));
    QCOMPARE(lw.isActive(),true);
    lw.active(false);
    QCOMPARE(lw.isActive(),false);
}

void LoggingTests::testLogStreamWriter()
{
    kipl::logging::LogStreamWriter badlw("sdfsdfsdfsfsg/dsfs.log");

    QCOMPARE(badlw.isValid(),false);

    kipl::logging::LogStreamWriter lw("tester.log");
    QCOMPARE(lw.isValid(),true);
}

void LoggingTests::testLogger()
{

}

void LoggingTests::testMultiTargetLogger()
{

}

QTEST_APPLESS_MAIN(LoggingTests)

#include "include/tst_logging.moc"
