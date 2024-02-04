#include <vector>

#include <QString>
#include <QtTest>
#include <QDebug>

#include <base/timage.h>

#include <math/mathfunctions.h>

class TMathFunctions : public QObject
{
    Q_OBJECT

public:
    TMathFunctions();
    
private Q_SLOTS:
    // void testImageAbs();
    // void testImageReal();
    // void testImageImaginary();
    // void testImageSqr();
    // void testImageSqrt();
    // void testSigmoid();
    // void testSigmoidWeights();
    // void testEntropy();
    // void testSign();
    // void testUnwrap();
    // void testPow();
    // void testFactorial();
    // void testRoundP();
    // void testReplaceInfNaN();
    void testFactorize();
private:
    
};

TMathFunctions::TMathFunctions()
{
    
}

TMathFunctions::testFactorize()
{
    std::vector<int> factors = kipl::math::factorize(10);
    QVERIFY(factors.size() == 2);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 5);
    
    factors = kipl::math::factorize(12);
    QVERIFY(factors.size() == 3);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 2);
    QVERIFY(factors[2] == 3);
    
    factors = kipl::math::factorize(13);
    QVERIFY(factors.size() == 1);
    QVERIFY(factors[0] == 13);
    
    factors = kipl::math::factorize(14);
    QVERIFY(factors.size() == 2);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 7);
    
    factors = kipl::math::factorize(15);
    QVERIFY(factors.size() == 2);
    QVERIFY(factors[0] == 3);
    QVERIFY(factors[1] == 5);
    
    factors = kipl::math::factorize(16);
    QVERIFY(factors.size() == 4);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 2);
    QVERIFY(factors[2] == 2);
    QVERIFY(factors[3] == 2);
    
    factors = kipl::math::factorize(17);
    QVERIFY(factors.size() == 1);
    QVERIFY(factors[0] == 17);
    
    factors = kipl::math::factorize(18);
    QVERIFY(factors.size() == 3);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 3);
    QVERIFY(factors[2] == 3);
    
    factors = kipl::math::factorize(19);
    QVERIFY(factors.size() == 1);
    QVERIFY(factors[0] == 19);
    
    factors = kipl::math::factorize(20);
    QVERIFY(factors.size() == 3);
    QVERIFY(factors[0] == 2);
    QVERIFY(factors[1] == 2);
    QVERIFY(factors[2] == 5);
}


QTEST_APPLESS_MAIN(TMathFunctions)

#include "tst_MathFunctions.moc"