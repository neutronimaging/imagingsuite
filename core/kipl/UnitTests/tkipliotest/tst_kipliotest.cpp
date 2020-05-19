#include <vector>
#include <string>
#include <limits>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/KiplException.h>
#include <io/io_fits.h>
#include <io/DirAnalyzer.h>
#include <io/io_vivaseq.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>
#include <io/io_stack.h>
#include <io/analyzefileext.h>

class kiplIOTest : public QObject
{
    Q_OBJECT

public:
    kiplIOTest();

private:
    void MakeFiles(std::string mask, int N, int first=0);
    QDir dir;
private Q_SLOTS:

    void testFITSreadwrite();
    void testCroppedFITSreading();
    void testSEQHeader();
    void testSEQRead();
    void testTIFFBasicReadWrite();
    void testTIFFMultiFrame();
    void testTIFFAdvancedMultiFrame();
    void testTIFF32();
    void testTIFFclamp();
    void testIOStack_enums();
};

kiplIOTest::kiplIOTest()
{
}

void kiplIOTest::testFITSreadwrite()
{
    std::vector<size_t> dims={100,256};
    kipl::base::TImage<short,2> img(dims);
    kipl::base::TImage<short,2> res;

    for (size_t i=0; i<img.Size(); ++i)
    {
            img[i]=static_cast<short>(i);
    }

    kipl::io::WriteFITS(img,"test.fits");

    kipl::io::ReadFITS(res,"test.fits");


    QVERIFY2(img.Size(0)==res.Size(0), "dim0 miss match");
    QVERIFY2(img.Size(1)==res.Size(1), "dim1 miss match");

    for (size_t i=0; i<img.Size(); ++i)
        QVERIFY2(img[i]==res[i], "value error");
}

void kiplIOTest::testCroppedFITSreading()
{

    QVERIFY2(true, "Failure");
}

void kiplIOTest::testSEQHeader()
{
   std::ostringstream msg;
   kipl::io::ViVaSEQHeader header;

   if (dir.exists("/Users/kaestner/Desktop/Video1.seq")==false)
       QSKIP( "Test data is missing");

   msg<<"sizeof(header)=="<<sizeof(header);
   QVERIFY2(sizeof(header)==2048,msg.str().c_str());

   kipl::io::GetViVaSEQHeader("/Users/kaestner/Desktop/Video1.seq",&header);

   size_t dims[2]={0,0};
   int numframes=0;
   numframes=kipl::io::GetViVaSEQDims("/Users/kaestner/Desktop/Video1.seq",dims);

   QVERIFY(dims[0]==header.imageWidth);
   QVERIFY(dims[1]==header.imageHeight);
   QVERIFY(numframes==static_cast<int>(header.numberOfFrames));
}

void kiplIOTest::testSEQRead()
{


    if (dir.exists("/Users/kaestner/Desktop/Video1.seq")==false)
        QSKIP( "Test data is missing");

    kipl::base::TImage<float,3> img;
    kipl::io::ViVaSEQHeader header;

    kipl::io::GetViVaSEQHeader("/Users/kaestner/Desktop/Video1.seq",&header);
    kipl::io::ReadViVaSEQ("/Users/kaestner/Desktop/Video1.seq",img);

    QVERIFY(img.Size(0)==header.imageWidth);
    QVERIFY(img.Size(1)==header.imageHeight);
    QVERIFY(img.Size(2)==header.numberOfFrames);

    size_t roi[]={100,100,300,200};

    kipl::io::ReadViVaSEQ("/Users/kaestner/Desktop/Video1.seq",img,roi);

    QVERIFY(img.Size(0)==(roi[2]-roi[0]));
    QVERIFY(img.Size(1)==(roi[3]-roi[1]));
    QVERIFY(img.Size(2)==header.numberOfFrames);


}

void kiplIOTest::testTIFFBasicReadWrite()
{
    std::vector<size_t> dims={100,50};

    kipl::base::TImage<float,2> fimg(dims);
    fimg.info.sArtist="UnitTest";
    fimg.info.sCopyright="none";
    fimg.info.sDescription="slope = 1.0E0 \noffset = 0.0E0";
    fimg.info.SetDPCMX(123.0f);
    fimg.info.SetDPCMY(321.0f);

    for (size_t i=0; i<fimg.Size(); i++)
        fimg[i]=static_cast<float>(i);

    kipl::io::WriteTIFF(fimg,"basicRW.tif");

    kipl::base::TImage<float,2> resimg;

    kipl::io::ReadTIFF(resimg,"basicRW.tif");
    std::ostringstream msg;
    QVERIFY2(fimg.Size(0)==resimg.Size(0),"X size missmatch");
    QVERIFY2(fimg.Size(1)==resimg.Size(1),"Y size missmatch");

    QVERIFY2(fimg.info.sArtist==resimg.info.sArtist,"Artist information error");
    QVERIFY2(fimg.info.sDescription==resimg.info.sDescription,"Description error");
    QVERIFY2(fimg.info.sCopyright==resimg.info.sCopyright,"Copyright error");
    QVERIFY2(fimg.info.GetDPCMX()==resimg.info.GetDPCMX(),"X Resolution error");
    msg<<"Y resolution error: fimg="<<fimg.info.GetDPCMY()<<", resimg="<<resimg.info.GetDPCMY();
    QVERIFY2(fimg.info.GetDPCMY()==resimg.info.GetDPCMY(),msg.str().c_str());

    for (size_t i=0; i<fimg.Size(); ++i) {
        QVERIFY2(fimg[i]==resimg[i],"Pixels not similar");
    }

}

void kiplIOTest::testTIFFMultiFrame()
{
#ifdef NDEBUG
    std::string fname="../../TestData/3D/tiff/multiframe.tif";
#else
    std::string fname="../TestData/3D/tiff/multiframe.tif";
#endif
    if (dir.exists(QString::fromStdString(fname))==false)
        QSKIP("Test data is missing");

    size_t dims[3]={0,0,0};
    size_t nframes = kipl::io::GetTIFFDims(fname,dims);

    QCOMPARE(dims[0],145UL);
    QCOMPARE(dims[1],249UL);
    QCOMPARE(nframes,5);

    auto dimsv=kipl::io::GetTIFFDims(fname);

    QCOMPARE(dimsv.size(),3UL);
    QCOMPARE(dimsv[0],145UL);
    QCOMPARE(dimsv[1],249UL);
    QCOMPARE(dimsv[2],5UL);

    float minvals[5]={4032.0f,4443.0f,5244.0f,5849.0f,3687.0f};
    float maxvals[5]={65192.0f,65534.0f,65534.0f,57957.0f,42548.0f};

    size_t crop[4]={10,10,100,100};

    kipl::base::TImage<float,2> img, img_crop;

    kipl::base::TImage<float,3> img3ref(dimsv);
    for (int i = 0 ; i<nframes; ++i) {
        kipl::io::ReadTIFF(img,fname.c_str(),nullptr,i);
        std::copy_n(img.GetDataPtr(),img.Size(),img3ref.GetLinePtr(0,i));

        QCOMPARE(img.Size(0),145UL);
        QCOMPARE(img.Size(1),249UL);

        float *mi=std::min_element(img.GetDataPtr(),img.GetDataPtr()+img.Size());
        QCOMPARE(*mi,minvals[i]);

        float *ma=std::max_element(img.GetDataPtr(),img.GetDataPtr()+img.Size());
        QCOMPARE(*ma,maxvals[i]);

        kipl::io::ReadTIFF(img_crop,fname.c_str(),crop,i);
        QCOMPARE(img_crop.Size(0),90UL);
        QCOMPARE(img_crop.Size(1),90UL);

        for (size_t y=0; y<img_crop.Size(1); ++y) {
            float *pLine0=img.GetLinePtr(y+crop[1])+crop[0];
            float *pLine1=img_crop.GetLinePtr(y);

            for (size_t x=0; x<img_crop.Size(0); x++)
                QCOMPARE(pLine1[x],pLine0[x]);
        }
    }

    kipl::base::TImage<float,3> img3;
    kipl::io::ReadTIFF(img3,fname.c_str());

    QCOMPARE(img3.Size(0),img3ref.Size(0));
    QCOMPARE(img3.Size(1),img3ref.Size(1));
    QCOMPARE(img3.Size(2),img3ref.Size(2));

    for (size_t i=0; i<img3.Size(); ++i) {
        QCOMPARE(img3[i],img3ref[i]);
    }

    kipl::io::WriteTIFF(img3ref,"test.tif");
    img3.FreeImage();
    kipl::io::ReadTIFF(img3,"test.tif");

    QCOMPARE(img3.Size(0),img3ref.Size(0));
    QCOMPARE(img3.Size(1),img3ref.Size(1));
    QCOMPARE(img3.Size(2),img3ref.Size(2));

    for (size_t i=0; i<img3.Size(); ++i) {
        QCOMPARE(img3[i],img3ref[i]);
    }

    kipl::base::TImage<float,3> img3ref2=img3ref;
    img3ref2+=1.0f;

    kipl::io::AppendTIFF(img3ref2,"test.tif");

    img3.FreeImage();
    kipl::io::ReadTIFF(img3,"test.tif");

    QCOMPARE(img3.Size(0),img3ref.Size(0));
    QCOMPARE(img3.Size(1),img3ref.Size(1));
    QCOMPARE(img3.Size(2),2*img3ref.Size(2));

    for (size_t i=0; i<img3ref.Size(); ++i) {
        QCOMPARE(img3[i],img3ref[i]);
        QCOMPARE(img3[i+img3ref.Size()],img3ref2[i]);
    }
}

void kiplIOTest::testTIFFAdvancedMultiFrame()
{
#ifdef NDEBUG
    std::string fname="../../TestData/3D/tiff/multiframe.tif";
#else
    std::string fname="../TestData/3D/tiff/multiframe.tif";
#endif
    if (dir.exists(QString::fromStdString(fname))==false)
        QSKIP("Test data is missing");
}

void kiplIOTest::testTIFF32()
{
    std::vector<size_t> dims={100,110,120};

    kipl::base::TImage<float,2> imgref2d(dims);
    kipl::base::TImage<float,3> imgref3d(dims);

    float scale=1.0f/static_cast<float>(imgref2d.Size());
    for (size_t i=0; i<imgref2d.Size(); ++i)
        imgref2d[i]=static_cast<float>(i)*scale;
    kipl::io::WriteTIFF32(imgref2d,"tiff32_2D.tif");

    scale=1.0f/static_cast<float>(imgref3d.Size());
    for (size_t i=0; i<imgref3d.Size(); ++i)
        imgref3d[i]=static_cast<float>(i)*scale;
    kipl::io::WriteTIFF32(imgref3d,"tiff32_3D.tif");

    kipl::base::TImage<float,2> img2d(dims);
    kipl::io::ReadTIFF(img2d,"tiff32_2D.tif");
    QCOMPARE(img2d.Size(0),imgref2d.Size(0));
    QCOMPARE(img2d.Size(1),imgref2d.Size(1));

    for (size_t i=0; i<img2d.Size(); ++i) {
        QCOMPARE(img2d[i],imgref2d[i]);
    }

    kipl::base::TImage<float,3> img3d(dims);
    kipl::io::ReadTIFF(img3d,"tiff32_3D.tif");

    QCOMPARE(img3d.Size(0),imgref3d.Size(0));
    QCOMPARE(img3d.Size(1),imgref3d.Size(1));
    QCOMPARE(img3d.Size(2),imgref3d.Size(2));

    for (size_t i=0; i<img3d.Size(); ++i) {
        QCOMPARE(img3d[i],imgref3d[i]);
    }

}

void kiplIOTest::testTIFFclamp()
{
    QSKIP("Seems to crash");
    QWARN("Test accepts a difference of +-1");
    std::ostringstream msg;
    std::vector<size_t> dims={100,110,120};

    kipl::base::TImage<float,2> imgref2d(dims);
    kipl::base::TImage<float,3> imgref3d(dims);

    float clampmin=0.1f;
    float clampmax=0.9f;

    float scale=1.0f/static_cast<float>(imgref2d.Size());
    for (size_t i=0; i<imgref2d.Size(); ++i)
        imgref2d[i]=static_cast<float>(i)*scale;
    kipl::io::WriteTIFF(imgref2d,"tiffclamp_2D.tif",clampmin,clampmax);

    scale=1.0f/static_cast<float>(imgref3d.Size());
    for (size_t i=0; i<imgref3d.Size(); ++i)
        imgref3d[i]=static_cast<float>(i)*scale;
    kipl::io::WriteTIFF(imgref3d,"tiffclamp_3D.tif",clampmin,clampmax);

    kipl::base::TImage<float,2> img2d(dims);
    kipl::io::ReadTIFF(img2d,"tiffclamp_2D.tif");
    QCOMPARE(img2d.Size(0),imgref2d.Size(0));
    QCOMPARE(img2d.Size(1),imgref2d.Size(1));

    float slope=0.0f;
    float intercept=0.0f;
    kipl::io::GetSlopeOffset(img2d.info.sDescription,slope,intercept);

//    QCOMPARE(slope,static_cast<float>(std::numeric_limits<unsigned short>::max())/(clampmax-clampmin));
//    QCOMPARE(intercept,clampmin);

    for (size_t i=0; i<img2d.Size(); ++i) {

        float val=(min(max(imgref2d[i],clampmin),clampmax)-clampmin)*static_cast<float>(std::numeric_limits<unsigned short>::max())/(clampmax-clampmin);
        float diff=fabs(img2d[i]-static_cast<float>(static_cast<unsigned short>(val)));

        msg.str("");
        msg<<"i="<<i<<", val="<<val<<", img2d[i]="<<img2d[i]<<", diff="<<diff;

        QVERIFY2(fabs(img2d[i]-val)<=1.0f,msg.str().c_str());
    }

    kipl::base::TImage<float,3> img3d(dims);
    kipl::io::ReadTIFF(img3d,"tiffclamp_3D.tif");

    QCOMPARE(img3d.Size(0),imgref3d.Size(0));
    QCOMPARE(img3d.Size(1),imgref3d.Size(1));
    QCOMPARE(img3d.Size(2),imgref3d.Size(2));

    for (size_t i=0; i<img3d.Size(); ++i) {
        float val=(min(max(imgref3d[i],clampmin),clampmax)-clampmin)*static_cast<float>(std::numeric_limits<unsigned short>::max())/(clampmax-clampmin);
        float diff=fabs(img2d[i]-static_cast<float>(static_cast<unsigned short>(val)));

        msg.str("");
        msg<<"i="<<i<<", val="<<val<<", img3d[i]="<<img3d[i]<<", diff="<<diff;

        QVERIFY2(fabs(img3d[i]-val)<=1.0f,msg.str().c_str());

    }

}

void kiplIOTest::testIOStack_enums()
{
    kipl::io::eFileType ft;

    string2enum("MatlabVolume",ft);
    QCOMPARE(ft, kipl::io::MatlabVolume);

    string2enum("MatlabSlices",ft);
    QCOMPARE(ft, kipl::io::MatlabSlices);

    string2enum("TIFF8bits",ft);
    QCOMPARE(ft, kipl::io::TIFF8bits);

    string2enum("TIFF16bits",ft);
    QCOMPARE(ft, kipl::io::TIFF16bits);

   string2enum("TIFFfloat",ft);
    QCOMPARE(ft, kipl::io::TIFFfloat);

    string2enum("TIFF16bitsMultiFrame",ft);
    QCOMPARE(ft, kipl::io::TIFF16bitsMultiFrame);

    string2enum("NeXusfloat",ft);
    QCOMPARE(ft, kipl::io::NeXusfloat);

    string2enum("NeXus16bits",ft);
    QCOMPARE(ft, kipl::io::NeXus16bits);

    string2enum("PNG8bits",ft);
    QCOMPARE(ft, kipl::io::PNG8bits);

    string2enum("PNG16bits",ft);
    QCOMPARE(ft, kipl::io::PNG16bits);

    // enum 2 string

    QCOMPARE(enum2string(kipl::io::MatlabVolume), std::string("MatlabVolume"));
    QCOMPARE(enum2string(kipl::io::MatlabSlices), std::string("MatlabSlices"));

    QCOMPARE(enum2string(kipl::io::TIFF8bits),    std::string("TIFF8bits"));
    QCOMPARE(enum2string(kipl::io::TIFF16bits),   std::string("TIFF16bits"));
    QCOMPARE(enum2string(kipl::io::TIFFfloat),    std::string("TIFFfloat"));
    QCOMPARE(enum2string(kipl::io::TIFF16bitsMultiFrame), std::string("TIFF16bitsMultiFrame"));

    QCOMPARE(enum2string(kipl::io::NeXusfloat),  std::string("NeXusfloat"));
    QCOMPARE(enum2string(kipl::io::NeXus16bits), std::string("NeXus16bits"));

    QCOMPARE(enum2string(kipl::io::PNG8bits),    std::string("PNG8bits"));
    QCOMPARE(enum2string(kipl::io::PNG16bits),   std::string("PNG16bits"));

}

QTEST_APPLESS_MAIN(kiplIOTest)

#include "tst_kipliotest.moc"
