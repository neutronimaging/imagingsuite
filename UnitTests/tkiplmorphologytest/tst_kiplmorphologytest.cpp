#include <iostream>
#include <QString>
#include <QtTest>
#include <base/kiplenums.h>
#include <morphology/pixeliterator.h>

using namespace std;

class kiplMorphologyTest : public QObject
{
    Q_OBJECT

public:
    kiplMorphologyTest();

private Q_SLOTS:
    void testPixelIteratorSetup();
    void testPixelIteratorAction();
};

kiplMorphologyTest::kiplMorphologyTest()
{

}

void kiplMorphologyTest::testPixelIteratorSetup()
{
    // Tests with 4-connectivity
    size_t dims4[2]={11,5};

    kipl::base::PixelIterator it4(dims4,kipl::base::conn4);

    QVERIFY2(it4.getCurrentPosition()==0,"Initialize to wrong start position");
    cout<<static_cast<int>(it4.getEdgeStatus())<<endl;
    QVERIFY2(it4.getEdgeStatus()==kipl::base::cornerX0Y0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()==2, "Wrong neighborhood size for corner x0y0 (conn4)");

    QVERIFY2(it4.forwardSize()==2, "Wrong sub neighborhood size (conn4)");
    QVERIFY2(it4.getConnectivity()==kipl::base::conn4, "Wrong connectivity (conn4)");
    QVERIFY2(it4.end()==static_cast<ptrdiff_t>(dims4[0]*dims4[1]), "Wrong end position");
    QVERIFY2(it4.begin()==0UL,"Wrong start position");


    ++it4;
    QVERIFY2(it4.getCurrentPosition()==1,"Initialize to wrong start position");
    QVERIFY2(it4.neighborhoodSize()==3, "Wrong neighborhood size for edge y0 (conn4)");

    it4.setPosition(0,10);
    QVERIFY2(it4.getCurrentPosition()==10,"Initialize to wrong start position");
    QVERIFY2(it4.neighborhoodSize()==2, "Wrong neighborhood size for corner x1y0 (conn4)");

    ++it4;
    QVERIFY2(it4.getCurrentPosition()==11,"Initialize to wrong start position");
    QVERIFY2(it4.neighborhoodSize()==3, "Wrong neighborhood size for edge x0 (conn4)");



    // Tests with 8-connectivity
    size_t dims8[2]={9,7};

    kipl::base::PixelIterator it8(dims8,kipl::base::conn8);

    QVERIFY2(it8.neighborhoodSize()==8, "Wrong neighborhood size (conn8)");
    QVERIFY2(it8.forwardSize()==3, "Wrong sub neighborhood size (conn8)");
    QVERIFY2(it8.getConnectivity()==kipl::base::conn8, "Wrong connectivity (conn8)");
    QVERIFY2(it8.end()==static_cast<ptrdiff_t>(dims8[0]*dims8[1]), "Wrong end position");
    QVERIFY2(it8.begin()==0UL,"Wrong start position");
    QVERIFY2(it8.getCurrentPosition()==0,"Initialize to wrong start position");

    // Test copying
    ++it8;
    kipl::base::PixelIterator itcpctor(it8);

    QVERIFY2(itcpctor.neighborhoodSize()    == it8.neighborhoodSize(),"Neighborhood size miss match");
    QVERIFY2(itcpctor.forwardSize() == it8.forwardSize(),"Neighborhood size miss match");
    QVERIFY2(itcpctor.getConnectivity()     == it8.getConnectivity(),"Copy connectivity failed");
    QVERIFY2(itcpctor.getCurrentPosition()  == it8.getCurrentPosition(),"Copy position failed");

    kipl::base::PixelIterator iteq(it8);

    iteq=it4;

    QVERIFY2(iteq.neighborhoodSize()    == it4.neighborhoodSize(),"Neighborhood size miss match");
    QVERIFY2(iteq.forwardSize()         == it4.forwardSize(),"Neighborhood size miss match");
    QVERIFY2(iteq.getConnectivity()     == it4.getConnectivity(),"Copy connectivity failed");
    QVERIFY2(iteq.getCurrentPosition()  == it4.getCurrentPosition(),"Copy position failed");


}

void kiplMorphologyTest::testPixelIteratorAction()
{

    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(kiplMorphologyTest)

#include "tst_kiplmorphologytest.moc"
