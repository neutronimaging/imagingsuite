#include <QtTest>

// add necessary includes here

#include <math/edgefunction.h>
#include <Faddeeva.hh>
#include <iostream>
#include <chrono>
#include <ctime>

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
    void compare_erfc();

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

void tedgefunction::compare_erfc()
{
    unsigned long size = 30;
    double *t = new double[size];

     for (int i=0; i<size; ++i)
     {
        t[i] = 3.0+i*(5.0-3.0)/size;
     }

     double t0 = 4.06667;
     double sigma = 0.0053;

    auto t1 = std::chrono::high_resolution_clock::now();
    double fun;

    for (int i=0; i<100000000; ++i)
    {
        // call Faddeeva erfc
        fun = Faddeeva::erfc(-(t[10]-t0)/(sigma*dsqrt2));
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    qDebug() << "duration of 100000000 Fadeevva erfc " << duration;


     t1 = std::chrono::high_resolution_clock::now();


    for (int i=0; i<100000000; ++i)
    {
        // call std erfc
         fun = std::erfc(-(t[10]-t0)/(sigma*dsqrt2));

    }
     t2 = std::chrono::high_resolution_clock::now();

     duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    qDebug() << "duration of 100000000 std erfc " << duration;

}

QTEST_APPLESS_MAIN(tedgefunction)

#include "tst_tedgefunction.moc"
