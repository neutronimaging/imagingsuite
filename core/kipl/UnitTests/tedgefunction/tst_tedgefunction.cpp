#include <QtTest>

// add necessary includes here

#include <math/edgefunction.h>
#include <iostream>
using namespace std;

class tedgefunction : public QObject
{
    Q_OBJECT

public:
    tedgefunction();
    ~tedgefunction();

private slots:
    void test_case1();

};

tedgefunction::tedgefunction()
{
    // here I should create the data that I want to use for testing, or something like this,  so for example a vector t of lambda values
     double *t = new double[30];

     for (int i=0; i<30; ++i)
     {
        t[i] = 3.0+i*(4.0-3.0)/30;
        std::cout <<t[i] << std::endl;
     }

}

tedgefunction::~tedgefunction()
{

}

void tedgefunction::test_case1()
{

}

QTEST_APPLESS_MAIN(tedgefunction)

#include "tst_tedgefunction.moc"
