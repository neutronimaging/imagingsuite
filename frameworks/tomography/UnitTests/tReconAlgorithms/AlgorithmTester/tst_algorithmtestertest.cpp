#include <list>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>

#include <basicforwardprojector.h>
#include <nnforwardprojector.h>
#include <linearforwardprojector.h>

class AlgorithmTesterTest : public QObject
{
    Q_OBJECT

public:
    AlgorithmTesterTest();

private Q_SLOTS:
    void testNN();
    void testLinearFwd();
};

AlgorithmTesterTest::AlgorithmTesterTest()
{
}

void AlgorithmTesterTest::testNN()
{
    list<float> angles;
    int N=360;

    float dt=360.0f/N;
    for (int i=0; i<N; i++)
        angles.push_back(i*dt);

   // BasicForwardProjector bfp;
    NNForwardProjector nnfp;
    kipl::base::TImage<float,2> slice;
    kipl::base::TImage<float,2> proj;

    kipl::io::ReadTIFF(slice,"../../data/simple_sample.tif");
    nnfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultNN_simplesample.tif");

    kipl::io::ReadTIFF(slice,"../../data/disk256.tif");
    nnfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultNN_disk256.tif");

    kipl::io::ReadTIFF(slice,"../../data/twodisks256.tif");
    nnfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultNN_twodisks256.tif");

  //  QVERIFY2(true, "Failure");
}

void AlgorithmTesterTest::testLinearFwd()
{
    list<float> angles;
    int N=360;

    float dt=360.0f/N;
    for (int i=0; i<N; i++)
        angles.push_back(i*dt);

   // BasicForwardProjector bfp;
    LinearForwardProjector linfp;
    kipl::base::TImage<float,2> slice;
    kipl::base::TImage<float,2> proj;

    kipl::io::ReadTIFF(slice,"../../data/simple_sample.tif");
    linfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultLin_simplesample.tif");

    kipl::io::ReadTIFF(slice,"../../data/disk256.tif");
    linfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultLin_disk256.tif");

    kipl::io::ReadTIFF(slice,"../../data/twodisks256.tif");
    linfp.project(slice,angles,proj);
    QVERIFY(angles.size()==proj.Size(1));
    kipl::io::WriteTIFF32(proj,"resultLin_twodisks256.tif");

  //  QVERIFY2(true, "Failure");
}
QTEST_APPLESS_MAIN(AlgorithmTesterTest)

#include "tst_algorithmtestertest.moc"
