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


#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(FillHoleTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(FillHoleTest)
#endif


#include "tst_fillholetest.moc"
