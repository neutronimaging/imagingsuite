#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_fits.h>

class kiplIOTest : public QObject
{
    Q_OBJECT

public:
    kiplIOTest();

private Q_SLOTS:

    void testFITSreadwrite();
    void testCroppedFITSreading();
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



QTEST_APPLESS_MAIN(kiplIOTest)

#include "tst_kipliotest.moc"
