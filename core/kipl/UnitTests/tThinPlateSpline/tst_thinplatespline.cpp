//<LICENSE>

#include <QtTest>

#include <vector>
#include <string>
#include <thread>
#include <cmath>

#include <math/thinplatespline.h>
#include <strings/filenames.h>
#include <io/io_tiff.h>


class TestThinPlateSpline : public QObject
{
    Q_OBJECT

public:
    TestThinPlateSpline();
    ~TestThinPlateSpline();

private slots:
    void test_Constructors();
    void test_SetPoints();
    void test_interpolate();
    void test_render();

    void test_render_large();

private:
    std::string dataPath;
};

TestThinPlateSpline::TestThinPlateSpline()
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}

TestThinPlateSpline::~TestThinPlateSpline()
{

}

void TestThinPlateSpline::test_Constructors()
{
    kipl::math::ThinPlateSpline tps;
    QCOMPARE(tps.size(), 0UL);

    std::vector<float> x = {1.0f, 2.0f, 3.0f};
    std::vector<float> y = {3.0f, 2.0f, 1.0f};
    std::vector<float> w = {2.0f, 4.0f, 6.0f};

    std::vector<float> x2 = {1.0f, 2.0f};
    std::vector<float> y2 = {3.0f, 2.0f};
    std::vector<float> w2 = {2.0f, 4.0f};

    kipl::math::ThinPlateSpline tps2(x, y, w);
    QCOMPARE(tps2.size(), 3UL);

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, kipl::math::ThinPlateSpline tps3(x, y, w2));
    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, kipl::math::ThinPlateSpline tps3(x, y2, w));
    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, kipl::math::ThinPlateSpline tps3(x2, y, w));
}

void TestThinPlateSpline::test_SetPoints()
{
    kipl::math::ThinPlateSpline tps;
    
    std::vector<float> x = {1.0f, 2.0f, 3.0f};
    std::vector<float> y = {3.0f, 2.0f, 1.0f};
    std::vector<float> w = {2.0f, 4.0f, 6.0f};

    tps.setPoints(x, y, w);
    QCOMPARE(tps.size(), 3UL);

    std::vector<float> x2 = {1.0f, 2.0f};
    std::vector<float> y2 = {3.0f, 2.0f};
    std::vector<float> w2 = {2.0f, 4.0f};

    kipl::math::ThinPlateSpline tps2;

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, tps2.setPoints(x, y, w2));
    QCOMPARE(tps2.size(), 0UL);

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, tps2.setPoints(x, y2, w));
    QCOMPARE(tps2.size(), 0UL);

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, tps2.setPoints(x2, y, w));
    QCOMPARE(tps2.size(), 0UL);
}

void TestThinPlateSpline::test_interpolate()
{
    kipl::math::ThinPlateSpline tps;
    
    std::vector<float> x = {1.0f, 2.0f, 3.0f};
    std::vector<float> y = {3.0f, 2.0f, 1.0f};
    std::vector<float> w = {2.0f, 4.0f, 6.0f};

    tps.setPoints(x, y, w);

    QCOMPARE(tps.interpolate(1.0f, 3.0f), 2.0f);
    QCOMPARE(tps.interpolate(2.0f, 2.0f), 4.0f);
    QCOMPARE(tps.interpolate(3.0f, 1.0f), 6.0f);
}

void TestThinPlateSpline::test_render()
{
    kipl::math::ThinPlateSpline tps;
    
    std::vector<float> x = {1.0f, 2.0f, 3.0f};
    std::vector<float> y = {3.0f, 2.0f, 1.0f};
    std::vector<float> w = {2.0f, 4.0f, 6.0f};

    tps.setPoints(x, y, w);

    std::vector<size_t> dims = {5, 6};
    std::vector<size_t> fov = {3, 3};

    kipl::base::TImage<float,2> img = tps.render(dims);

    auto imgdims = img.dims();

    QCOMPARE(imgdims[0], dims[0]);
    QCOMPARE(imgdims[1], dims[1]);

    QCOMPARE(img(1, 3), 2.0f);
    QCOMPARE(img(2, 2), 4.0f);
    QCOMPARE(img(3, 1), 6.0f);
}

void TestThinPlateSpline::test_render_large()
{

    
    size_t n = 5;
    std::vector<float> x(n);
    std::vector<float> y(n);
    std::vector<float> w(n);

    float fn = static_cast<float>(n);
    for (size_t xx = 0, i=0; xx < n; ++xx)
        for (size_t yy = 0; yy < n; ++yy,++i)
    {   
        x[i] = static_cast<float>(xx)*200.0f+100.0f;
        y[i] = static_cast<float>(yy)*200.0f+100.0f;
        w[i] = static_cast<float>(std::sin(static_cast<float>(xx)/fn*3.14f)*std::cos(static_cast<float>(yy)/fn*3.14f));
    }

    std::vector<size_t> dims = {1000, 1000};
    std::vector<size_t> fov = {3, 3};

    kipl::base::TImage<float,2> img ;
    QBENCHMARK
    {
        kipl::math::ThinPlateSpline tps;
        tps.setPoints(x, y, w);

        img = tps.render(dims);
    }
    
    kipl::io::WriteTIFF(img,"tps.tif",kipl::base::Float32);
    auto imgdims = img.dims();

    QCOMPARE(imgdims[0], dims[0]);
    QCOMPARE(imgdims[1], dims[1]);


    for (size_t i = 0; i < n; ++i)
        QCOMPARE(img(static_cast<size_t>(x[i]), static_cast<size_t>(y[i])), w[i]);
    
    

}


QTEST_APPLESS_MAIN(TestThinPlateSpline)

#include "tst_thinplatespline.moc"
