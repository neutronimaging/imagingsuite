#include <QtTest>

// add necessary includes here

#include <math/edgefunction.h>
#include <Faddeeva.hh>
#include <iostream>
using namespace std;
using namespace Faddeeva;

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

     double *gauss = new double[30];
     double *erfc1 = new double[30];
     double *erfc2 = new double[30];
     double *edgefunction = new double[30];

//     double *gauss;
//     double *erfc1;
//     double *erfc2;
//     double *edgefunction;



     kipl::math::edgefunction myedge;
     myedge.t = t;
     myedge.t0 = 3.5;
//     myedge.erfc1 = erfc1;
     myedge.computeErfc1(erfc1,t);
     myedge.edgeFunction = edgefunction;
     myedge.erfc1 = erfc1;
     myedge.erfc2 = erfc2;
     myedge.gauss = gauss;

//     myedge.sigma =0.1;
//     myedge.alpha =0.1;

//     std::cout << "erfc1:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << erfc1[i] << std::endl;
//     }

//     myedge.computeErfc2(erfc2,t);
//      std::cout << "erfc2:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << erfc2[i] << std::endl;
//     }

//     myedge.computeGauss(gauss,t);

//      std::cout << "gauss:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << gauss[i] << std::endl;
//     }

     myedge.computeEdge(t);

     std::cout << "edge function:" << std::endl;

    for (int i=0; i<30; i++)
   {
        std::cout << myedge.edgeFunction[i] << std::endl;
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
