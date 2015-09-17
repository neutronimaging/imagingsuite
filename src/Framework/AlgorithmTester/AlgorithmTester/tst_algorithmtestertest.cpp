#include <list>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>

#include <basicforwardprojector.h>

class AlgorithmTesterTest : public QObject
{
    Q_OBJECT

public:
    AlgorithmTesterTest();

private Q_SLOTS:
    void testCase1();

};

AlgorithmTesterTest::AlgorithmTesterTest()
{
}

void AlgorithmTesterTest::testCase1()
{
    list<float> angles;
    for (int i=0; i<7; i++)
        angles.push_back(i*15.0f);

    BasicForwardProjector bfp;
    kipl::base::TImage<float,2> slice;
    kipl::base::TImage<float,2> proj;

    kipl::io::ReadTIFF(slice,"../../data/phantom256.tif");

    bfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));

    kipl::io::WriteTIFF32(proj,"proj_phantom256.tif");

    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(AlgorithmTesterTest)

#include "tst_algorithmtestertest.moc"
