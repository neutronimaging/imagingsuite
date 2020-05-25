#include <QString>
#include <QtTest>

#include <base/index2coord.h>

class TIndex2CoordTest : public QObject
{
    Q_OBJECT

public:
    TIndex2CoordTest();

private Q_SLOTS:
    void testCoord3D();
    void testCoord3Df();
    void testCoord2D();
    void testIndex2Coord2D();
    void testIndex2Coord3D();
};

TIndex2CoordTest::TIndex2CoordTest()
{

}

void TIndex2CoordTest::testCoord3D()
{
    kipl::base::coords3D c0;

    QCOMPARE(c0.x,short(0));
    QCOMPARE(c0.y,short(0));
    QCOMPARE(c0.z,short(0));
    QCOMPARE(c0.reserved,short(0));

    kipl::base::coords3D c1(10,20,30);

    QCOMPARE(c1.x,short(10));
    QCOMPARE(c1.y,short(20));
    QCOMPARE(c1.z,short(30));
    QCOMPARE(c1.reserved,short(0));

    kipl::base::coords3D c2(c1);

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);
    QCOMPARE(c2.z,c1.z);
    QCOMPARE(c2.reserved,short(0));

    c2=c0;
    c1=kipl::base::coords3D(50,60,70);
    c2=c1;

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);
    QCOMPARE(c2.z,c1.z);
    QCOMPARE(c2.reserved,short(0));

    QVERIFY(c1==c2);
    QVERIFY(c1!=c0);

}

void TIndex2CoordTest::testCoord3Df()
{
    kipl::base::coords3Df c0;

    QCOMPARE(c0.x,0.0f);
    QCOMPARE(c0.y,0.0f);
    QCOMPARE(c0.z,0.0f);
    QCOMPARE(c0.reserved,0.0f);

    kipl::base::coords3Df c1(10.0f,20.0f,30.0f);

    QCOMPARE(c1.x,10.0f);
    QCOMPARE(c1.y,20.0f);
    QCOMPARE(c1.z,30.0f);
    QCOMPARE(c1.reserved,0.0f);

    kipl::base::coords3Df c2(c1);

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);
    QCOMPARE(c2.z,c1.z);
    QCOMPARE(c2.reserved,0.0f);

    c2=c0;
    c1=kipl::base::coords3Df(50.0f,60.0f,70.0f);
    c2=c1;

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);
    QCOMPARE(c2.z,c1.z);
    QCOMPARE(c2.reserved,0.0f);

    QVERIFY(c1==c2);
    QVERIFY(c1!=c0);
}

void TIndex2CoordTest::testCoord2D()
{
    kipl::base::coords2D c0;

    QCOMPARE(c0.x,short(0));
    QCOMPARE(c0.y,short(0));


    kipl::base::coords2D c1(10,20);

    QCOMPARE(c1.x,short(10));
    QCOMPARE(c1.y,short(20));

    kipl::base::coords2D c2(c1);

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);

    c2=c0;
    c1=kipl::base::coords2D(50,60);
    c2=c1;

    QCOMPARE(c2.x,c1.x);
    QCOMPARE(c2.y,c1.y);

    QVERIFY(c1==c2);
    QVERIFY(c1!=c0);
}

void TIndex2CoordTest::testIndex2Coord2D()
{
    std::vector<size_t> dims={5,6};
    kipl::base::Index2Coordinates i2c(dims,2);
    kipl::base::coords2D c2;
    kipl::base::coords3D c3;

    for (size_t y=0; y<dims[1]; ++y)
        for (size_t x=0; x<dims[0]; ++x)
    {
        i2c(y*dims[0]+x,c3);

        QCOMPARE(c3.x,short(x));
        QCOMPARE(c3.y,short(y));
        QCOMPARE(c3.z,short(0));

        i2c(y*dims[0]+x,c2);

        QCOMPARE(c2.x,short(x));
        QCOMPARE(c2.y,short(y));

    }
}

void TIndex2CoordTest::testIndex2Coord3D()
{
    std::vector<size_t> dims={5,6,9};
    kipl::base::Index2Coordinates i2c(dims,3);
    kipl::base::coords2D c2;
    kipl::base::coords3D c3;

    for (size_t z=0; z<dims[2]; ++z)
        for (size_t y=0; y<dims[1]; ++y)
            for (size_t x=0; x<dims[0]; ++x)
    {
        i2c(z*dims[0]*dims[1]+y*dims[0]+x,c3);

        QCOMPARE(c3.x,short(x));
        QCOMPARE(c3.y,short(y));
        QCOMPARE(c3.z,short(z));

        i2c(z*dims[0]*dims[1]+y*dims[0]+x,c2);

        QCOMPARE(c2.x,short(x));
        QCOMPARE(c2.y,short(y));

    }

}



QTEST_APPLESS_MAIN(TIndex2CoordTest)

#include "tst_tindex2coordtest.moc"
