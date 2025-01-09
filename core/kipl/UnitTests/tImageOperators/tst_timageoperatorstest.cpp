#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/imageoperators.h>

class TImageOperatorsTest : public QObject
{
    Q_OBJECT

public:
    TImageOperatorsTest();

private Q_SLOTS:
    void testMin();
    void testMax();
    void testSum();
    void testMean();
    void testCountNaN();
    void testCountInf();

private:
    void makeTestImage();

    kipl::base::TImage<float,2> img;

};

TImageOperatorsTest::TImageOperatorsTest()
{
    makeTestImage();
}

void TImageOperatorsTest::testMin()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    float val=kipl::base::min(img2);
    QCOMPARE(val,1.0f);

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(img2[i],img[i]);
}

void TImageOperatorsTest::testMax()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    float val=kipl::base::max(img2);
    QCOMPARE(val,static_cast<float>(img.Size(0)*img.Size(1)));

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(img2[i],img[i]);
}

void TImageOperatorsTest::testSum()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    float val=kipl::base::sum(img2);
    float N=img2[img2.Size()-1];

    QCOMPARE(val,(N*(N+1.0f))*0.5f);

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(img2[i],img[i]);
}

void TImageOperatorsTest::testMean()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    float val=kipl::base::mean(img2);
    float N=img2[img2.Size()-1];

    QCOMPARE(val,(N*(N+1.0f))*0.5f/N);

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(img2[i],img[i]);
}

void TImageOperatorsTest::testCountNaN()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    img2[2]=NAN;
    img2[5]=NAN;
    img2[img2.Size()-1]=-NAN;

    QCOMPARE(kipl::base::count_NaN(img2),3UL);
    QCOMPARE(kipl::base::count_NaN(img),0UL);
}

void TImageOperatorsTest::testCountInf()
{
    kipl::base::TImage<float,2> img2=img;
    img2.Clone();

    img2[2]=INFINITY;
    img2[5]=INFINITY;
    img2[img2.Size()-1]=-INFINITY;

    QCOMPARE(kipl::base::count_Inf(img2),3UL);
    QCOMPARE(kipl::base::count_Inf(img),0UL);
}

void TImageOperatorsTest::makeTestImage()
{
    std::vector<size_t> dims={4,4};

    img.resize(dims);
    for (size_t i=0; i<img.Size(); ++i)
        img[i]=i+1;

}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(TImageOperatorsTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(TImageOperatorsTest)
#endif


#include "tst_timageoperatorstest.moc"


