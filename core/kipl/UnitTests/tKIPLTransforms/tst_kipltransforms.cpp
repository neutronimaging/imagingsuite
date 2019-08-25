#include <QtTest>

// add necessary includes here

class KIPLTransforms : public QObject
{
    Q_OBJECT

public:
    KIPLTransforms();
    ~KIPLTransforms();

private slots:
    void test_case1();

};

KIPLTransforms::KIPLTransforms()
{

}

KIPLTransforms::~KIPLTransforms()
{

}

void KIPLTransforms::test_case1()
{

}

QTEST_APPLESS_MAIN(KIPLTransforms)

#include "tst_kipltransforms.moc"
