//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>
#include <filesystem>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_csv.h>
#include <imagewriter.h>
#include <imagereader.h>
#include <normalizeimage.h>
#include <ImagingException.h>

#include <StripeFilter.h>
#include <StripeFilterManager.h>
#include <sinogram.h>

class TestStripeFilter : public QObject
{
    Q_OBJECT
    
public:
    TestStripeFilter();
    void createTestResultFolder();
    
private Q_SLOTS:
    void testExtractSinogram();
    void testInsertSinogram();
    void testStripeFilterParameters();
    void testStripeFilterProcessing2D();
    void testManagerInitialization();
    // void testManagerInitialization();
    // void testManagerParameters();
    // void testManagerSingleThreadProcessing();
    void testManagerMultiThreadProcessing();
    void benchmarkManagerProcessingSingleThreaded();
    void benchmarkManagerProcessingMultiThreaded();
    // void testManagerBoundaryConditions();
private:
    void MorphSpotClean_ListAlgorithm();
private:
    std::string dataPath;
    std::string resultPath;
    kipl::base::TImage<float,3> proj;
    kipl::base::TImage<float,2> ob;
    kipl::base::TImage<float,2> dc;
};

TestStripeFilter::TestStripeFilter() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    resultPath = QT_TESTCASE_BUILDDIR;
    #ifdef __APPLE__
        dataPath   = dataPath + "/../../../../../../TestData/";
        resultPath = resultPath + "/../../../../results/stripetest/";
    #elif defined(__linux__)
        dataPath = dataPath + "/../../../../../../TestData/";
        resultPath = resultPath + "/../../../../results/stripetest/";
    #else
        dataPath = dataPath + "/../../../../../TestData/";
        resultPath = resultPath + "/../../../results/stripetest/";
    #endif
    
    createTestResultFolder();
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
    kipl::strings::filenames::CheckPathSlashes(resultPath,true);

    // std::string fname = dataPath+"2D/tiff/spots/balls.tif";
    // kipl::strings::filenames::CheckPathSlashes(fname,false);
    // kipl::io::ReadTIFF(holes,fname);
    ImageReader reader;

    proj = reader.Read(dataPath+"2D/tiff/tomo/04_ct5s375_128lines/ct5s_#####.tif",
                        1,376,1,
                        kipl::base::eImageFlip::ImageFlipNone,
                        kipl::base::eImageRotate::ImageRotateNone,
                        1.0f,
                        {});

    qDebug() << "proj size: " << proj.Size(0) << "x" << proj.Size(1) << "x" << proj.Size(2);

    ob  = reader.Read(dataPath+"2D/tiff/tomo/04_ct5s375_128lines/ob_00001.tif");
    dc  = reader.Read(dataPath+"2D/tiff/tomo/04_ct5s375_128lines/dc_00001.tif");

    ImagingAlgorithms::NormalizeImage norm(true);

    norm.setReferences(ob,dc);
    norm.process(proj);
}

void TestStripeFilter::createTestResultFolder() 
{
    
    std::filesystem::path path = resultPath;

    try {
        if (std::filesystem::create_directories(path)) {
            std::cout << "Directory created successfully: " << path << std::endl;
        } else {
            std::cout << "Directory already exists or failed to create: " << path << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }


}

void TestStripeFilter::testExtractSinogram()
{
    kipl::base::TImage<float,3> img({10,11,12});

    std::iota(img.GetDataPtr(),img.GetDataPtr()+img.Size(),0);

    kipl::base::TImage<float,2> sino;

    for (size_t y=0; y<img.Size(1); ++y)
    {
        ImagingAlgorithms::ExtractSinogram(img,sino,y);

        QCOMPARE(sino.Size(),img.Size(0)*img.Size(2));
        QCOMPARE(sino.Size(0),img.Size(0));
        QCOMPARE(sino.Size(1),img.Size(2));

        for (size_t z=0; z<img.Size(2); ++z)
        {
            for (size_t x=0; x<img.Size(0); ++x)
            {
                QCOMPARE(sino(x,z),img(x,y,z));
            }
        }
    }
}

void TestStripeFilter::testInsertSinogram()
{
    kipl::base::TImage<float,3> img({10,11,12});

    img = 0.0f;

    kipl::base::TImage<float,2> sino({img.Size(0),img.Size(1)});
    for (size_t y=0; y<img.Size(1); ++y)
    {
        std::iota(sino.GetDataPtr(),sino.GetDataPtr()+sino.Size(),y);

        ImagingAlgorithms::InsertSinogram(sino,img,y);

        for (size_t z=0; z<img.Size(2); ++z)
        {
            for (size_t x=0; x<img.Size(0); ++x)
            {
                QCOMPARE(sino(x,z),img(x,y,z));
            }
        }
    }

}

void TestStripeFilter::testStripeFilterParameters()
{
   kipl::base::TImage<float,2> sino;
#ifdef DEBUG
   kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#else
   kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#endif
   ImagingAlgorithms::StripeFilter sf(sino.dims(),"daub17",4,0.21f);
   QCOMPARE(static_cast<size_t>(sf.dims()[0]),sino.Size(0));
   QCOMPARE(static_cast<size_t>(sf.dims()[1]),sino.Size(1));
   QCOMPARE(sf.sigma(),0.21f);
   QCOMPARE(sf.decompositionLevels(),4);
   std::vector<size_t> dims={sino.Size(0),sino.Size(1)};
   QCOMPARE(sf.checkDims(dims),true);
   dims[0]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));
   dims[1]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));
   dims[0]++;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));

   std::vector<int> dims2={static_cast<int>(sino.Size(0)), static_cast<int>(sino.Size(1))};

   ImagingAlgorithms::StripeFilter sf2(dims2,"daub17",4,0.21f);
   QCOMPARE(static_cast<size_t>(sf2.dims()[0]),sino.Size(0));
   QCOMPARE(static_cast<size_t>(sf2.dims()[1]),sino.Size(1));
   QCOMPARE(sf2.sigma(),0.21f);
   QCOMPARE(sf2.decompositionLevels(),4);
   dims[0]=sino.Size(0);
   dims[1]=sino.Size(1);
   QCOMPARE(sf2.checkDims(dims),true);
   dims[0]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));
   dims[1]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));
   dims[0]++;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));

}

void TestStripeFilter::testStripeFilterProcessing2D()
{
    kipl::base::TImage<float,2> sino;

    std::string fname = dataPath+"2D/tiff/woodsino_0200.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadTIFF(sino,fname);

    ImagingAlgorithms::StripeFilter sf(sino.dims(),"daub17",4,0.21f);

    sf.process(sino);
}

void TestStripeFilter::testManagerInitialization() 
{
    std::vector<size_t> dims = {100, 101}; // Example image dimensions
    ImagingAlgorithms::StripeFilterManager manager(dims, "daub7", 2, 0.001f);
    auto dims2 = manager.dims();
    QCOMPARE(dims2.size(), dims.size()); // Replace expected_size with the actual expected size
    QCOMPARE(dims2[0],dims[0]);
    QCOMPARE(dims2[1],dims[1]);
    QCOMPARE(manager.waveletName(), "daub7"); // Replace expected_waveletName with the actual expected wavelet name
    QCOMPARE(manager.decompositionLevels(), 2); // Replace expected_decompositionLevels with the actual expected decomposition levels
    QCOMPARE(manager.sigma(), 0.001f); // Replace expected_sigma with the actual expected sigma value
    QCOMPARE(manager.numberOfThreads(), std::thread::hardware_concurrency()); // Replace expected_numberOfThreads with the actual expected number of threads
}

// void TestStripeFilter::testSingleThreadProcessing() {
//     StripeFilterManager manager;
//     kipl::base::TImage<float, 2> img({100, 100}); // Example image dimensions
//     size_t nStart = 0;
//     size_t nEnd = 1;
//     size_t nBlockSize = 1;
//     size_t nRemainder = 0;
//     auto op = [](auto& sino) { /* Example operation */ };

//     manager.processImage(img, nStart, nEnd, nBlockSize, nRemainder, op);

//     // Verify the processing results
//     QVERIFY(/* condition to verify the result */);
// }

void TestStripeFilter::testManagerMultiThreadProcessing() 
{
    std::vector<size_t> dims = {proj.Size(0), proj.Size(2)}; // Example image dimensions
    ImagingAlgorithms::StripeFilterManager manager(dims, "daub7", 2, 0.001f);

    kipl::base::TImage<float, 3> img;
    img.Clone(proj); // Example image
    // qDebug() << "proj size: " << proj.Size(0) << "x" << proj.Size(1) << "x" << proj.Size(2);
    // qDebug() << "img size: " << img.Size(0) << "x" << img.Size(1) << "x" << img.Size(2);

    ImageWriter writer;
    writer.write(img, resultPath+"orig_####.tif", kipl::base::Float32 );
    manager.process(img, ImagingAlgorithms::VerticalComponentFFT,true);
    writer.write(img, resultPath+"flt_####.tif", kipl::base::Float32);
}

void TestStripeFilter::benchmarkManagerProcessingMultiThreaded() 
{
    std::vector<size_t> dims = {proj.Size(0), proj.Size(2)}; // Example image dimensions
    ImagingAlgorithms::StripeFilterManager manager(dims, "daub7", 2, 0.001f);

    kipl::base::TImage<float, 3> img;
    img.Clone(proj); // Example image
    
    QBENCHMARK {
        manager.process(img, ImagingAlgorithms::VerticalComponentFFT,true);
    }

}

void TestStripeFilter::benchmarkManagerProcessingSingleThreaded() 
{
    std::vector<size_t> dims = {proj.Size(0), proj.Size(2)}; // Example image dimensions
    ImagingAlgorithms::StripeFilterManager manager(dims, "daub7", 2, 0.001f);

    kipl::base::TImage<float, 3> img;
    img.Clone(proj); // Example image
    
    QBENCHMARK {
        manager.process(img, ImagingAlgorithms::VerticalComponentFFT,false);
    }

}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
QTEST_APPLESS_MAIN(TestStripeFilter)
#pragma clang diagnostic pop

#include "tst_StripeFilter.moc"
