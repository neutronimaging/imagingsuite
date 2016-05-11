#include <QString>
#include <QtTest>

class TkiplbasetestTest : public QObject
{
    Q_OBJECT

public:
    TkiplbasetestTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // Tests for TImage
    void testTImageInitialization();

    // Tests for BivariateHistogram
    void testBivariateHistogram();
};

TkiplbasetestTest::TkiplbasetestTest()
{
}

void TkiplbasetestTest::initTestCase()
{
}

void TkiplbasetestTest::cleanupTestCase()
{
}

void TkiplbasetestTest::testTImageInitialization()
{
    QVERIFY2(true, "Failure");
}

void TkiplbasetestTest::testBivariateHistogram()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TkiplbasetestTest)

#include "tst_kiplbase.moc"
