#include <vector>
#include <cmath>

#include <QString>
#include <QtTest>
#include <QDebug>

#include <base/timage.h>

#include <math/mathfunctions.h>

class MathFunctionsTest : public QObject
{
    Q_OBJECT

public:
    MathFunctionsTest();
    
private Q_SLOTS:
    // void testImageAbs();
    // void testImageReal();
    // void testImageImaginary();
    void testImageSqr();
    void testImageSqrt();
    void testSigmoid();
    // void testSigmoidWeights();
    void testEntropy();
    void testSign();
    // void testUnwrap();
    // void testPow();
    void testFactorial();
    // void testRoundP();
    // void testReplaceInfNaN();
    void testFactorize();
private:
    
};

MathFunctionsTest::MathFunctionsTest()
{
    
}

void MathFunctionsTest::testImageSqr()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t i = 0; i < img.Size(); ++i)
    {
        img[i] = i;
    }
    
    kipl::base::TImage<float,2> res = kipl::math::sqr(img);
    for (size_t i = 0; i < img.Size(); ++i)
    {
        QVERIFY(res[i] == i*i);
    }
}

void MathFunctionsTest::testImageSqrt()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t i = 0; i < img.Size(); ++i)
    {
        img[i] = i*i;
    }
    
    kipl::base::TImage<float,2> res = kipl::math::sqrt(img);
    for (size_t i = 0; i < img.Size(); ++i)
    {
        QVERIFY(res[i] == i);
    }
}

void MathFunctionsTest::testSigmoid()
{
    double x = 0.0;
    double level = 0.0;
    double width = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.5);
    
    x = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.7310585786300049);
    
    x = -1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.2689414213699951);
    
    x = 0.0;
    level = 1.0;
    width = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.2689414213699951);
    
    x = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.5);
    
    x = -1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.11920292202211755);
    
    x = 0.0;
    level = 0.0;
    width = 2.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.5);
    
    x = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.6224593312018546);
    
    x = -1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.3775406687981454);
    
    x = 0.0;
    level = 1.0;
    width = 2.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.3775406687981454);
    
    x = 1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.5);
    
    x = -1.0;
    QCOMPARE(kipl::math::sigmoid(x,level,width), 0.2689414213699951);
}

void MathFunctionsTest::testFactorize()
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

    factors = kipl::math::factorize(1125);
    QVERIFY(factors.size() == 5);
    QVERIFY(factors[0] == 3);
    QVERIFY(factors[1] == 3);
    QVERIFY(factors[2] == 5);
    QVERIFY(factors[3] == 5);
    QVERIFY(factors[4] == 5);

}

void MathFunctionsTest::testEntropy()
{
    std::vector<double> vec = {0.1, 0.2, 0.3, 0.4};
    QCOMPARE(kipl::math::entropy(vec), 1.8464393446710154);
    
    vec = {0.25, 0.25, 0.25, 0.25};
    QCOMPARE(kipl::math::entropy(vec), 2.0);
    
    vec = {0.0, 0.0, 0.0, 1.0};
    QCOMPARE(kipl::math::entropy(vec), 0.0);
    
    vec = {0.0, 0.0, 1.0, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 0.0);
    
    vec = {0.0, 1.0, 0.0, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 0.0);
    
    vec = {1.0, 0.0, 0.0, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 0.0);
    
    vec = {0.5, 0.5, 0.0, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 1.0);
    
    vec = {0.5, 0.0, 0.5, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 1.0);
    
    vec = {0.5, 0.0, 0.0, 0.5};
    QCOMPARE(kipl::math::entropy(vec), 1.0);
    
    vec = {0.0, 0.5, 0.5, 0.0};
    QCOMPARE(kipl::math::entropy(vec), 1.0);
    
    vec = {0.0, 0.5, 0.0, 0.5};
    QCOMPARE(kipl::math::entropy(vec), 1.0);
    
    vec = {0.0, 0.0, 0.5, 0.5};
    QCOMPARE(kipl::math::entropy(vec), 1.0);

    vec = {0.0, 0.0, 0.0, 0.0};

    QVERIFY_EXCEPTION_THROWN(kipl::math::entropy(vec), std::runtime_error);
    
}

void MathFunctionsTest::testSign()
{
    QCOMPARE(kipl::math::sign(0), 0);
    QCOMPARE(kipl::math::sign(1), 1);
    QCOMPARE(kipl::math::sign(-1), -1);
    QCOMPARE(kipl::math::sign(0.0), 0);
    QCOMPARE(kipl::math::sign(1.0), 1);
    QCOMPARE(kipl::math::sign(-1.0), -1);
    QCOMPARE(kipl::math::sign(0.0f), 0);
    QCOMPARE(kipl::math::sign(1.0f), 1);
    QCOMPARE(kipl::math::sign(-1.0f), -1);
}

void MathFunctionsTest::testFactorial()
{
    QCOMPARE(kipl::math::factorial(0), 1);
    QCOMPARE(kipl::math::factorial(1), 1);
    QCOMPARE(kipl::math::factorial(2), 2);
    QCOMPARE(kipl::math::factorial(3), 6);
    QCOMPARE(kipl::math::factorial(4), 24);
    QCOMPARE(kipl::math::factorial(5), 120);
    QCOMPARE(kipl::math::factorial(6), 720);
    QCOMPARE(kipl::math::factorial(7), 5040);
    QCOMPARE(kipl::math::factorial(8), 40320);
    QCOMPARE(kipl::math::factorial(9), 362880);
    QCOMPARE(kipl::math::factorial(10), 3628800);
}

QTEST_APPLESS_MAIN(MathFunctionsTest)

#include "tst_MathFunctions.moc"