#include <QString>
#include <QtTest>
#include <math/linfit.h>
#include <iostream>

static inline bool qFuzzyCompare(double p1, double p2, double e)
{
    return (qAbs(p1 - p2) <= e * qMin(qAbs(p1), qAbs(p2)));
}

class TLinFitTest : public QObject
{
    Q_OBJECT

public:
    TLinFitTest();

private Q_SLOTS:
    void testFit();
    void testCalls();
};

TLinFitTest::TLinFitTest()
{
}

void TLinFitTest::testFit()
{
    float x[5]={0.1f, 1.0f, 0.5f, 0.3f, 10.0f};
    float y[5];

    const double cm=10.0f;
    const double ck=1.5f;

    double m,k,R2;

    for (int i=0; i<5; i++)
        y[i]=x[i]*ck+cm;

    kipl::math::LinearLSFit(x,y,5,&m,&k, &R2);

    QVERIFY(qFuzzyCompare(m,cm, 1e-7));
    QVERIFY(qFuzzyCompare(k,ck,1e-7));
    QVERIFY(qFuzzyCompare(R2,1.0,1e-7));


}

void TLinFitTest::testCalls()
{
    float x[5]={0.1f, 1.0f, 0.5f, 0.3f, 10.0f};
    float y[5];

    const double cm=10.0f;
    const double ck=1.5f;

    double m,k,R2;
    double q[3];

    for (int i=0; i<5; i++)
        y[i]=x[i]*ck+cm;

    kipl::math::LinearLSFit(x,y,5,&m,&k, &R2);
    kipl::math::LinearLSFit(x,y,5,q);

    QVERIFY(qFuzzyCompare(m,q[0], 1e-7));
    QVERIFY(qFuzzyCompare(k,q[1],1e-7));
    QVERIFY(qFuzzyCompare(R2,q[2],1e-7));
}

QTEST_APPLESS_MAIN(TLinFitTest)

#include "tst_tlinfittest.moc"
