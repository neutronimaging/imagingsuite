#include <vector>
#include <string>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/KiplException.h>
#include <io/io_fits.h>
#include <io/DirAnalyzer.h>
#include <io/io_vivaseq.h>
#include <strings/filenames.h>

class kiplIOTest : public QObject
{
    Q_OBJECT

public:
    kiplIOTest();

private:
    void MakeFiles(std::string mask, int N, int first=0);

private Q_SLOTS:

    void testFITSreadwrite();
    void testCroppedFITSreading();
    void testSEQHeader();
    void testSEQRead();
};

kiplIOTest::kiplIOTest()
{
}

void kiplIOTest::testFITSreadwrite()
{
    size_t dims[2]={10,12};
    kipl::base::TImage<short,2> img(dims);
    kipl::base::TImage<short,2> res;

    for (size_t i=0; i<img.Size(); ++i)
    {
            img[i]=i;
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

   msg<<"sizeof(header)=="<<sizeof(header);
   QVERIFY2(sizeof(header)==2048,msg.str().c_str());

   kipl::io::GetViVaSEQHeader("/Users/kaestner/Desktop/Video1.seq",&header);
//   std::cout << header.headerSize  << ", "
//             << header.imageWidth  << ", "
//             << header.imageHeight << ", "
//             << header.numberOfFrames<<std::endl;
   size_t dims[2]={0,0};
   int numframes=0;
   numframes=kipl::io::GetViVaSEQDims("/Users/kaestner/Desktop/Video1.seq",dims);

   QVERIFY(dims[0]==header.imageWidth);
   QVERIFY(dims[1]==header.imageHeight);
   QVERIFY(numframes==static_cast<int>(header.numberOfFrames));
}

void kiplIOTest::testSEQRead()
{
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

QTEST_APPLESS_MAIN(kiplIOTest)

#include "tst_kipliotest.moc"
