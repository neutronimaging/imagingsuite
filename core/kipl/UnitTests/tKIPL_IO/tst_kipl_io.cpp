#include <sstream>
#include <string>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>


class tKIPL_IOTest : public QObject
{
    Q_OBJECT

public:
    tKIPL_IOTest();

private Q_SLOTS:
    void testBasicReadWriteTIFF();
    void testReadRoiTIFF();
    void testMultiFrameReadTIFF();
    void testMultiFrameWriteTIFF();
    void testDataTypesWriteTIFF();
    void testReadNexus();

private:
    std::string data_path;

};

tKIPL_IOTest::tKIPL_IOTest()
{
    data_path = QT_TESTCASE_BUILDDIR;
    #ifdef __APPLE__
        data_path = data_path + "/../../../../../../TestData/";
    #elif defined(__linux__)
        data_path = data_path + "/../../../../../../TestData/";
    #else
        data_path = data_path + "/../../../../../TestData/";
    #endif
    
    kipl::strings::filenames::CheckPathSlashes(data_path,true);
}

void tKIPL_IOTest::testBasicReadWriteTIFF()
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

    for (size_t i=0; i<fimg.Size(); i++) {
        QVERIFY2(fimg[i]==resimg[i],"Pixels not similar");
    }

}

void tKIPL_IOTest::testReadRoiTIFF()
{
    std::string fname=data_path+"2D/tiff/normalized_edge.tif"; // Image size 122x300
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::base::TImage<float,2> img;

    kipl::io::ReadTIFF(img,fname);

    std::vector<size_t> roi={10,10,100,100};
    kipl::io::ReadTIFF(img,fname,roi,0);

    QCOMPARE(img.Size(0),roi[2]-roi[0]);
    QCOMPARE(img.Size(1),roi[3]-roi[1]);

    std::vector<size_t> roi2={10,10,20,200}; // Vertical slab taller than the image size
    kipl::io::ReadTIFF(img,fname,roi2,0);

    QCOMPARE(img.Size(0),roi2[2]-roi2[0]);
    QCOMPARE(img.Size(1),roi2[3]-roi2[1]);

    std::vector<size_t> roi3={10,10,20,400}; // Vertical slab outside the image size
    QVERIFY_THROWS_EXCEPTION( kipl::base::KiplException, kipl::io::ReadTIFF(img,fname,roi3,0));

    std::vector<size_t> roi4={10,300,20,200}; // Vertical slab outside the image size
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException,kipl::io::ReadTIFF(img,fname,roi4,0));
}



void tKIPL_IOTest::testMultiFrameReadTIFF()
{

}

void tKIPL_IOTest::testMultiFrameWriteTIFF()
{

}

void tKIPL_IOTest::testDataTypesWriteTIFF()
{
    std::vector<size_t> dims={100,50};

    kipl::base::TImage<float,2> fimg(dims);
    fimg.info.sArtist="UnitTest";
    fimg.info.sCopyright="none";
    fimg.info.sDescription="slope = 1.0E0 \noffset = 0.0E0";
    fimg.info.SetDPCMX(123.0f);
    fimg.info.SetDPCMY(321.0f);

    for (size_t i=0; i<fimg.Size(); i++)
        fimg[i]=static_cast<float>(i)*0.5;
        
    kipl::io::WriteTIFF(fimg,"basicRW8.tif",kipl::base::UInt8);
    kipl::io::WriteTIFF(fimg,"basicRW16.tif",kipl::base::UInt16);
    kipl::io::WriteTIFF(fimg,"basicRWfloat.tif",kipl::base::Float32);

    kipl::base::TImage<float,2> res;
    kipl::io::ReadTIFF(res,"basicRWfloat.tif");
    QCOMPARE(res.Size(0),fimg.Size(0));
    QCOMPARE(res.Size(1),fimg.Size(1));
    for (size_t i = 0 ; i<fimg.Size(); ++i)
        QCOMPARE(res[i],fimg[i]);

    kipl::io::ReadTIFF(res,"basicRW16.tif");
    QCOMPARE(res.Size(0),fimg.Size(0));
    QCOMPARE(res.Size(1),fimg.Size(1));
    for (size_t i = 0 ; i<fimg.Size(); ++i)
        QCOMPARE(res[i],floor(fimg[i]));

    kipl::io::ReadTIFF(res,"basicRW8.tif");
    QCOMPARE(res.Size(0),fimg.Size(0));
    QCOMPARE(res.Size(1),fimg.Size(1));

    kipl::base::TImage<unsigned char,2> img8bit(dims);
    std::copy_n(fimg.GetDataPtr(),fimg.Size(),img8bit.GetDataPtr());

    for (size_t i = 0 ; i<fimg.Size(); ++i)
        QCOMPARE(res[i],img8bit[i]);
        //QCOMPARE(res[i],fmod(floor(fimg[i]),256.0f));

}

void tKIPL_IOTest::testReadNexus()
{

}

QTEST_APPLESS_MAIN(tKIPL_IOTest)

#include "tst_kipl_io.moc"
