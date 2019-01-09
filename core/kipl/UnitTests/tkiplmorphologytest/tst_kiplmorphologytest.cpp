#include <iostream>
#include <algorithm>
#include <list>

#include <QString>
#include <QtTest>
#include <QDebug>
#include <base/kiplenums.h>
#include <morphology/pixeliterator.h>
#include <base/KiplException.h>
#include <morphology/morphextrema.h>
#include <morphology/morphfilters.h>
#include <morphology/repairhole.h>
#include <io/io_tiff.h>

using namespace std;

class kiplMorphologyTest : public QObject
{
    Q_OBJECT

public:
    kiplMorphologyTest();

private Q_SLOTS:
    void testPixelIteratorSetup();
    void testPixelIteratorMoving();
    void testPixelIteratorNeighborhood();
    void testPixelIteratorHalfNeighborhood();
    void testErosion2D();
    void testDilation2D();
    void testErosion3D();
    void testDilation3D();
    void testhMax();
    void testRepairHoles();
};

kiplMorphologyTest::kiplMorphologyTest()
{

}

void kiplMorphologyTest::testPixelIteratorSetup()
{
    // Tests with 4-connectivity
    size_t dims4[2]={11,5};

    kipl::base::PixelIterator it4(dims4,kipl::base::conn4);

    QVERIFY2(it4.currentPosition() == 0,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::cornerX0Y0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 2, "Wrong neighborhood size for corner x0y0 (conn4)");

    QVERIFY2(it4.forwardSize()        == 2, "Wrong sub neighborhood size (conn4)");
    QVERIFY2(it4.connectivity()    == kipl::base::conn4, "Wrong connectivity (conn4)");
    QVERIFY2(it4.end()==static_cast<ptrdiff_t>(dims4[0]*dims4[1]), "Wrong end position");
    QVERIFY2(it4.begin()              == 0UL,"Wrong start position");

    // Tests with 8-connectivity
    size_t dims8[2]={9,7};

    kipl::base::PixelIterator it8(dims8,kipl::base::conn8);

    QVERIFY2(it8.neighborhoodSize()   == 3, "Wrong neighborhood size (conn8)");
    QVERIFY2(it8.forwardSize()        == 3, "Wrong sub neighborhood size (conn8)");
    QVERIFY2(it8.connectivity()    == kipl::base::conn8, "Wrong connectivity (conn8)");
    QVERIFY2(it8.end() == static_cast<ptrdiff_t>(dims8[0]*dims8[1]), "Wrong end position");
    QVERIFY2(it8.begin()              == 0UL,"Wrong start position");
    QVERIFY2(it8.currentPosition() == 0,"Initialize to wrong start position");

    // Test copying
    ++it8;
    kipl::base::PixelIterator itcpctor(it8);

    QVERIFY2(itcpctor.neighborhoodSize()    == it8.neighborhoodSize(), "Neighborhood size miss match");
    QVERIFY2(itcpctor.forwardSize()         == it8.forwardSize()     , "Neighborhood size miss match");
    QVERIFY2(itcpctor.connectivity()     == it8.connectivity() , "Copy connectivity failed");
    QVERIFY2(itcpctor.currentPosition()  == it8.currentPosition(), "Copy position failed");

    kipl::base::PixelIterator iteq(it8);

    iteq=it4;

    QVERIFY2(iteq.neighborhoodSize()    == it4.neighborhoodSize(),"Neighborhood size miss match");
    QVERIFY2(iteq.forwardSize()         == it4.forwardSize(),"Neighborhood size miss match");
    QVERIFY2(iteq.connectivity()     == it4.connectivity(),"Copy connectivity failed");
    QVERIFY2(iteq.currentPosition()  == it4.currentPosition(),"Copy position failed");

    QVERIFY_EXCEPTION_THROWN(it4.setPosition(-1,0),kipl::base::KiplException);

}

void kiplMorphologyTest::testPixelIteratorMoving()
{
    size_t dims4u[2]={11,5};
    ptrdiff_t dims4[2];
    std::copy(dims4u,dims4u+2,dims4);
    kipl::base::PixelIterator it4(dims4u,kipl::base::conn4);
    ++it4;
    QVERIFY2(it4.currentPosition() == 1,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::edgeY0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 3, "Wrong neighborhood size for edge y0 (conn4)");

    it4.setPosition(10,0);
    QVERIFY2(it4.currentPosition() == 10,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::cornerX1Y0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 2, "Wrong neighborhood size for edge x0 (conn4)");

    --it4;
    QVERIFY2(it4.currentPosition() == 9,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::edgeY0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 3, "Wrong neighborhood size for edge x0 (conn4)");

    it4.setPosition(0,3);
    QVERIFY2(it4.currentPosition() == 3*dims4[0],"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::edgeX0,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 3, "Wrong neighborhood size for edge x0 (conn4)");

    ++it4;
    QVERIFY2(it4.currentPosition() == 3*dims4[0]+1,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::noEdge,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 4, "Wrong neighborhood size for edge x0 (conn4)");

    it4.setPosition(10,3);
    QVERIFY2(it4.currentPosition() == 3*dims4[0]+dims4[0]-1,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::edgeX1,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 3, "Wrong neighborhood size for edge x0 (conn4)");

    --it4;
    QVERIFY2(it4.currentPosition() == 3*dims4[0]+dims4[0]-2,"Initialize to wrong start position");
    QVERIFY2(it4.edgeStatus()      == kipl::base::noEdge,"Wrong edge status at init");
    QVERIFY2(it4.neighborhoodSize()   == 4, "Wrong neighborhood size for edge x0 (conn4)");

    kipl::base::PixelIterator it8(dims4u,kipl::base::conn8);
    std::copy(dims4u,dims4u+2,dims4);
    ++it8;
    QVERIFY2(it8.currentPosition() == 1,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::edgeY0,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 5, "Wrong neighborhood size for edge y0 (conn4)");

    it8.setPosition(10,0);
    QVERIFY2(it8.currentPosition() == 10,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::cornerX1Y0,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 3, "Wrong neighborhood size for edge x0 (conn4)");

    --it8;
    QVERIFY2(it8.currentPosition() == 9,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::edgeY0,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 5, "Wrong neighborhood size for edge x0 (conn4)");

    it8.setPosition(0,3);
    QVERIFY2(it8.currentPosition() == 3*dims4[0],"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::edgeX0,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 5, "Wrong neighborhood size for edge x0 (conn4)");

    ++it8;
    QVERIFY2(it8.currentPosition() == 3*dims4[0]+1,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::noEdge,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 8, "Wrong neighborhood size for edge x0 (conn4)");

    it8.setPosition(10,3);
    QVERIFY2(it8.currentPosition() == 3*dims4[0]+dims4[0]-1,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::edgeX1,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 5, "Wrong neighborhood size for edge x0 (conn4)");

    --it8;
    QVERIFY2(it8.currentPosition() == 3*dims4[0]+dims4[0]-2,"Initialize to wrong start position");
    QVERIFY2(it8.edgeStatus()      == kipl::base::noEdge,"Wrong edge status at init");
    QVERIFY2(it8.neighborhoodSize()   == 8, "Wrong neighborhood size for edge x0 (conn4)");

}

void kiplMorphologyTest::testPixelIteratorNeighborhood()
{
    size_t dims4[2]={11,5};
    kipl::base::PixelIterator it4(dims4,kipl::base::conn4);

    QVERIFY2(it4.neighborhood(0) == 1,"Corner X0Y0");
    QVERIFY2(it4.neighborhood(1) == 11,"Corner X0Y0");

    it4.setPosition(dims4[0]-1,0);
    QVERIFY2(it4.neighborhood(0) == 9,"Corner X1Y0");
    QVERIFY2(it4.neighborhood(1) == 21,"Corner X1Y0");

    it4.setPosition(0,dims4[1]-1);
    QVERIFY2(it4.neighborhood(0) == 45,"Corner X0Y1");
    QVERIFY2(it4.neighborhood(1) == 33,"Corner X0Y1");

    it4.setPosition(dims4[0]-1,dims4[1]-1);
    QVERIFY2(it4.neighborhood(0) == 53,"Corner X1Y1");
    QVERIFY2(it4.neighborhood(1) == 43,"Corner X1Y1");

    it4.setPosition(5,0);
    QVERIFY2(it4.neighborhood(0) == 4,"Edge Y0");
    QVERIFY2(it4.neighborhood(1) == 6,"Edge Y0");
    QVERIFY2(it4.neighborhood(2) == 16,"Edge Y0");

    it4.setPosition(5,4);
    QVERIFY2(it4.neighborhood(0) == 50,"Edge Y1");
    QVERIFY2(it4.neighborhood(1) == 48,"Edge Y1");
    QVERIFY2(it4.neighborhood(2) == 38,"Edge Y1");

    it4.setPosition(0,3);
    QVERIFY2(it4.neighborhood(0) == 22,"Edge Y0");
    QVERIFY2(it4.neighborhood(1) == 34,"Edge Y0");
    QVERIFY2(it4.neighborhood(2) == 44,"Edge Y0");

    it4.setPosition(dims4[0]-1,3);
    QVERIFY2(it4.neighborhood(0) == 54,"Edge Y1");
    QVERIFY2(it4.neighborhood(1) == 42,"Edge Y1");
    QVERIFY2(it4.neighborhood(2) == 32,"Edge Y1");

    it4.setPosition(5,2);
    QVERIFY2(it4.neighborhood(0) == 16,"NoEdge");
    QVERIFY2(it4.neighborhood(1) == 26,"NoEdge");
    QVERIFY2(it4.neighborhood(2) == 28,"NoEdge");
    QVERIFY2(it4.neighborhood(3) == 38,"NoEdge");

    kipl::base::PixelIterator it8(dims4,kipl::base::conn8);

    QVERIFY2(it8.neighborhood(0) == 1,"Corner X0Y0");
    QVERIFY2(it8.neighborhood(1) == 11,"Corner X0Y0");
    QVERIFY2(it8.neighborhood(2) == 12,"Corner X0Y0");

    it8.setPosition(dims4[0]-1,0);
    QVERIFY2(it8.neighborhood(0) == 9,"Corner X1Y0");
    QVERIFY2(it8.neighborhood(1) == 21,"Corner X1Y0");
    QVERIFY2(it8.neighborhood(2) == 20,"Corner X1Y0");

    it8.setPosition(0,dims4[1]-1);
    QVERIFY2(it8.neighborhood(0) == 45,"Corner X0Y1");
    QVERIFY2(it8.neighborhood(1) == 33,"Corner X0Y1");
    QVERIFY2(it8.neighborhood(2) == 34,"Corner X0Y1");

    it8.setPosition(dims4[0]-1,dims4[1]-1);
    QVERIFY2(it8.neighborhood(0) == 53,"Corner X1Y1");
    QVERIFY2(it8.neighborhood(1) == 43,"Corner X1Y1");
    QVERIFY2(it8.neighborhood(2) == 42,"Corner X1Y1");

    it8.setPosition(5,0);
    QVERIFY2(it8.neighborhood(0) == 4,"Edge Y0");
    QVERIFY2(it8.neighborhood(1) == 6,"Edge Y0");
    QVERIFY2(it8.neighborhood(2) == 15,"Edge Y0");
    QVERIFY2(it8.neighborhood(3) == 16,"Edge Y0");
    QVERIFY2(it8.neighborhood(4) == 17,"Edge Y0");

    it8.setPosition(5,4);
    QVERIFY2(it8.neighborhood(0) == 50,"Edge Y1");
    QVERIFY2(it8.neighborhood(1) == 48,"Edge Y1");
    QVERIFY2(it8.neighborhood(2) == 39,"Edge Y1");
    QVERIFY2(it8.neighborhood(3) == 38,"Edge Y1");
    QVERIFY2(it8.neighborhood(4) == 37,"Edge Y1");

    it8.setPosition(0,3);
    QVERIFY2(it8.neighborhood(0) == 22,"Edge X0");
    QVERIFY2(it8.neighborhood(1) == 23,"Edge X0");
    QVERIFY2(it8.neighborhood(2) == 34,"Edge X0");
    QVERIFY2(it8.neighborhood(3) == 44,"Edge X0");
    QVERIFY2(it8.neighborhood(4) == 45,"Edge X0");

    it8.setPosition(dims4[0]-1,3);
    QVERIFY2(it8.neighborhood(0) == 54,"Edge X1");
    QVERIFY2(it8.neighborhood(1) == 53,"Edge X1");
    QVERIFY2(it8.neighborhood(2) == 42,"Edge X1");
    QVERIFY2(it8.neighborhood(3) == 32,"Edge X1");
    QVERIFY2(it8.neighborhood(4) == 31,"Edge X1");

    it8.setPosition(5,2);
    QVERIFY2(it8.neighborhood(0) == 15,"NoEdge");
    QVERIFY2(it8.neighborhood(1) == 16,"NoEdge");
    QVERIFY2(it8.neighborhood(2) == 17,"NoEdge");
    QVERIFY2(it8.neighborhood(3) == 26,"NoEdge");
    QVERIFY2(it8.neighborhood(4) == 28,"NoEdge");
    QVERIFY2(it8.neighborhood(5) == 37,"NoEdge");
    QVERIFY2(it8.neighborhood(6) == 38,"NoEdge");
    QVERIFY2(it8.neighborhood(7) == 39,"NoEdge");
}

void kiplMorphologyTest::testPixelIteratorHalfNeighborhood()
{
    size_t dims4[2]={11,5};

    // Test 4-connected neighbors
    {
    kipl::base::PixelIterator it4(dims4,kipl::base::conn4);

    QVERIFY2(it4.forwardSize()  == 2, "Forward size at Corner X0Y0");
    QVERIFY2(it4.backwardSize() == 0, "backward size at Corner X0Y0");

    QVERIFY2(it4.forwardNeighborhood(0) == 1,"Forward corner X0Y0");
    QVERIFY2(it4.forwardNeighborhood(1) == 11,"Forward corner X0Y0");

    it4.setPosition(dims4[0]-1,0);

    QVERIFY2(it4.forwardSize()  == 1, "Forward size at Corner X1Y0");
    QVERIFY2(it4.backwardSize() == 1, "Backward size at Corner X1Y0");

    QVERIFY2(it4.forwardNeighborhood(0) == ptrdiff_t(2*dims4[0]-1),"Forward corner X1Y0");
    QVERIFY2(it4.backwardNeighborhood(0) == ptrdiff_t(dims4[0]-2),"BackwardCorner X1Y0");

    it4.setPosition(0,dims4[1]-1);
    QVERIFY2(it4.forwardSize()  == 1, "Forward size at Corner X0Y1");
    QVERIFY2(it4.backwardSize() == 1, "Backward size at Corner X0Y1");

    QVERIFY2(it4.forwardNeighborhood(0) == 45,"Corner X0Y1");
    QVERIFY2(it4.backwardNeighborhood(0) == 33,"Corner X0Y1");

    it4.setPosition(dims4[0]-1,dims4[1]-1);
    QVERIFY2(it4.forwardSize()  == 0, "Forward size at Corner X1Y1");
    QVERIFY2(it4.backwardSize() == 2, "Backward size at Corner X1Y1");

    QVERIFY2(it4.backwardNeighborhood(0) == 53,"Corner X1Y1");
    QVERIFY2(it4.backwardNeighborhood(1) == 43,"Corner X1Y1");

    it4.setPosition(5,0);
    QVERIFY2(it4.forwardSize()  == 2, "Forward size at Edge Y0");
    QVERIFY2(it4.backwardSize() == 1, "Backward size at Edge Y0");

    QVERIFY2(it4.forwardNeighborhood(0) == 6,"Edge Y0");
    QVERIFY2(it4.forwardNeighborhood(1) == 16,"Edge Y0");
    QVERIFY2(it4.backwardNeighborhood(0) == 4,"Edge Y0");

    it4.setPosition(5,4);
    QVERIFY2(it4.forwardSize()  == 1, "Forward size at Edge Y1");
    QVERIFY2(it4.backwardSize() == 2, "Backward size at Edge Y1");

    QVERIFY2(it4.forwardNeighborhood(0) == 50,"Edge Y1");
    QVERIFY2(it4.backwardNeighborhood(0) == 48,"Edge Y1");
    QVERIFY2(it4.backwardNeighborhood(1) == 38,"Edge Y1");

    it4.setPosition(0,3);
    QVERIFY2(it4.forwardSize()  == 2, "Forward size at Edge X0");
    QVERIFY2(it4.backwardSize() == 1, "Backward size at Edge X0");

    QVERIFY2(it4.forwardNeighborhood(0) == 34,"Edge X0");
    QVERIFY2(it4.forwardNeighborhood(1) == 44,"Edge X0");
    QVERIFY2(it4.backwardNeighborhood(0) == 22,"Edge X0");

    it4.setPosition(dims4[0]-1,3);
    QVERIFY2(it4.forwardSize()  == 1, "Forward size at Edge X1");
    QVERIFY2(it4.backwardSize() == 2, "Backward size at Edge X1");

    QVERIFY2(it4.forwardNeighborhood(0) == 54,"Edge X1");
    QVERIFY2(it4.backwardNeighborhood(0) == 42,"Edge X1");
    QVERIFY2(it4.backwardNeighborhood(1) == 32,"Edge X1");

    it4.setPosition(5,2);
    QVERIFY2(it4.forwardSize()  == 2, "Forward size at NoEdge");
    QVERIFY2(it4.backwardSize() == 2, "Backward size at NoEdge");

    QVERIFY2(it4.forwardNeighborhood(0) == 28,"NoEdge");
    QVERIFY2(it4.forwardNeighborhood(1) == 38,"NoEdge");
    QVERIFY2(it4.backwardNeighborhood(0) == 16,"NoEdge");
    QVERIFY2(it4.backwardNeighborhood(1) == 26,"NoEdge");
    }

    // Test 8-connected neighbors

    {
    kipl::base::PixelIterator it8(dims4,kipl::base::conn8);

    QVERIFY2(it8.forwardSize()  == 3, "Forward size at Corner X0Y0");
    QVERIFY2(it8.backwardSize() == 0, "backward size at Corner X0Y0");


    QVERIFY2(it8.forwardNeighborhood(0) == 1,"Corner X0Y0");
    QVERIFY2(it8.forwardNeighborhood(1) == 11,"Corner X0Y0");
    QVERIFY2(it8.forwardNeighborhood(2) == 12,"Corner X0Y0");

    it8.setPosition(dims4[0]-1,0);
    QVERIFY2(it8.forwardSize()  == 2, "Forward size at Corner X1Y0");
    QVERIFY2(it8.backwardSize() == 1, "backward size at Corner X1Y0");

    QVERIFY2(it8.forwardNeighborhood(0) == 21,"Corner X1Y0");
    QVERIFY2(it8.forwardNeighborhood(1) == 20,"Corner X1Y0");
    QVERIFY2(it8.backwardNeighborhood(0) == 9,"Corner X1Y0");

    it8.setPosition(0,dims4[1]-1);
    QVERIFY2(it8.forwardSize()  == 1, "Forward size at Corner X0Y1");
    QVERIFY2(it8.backwardSize() == 2, "backward size at Corner X0Y1");

    QVERIFY2(it8.forwardNeighborhood(0) == 45,"Corner X0Y1");
    QVERIFY2(it8.backwardNeighborhood(0) == 33,"Corner X0Y1");
    QVERIFY2(it8.backwardNeighborhood(1) == 34,"Corner X0Y1");

    it8.setPosition(dims4[0]-1,dims4[1]-1);
    QVERIFY2(it8.forwardSize()  == 0, "Forward size at Corner X1Y1");
    QVERIFY2(it8.backwardSize() == 3, "backward size at Corner X1Y1");

    QVERIFY2(it8.backwardNeighborhood(0) == 53,"Corner X1Y1");
    QVERIFY2(it8.backwardNeighborhood(1) == 43,"Corner X1Y1");
    QVERIFY2(it8.backwardNeighborhood(2) == 42,"Corner X1Y1");

    it8.setPosition(5,0);
    QVERIFY2(it8.forwardSize()  == 4, "Forward size at Edge Y0");
    QVERIFY2(it8.backwardSize() == 1, "backward size at Edge Y0");

    QVERIFY2(it8.forwardNeighborhood(0) == 6,"Edge Y0");
    QVERIFY2(it8.forwardNeighborhood(1) == 15,"Edge Y0");
    QVERIFY2(it8.forwardNeighborhood(2) == 16,"Edge Y0");
    QVERIFY2(it8.forwardNeighborhood(3) == 17,"Edge Y0");
    QVERIFY2(it8.backwardNeighborhood(0) == 4,"Edge Y0");

    it8.setPosition(5,4);
    QVERIFY2(it8.forwardSize()  == 1, "Forward size at Edge Y1");
    QVERIFY2(it8.backwardSize() == 4, "backward size at Edge Y1");

    QVERIFY2(it8.forwardNeighborhood(0) == 50,"Edge Y1");
    QVERIFY2(it8.backwardNeighborhood(0) == 48,"Edge Y1");
    QVERIFY2(it8.backwardNeighborhood(1) == 39,"Edge Y1");
    QVERIFY2(it8.backwardNeighborhood(2) == 38,"Edge Y1");
    QVERIFY2(it8.backwardNeighborhood(3) == 37,"Edge Y1");

    it8.setPosition(0,3);
    QVERIFY2(it8.forwardSize()  == 3, "Forward size at Edge X0");
    QVERIFY2(it8.backwardSize() == 2, "backward size at Edge X0");

    QVERIFY2(it8.forwardNeighborhood(0) == 34,"Edge X0");
    QVERIFY2(it8.forwardNeighborhood(1) == 44,"Edge X0");
    QVERIFY2(it8.forwardNeighborhood(2) == 45,"Edge X0");
    QVERIFY2(it8.backwardNeighborhood(0) == 22,"Edge X0");
    QVERIFY2(it8.backwardNeighborhood(1) == 23,"Edge X0");

    it8.setPosition(dims4[0]-1,3);
    QVERIFY2(it8.forwardSize()  == 2, "Forward size at Edge X1");
    QVERIFY2(it8.backwardSize() == 3, "Backward size at Edge X1");

    QVERIFY2(it8.forwardNeighborhood(0) == 54,"Edge X1");
    QVERIFY2(it8.forwardNeighborhood(1) == 53,"Edge X1");
    QVERIFY2(it8.backwardNeighborhood(0) == 42,"Edge X1");
    QVERIFY2(it8.backwardNeighborhood(1) == 32,"Edge X1");
    QVERIFY2(it8.backwardNeighborhood(2) == 31,"Edge X1");

    it8.setPosition(5,2);
    QVERIFY2(it8.forwardSize()  == 4, "Forward size at NoEdge");
    QVERIFY2(it8.backwardSize() == 4, "backward size at NoEdge");

    QVERIFY2(it8.forwardNeighborhood(0) == 28,"NoEdge");
    QVERIFY2(it8.forwardNeighborhood(1) == 37,"NoEdge");
    QVERIFY2(it8.forwardNeighborhood(2) == 38,"NoEdge");
    QVERIFY2(it8.forwardNeighborhood(3) == 39,"NoEdge");
    QVERIFY2(it8.backwardNeighborhood(0) == 15,"NoEdge");
    QVERIFY2(it8.backwardNeighborhood(1) == 16,"NoEdge");
    QVERIFY2(it8.backwardNeighborhood(2) == 17,"NoEdge");
    QVERIFY2(it8.backwardNeighborhood(3) == 26,"NoEdge");
    }
}

void kiplMorphologyTest::testhMax()
{
    size_t dims[2]={11,13};
    kipl::base::TImage<float,2> img(dims);
    int idx=0;
    for (int y=0;y<dims[1]; ++y)
        for (int x=0; x<dims[0]; ++x,++idx) {
            img[idx]=sqrt((x-dims[0]/2)*(x-dims[0]/2)+(y-dims[1]/2)*(y-dims[1]/2));
        }

    kipl::base::TImage<float,2> res;
    kipl::morphology::hMax(img,res,3.0f);

    QVERIFY(img.Size()==res.Size());
    QVERIFY(img.Size(0)==res.Size(0));
    QVERIFY(img.Size(1)==res.Size(1));

    kipl::io::WriteTIFF32(img,"hmax_orig.tif");
    kipl::io::WriteTIFF32(res,"hmax_res.tif");

}

void kiplMorphologyTest::testErosion2D()
{
    size_t dims[]={7,7};
    kipl::base::TImage<float,2> orig(dims);
    for (int i=2; i<5; ++i)
        std::fill(orig.GetLinePtr(i)+2,orig.GetLinePtr(i)+5,1);

    // Simple case
    float se4[]={0,1,0,1,1,1,0,1,0};
    size_t sedims[2]={3,3};
    kipl::morphology::TErode<float,2> er4(se4,sedims);

    kipl::base::TImage<float,2> res;

    res=er4(orig,kipl::filters::FilterBase::EdgeMirror);
    for (size_t i=0; i<res.Size(); ++i) {
    //    qDebug() << i<<" -> "<<res[i];
        QVERIFY(res[i]== (i==24 ? 1.0f :0.0f));
    }

    res=er4(orig,kipl::filters::FilterBase::EdgeValid);
    for (size_t i=0; i<res.Size(); ++i) {
    //    qDebug() << i<<" -> "<<res[i];
        QVERIFY(res[i]== (i==24 ? 1.0f :0.0f));
    }

    res=er4(orig,kipl::filters::FilterBase::EdgeZero);
    for (size_t i=0; i<res.Size(); ++i) {
    //    qDebug() << i<<" -> "<<res[i];
        QVERIFY(res[i]== (i==24 ? 1.0f :0.0f));
    }
}

void kiplMorphologyTest::testDilation2D()
{

}

void kiplMorphologyTest::testErosion3D()
{

}

void kiplMorphologyTest::testDilation3D()
{

}

void kiplMorphologyTest::testRepairHoles()
{
    size_t dims[2]={10,10};
    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<dims[1]; ++i)
    {
        for (size_t j=0; j<dims[0]; ++j)
        {
          img(j,i)=i*dims[0]+j;
        }
    }

    std::list<size_t> plist;

    for (size_t i=3; i<7; ++i)
    {
        for (size_t j=4; j<8; ++j)
        {
          img(j,i)=-1;
          plist.push_back(i*dims[0]+j);
        }
    }

    kipl::io::WriteTIFF(img,"preRepair.tiff");
    kipl::morphology::RepairHoles(img,plist,kipl::morphology::conn8);
    kipl::io::WriteTIFF(img,"repaired.tiff");
}

QTEST_APPLESS_MAIN(kiplMorphologyTest)

#include "tst_kiplmorphologytest.moc"
