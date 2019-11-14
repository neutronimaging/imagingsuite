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
    void testQuantileFit();
    void testCalls();
    void testWeightedFit();
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

void TLinFitTest::testQuantileFit()
{
    const int N=100;
    double x[N];
    double y[N];
    double ck=1.5;
    double cm=10;
    for (int i=0; i<N; ++i)
    {
        x[i]=static_cast<double>(i);
        y[i]=ck*x[i]+cm+2.0*((i % 2)-0.5)*(fmod(x[i],10.0)==0 ? 10.0: 0.0);
    }

    double m,k,R2;
    kipl::math::LinearLSFit(x,y,N,&m,&k, &R2,0.9);

    qDebug() <<"m="<<m<<", k="<<k<<", R2="<<R2;
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
    QVERIFY(qFuzzyCompare(m,cm, 1e-7));
    QVERIFY(qFuzzyCompare(k,ck,1e-7));
    QVERIFY(qFuzzyCompare(R2,1.0,1e-7));

    kipl::math::LinearLSFit(x,y,5,q);

    QVERIFY(qFuzzyCompare(m,q[0], 1e-7));
    QVERIFY(qFuzzyCompare(k,q[1],1e-7));
    QVERIFY(qFuzzyCompare(R2,q[2],1e-7));

    kipl::math::LinearLSFit(x,y,5,&m,&k,&R2,0.6);
    std::cout<<"m="<<m<<", k="<<k<<", R2"<<R2<<std::endl;


}

void TLinFitTest::testWeightedFit()
{
    arma::mat H(7,3);
    arma::vec y(7);
    arma::vec q0(3);

    q0 << 3. << 2.<< 1.;

    double x=-3.0;
    for (int r=0; r<H.n_rows; ++r, ++x)
    {
        y(r)=0.0;
        for (int c = 0; c<H.n_cols; ++c)
        {
            y(r)+=q0(c)*std::pow(x,c);
            H(r,c)=std::pow(x,c);
        }
    }
    y(0)=8.0;
    arma::mat C(y.n_elem,y.n_elem,arma::fill::eye);
    C(0,0)=0.01;

    arma::vec q;

    kipl::math::weightedLSFit(H, C, y, q);
    QCOMPARE(q.n_elem,3);
    double eps=1e-6;
    QVERIFY(fabs(q(0)-2.99224806)<eps);
    QVERIFY(fabs(q(1)-1.99127907)<eps);
    QVERIFY(fabs(q(2)-1.00484496)<eps);
}

QTEST_APPLESS_MAIN(TLinFitTest)

#include "tst_tlinfittest.moc"
