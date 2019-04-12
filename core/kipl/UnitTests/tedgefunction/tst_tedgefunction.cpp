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
        std::cout <<t[i] <<  ",  " ;
     }

      std::cout << std::endl;

     double *term4 = new double[30];
     double *term3 = new double[30];
     double *term5 = new double[30];
     double *edgefunction = new double[30];



     kipl::math::edgefunction myedge;
     myedge.t = t;
     myedge.t0 = 3.5;
//     myedge.erfc1 = erfc1;
     myedge.computeTerm3(term3,t);
     myedge.edgeFunction = edgefunction;
     myedge.term3 = term3;
     myedge.term4 = term4;
     myedge.term5 = term5;

//     myedge.sigma =0.1;
//     myedge.alpha =0.1;

     std::cout << "term3:" << std::endl;

     for (int i=0; i<30; i++)
    {
         std::cout << term3[i] << ", " ;
     }
     std::cout << std::endl;

     myedge.computeTerm5(term5,t);
      std::cout << "term5:" << std::endl;

     for (int i=0; i<30; i++)
    {
         std::cout << term5[i] << ", " ;
     }
      std::cout << std::endl;

     myedge.computeTerm4(term4,t);

      std::cout << "term4:" << std::endl;

     for (int i=0; i<30; i++)
    {
         std::cout << term4[i] <<  ", " ;
     }

      std::cout << std::endl;
     myedge.computeEdge(t);

     std::cout << "edge function:" << std::endl;

    for (int i=0; i<30; i++)
   {
        std::cout << myedge.edgeFunction[i] <<  ", " ;
    }
     std::cout << std::endl;

     std::cout << "end of constructor" << std::endl;



}

tedgefunction::~tedgefunction()
{

}

void tedgefunction::test_case1()
{

    std::cout << "testing case 1 empty" << std::endl;
}

QTEST_APPLESS_MAIN(tedgefunction)

#include "tst_tedgefunction.moc"
