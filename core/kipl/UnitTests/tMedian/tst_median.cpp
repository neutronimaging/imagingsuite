#include <QString>
#include <QtTest>
#include <QDebug>

#include <math/median.h>
#include <iostream>
#include <vector>

static inline bool qFuzzyCompare(double p1, double p2, double e)
{
    return (qAbs(p1 - p2) <= e * qMin(qAbs(p1), qAbs(p2)));
}

class TMedianTest : public QObject
{
    Q_OBJECT

public:
    TMedianTest();

private Q_SLOTS:
    void testMedian();
    void test_median_STL();

private:
    std::vector<float> m_vec_odd;
    std::vector<float> m_vec_even;
    float m_median_odd;
    float m_median_even;

    void test_median_quick_select();
    // void test_median_quick_select_sse();
};

TMedianTest::TMedianTest()
{
    m_vec_odd={0.1f, 1.0f, 0.5f, 0.3f, 10.0f};
    m_vec_even={0.1f, 1.0f, 0.5f, 0.3f};

    m_median_odd=0.5f;
    m_median_even=(0.3f+0.5f)/2.0f;
}

void TMedianTest::testMedian()
{
    // void median(T *v, size_t n, S * med);
    // void median(const std::vector<T> &v, S * med);

    float med;
    auto v1=m_vec_odd;
    auto v2=m_vec_even;
    kipl::math::median(v1.data(), v1.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));    

    kipl::math::median(v2.data(), v2.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));

    v1=m_vec_odd;
    kipl::math::median(v1, &med);
    QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));
    v2=m_vec_even;
    kipl::math::median(v2, &med);
    QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));
}

void TMedianTest::test_median_quick_select()
{
    // void median_quick_select(T *arr, size_t n, S *med);

    auto v1=m_vec_odd;
    auto v2=m_vec_even;

    float med;
    kipl::math::median_quick_select(v1.data(), v1.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));

    kipl::math::median_quick_select(v2.data(), v2.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));
}

// void TMedianTest::test_median_quick_select_sse()
// {

//     float med;
//     auto v1=m_vec_odd;
//     auto v2=m_vec_even;
//     kipl::math::median_quick_select_sse(v1.data(), v1.size(), &med);
//     QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));

//     kipl::math::median_quick_select_sse(v2.data(), v2   .size(), &med);
//     QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));
// }

void TMedianTest::test_median_STL()
{
    // void median_STL(T *arr, size_t n, S *med);
    // T median_STL(const std::vector<T> &v);
    float med;
    auto v1=m_vec_odd;
    auto v2=m_vec_even;
    kipl::math::median_STL(v1.data(), v1.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));

    kipl::math::median_STL(v2.data(), v2.size(), &med);
    QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));

    v1=m_vec_odd;
    v2=m_vec_even;
    med=kipl::math::median_STL(v1);
    QVERIFY(qFuzzyCompare(med, m_median_odd, 1e-7));
    med=kipl::math::median_STL(v2);
    QVERIFY(qFuzzyCompare(med, m_median_even, 1e-7));
    
}

QTEST_APPLESS_MAIN(TMedianTest)


#include "tst_median.moc"

