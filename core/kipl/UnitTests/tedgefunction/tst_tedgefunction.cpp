#include <QtTest>

// add necessary includes here

//#include <math/edgefunction.h>
//#include <Faddeeva.hh>
#include <iostream>
using namespace std;
//using namespace Faddeeva;

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
    // QSKIP("Testing non-supported features");
    // here I should create the data that I want to use for testing, or something like this,  so for example a vector t of lambda values
//     double *t = new double[30];

//     for (int i=0; i<30; ++i)
//     {
//        t[i] = 3.0+i*(4.0-3.0)/30;
//        std::cout <<t[i] <<  ",  " ;
//     }

//      std::cout << std::endl;


//     double *term1 = new double[30];
//     double *term2 = new double[30];
//     double *term4 = new double[30];
//     double *term3 = new double[30];
//     double *term5 = new double[30];
//     double *edgefunction = new double[30];
//     double *simple_edge = new double[30];



//     kipl::math::edgefunction myedge;
//     myedge.t = t;
//     myedge.t0 = 3.5;
//     myedge.alpha = -0.1;
//     myedge.sigma = -0.1;
//     myedge.a1 = 0.1;
//     myedge.a2 = 1.0;
//     myedge.a5 = -0.1;
//     myedge.a6 = -1.0;


//     myedge.computeTerm3(term3,t);
//     myedge.edgeFunction = edgefunction;
//     myedge.term1 = term1;
//     myedge.term2 = term2;
//     myedge.term3 = term3;
//     myedge.term4 = term4;
//     myedge.term5 = term5;

////     myedge.sigma =0.1;
////     myedge.alpha =0.1;

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



//     myedge.computeTerm1(term1, t);
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


//       myedge.computeEdge(t);


//     std::cout << "edge function:" << std::endl;

//    for (int i=0; i<30; i++)
//   {
//        std::cout << myedge.edgeFunction[i] <<  ", " ;
//    }
//     std::cout << std::endl;



//     myedge.computeSimplifiedEdge(t);


//     std::cout << "simplified edge function:" << std::endl;

//    for (int i=0; i<30; i++)
//   {
//        std::cout << myedge.edgeFunction[i] <<  ", " ;
//    }
//     std::cout << std::endl;

//     std::cout << "end of constructor" << std::endl;



}

tedgefunction::~tedgefunction()
{

}

void tedgefunction::test_case1()
{

    std::cout << "testing case 1 empty" << std::endl;
}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(tedgefunction)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(tedgefunction)
#endif


#include "tst_tedgefunction.moc"
