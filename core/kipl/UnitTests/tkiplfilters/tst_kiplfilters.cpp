#include <QtTest>

#include <vector>
#include <filters/savitzkygolayfilter.h>


class KiplFilters : public QObject
{
    Q_OBJECT

public:
    KiplFilters();
    ~KiplFilters();

private slots:
    void test_SavGolCoeffs();
    void test_SavGolFilter();

};

KiplFilters::KiplFilters()
{

}

KiplFilters::~KiplFilters()
{

}

void KiplFilters::test_SavGolCoeffs()
{
    kipl::filters::SavitzkyGolayFilter<double> sg;
    auto res=sg.coeffs(5,2);
    std::vector<double> expres = {-0.0857142857143,  0.342857142857,  0.485714285714,  0.342857142857, -0.0857142857143};

    QCOMPARE(res.size(),expres.size());

    for (auto itA=res.begin(),itB=expres.begin(); itA!=res.end(); ++itA, ++itB)
    {
        QVERIFY(fabs(*itA-*itB)<1e-5);
    }


    res = sg.coeffs(5, 2, 1);
    std::vector<double> expres2 = { -2.00000000e-01,  -1.00000000e-01,  -2.02197100567e-16, 1.00000000e-01, 2.00000000e-01};
    QCOMPARE(res.size(),expres2.size());
    for (auto itA=res.begin(),itB=expres2.begin(); itA!=res.end(); ++itA, ++itB)
    {
       QVERIFY(fabs(*itA-*itB)<1e-5);
    }
}

void KiplFilters::test_SavGolFilter()
{
    std::vector<double> x = {2, 2, 5, 2, 1, 0, 1, 4, 9};
    kipl::filters::SavitzkyGolayFilter<double> sg;
    std::vector<double> res;

    try {
        res = sg(x, 5, 2);
    }
    catch (std::exception &e) {
        QFAIL(e.what()) ;
    }
    catch (kipl::base::KiplException &e) {
        QFAIL(e.what());
    }

    std::vector<double> expres= {1.65714285714, 3.17143, 3.54286, 2.85714, 0.657143, 0.171429, 1.  , 4.  , 9.};

    QCOMPARE(res.size(),expres.size());
    for (auto itA=res.begin(),itB=expres.begin(); itA!=res.end(); ++itA, ++itB)
    {
      QVERIFY(fabs(*itA-*itB)<1e-5);
      // QCOMPARE is too sensitive for the test data
    }
}

QTEST_APPLESS_MAIN(KiplFilters)

#include "tst_kiplfilters.moc"
