#include <QString>
#include <QtTest>

class FillHoleTest : public QObject
{
    Q_OBJECT

public:
    FillHoleTest();

private Q_SLOTS:
    void testCase1();
};

FillHoleTest::FillHoleTest()
{
}

void FillHoleTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(FillHoleTest)

#include "tst_fillholetest.moc"
