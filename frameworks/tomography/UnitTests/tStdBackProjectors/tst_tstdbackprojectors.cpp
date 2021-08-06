#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <StdBackProjectors.h>
#include <MultiProjBP.h>
#include <MultiProjBPparallel.h>
#include <NNMultiProjBP.h>

class TStdBackProjectors : public QObject
{
    Q_OBJECT

public:
    TStdBackProjectors();
    ~TStdBackProjectors();

private slots:
    void test_GetModuleList();
    void test_MultiProjBP_Initialize();
    void test_MultiProjBPparallel_Initialize();
    void test_NNMultiProjBP_Initialize();

};

TStdBackProjectors::TStdBackProjectors()
{

}

TStdBackProjectors::~TStdBackProjectors()
{

}

void TStdBackProjectors::test_GetModuleList()
{
    std::map<std::string, std::map<std::string, std::string> > modulelist;
    GetModuleList("muhrecbp", reinterpret_cast<void *>(&modulelist));
    qDebug() << modulelist.size();

    QCOMPARE(modulelist.size(),3UL);
}

void TStdBackProjectors::test_MultiProjBP_Initialize()
{
    MultiProjectionBP bp;

}

void TStdBackProjectors::test_MultiProjBPparallel_Initialize()
{

}

void TStdBackProjectors::test_NNMultiProjBP_Initialize()
{

}

QTEST_APPLESS_MAIN(TStdBackProjectors)

#include "tst_tstdbackprojectors.moc"
