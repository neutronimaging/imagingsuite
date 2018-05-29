#include <string>
#include <QString>
#include <QtTest>

#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>

class TKIPLStringsTest : public QObject
{
    Q_OBJECT

public:
    TKIPLStringsTest();

private Q_SLOTS:
    void testMakeFileName();
    void testGetStrNum();
    void testStripFileName();
    void testExtractWildCard();
    void testCheckPathSlashes();
    void testGetFileExtension();
};

TKIPLStringsTest::TKIPLStringsTest()
{
}

void TKIPLStringsTest::testMakeFileName()
{
    std::string mask="test_###.txt";
    std::string fname,ext;
    std::string validation_name="test_001.txt";
    std::string validation_ext=".txt";

    kipl::strings::filenames::MakeFileName(mask,1,fname,ext,'#','0');

    QVERIFY(fname==validation_name);
    QVERIFY(ext==validation_ext);

    mask="test#.abc";
    validation_name="test100.abc";
    validation_ext=".abc";

    kipl::strings::filenames::MakeFileName(mask,100,fname,ext,'#','0');

    QVERIFY(fname==validation_name);
    QVERIFY(ext==validation_ext);

}

void TKIPLStringsTest::testGetStrNum()
{


}

void TKIPLStringsTest::testStripFileName()
{
    std::string fname;
    std::string name;
    std::string path;
    std::vector<std::string> ext;

    fname="file01.txt";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("./"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));

    fname="file01.txt.csv";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("./"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));
    QCOMPARE(ext[1],std::string(".csv"));

    fname="w/file01.txt";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));

    fname="/w/file01.txt";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));

    fname="q/w/file01.txt";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("q/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));

    fname="/q/w/file01.txt";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("/q/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));


    fname="w/file01.txt.csv";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));
    QCOMPARE(ext[1],std::string(".csv"));

    fname="/w/file01.txt.csv";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));
    QCOMPARE(ext[1],std::string(".csv"));

    fname="q/w/file01.txt.csv";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("q/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));
    QCOMPARE(ext[1],std::string(".csv"));

    fname="/q/w/file01.txt.csv";
    kipl::strings::filenames::StripFileName(fname,path,name,ext);
    QCOMPARE(path,std::string("/q/w/"));
    QCOMPARE(name,std::string("file01"));
    QCOMPARE(ext[0],std::string(".txt"));
    QCOMPARE(ext[1],std::string(".csv"));
}

void TKIPLStringsTest::testExtractWildCard()
{


}

void TKIPLStringsTest::testCheckPathSlashes()
{


}

void TKIPLStringsTest::testGetFileExtension()
{


}

QTEST_APPLESS_MAIN(TKIPLStringsTest)

#include "tst_tkiplstringstest.moc"
