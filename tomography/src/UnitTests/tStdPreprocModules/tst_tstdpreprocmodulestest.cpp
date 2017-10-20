#include <QString>
#include <QtTest>


class TStdPreprocModulesTest : public QObject
{
    Q_OBJECT

public:
    TStdPreprocModulesTest();

private Q_SLOTS:
    void testFullLogNorm();
};

TStdPreprocModulesTest::TStdPreprocModulesTest()
{
}

void TStdPreprocModulesTest::testFullLogNorm()
{

    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TStdPreprocModulesTest)

#include "tst_tstdpreprocmodulestest.moc"
