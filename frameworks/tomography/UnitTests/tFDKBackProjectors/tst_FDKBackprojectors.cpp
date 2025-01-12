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

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(TFDKBackProjectors)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(TFDKBackProjectors)
#endif


#include "tst_FDKBackprojectors.moc"
