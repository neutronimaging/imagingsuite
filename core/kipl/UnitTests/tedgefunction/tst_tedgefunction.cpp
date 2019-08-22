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
    unsigned long size = 30;
    double *t = new double[size];

     for (int i=0; i<size; ++i)
     {
        t[i] = 3.0+i*(5.0-3.0)/size;
        std::cout <<t[i] <<  ",  " ;
     }

      std::cout << std::endl;


//     double *term1 = new double[size];
//     double *term2 = new double[size];
//     double *term4 = new double[size];
//     double *term3 = new double[size];
//     double *term5 = new double[size];
//     double *edgefunction = new double[size];
//     double *simple_edge = new double[size];

//     double *term1 = nullptr;
//     double *term2 = nullptr;
//     double *term4 = nullptr;
//     double *term3 = nullptr;
//     double *term5 = nullptr;
//     double *edgefunction = nullptr;
//     double *simple_edge = nullptr;

      double *exp_before=new double[size];
      double *exp_after = new double[size];



     kipl::math::edgefunction myedge(size, t);
     myedge.t = t;
     myedge.t0 = 4.06667;
     myedge.alpha = 0.024;
     myedge.sigma = 0.0053;
     myedge.a1 = 0.177;
     myedge.a2 = 0.10;
     myedge.a5 = -0.47;
     myedge.a6 = 0.39;


//     myedge.computeTerm3(term3,t);
//     myedge.edgeFunction = edgefunction;
//     myedge.term1 = term1;
//     myedge.term2 = term2;
//     myedge.term3 = term3;
//     myedge.term4 = term4;
//     myedge.term5 = term5;

//     myedge.sigma =0.1;
//     myedge.alpha =0.1;

//     std::cout << "term3:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << term3[i] << ", " ;
//     }
//     std::cout << std::endl;

//     myedge.computeTerm5(term5,t);
//      std::cout << "term5:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << term5[i] << ", " ;
//     }
//      std::cout << std::endl;

//     myedge.computeTerm4(term4,t);

//      std::cout << "term4:" << std::endl;

//     for (int i=0; i<30; i++)
//    {
//         std::cout << term4[i] <<  ", " ;
//     }

//      std::cout << std::endl;



//     myedge.computeTerm1(term1,t);
//     myedge.computeTerm2(term2,t);


//     std::cout << "term1: " << std::endl;
//     for (int i=0; i<30; i++)
//    {
//         std::cout << term1[i] <<  ", " ;
//     }

//      std::cout << std::endl;

//      std::cout << "term2: " << std::endl;

//      for (int i=0; i<30; i++)
//     {
//          std::cout << term2[i] <<  ", " ;
//      }

//       std::cout << std::endl;

     myedge.computeExpBefore(exp_before,t);
     myedge.computeExpAfter(exp_after,t);
     myedge.computeEdgeExponential(t);

      std::cout << "exp_after: " << std::endl;
      for (int i=0; i<30; i++)
     {
          std::cout << exp_after[i] <<  ", " ;
      }

       std::cout << std::endl;

       std::cout << "exp_before: " << std::endl;

       for (int i=0; i<30; i++)
      {
           std::cout << exp_before[i]<<  ", " ;
       }





     std::cout << "edge function:" << std::endl;

    for (int i=0; i<30; i++)
   {
        std::cout << myedge.fullEdge[i] <<  ", " ;
    }
     std::cout << std::endl;



     myedge.computeSimplifiedEdge(t);


     std::cout << "simplified edge function:" << std::endl;

    for (int i=0; i<30; i++)
   {
        std::cout << myedge.edgeFunction[i] <<  ", " ;
    }
     std::cout << std::endl;

     std::cout << "end of constructor" << std::endl;



}

tedgefunction::~tedgefunction()
{
    std::cout << "destroying" << std::endl;
}

void tedgefunction::test_case1()
{

    std::cout << "testing case 1 empty" << std::endl;
}

QTEST_APPLESS_MAIN(tedgefunction)

#include "tst_tedgefunction.moc"
