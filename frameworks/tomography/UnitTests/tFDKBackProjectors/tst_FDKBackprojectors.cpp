#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <string>
#include <map>

#include <fdkbackproj.h>
#include <fdkbp.h>
#include <fdkbp_single.h>

class TFDKBackProjectors : public QObject
{
    Q_OBJECT

public:
    TFDKBackProjectors();
    ~TFDKBackProjectors();

private slots:
    void test_GetModuleList();
    void test_ftkbp_Initialize();
    void test_ftkbp_single_Initialize();

};

TFDKBackProjectors::TFDKBackProjectors()
{

}

TFDKBackProjectors::~TFDKBackProjectors()
{

}

void TFDKBackProjectors::test_GetModuleList()
{
    std::map<std::string, std::map<std::string, std::string> > modulelist;
    GetModuleList("muhrecbp", reinterpret_cast<void *>(&modulelist));
    qDebug() << modulelist.size();

    QCOMPARE(modulelist.size(),2UL);
}

void TFDKBackProjectors::test_ftkbp_Initialize()
{

}

void TFDKBackProjectors::test_ftkbp_single_Initialize()
{

}

QTEST_APPLESS_MAIN(TFDKBackProjectors)

#include "tst_FDKBackprojectors.moc"
