#include <QtTest>

// add necessary includes here

class TBBLogNorm : public QObject
{
    Q_OBJECT

public:
    TBBLogNorm();
    ~TBBLogNorm();

private slots:
    void test_parameters();

};

TBBLogNorm::TBBLogNorm()
{

}

TBBLogNorm::~TBBLogNorm()
{

}

void TBBLogNorm::test_parameters()
{

}

QTEST_APPLESS_MAIN(TBBLogNorm)

#include "tst_tbblognorm.moc"
