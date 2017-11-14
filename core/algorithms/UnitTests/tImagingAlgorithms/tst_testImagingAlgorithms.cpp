//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <MorphSpotClean.h>
#include <averageimage.h>
#include <piercingpointestimator.h>
#include <pixelinfo.h>


class TestImagingAlgorithms : public QObject
{
    Q_OBJECT
    
public:
    TestImagingAlgorithms();
    
private Q_SLOTS:
    void PixelInfo();
    void MorphSpotClean_CleanHoles();
    void MorphSpotClean_CleanPeaks();
    void MorphSpotClean_CleanBoth();
    void MorphSpotClean_ListAlgorithm();
    void AverageImage_Enums();
    void AverageImage_Processing();
    void AverageImage_ProcessingWeights();
    void PiercingPoint_Processing();

private:
    kipl::base::TImage<float,2> holes;
    std::map<int,float> points;
    size_t pos1;
    size_t pos2;

};

TestImagingAlgorithms::TestImagingAlgorithms()
{
    size_t dims[2]={7,7};
    holes.Resize(dims);

    for (size_t i=0; i<holes.Size(); i++) {
        holes[i]=i/10.0f;
    }

    pos1=24;
    pos2=10;

    points[pos1]=0.0f;
    points[pos2]=100.0f;

    holes[pos1]=0.0f;
    holes[pos2]=100.0f;
}

void TestImagingAlgorithms::PixelInfo()
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

void TestImagingAlgorithms::MorphSpotClean_CleanHoles()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectHoles,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.Process(img,1.0f,0.05);

    QCOMPARE(img[pos1],1.6f);
    QCOMPARE(img[pos2],100.0f);
}

void TestImagingAlgorithms::MorphSpotClean_CleanPeaks()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectPeaks, ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.Process(img,1.0f,0.05f);

    QCOMPARE(img[pos1],0.0f);
    QCOMPARE(img[pos2],1.8f);
}

void TestImagingAlgorithms::MorphSpotClean_CleanBoth()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.Process(img,1.0f,0.05f);

    QCOMPARE(img[pos1],1.6f);
    QCOMPARE(img[pos2],1.8f);
}

void TestImagingAlgorithms::MorphSpotClean_ListAlgorithm()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img,res;

    kipl::io::ReadTIFF(img,"../qni/trunk/data/spots.tif");

    try {
        res.Clone(img);
    } catch (kipl::base::KiplException &e) {
        std::cout<<"Clone failed with "<<e.what()<<std::endl;
    }

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectHoles,ImagingAlgorithms::MorphCleanFill);
    cleaner.setConnectivity(kipl::morphology::conn4);

    cleaner.Process(res,0.04,0.01);


}


void TestImagingAlgorithms::AverageImage_Enums()
{

//    std::string key;
//    key = enum2string(ImagingAlgorithms::AverageImage::ImageAverage);
//    QCOMPARE(key,"ImageAverage");
//    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageSum),"ImageSum");
//    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMedian),"ImageMedian");
//    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageWeightedAverage),"ImageWeightedAverage");
//    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMin),"ImageMin");
//    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMax),"ImageMax");

    ImagingAlgorithms::AverageImage::eAverageMethod e;
    string2enum("ImageAverage",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageAverage);
    string2enum("ImageSum",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageSum);
    string2enum("ImageMedian",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageMedian);
    string2enum("ImageWeightedAverage",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageWeightedAverage);
    string2enum("ImageMin",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageMin);
    string2enum("ImageMax",e);
    QCOMPARE(e,ImagingAlgorithms::AverageImage::ImageMax);
}

void TestImagingAlgorithms::AverageImage_Processing()
{
    ImagingAlgorithms::AverageImage avg;

    size_t dims[3]={11,11,5};

    kipl::base::TImage<float,3> stack(dims);
    kipl::base::TImage<float,2> res(dims);

    for (size_t i=0; i<dims[2]; i++) {
        float *pStack=stack.GetLinePtr(0,i);
        for (size_t j=0; j<res.Size(); j++)
        {
            pStack[j]=i+1;
        }
    }

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageSum);
    float r0=res[0];
    QCOMPARE(r0,15.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageAverage);
    r0=res[0];
    QCOMPARE(r0,3.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageWeightedAverage);
    r0=res[0];
    QCOMPARE(r0,3.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMedian);
    r0=res[0];
    QCOMPARE(r0,3.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMin);
    r0=res[0];
    QCOMPARE(r0,1.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMax);
    r0=res[0];
    QCOMPARE(r0,5.0f);
}

void TestImagingAlgorithms::AverageImage_ProcessingWeights()
{
    ImagingAlgorithms::AverageImage avg;

    size_t dims[3]={11,11,5};

    kipl::base::TImage<float,3> stack(dims);
    kipl::base::TImage<float,2> res(dims);
    float *w=new float[dims[2]];
    for (size_t i=0; i<dims[2]; i++) {
        float *pStack=stack.GetLinePtr(0,i);
        for (size_t j=0; j<res.Size(); j++)
        {
            pStack[j]=i+1;
        }
        w[i]=static_cast<float>(i+1);
    }

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageSum,w);
    float r0=res[0];
    QCOMPARE(r0,55.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageAverage,w);
    r0=res[0];
    QCOMPARE(r0,11.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageWeightedAverage,w);
    r0=res[0];
    QCOMPARE(r0,11.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMedian,w);
    r0=res[0];
    QCOMPARE(r0,9.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMin,w);
    r0=res[0];
    QCOMPARE(r0,1.0f);

    res=avg(stack,ImagingAlgorithms::AverageImage::ImageMax,w);
    r0=res[0];
    QCOMPARE(r0,25.0f);

    delete [] w;
}

void TestImagingAlgorithms::PiercingPoint_Processing()
{
    size_t dims[2]={386,256};
    kipl::base::TImage<float,2> img(dims);

    float a =  80800.0f; //220*220+180*180;
    float b = -220.0f;
    float c = -180.0f;
    float d =  0.0f;
    float e =  0.5f;
    float f =  0.75f;

    int i=0;
    for (float y=0.0f; y<256.0f; ++y) {
        for (float x=0.0f; x<386.0f; ++x, ++i) {
            img[i]=-(a+b*x+c*y+d*x*y+e*x*x+f*y*y);
        }
    }

//    kipl::io::WriteTIFF32(img,"/Users/kaestner/repos/lib/sq.tif");

    ImagingAlgorithms::PiercingPointEstimator pe;

    pair<float,float> pos=pe(img);

    std::ostringstream msg;
    msg<<"pos=("<<pos.first<<", "<<pos.second<<")";
    QVERIFY2(pos.first==220.0f,msg.str().c_str());
    QVERIFY2(pos.second==120.0f,msg.str().c_str());

}

QTEST_APPLESS_MAIN(TestImagingAlgorithms)

#include "tst_testImagingAlgorithms.moc"
