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
