#include <QtTest>
#include <QDebug>

#include <algorithm>
#include <vector>

// add necessary includes here
#include <stltools/stlvecmath.h>
#include <stltools/stlmorphology.h>
#include <stltools/zerocrossings.h>

class STLVecOperations : public QObject
{
    Q_OBJECT

public:
    STLVecOperations();
    ~STLVecOperations();

private slots:
    void test_medianFilter();
    void test_binaryDilate();
    void test_MAD();
    void test_findContinuousBlocks();

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

    auto r0=binaryDilation(x0,1);

    QCOMPARE(r0.size(),v0.size());

    for (size_t i=0; i<x0.size(); ++i)
        QCOMPARE(r0[i],v0[i]);

    std::vector<int> x1 = {0,0,1,1,0,0,0,1,0,1,0,0};
    std::vector<int> v1 = {0,1,1,1,1,0,1,1,1,1,1,0};

    auto r1=binaryDilation(x1,1);

    QCOMPARE(r1.size(),v1.size());

    for (size_t i=0; i<x1.size(); ++i)
        QCOMPARE(r1[i],v1[i]);

    std::vector<int> x2 = {1,0,0,1,1,0,0,0,1,0,1,0,0,0,1,0};
    std::vector<int> v2 = {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1};

    auto r2=binaryDilation(x2,1);

    QCOMPARE(r2.size(),v2.size());

    for (size_t i=0; i<x2.size(); ++i)
        QCOMPARE(r2[i],v2[i]);


    std::vector<int> x3 = {0,0,0,1,0,0,0,0,0,0,1,0,0,0};
    std::vector<int> v3 = {0,1,1,1,1,1,0,0,1,1,1,1,1,0};

    auto r3=binaryDilation(x3,2);

    QCOMPARE(r3.size(),v3.size());

    for (size_t i=0; i<x3.size(); ++i)
        QCOMPARE(r3[i],v3[i]);
}

void STLVecOperations::test_MAD()
{
    std::vector<float> v={0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 1000.0f};

    auto mad = MAD(v);

    QVERIFY(mad==3.0);

}

void STLVecOperations::test_findContinuousBlocks()
{
    std::vector<float> v0={0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f,0.0f};

    auto r0=findContinuousBlocks(v0);
    std::vector<std::pair<ptrdiff_t,ptrdiff_t>> gt0={{1,4},{6,7},{10,11}};

    QCOMPARE(r0.size(),gt0.size());

    for (size_t i=0; i<gt0.size(); ++i)
    {
        QCOMPARE(r0[i].first,gt0[i].first);
        QCOMPARE(r0[i].second,gt0[i].second);
    }

    std::vector<float> v1={1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f,1.0f};

    auto r1=findContinuousBlocks(v1);
    std::vector<std::pair<ptrdiff_t,ptrdiff_t>> gt1={{0,4},{6,7},{10,12}};

    QCOMPARE(r1.size(),gt1.size());

    for (size_t i=0; i<gt1.size(); ++i)
    {
        QCOMPARE(r1[i].first,gt1[i].first);
        QCOMPARE(r1[i].second,gt1[i].second);
    }

    std::vector<float> v2(12,1);

    auto r2=findContinuousBlocks(v2);
    std::vector<std::pair<ptrdiff_t,ptrdiff_t>> gt2={{0,12}};

    QCOMPARE(r2.size(),gt2.size());

    for (size_t i=0; i<gt2.size(); ++i)
    {
        QCOMPARE(r2[i].first,gt2[i].first);
        QCOMPARE(r2[i].second,gt2[i].second);
    }

    std::vector<float> v3(12,0);

    auto r3=findContinuousBlocks(v3);
    std::vector<std::pair<ptrdiff_t,ptrdiff_t>> gt3={};

    QCOMPARE(r3.size(),gt3.size());

    for (size_t i=0; i<gt3.size(); ++i)
    {
        QCOMPARE(r3[i].first,gt3[i].first);
        QCOMPARE(r3[i].second,gt3[i].second);
    }
}

QTEST_APPLESS_MAIN(STLVecOperations)

#include "tst_stlvecoperations.moc"
