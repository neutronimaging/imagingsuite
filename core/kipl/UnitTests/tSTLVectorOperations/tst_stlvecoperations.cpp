#include <QtTest>

// add necessary includes here
#include <stltools/stlvecmath.h>

class STLVecOperations : public QObject
{
    Q_OBJECT

public:
    STLVecOperations();
    ~STLVecOperations();

private slots:
    void test_medianFilter();

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

QTEST_APPLESS_MAIN(STLVecOperations)

#include "tst_stlvecoperations.moc"
