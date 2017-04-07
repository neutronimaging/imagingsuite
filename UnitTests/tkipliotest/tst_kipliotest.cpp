#include <vector>
#include <string>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/KiplException.h>
#include <io/io_fits.h>
#include <io/DirAnalyzer.h>

class kiplIOTest : public QObject
{
    Q_OBJECT

public:
    kiplIOTest();

private Q_SLOTS:

    void testFITSreadwrite();
    void testCroppedFITSreading();
    void testFileItem();
    void testDirAnalyzer();
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

void kiplIOTest::testFileItem()
{
    kipl::io::FileItem fi;

    QVERIFY(fi.m_nIndex==0);
    QVERIFY(fi.m_sExt=="tif");
    QVERIFY(fi.m_sMask=="noname_####.tif");

    kipl::io::FileItem fi2("hepp_####.abc",1,"abc");
    QVERIFY(fi2.m_nIndex==1);
    QVERIFY(fi2.m_sExt=="abc");
    QVERIFY(fi2.m_sMask=="hepp_####.abc");

    fi=fi2;

    QVERIFY(fi2.m_nIndex==fi.m_nIndex);
    QVERIFY(fi2.m_sExt==fi.m_sExt);
    QVERIFY(fi2.m_sMask==fi.m_sMask);

}

void kiplIOTest::testDirAnalyzer()
{
    kipl::io::DirAnalyzer di;

    kipl::io::FileItem fi=di.GetFileMask("hepp_1234.xyz");

    QVERIFY(fi.m_nIndex==1234);
    QVERIFY(fi.m_sExt=="xyz");
    QVERIFY(fi.m_sMask=="hepp_####.xyz");

    kipl::io::FileItem fi2=di.GetFileMask("hopp2_56780.abc");

    QVERIFY(fi2.m_nIndex==56780);
    QVERIFY(fi2.m_sExt=="abc");
    QVERIFY(fi2.m_sMask=="hopp2_#####.abc");

    kipl::io::FileItem fi3=di.GetFileMask("hopp567.xyz");

    QVERIFY(fi3.m_nIndex==567);
    QVERIFY(fi3.m_sExt=="xyz");
    QVERIFY(fi3.m_sMask=="hopp###.xyz");

    std::string path="/";
    std::vector<std::string> dirlist=di.GetDirList(path);

    for (auto it=dirlist.begin(); it!=dirlist.end(); ++it)
        std::cout<<*it<<", ";

    std::cout<<std::endl;
    int cnt,first,last;

    di.AnalyzeMatchingNames("/Users/data/02_CT/ob_#####.fits",cnt,first,last);
    QVERIFY(cnt==5);
    QVERIFY(first==1);
    QVERIFY(last==5);
}

QTEST_APPLESS_MAIN(kiplIOTest)

#include "tst_kipliotest.moc"
