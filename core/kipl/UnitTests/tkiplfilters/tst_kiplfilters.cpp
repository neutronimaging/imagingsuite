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
        QCOMPARE(*itA, *itB);
    }


    res = sg.coeffs(5, 2, 1);
    std::vector<double> expres2 = { -2.00000000e-01,  -1.00000000e-01,  -2.02197100567e-16, 1.00000000e-01, 2.00000000e-01};
    QCOMPARE(res.size(),expres2.size());
    for (auto itA=res.begin(),itB=expres2.begin(); itA!=res.end(); ++itA, ++itB)
    {
       QCOMPARE(*itA, *itB);
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

    std::vector<double> expres= {1.66, 3.17, 3.54286, 2.85714, 0.657143, 0.171429, 1.  , 4.  , 9.};

    QCOMPARE(res.size(),expres.size());
    for (auto itA=res.begin(),itB=expres.begin(); itA!=res.end(); ++itA, ++itB)
    {
      //  QCOMPARE(*itA, *itB);
        qDebug() << *itA << *itB;
    }


//    savgol_filter(x, 5, 2, mode='nearest')
//    array([1.74, 3.03, 3.54, 2.86, 0.66, 0.17, 1.  , 4.6 , 7.97])
}

QTEST_APPLESS_MAIN(KiplFilters)

#include "tst_kiplfilters.moc"
