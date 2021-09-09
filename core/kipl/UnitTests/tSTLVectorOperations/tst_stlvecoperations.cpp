#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <stltools/stlvecmath.h>
#include <stltools/stlmorphology.h>

class STLVecOperations : public QObject
{
    Q_OBJECT

public:
    STLVecOperations();
    ~STLVecOperations();

private slots:
    void test_medianFilter();
    void test_binaryDilate();

};

STLVecOperations::STLVecOperations()
{

}

STLVecOperations::~STLVecOperations()
{

}

void STLVecOperations::test_medianFilter()
{
    size_t N=10000000;
    std::vector<float> orig(N);
    std::vector<float> result(N);

    float val=1;
    for (auto &x : orig)
    {
        x+=val;
        val+=x;
    }
    orig=medianFilter(orig,7);
    if (N<30)
    {
        auto it=result.begin();
        for (auto &x: orig) {
            qDebug()<<x<<(*it);
            ++it;
        }

    }



}

void STLVecOperations::test_binaryDilate()
{
    std::vector<float> x0 = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    std::vector<float> v0 = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

    auto r0=binaryDilate(x0,1);

    QCOMPARE(r0.size(),v0.size());

    for (size_t i=0; i<x0.size(); ++i)
        QCOMPARE(r0[i],v0[i]);

    std::vector<int> x1 = {0,0,1,1,0,0,0,1,0,1,0,0};
    std::vector<int> v1 = {0,1,1,1,1,0,1,1,1,1,1,0};

    auto r1=binaryDilate(x1,1);

    QCOMPARE(r1.size(),v1.size());

    for (size_t i=0; i<x1.size(); ++i)
        QCOMPARE(r1[i],v1[i]);

    std::vector<int> x2 = {1,0,0,1,1,0,0,0,1,0,1,0,0,0,1,0};
    std::vector<int> v2 = {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1};

    auto r2=binaryDilate(x2,1);

    QCOMPARE(r2.size(),v2.size());

    for (size_t i=0; i<x2.size(); ++i)
        QCOMPARE(r2[i],v2[i]);


    std::vector<int> x3 = {0,0,0,1,0,0,0,0,0,0,1,0,0,0};
    std::vector<int> v3 = {0,1,1,1,1,1,0,0,1,1,1,1,1,0};

    auto r3=binaryDilate(x3,2);

    QCOMPARE(r3.size(),v3.size());

    for (size_t i=0; i<x3.size(); ++i)
        QCOMPARE(r3[i],v3[i]);

}

QTEST_APPLESS_MAIN(STLVecOperations)

#include "tst_stlvecoperations.moc"
