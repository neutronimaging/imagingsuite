#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <string>
#include <map>
#include <exception>

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
    void test_enum_algorithm();
    void test_enum_interpolation();

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

void TFDKBackProjectors::test_enum_algorithm()
{
    eFDKbp_singleAlgorithms alg;
    string2enum("c", alg);
    QCOMPARE(alg, eFDKbp_singleAlgorithms::c);

    string2enum("c2", alg);
    QCOMPARE(alg, eFDKbp_singleAlgorithms::c2);

    string2enum("stl", alg);
    QCOMPARE(alg, eFDKbp_singleAlgorithms::stl);

    string2enum("reference", alg);
    QCOMPARE(alg, eFDKbp_singleAlgorithms::reference);

    QCOMPARE(enum2string(eFDKbp_singleAlgorithms::reference), "reference");
    QCOMPARE(enum2string(eFDKbp_singleAlgorithms::c), "c");
    QCOMPARE(enum2string(eFDKbp_singleAlgorithms::c2), "c2");
    QCOMPARE(enum2string(eFDKbp_singleAlgorithms::stl), "stl");

    QVERIFY_THROWS_EXCEPTION(std::runtime_error, string2enum("unknown", alg));
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, enum2string(static_cast<eFDKbp_singleAlgorithms>(-1)));
}

void TFDKBackProjectors::test_enum_interpolation()
{
    eFDKbp_interpolation interp;
    string2enum("nearest", interp);
    QCOMPARE(interp, eFDKbp_interpolation::nearest);

    string2enum("bilinear", interp);
    QCOMPARE(interp, eFDKbp_interpolation::bilinear);

    QCOMPARE(enum2string(eFDKbp_interpolation::nearest), "nearest");
    QCOMPARE(enum2string(eFDKbp_interpolation::bilinear), "bilinear");

    QVERIFY_THROWS_EXCEPTION(std::runtime_error, string2enum("unknown", interp));
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, enum2string(static_cast<eFDKbp_interpolation>(-1)));

}

QTEST_APPLESS_MAIN(TFDKBackProjectors)

#include "tst_FDKBackprojectors.moc"
