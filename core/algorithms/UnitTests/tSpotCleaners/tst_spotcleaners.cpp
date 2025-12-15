//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>


#include <MorphSpotClean.h>
#include <sortspotclean.h>

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
private:
    void MorphSpotClean_ListAlgorithm();
private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    kipl::base::TImage<float,2> spots;

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

    fname = dataPath+"2D/tiff/manyspots.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(spots,fname);
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

    QCOMPARE(cleaner.detectionMethod(),ImagingAlgorithms::MorphDetectHoles);
    QCOMPARE(cleaner.cleanMethod(),ImagingAlgorithms::MorphCleanReplace);

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth, ImagingAlgorithms::MorphCleanFill);
    QCOMPARE(cleaner.detectionMethod(),ImagingAlgorithms::MorphDetectBoth);
    QCOMPARE(cleaner.cleanMethod(),ImagingAlgorithms::MorphCleanFill);

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

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectDarkSpots,ImagingAlgorithms::MorphCleanReplace);
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

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBrightSpots, ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanbright.tif",kipl::base::Float32);
}

void TestSpotCleaners::MorphSpotClean_CleanBoth()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanall.tif",kipl::base::Float32);
}

void TestSpotCleaners::MorphSpotClean_EdgePreparation()
{
    kipl::base::TImage<float,2> img;

    img.Clone(holes);
    ImagingAlgorithms::MorphSpotClean cleaner;

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,1.0f,0.05f);

    kipl::io::WriteTIFF(img,"spotcleaned.tif",kipl::base::Float32);

}

void TestSpotCleaners::MorphSpotClean_enums()
{
    std::map<std::string, ImagingAlgorithms::eMorphDetectionMethod> enummap;

    enummap["morphdetectbrightspots"] = ImagingAlgorithms::MorphDetectBrightSpots ;
    enummap["morphdetectdarkspots"]   = ImagingAlgorithms::MorphDetectDarkSpots ;
    enummap["morphdetectallspots"]    = ImagingAlgorithms::MorphDetectAllSpots ;
    enummap["morphdetectholes"]       = ImagingAlgorithms::MorphDetectHoles ;
    enummap["morphdetectpeaks"]       = ImagingAlgorithms::MorphDetectPeaks ;
    enummap["morphdetectboth"]        = ImagingAlgorithms::MorphDetectBoth ;

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

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectHoles,ImagingAlgorithms::MorphCleanFill);
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
    ImagingAlgorithms::SortSpotClean cleaner(true,16UL,false);

    auto img=holes;
    img.Clone();
    kipl::io::WriteTIFF(img,"sortspot_orig.tif",kipl::base::Float32);

    cleaner.process(img,0.99f,1.0f);
    // auto mask=cleaner.getSpotMask();
    // kipl::io::WriteTIFF(mask,"sortspotmask.tif",kipl::base::Float32);

    // auto diff=cleaner.getDifferenceImage();
    kipl::io::WriteTIFF(img,"sortspotdiff_full_cleaned.tif",kipl::base::Float32);
}
QTEST_APPLESS_MAIN(TestSpotCleaners)


#include "tst_spotcleaners.moc"