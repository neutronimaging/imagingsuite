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
#include <io/io_csv.h>

#include <MorphSpotClean.h>
#include <averageimage.h>
#include <piercingpointestimator.h>
#include <pixelinfo.h>
#include <PolynomialCorrection.h>
#include <projectionfilter.h>
#include <ImagingException.h>
#include <StripeFilter.h>
#include <ReferenceImageCorrection.h>
#include <ReplaceUnderexposed.h>

class TestImagingAlgorithms : public QObject
{
    Q_OBJECT
    
public:
    TestImagingAlgorithms();
    
private Q_SLOTS:
    void PixelInfo();
    void MorphSpotClean_Initialization();
    void MorphSpotClean_CleanHoles();
    void MorphSpotClean_CleanPeaks();
    void MorphSpotClean_CleanBoth();
    void MorphSpotClean_EdgePreparation();
    void MorphSpotClean_enums();

    void AverageImage_Enums();
    void AverageImage_Processing();
    void AverageImage_ProcessingWeights();
    void PiercingPoint_Processing();
    void piercingPointExperiment();

    void PolynomialCorrection_init();
    void PolynomialCorrection_numeric();

    void ProjectionFilterParameters();
    void ProjectionFilterProcessing();
    void ProjectionFilterPadding();
    void StripeFilterParameters();
    void StripeFilterProcessing2D();

    void RefImgCorrection_Initialization();

    void RefImgCorrection_enums();

    void ProjSeriesCorr_Enums();
    void ProjSeriesCorr_Detection();
    void ProjSeriesCorr_Correction();
private:
    void MorphSpotClean_ListAlgorithm();
private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    std::map<size_t,float> points;
    size_t pos1;
    size_t pos2;


};

TestImagingAlgorithms::TestImagingAlgorithms() 
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

void TestImagingAlgorithms::MorphSpotClean_Initialization()
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

void TestImagingAlgorithms::MorphSpotClean_CleanHoles()
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

void TestImagingAlgorithms::MorphSpotClean_CleanPeaks()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBrightSpots, ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanbright.tif",kipl::base::Float32);
}

void TestImagingAlgorithms::MorphSpotClean_CleanBoth()
{
    ImagingAlgorithms::MorphSpotClean cleaner;

    kipl::base::TImage<float,2> img=holes;
    img.Clone();

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,0.95f,0.05f);

    kipl::io::WriteTIFF(img,"cleanall.tif",kipl::base::Float32);
}

void TestImagingAlgorithms::MorphSpotClean_EdgePreparation()
{
    kipl::base::TImage<float,2> img;

    img.Clone(holes);
    ImagingAlgorithms::MorphSpotClean cleaner;

    cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectBoth,ImagingAlgorithms::MorphCleanReplace);
    cleaner.setConnectivity(kipl::base::conn8);
    cleaner.process(img,1.0f,0.05f);

    kipl::io::WriteTIFF(img,"spotcleaned.tif",kipl::base::Float32);

}

void TestImagingAlgorithms::MorphSpotClean_enums()
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

void TestImagingAlgorithms::MorphSpotClean_ListAlgorithm()
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

    std::vector<size_t> dims={11,11,5};

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

    std::vector<size_t> dims={11,11,5};

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
    std::vector<size_t> dims={386,256};
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

//    kipl::io::WriteTIFF(img,"/Users/kaestner/repos/lib/sq.tif",kipl::base::Float32);

    ImagingAlgorithms::PiercingPointEstimator pe;

    pair<float,float> pos=pe(img);

//    std::ostringstream msg;
//    msg<<"pos=("<<pos.first<<", "<<pos.second<<")";
//    QVERIFY2(pos.first==220.0f,msg.str().c_str());
//    QVERIFY2(pos.second==120.0f,msg.str().c_str());

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
    std::vector<float> val={0.0f,1.0f,2.0f,3.0f,4.0f};
    std::vector<float> result(N);

    for (size_t i=1; i<10; ++i)
    {

        pc.setup(c,static_cast<int>(i));
        std::vector<float> vec=pc.coefficients();

        QCOMPARE(vec.size(),static_cast<size_t>(i+1));
        for (size_t j=0; j<(i+1); ++j)
            QCOMPARE(vec[j],c[j]);

        result = pc.process(val);

        for (size_t k=0; k<N; ++k)
        {
            float sum=c[0];

            for (size_t j=1; j<(i+1); ++j)
                sum+= c[j]*std::pow(val[k],j);

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

    QVERIFY_THROWS_EXCEPTION(ImagingException, pf.setFilter(ImagingAlgorithms::ProjectionFilterButterworth,1.0f,3.0f));
    QVERIFY_THROWS_EXCEPTION(ImagingException, pf.setFilter(ImagingAlgorithms::ProjectionFilterButterworth,-1.0f,3.0f));
}

void TestImagingAlgorithms::ProjectionFilterProcessing()
{
    kipl::base::TImage<float,2> sino;
#ifdef DEBUG
    kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#else
    kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#endif

    ImagingAlgorithms::ProjectionFilter pf(nullptr);

    pf.process(sino);

    kipl::io::WriteTIFF(sino,"projfilt_result.tif",kipl::base::Float32);
    QCOMPARE(pf.currentFFTSize(),2048UL);
    QCOMPARE(pf.currentImageSize(),sino.Size(0));

}

void TestImagingAlgorithms::ProjectionFilterPadding()
{
//    kipl::base::TImage<float,2> sino;
//#ifdef DEBUG
//    kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
//#else
//    kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
//#endif

//    ImagingAlgorithms::ProjectionFilter pf(nullptr);
//    kipl::base::TImage<float,2> tmp=sino;
//    tmp.Clone();

//    pf.process(tmp);


//    pf.pad(sino.GetLinePtr(0), sino.Size(0), float *pDest, const size_t nDestLen);

//    kipl::io::WriteTIFF(sino,"projfilt_result.tif",kipl::base::Float32);
//    QCOMPARE(pf.currentFFTSize(),2048UL);
//    QCOMPARE(pf.currentImageSize(),sino.Size(0));

}
void TestImagingAlgorithms::StripeFilterParameters()
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

void TestImagingAlgorithms::StripeFilterProcessing2D()
{
    kipl::base::TImage<float,2> sino;

    std::string fname = dataPath+"2D/tiff/woodsino_0200.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadTIFF(sino,fname);

    ImagingAlgorithms::StripeFilter sf(sino.dims(),"daub17",4,0.21f);

    sf.process(sino);
}

void TestImagingAlgorithms::RefImgCorrection_Initialization()
{
    ImagingAlgorithms::ReferenceImageCorrection bb;
}

void TestImagingAlgorithms::RefImgCorrection_enums()
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod> strmap;
    strmap["otsumask"]                = ImagingAlgorithms::ReferenceImageCorrection::otsuMask;
    strmap["manuallythresholdedmask"] = ImagingAlgorithms::ReferenceImageCorrection::manuallyThresholdedMask;
    strmap["userdefinedmask"]         = ImagingAlgorithms::ReferenceImageCorrection::userDefinedMask;
    strmap["referencefreemask"]       = ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask;

    ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod em;
    for (auto & item : strmap)
    {
        QCOMPARE(item.first,enum2string(item.second));
        string2enum(item.first,em);
        QCOMPARE(item.second,em);
    }
    em=static_cast<ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod>(9999);
    QVERIFY_THROWS_EXCEPTION(ImagingException,enum2string(em));
    QVERIFY_THROWS_EXCEPTION(ImagingException,string2enum("flipfolp",em));
}

void TestImagingAlgorithms::ProjSeriesCorr_Enums()
{
    {
        std::map<std::string, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection> strmap;
        strmap["fixedthreshold"] = ImagingAlgorithms::ReplaceUnderexposed::FixedThreshold;
        strmap["mad"]            = ImagingAlgorithms::ReplaceUnderexposed::MAD;

        ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection em;
        for (auto & item : strmap)
        {
            QCOMPARE(item.first,enum2string(item.second));
            string2enum(item.first,em);
            QCOMPARE(item.second,em);
        }
        em=static_cast<ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierDetection>(9999);
        QVERIFY_THROWS_EXCEPTION(ImagingException,enum2string(em));
        QVERIFY_THROWS_EXCEPTION(ImagingException,string2enum("flipfolp",em));
    }

    {
        std::map<std::string, ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement> strmap;
        strmap["neighboraverage"] = ImagingAlgorithms::ReplaceUnderexposed::NeighborAverage;
        strmap["weightedaverage"] = ImagingAlgorithms::ReplaceUnderexposed::WeightedAverage;

        ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement em;
        for (auto & item : strmap)
        {
            QCOMPARE(item.first,enum2string(item.second));
            string2enum(item.first,em);
            QCOMPARE(item.second,em);
        }
        em=static_cast<ImagingAlgorithms::ReplaceUnderexposed::eDoseOutlierReplacement>(9999);
        QVERIFY_THROWS_EXCEPTION(ImagingException,enum2string(em));
        QVERIFY_THROWS_EXCEPTION(ImagingException,string2enum("flipfolp",em));
    }
}

void TestImagingAlgorithms::ProjSeriesCorr_Detection()
{
 auto data=kipl::io::readCSV(dataPath+"1D/dosedata.txt",',',false);
 auto doses = data["1"];

 qDebug() << doses.size();
}

void TestImagingAlgorithms::ProjSeriesCorr_Correction()
{

}

QTEST_APPLESS_MAIN(TestImagingAlgorithms)

#include "tst_testImagingAlgorithms.moc"
