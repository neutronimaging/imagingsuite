#include <QtTest>

#include "../../src/Reslicer.h"

// add necessary includes here

class TestReslicer : public QObject
{
    Q_OBJECT

public:
    TestReslicer();
    ~TestReslicer();

private slots:
    void test_Process();

};

TestReslicer::TestReslicer()
{

}

TestReslicer::~TestReslicer()
{

}

void TestReslicer::test_Process()
{
    TIFFReslicer reslicer;

    QDir dir;
    QString confname=dir.homePath()+"/"+".imagingtools/reslicer.xml" ;
    if (dir.exists(confname)) {
        reslicer.ParseXML(confname.toStdString());
    }
    else {
        QSKIP("Couldn't find par file");
    }

    reslicer.process();
}

QTEST_APPLESS_MAIN(TestReslicer)

#include "tst_reslicer.moc"
