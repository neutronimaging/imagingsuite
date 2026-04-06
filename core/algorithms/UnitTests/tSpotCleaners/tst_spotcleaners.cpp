//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QElapsedTimer>


#include <base/timage.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>


#include <MorphSpotClean.h>
#include <sortspotclean.h>
#include <gammaclean.h>
#include <ImagingException.h>

class TestSpotCleaners : public QObject
{
    Q_OBJECT
    
public:
    TestSpotCleaners();
    
private Q_SLOTS:
    void PixelInfo();
    void MorphSpotClean_Initialization();
    void MorphSpotClean_CleanHoles();
    void MorphSpotClean_CleanPeaks();
    void MorphSpotClean_CleanBoth();
    void MorphSpotClean_EdgePreparation();
    void MorphSpotClean_enums();

    void SortSpotClean_BasicRun();
    void SortSpotClean_RunPatches();
    void SortSpotClean_RunThreaded();
    void SortSpotClean_Run3D();
    void SortSpotClean_enums();

    void GammaClean_BasicRun();
    void GammaClean_BenchMark();

private:
    void MorphSpotClean_ListAlgorithm();
private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    kipl::base::TImage<float,2> spots;
    kipl::base::TImage<float,3> volume;

    std::map<size_t,float> points;
    // size_t pos1;
    // size_t pos2;


};

TestSpotCleaners::TestSpotCleaners() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    #ifdef __APPLE__
        dataPath = dataPath + "/../../../../../../TestData/";
    #elif defined(__linux__)
        dataPath = dataPath + "/../../../../../../TestData/";
    #else
        dataPath = dataPath + "/../../../../../TestData/";
    #endif
    
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);

    std::string fname = dataPath+"2D/tiff/spots/balls.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(holes,fname);

    size_t patchSize=200;

    kipl::base::ImagePatchExtractor<float,2> extractor(holes.dims(), {patchSize,patchSize}, 0 ,false);

    auto patches = extractor.getAllSubImages();

    volume.resize({patchSize,patchSize,patches.size()});

    size_t idx=0;
    for (const auto & patch : patches)
    {
        auto subImg = patch.extract(holes);
        kipl::base::InsertSlice(subImg, volume, idx++, kipl::base::eImagePlanes::ImagePlaneXY);
    }

}

void TestSpotCleaners::PixelInfo()
{
    ImagingAlgorithms::PixelInfo pi;

    QCOMPARE(pi.pos,0);
    QCOMPARE(pi.value,0.0f);
    QCOMPARE(pi.weight,1.0f);
    pi.pos=1; pi.value=2.0f; pi.weight=4.0f;

    ImagingAlgorithms::PixelInfo pi2(pi);
    QCOMPARE(pi2.pos,1);
    QCOMPARE(pi2.value,2.0f);
    QCOMPARE(pi2.weight,4.0f);

    ImagingAlgorithms::PixelInfo pi3;
    pi.pos=10; pi.value=20.0f; pi.weight=40.0f;
    pi3=pi;
    QCOMPARE(pi3.pos,10);
    QCOMPARE(pi3.value,20.0f);
    QCOMPARE(pi3.weight,40.0f);
}

void TestSpotCleaners::MorphSpotClean_Initialization()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    QCOMPARE(cleaner.connectivity(),kipl::base::conn8);
    cleaner.setConnectivity(kipl::base::conn4);
    QCOMPARE(cleaner.connectivity(),kipl::base::conn4);
    
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, cleaner.setConnectivity(kipl::base::conn6));
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, cleaner.setConnectivity(kipl::base::conn18));
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, cleaner.setConnectivity(kipl::base::conn26));

    QCOMPARE(cleaner.detectionMethod(),ImagingAlgorithms::eMorphDetectionMethod::Holes);
    QCOMPARE(cleaner.cleanMethod(),ImagingAlgorithms::eMorphCleanMethod::Replace);

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::Both, ImagingAlgorithms::eMorphCleanMethod::Fill);
    QCOMPARE(cleaner.detectionMethod(),ImagingAlgorithms::eMorphDetectionMethod::Both);
    QCOMPARE(cleaner.cleanMethod(),ImagingAlgorithms::eMorphCleanMethod::Fill);

    QCOMPARE(cleaner.clampLimits(),std::vector<float>({-0.1f,7.0f}));
    QCOMPARE(cleaner.clampActive(),false);
    QCOMPARE(cleaner.maxArea(),100);
    cleaner.setLimits(true, -1, 20, 21);
    QCOMPARE(cleaner.clampLimits(),std::vector<float>({-1.0f,20.0f}));
    QCOMPARE(cleaner.clampActive(),true);
    QCOMPARE(cleaner.maxArea(),21);

    QCOMPARE(cleaner.edgeConditionLength(),9);
    cleaner.setEdgeConditioning(15);
    QCOMPARE(cleaner.edgeConditionLength(),15);

    QCOMPARE(cleaner.cleanInfNan(),false);
    cleaner.setCleanInfNan(true);
    QCOMPARE(cleaner.cleanInfNan(),true);
}

void TestSpotCleaners::MorphSpotClean_CleanHoles()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::DarkSpots,ImagingAlgorithms::eMorphCleanMethod::Replace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.setThresholdByFraction(true);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleandark.tif",kipl::base::Float32);
}

void TestSpotCleaners::MorphSpotClean_CleanPeaks()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::BrightSpots, ImagingAlgorithms::eMorphCleanMethod::Replace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanbright.tif",kipl::base::Float32);
}

void TestSpotCleaners::MorphSpotClean_CleanBoth()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::Both,ImagingAlgorithms::eMorphCleanMethod::Replace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanall.tif",kipl::base::Float32);
}

void TestSpotCleaners::MorphSpotClean_EdgePreparation()
{
    kipl::base::TImage<float,2> img;

    img.Clone(holes);
    ImagingAlgorithms::MorphSpotClean cleaner;

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::Both,ImagingAlgorithms::eMorphCleanMethod::Replace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,1.0f,0.05f);

    kipl::io::WriteTIFF(img,"spotcleaned.tif",kipl::base::Float32);

}

void TestSpotCleaners::MorphSpotClean_enums()
{
    std::map<std::string, ImagingAlgorithms::eMorphDetectionMethod> enummap;

    enummap["morphdetectbrightspots"] = ImagingAlgorithms::eMorphDetectionMethod::BrightSpots ;
    enummap["morphdetectdarkspots"]   = ImagingAlgorithms::eMorphDetectionMethod::DarkSpots ;
    enummap["morphdetectallspots"]    = ImagingAlgorithms::eMorphDetectionMethod::AllSpots ;
    enummap["morphdetectholes"]       = ImagingAlgorithms::eMorphDetectionMethod::Holes ;
    enummap["morphdetectpeaks"]       = ImagingAlgorithms::eMorphDetectionMethod::Peaks ;
    enummap["morphdetectboth"]        = ImagingAlgorithms::eMorphDetectionMethod::Both ;

    ImagingAlgorithms::eMorphDetectionMethod mdm;

    for (auto & item : enummap)
    {
        QCOMPARE(enum2string(item.second),item.first);

        string2enum(item.first,mdm);
        QCOMPARE(mdm,item.second);
    }

    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, string2enum("qwerty",mdm));
}

void TestSpotCleaners::MorphSpotClean_ListAlgorithm()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img,res;

    std::string fname = dataPath+"2D/tiff/spots/balls.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);
    
    try {
        res.Clone(img);
    } catch (kipl::base::KiplException &e) {
        std::cout<<"Clone failed with "<<e.what()<<std::endl;
    }

    cleaner.setCleanMethod(ImagingAlgorithms::eMorphDetectionMethod::Holes,ImagingAlgorithms::eMorphCleanMethod::Fill);
    cleaner.setConnectivity(kipl::base::conn4);

    cleaner.process(res,0.04,0.01);
}

void TestSpotCleaners::SortSpotClean_BasicRun()
{
    ImagingAlgorithms::SortSpotClean cleaner;

    auto img=kipl::base::TSubImage<float,2>::Get(holes,{200,200},{32,32});
    kipl::io::WriteTIFF(img,"sortspot_patch.tif",kipl::base::Float32);

    cleaner.process(img,0.95f,1.0f);
    auto mask=cleaner.getSpotMask();
    kipl::io::WriteTIFF(mask,"sortspotmask.tif",kipl::base::Float32);

    auto diff=cleaner.getDifferenceImage();
    kipl::io::WriteTIFF(diff,"sortspotdiff.tif",kipl::base::Float32);
}


void TestSpotCleaners::SortSpotClean_RunPatches()
{
    ImagingAlgorithms::SortSpotClean cleaner(true,24UL,false);
    QCOMPARE(cleaner.isThreaded(),false);
    QCOMPARE(cleaner.numberOfThreads(),1);
    auto img=holes;
    // auto img=spots;
    img.Clone();
    kipl::io::WriteTIFF(img,"sortspot_orig.tif",kipl::base::Float32);

    // timer.start();
    QBENCHMARK{
        cleaner.process(img,0.95f,1.0f);
    }
    // qint64 elapsed = timer.elapsed();
    // std::cout << "SortSpotClean_RunPatches processing time: " << elapsed << " ms" << std::endl;

    // auto mask=cleaner.getSpotMask();
    // kipl::io::WriteTIFF(mask,"sortspotmask.tif",kipl::base::Float32);

    // auto diff=cleaner.getDifferenceImage();
    kipl::io::WriteTIFF(img,"sortspotdiff_full_cleaned.tif",kipl::base::Float32);
}


void TestSpotCleaners::SortSpotClean_RunThreaded()
{
    ImagingAlgorithms::SortSpotClean cleaner_s(true,24UL,false);
    ImagingAlgorithms::SortSpotClean cleaner_p(true,24UL,true);

    auto img=holes;
    // auto img=spots;
    img.Clone();
    kipl::io::WriteTIFF(img,"sortspot_orig.tif",kipl::base::Float32);

    QCOMPARE(cleaner_p.isThreaded(),true);
    QCOMPARE(cleaner_p.numberOfThreads(),std::thread::hardware_concurrency());
    // QElapsedTimer timer;
    // timer.start();
    // QBENCHMARK{
        cleaner_p.process(img,0.95f,1.0f);
    // }
    // qint64 elapsed = timer.elapsed();
    // std::cout << "SortSpotClean_RunThreaded processing time: " << elapsed << " ms" << std::endl;
    kipl::io::WriteTIFF(img,"sortspotdiff_full_th_all_cleaned.tif",kipl::base::Float32);

    auto img_s = holes;
    // auto img_s = spots;
    img_s.Clone();

    cleaner_s.process(img_s,0.95f,1.0f);
    kipl::io::WriteTIFF(img_s,"sortspotdiff_full_th_single_cleaned.tif",kipl::base::Float32);

    size_t diffs = 0UL;

    for (size_t i=0; i<img.Size(); ++i)
    {
        diffs += (img[i] != img_s[i]) ? 1 : 0;
    }
    
    QCOMPARE(diffs,0UL);
}

void TestSpotCleaners::SortSpotClean_Run3D()
{
    ImagingAlgorithms::SortSpotClean cleaner_s(true,32UL,false);

    auto volA = volume;
    auto volB = volume;
    auto volC = volume;
    volA.Clone();
    volB.Clone();
    volC.Clone();

    for (size_t i=0; i<volA.Size(2); ++i)
    {
        auto img = kipl::base::ExtractSlice(volA, i, kipl::base::eImagePlanes::ImagePlaneXY);
        cleaner_s.process(img,0.95f,1.0f);
        kipl::base::InsertSlice(img, volA, i, kipl::base::eImagePlanes::ImagePlaneXY);
    }

    cleaner_s.process(volB,0.95f,1.0f);

    size_t sum=0UL;
    size_t diffsum = 0UL;
    for (size_t i=0; i<volA.Size(); ++i)
    {
        sum += (volA[i] != volB[i]) ? 1 : 0;
        diffsum += (volume[i] != volB[i]) ? 1 : 0;
    }
    QCOMPARE(sum,0UL);
    qDebug()<<"Total differences between slice-wise and 3D processing (single thread): "<<diffsum<<" out of "<<volA.Size()<<" voxels.";

    ImagingAlgorithms::SortSpotClean cleaner_p(true,32UL,true);

    cleaner_p.process(volC,0.95f,1.0f);
    sum=0UL;
    diffsum = 0UL;

    for (size_t i=0; i<volA.Size(); ++i)
    {
        sum += (volA[i] != volC[i]) ? 1 : 0;
        diffsum += (volume[i] != volC[i]) ? 1 : 0;
    }
    QCOMPARE(sum,0UL);
    qDebug()<<"Total differences between slice-wise and 3D processing (multi threaded): "<<diffsum<<" out of "<<volA.Size()<<" voxels.";

}

void TestSpotCleaners::SortSpotClean_enums()
{
    std::map<std::string, ImagingAlgorithms::eSortSpotQuantile> enummap;

    enummap["All"]         = ImagingAlgorithms::eSortSpotQuantile::All ;
    enummap["BrightSpots"] = ImagingAlgorithms::eSortSpotQuantile::BrightSpots ;
    enummap["DarkSpots"]   = ImagingAlgorithms::eSortSpotQuantile::DarkSpots ;
    enummap["Both"]        = ImagingAlgorithms::eSortSpotQuantile::Both ;

    ImagingAlgorithms::eSortSpotQuantile esq;

    for (auto & item : enummap)
    {
        QCOMPARE(enum2string(item.second),item.first);

        string2enum(item.first,esq);
        QCOMPARE(esq,item.second);

        std::stringstream ss;
        ss << item.second;
        QCOMPARE(ss.str(),item.first);
    }

    QVERIFY_THROWS_EXCEPTION(ImagingException, string2enum("qwerty",esq));
}   

void TestSpotCleaners::GammaClean_BasicRun()
{
    // Basic test to check if the algorithm runs without errors and produces output. No assertions on the output are made here.

    ImagingAlgorithms::GammaClean cleaner;

    auto img=kipl::base::TSubImage<float,2>::Get(holes,{200,200},{32,32});
    kipl::io::WriteTIFF(img,"gammaclean_patch.tif",kipl::base::Float32);

    cleaner.process(img);
    kipl::io::WriteTIFF(img,"gammaclean_patch_cleaned.tif",kipl::base::Float32);
    auto mask=cleaner.spotMask();
    kipl::io::WriteTIFF(mask,"gammaclean_spotmask.tif",kipl::base::UInt16);
    auto diff=cleaner.detectionImage();
    kipl::io::WriteTIFF(diff,"gammaclean_diff.tif",kipl::base::Float32);
}

void TestSpotCleaners::GammaClean_BenchMark()
{
    ImagingAlgorithms::GammaClean cleaner;

    QBENCHMARK
    {
        auto img=holes;
        img.Clone();
        cleaner.process(img);
    }
}
QTEST_APPLESS_MAIN(TestSpotCleaners)


#include "tst_spotcleaners.moc"