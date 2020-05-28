#include <QtTest>

#include <string>
#include <map>

#include <iterativebackproj.h>
#include <sirtbp.h>

// add necessary includes here

class TIterativeBackProj : public QObject
{
    Q_OBJECT

public:
    TIterativeBackProj();
    ~TIterativeBackProj();

private slots:
    void test_GetModuleList();
    void test_SIRTbp_Initialize();

};

TIterativeBackProj::TIterativeBackProj()
{

}

TIterativeBackProj::~TIterativeBackProj()
{

}

void TIterativeBackProj::test_GetModuleList()
{
    std::map<std::string, std::map<std::string, std::string> > modulelist;
    GetModuleList("muhrecbp", reinterpret_cast<void *>(&modulelist));
    qDebug() << modulelist.size();

    QCOMPARE(modulelist.size(),1UL);
}

void TIterativeBackProj::test_SIRTbp_Initialize()
{

}

QTEST_APPLESS_MAIN(TIterativeBackProj)

#include "tst_titerativebackproj.moc"
