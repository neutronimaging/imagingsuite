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
    // QSKIP("Placeholder for future development of the BB plugin.");
}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(TBBLogNorm)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(TBBLogNorm)
#endif


#include "tst_BBLogNorm.moc"
