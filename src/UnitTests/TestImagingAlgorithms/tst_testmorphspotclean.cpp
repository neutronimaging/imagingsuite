#include <QtCore/QString>
#include <QtTest/QtTest>
#include <MorphSpotClean.h>
#include <base/timage.h>

#include <sstream>
#include <iostream>
#include <map>

class TestMorphSpotClean : public QObject
{
    Q_OBJECT
    
public:
    TestMorphSpotClean();
    
private Q_SLOTS:
    void CleanHoles();
    void CleanPeaks();
    void CleanBoth();
private:
    kipl::base::TImage<float,2> holes;
    std::map<int,float> points;
    size_t pos1;
    size_t pos2;

};

TestMorphSpotClean::TestMorphSpotClean()
{
    size_t dims[2]={7,7};
    holes.Resize(dims);

    for (int i=0; i<holes.Size(); i++) {
        holes[i]=i/10.0f;
    }

    pos1=24;
    pos2=10;
    float ref=holes[pos1];
    points[pos1]=0.0f;
    points[pos2]=100.0f;

    holes[pos1]=0.0f;
    holes[pos2]=100.0f;
}


void TestMorphSpotClean::CleanHoles()
{
    std::stringstream msg;
    ImagingAlgorithms::MorphSpotClean cleaner;

    size_t dims[2]={7,7};
    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphCleanHoles);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.Process(img,1.0f);

    QCOMPARE(img[pos1],1.6f);
    QCOMPARE(img[pos2],100.0f);
}

void TestMorphSpotClean::CleanPeaks()
{
    std::stringstream msg;
    ImagingAlgorithms::MorphSpotClean cleaner;

    size_t dims[2]={7,7};
    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphCleanPeaks);
    cleaner.setConnectivity(kipl::morphology::conn8);
 //   cleaner.Process(img,1.0f);

    QCOMPARE(img[pos1],0.0f);
    QCOMPARE(img[pos2],1.8f);
}

void TestMorphSpotClean::CleanBoth()
{
    std::stringstream msg;
    ImagingAlgorithms::MorphSpotClean cleaner;

    size_t dims[2]={7,7};
    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphCleanBoth);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.Process(img,1.0f);

    QCOMPARE(img[pos1],1.6f);
    QCOMPARE(img[pos2],1.8f);
}

QTEST_APPLESS_MAIN(TestMorphSpotClean)

#include "tst_testmorphspotclean.moc"
