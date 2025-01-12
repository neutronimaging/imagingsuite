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
    // QSKIP("Under development");
//     list<float> angles;
//     int N=360;

//     float dt=360.0f/N;
//     for (int i=0; i<N; i++)
//         angles.push_back(i*dt);

//    // BasicForwardProjector bfp;
//     NNForwardProjector nnfp;
//     kipl::base::TImage<float,2> slice;
//     kipl::base::TImage<float,2> proj;

//     kipl::io::ReadTIFF(slice,"../../data/simple_sample.tif");
//     nnfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultNN_simplesample.tif",kipl::base::Float32);

//     kipl::io::ReadTIFF(slice,"../../data/disk256.tif");
//     nnfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultNN_disk256.tif",kipl::base::Float32);

//     kipl::io::ReadTIFF(slice,"../../data/twodisks256.tif");
//     nnfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultNN_twodisks256.tif",kipl::base::Float32);

  //  QVERIFY2(true, "Failure");
}

void AlgorithmTesterTest::testLinearFwd()
{
//     QSKIP("Under development");
//     list<float> angles;
//     int N=360;

//     float dt=360.0f/N;
//     for (int i=0; i<N; i++)
//         angles.push_back(i*dt);

//    // BasicForwardProjector bfp;
//     LinearForwardProjector linfp;
//     kipl::base::TImage<float,2> slice;
//     kipl::base::TImage<float,2> proj;

//     kipl::io::ReadTIFF(slice,"../../data/simple_sample.tif");
//     linfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultLin_simplesample.tif",kipl::base::Float32);

//     kipl::io::ReadTIFF(slice,"../../data/disk256.tif");
//     linfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultLin_disk256.tif",kipl::base::Float32);

//     kipl::io::ReadTIFF(slice,"../../data/twodisks256.tif");
//     linfp.project(slice,angles,proj);
//     QVERIFY(angles.size()==proj.Size(1));
//     kipl::io::WriteTIFF(proj,"resultLin_twodisks256.tif",kipl::base::Float32);

  //  QVERIFY2(true, "Failure");
}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(AlgorithmTesterTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(AlgorithmTesterTest)
#endif


#include "tst_algorithmtester.moc"
