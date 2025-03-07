//<LICENSE>

#include <QtTest>

#include <vector>
#include <string>
#include <thread>
#include <cmath>

#include <math/thinplatespline.h>
#include <strings/filenames.h>


class TestThinPlateSpline : public QObject
{
    Q_OBJECT

public:
    TestThinPlateSpline();
    ~TestThinPlateSpline();

private slots:
    void test_Constructors();
    void test_SetPoints();
    void test_Processor();
    void test_ProcessorSingle();
    void test_Transform();


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

    QVERIFY_EXCEPTION_THROWN(kipl::math::ThinPlateSpline tps3(x, y, w2), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(kipl::math::ThinPlateSpline tps3(x, y2, w), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(kipl::math::ThinPlateSpline tps3(x2, y, w), std::invalid_argument);

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

    QVERIFY_EXCEPTION_THROWN(tps2.setPoints(x, y, w2), std::invalid_argument);
    QCOMPARE(tps2.size(), 0UL);

    QVERIFY_EXCEPTION_THROWN(tps2.setPoints(x, y2, w), std::invalid_argument);
    QCOMPARE(tps2.size(), 0UL);

    QVERIFY_EXCEPTION_THROWN(tps2.setPoints(x2, y, w), std::invalid_argument);
    QCOMPARE(tps2.size(), 0UL);
}
 

QTEST_APPLESS_MAIN(TestThinPlateSpline)

#include "tst_thinplatespline.moc"
