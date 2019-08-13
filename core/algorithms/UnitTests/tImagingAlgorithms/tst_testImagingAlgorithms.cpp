//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <MorphSpotClean.h>
#include <averageimage.h>
#include <piercingpointestimator.h>
#include <pixelinfo.h>
#include <PolynomialCorrection.h>
#include <projectionfilter.h>
#include <ImagingException.h>

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
    void MorphSpotClean_EdgePreparation();

    void AverageImage_Enums();
    void AverageImage_Processing();
    void AverageImage_ProcessingWeights();
    void PiercingPoint_Processing();
    void piercingPointExperiment();

    void PolynomialCorrection_init();
    void PolynomialCorrection_numeric();

    void ProjectionFilterParameters();
    void ProjectionFilterProcessing();

private:
    void MorphSpotClean_ListAlgorithm();
private:
    kipl::base::TImage<float,2> holes;
    std::map<size_t,float> points;
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
    cleaner.process(img,1.0f,0.05f);

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
    cleaner.process(img,1.0f,0.05f);

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
    cleaner.process(img,1.0f,0.05f);

    QCOMPARE(img[pos1],1.6f);
    QCOMPARE(img[pos2],1.8f);
}

void TestImagingAlgorithms::MorphSpotClean_EdgePreparation()
{
    kipl::base::TImage<float,2> img;
#ifndef DEBUG
    kipl::io::ReadTIFF(img,"../imagingsuite/core/algorithms/UnitTests/data/spotprojection_0001.tif");
#else
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/algorithms/UnitTests/data/spotprojection_0001.tif");
#endif

    ImagingAlgorithms::MorphSpotClean cleaner;

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::morphology::conn8);
    cleaner.process(img,1.0f,0.05f);

    kipl::io::WriteTIFF32(img,"spotcleaned.tif");

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

    cleaner.process(res,0.04,0.01);


}


void TestImagingAlgorithms::AverageImage_Enums()
{

    std::string key;
    key = enum2string(ImagingAlgorithms::AverageImage::ImageAverage);
    QCOMPARE(key,std::string("ImageAverage"));
    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageSum),             std::string("ImageSum"));
    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMedian),          std::string("ImageMedian"));
    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageWeightedAverage), std::string("ImageWeightedAverage"));
    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMin),             std::string("ImageMin"));
    QCOMPARE(enum2string(ImagingAlgorithms::AverageImage::ImageMax),             std::string("ImageMax"));

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
    std::vector<float> w(dims[2]);
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

void TestImagingAlgorithms::piercingPointExperiment()
{
    QSKIP("Skipping time consuming test",__FILE__,__LINE__);
    kipl::base::TImage<float,2> ob;
    kipl::base::TImage<float,2> dc;
#ifndef DEBUG
    kipl::io::ReadTIFF(ob,"../imagingsuite/frameworks/tomography/data/cbct/ob_3s_5fps_60kV_150uA_00001.tif");
    kipl::io::ReadTIFF(dc,"../imagingsuite/frameworks/tomography/data/cbct/dc_3s_5fps_00001.tif");
#else
    kipl::io::ReadTIFF(ob,"../../imagingsuite/frameworks/tomography/data/cbct/ob_3s_5fps_60kV_150uA_00001.tif");
    kipl::io::ReadTIFF(dc,"../../imagingsuite/frameworks/tomography/data/cbct/dc_3s_5fps_00001.tif");
#endif
    ImagingAlgorithms::PiercingPointEstimator pe;

    pair<float,float> pos0=pe(ob);
    pair<float,float> pos1=pe(ob,dc,false);

    QVERIFY(fabs(pos0.first-pos1.first)<3.0f);
    QVERIFY(fabs(pos0.second-pos1.second)<3.0f);

    // Gain correction
    //pair<float,float> pos2=pe(ob,dc,true);

}

void TestImagingAlgorithms::PolynomialCorrection_init()
{
    ImagingAlgorithms::PolynomialCorrection pc;

    QCOMPARE(pc.polynomialOrder(),3);

    auto vec = pc.coefficients();

    QCOMPARE(vec.size(),4UL);
    QCOMPARE(vec[0],0.0f);
    QCOMPARE(vec[1],0.879f);
    QCOMPARE(vec[2],0.0966f);
    QCOMPARE(vec[3],0.0998f);

}

void TestImagingAlgorithms::PolynomialCorrection_numeric()
{
    float c[]={1.0f,0.5f,0.2f,0.1f,0.05f,0.02f,0.01f,0.005f,0.002f,0.001f,0.0005f};

    ImagingAlgorithms::PolynomialCorrection pc;
    const int N=5;
    float val[N]={0.0f,1.0f,2.0f,3.0f,4.0f};
    std::vector<float> result(N);

    for (size_t i=1; i<10; ++i)
    {
        pc.setup(c,static_cast<int>(i));
        std::vector<float> vec=pc.coefficients();

        QCOMPARE(vec.size(),static_cast<size_t>(i+1));
        for (size_t j=0; j<(i+1); ++j)
            QCOMPARE(vec[j],c[j]);

        std::copy_n(val,N,result.begin());
        pc.process(result);

        for (size_t k=0; k<N; ++k)
        {
            float sum=c[0];

            for (size_t j=1; j<(i+1); ++j)
                sum+= c[j]*std::powf(val[k],j);

            QCOMPARE(result[k],sum);
        }
    }
}

void TestImagingAlgorithms::ProjectionFilterParameters()
{
    ImagingAlgorithms::ProjectionFilter pf(nullptr);

    // Check defualt values
    QCOMPARE(pf.filterType(),       ImagingAlgorithms::ProjectionFilterHamming);
    QCOMPARE(pf.order(),            1.0f);
    QCOMPARE(pf.cutOff(),           0.5f);
    QCOMPARE(pf.useBias(),          true);
    QCOMPARE(pf.biasWeight(),       0.1f);
    QCOMPARE(pf.currentFFTSize(),   0UL);
    QCOMPARE(pf.currentImageSize(), 0UL);
    QCOMPARE(pf.paddingDoubler(),   2UL);

    auto params = pf.parameters();
    QCOMPARE(params["filtertype"],     std::string("Hamming"));
    QCOMPARE(params["order"],          std::string("1"));
    QCOMPARE(params["cutoff"],         std::string("0.5"));
    QCOMPARE(params["usebias"],        std::string("true"));
    QCOMPARE(params["biasweight"],     std::string("0.1"));
    QCOMPARE(params["paddingdoubler"], std::string("2"));

    params["filtertype"]     = "Parzen";
    params["order"]          = "2";
    params["cutoff"]         = "0.4";
    params["usebias"]        = "false";
    params["biasweight"]     = "0.3";
    params["paddingdoubler"] = "1";

    pf.setParameters(params);

    QCOMPARE(pf.filterType(),       ImagingAlgorithms::ProjectionFilterParzen);
    QCOMPARE(pf.order(),            2.0f);
    QCOMPARE(pf.cutOff(),           0.4f);
    QCOMPARE(pf.useBias(),          false);
    QCOMPARE(pf.biasWeight(),       0.3f);
    QCOMPARE(pf.paddingDoubler(),   1UL);

    pf.setFilter(ImagingAlgorithms::ProjectionFilterSheppLogan,0.4f);
    QCOMPARE(pf.filterType(),       ImagingAlgorithms::ProjectionFilterSheppLogan);
    QCOMPARE(pf.order(),            0.0f);
    QCOMPARE(pf.cutOff(),           0.4f);

    pf.setFilter(ImagingAlgorithms::ProjectionFilterButterworth,0.5f,3.0f);
    QCOMPARE(pf.filterType(),       ImagingAlgorithms::ProjectionFilterButterworth);
    QCOMPARE(pf.order(),            3.0f);
    QCOMPARE(pf.cutOff(),           0.5f);

    QVERIFY_EXCEPTION_THROWN(pf.setFilter(ImagingAlgorithms::ProjectionFilterButterworth,1.0f,3.0f),
                             ImagingException);

    QVERIFY_EXCEPTION_THROWN(pf.setFilter(ImagingAlgorithms::ProjectionFilterButterworth,-1.0f,3.0f),
                             ImagingException);
}

void TestImagingAlgorithms::ProjectionFilterProcessing()
{
    kipl::base::TImage<float,2> sino;
#ifdef DEBUG
    kipl::io::ReadTIFF(sino,"../../imagingsuite/core/algorithms/UnitTests/data/woodsino_0200.tif");
#else
    kipl::io::ReadTIFF(sino,"../imagingsuite/core/algorithms/UnitTests/data/woodsino_0200.tif");
#endif

    ImagingAlgorithms::ProjectionFilter pf(nullptr);

    pf.process(sino);

    kipl::io::WriteTIFF32(sino,"projfilt_result.tif");
    QCOMPARE(pf.currentFFTSize(),2048);
    QCOMPARE(pf.currentImageSize(),sino.Size(0));
}

QTEST_APPLESS_MAIN(TestImagingAlgorithms)

#include "tst_testImagingAlgorithms.moc"
