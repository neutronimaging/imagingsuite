#include <QString>
#include <QtTest>

class TMorphologyTest : public QObject
{
    Q_OBJECT

public:
    TMorphologyTest();

private Q_SLOTS:
    void testCase1();
};

TMorphologyTest::TMorphologyTest()
{
}

void TMorphologyTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TMorphologyTest)

#include "tst_tmorphologytest.moc"
