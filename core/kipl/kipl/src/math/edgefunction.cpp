#include "../include/math/edgefunction.h"
using namespace std;


namespace kipl {namespace math {



edgefunction::edgefunction() :
    erfc1(nullptr),
    erfc2(nullptr),
    gauss(nullptr),
    edgeFunction(nullptr),
    sigma(-0.1),
    alpha(-0.1),
    t0(0.0),
    t(nullptr),
    size(200)
{
        if(t!=nullptr){
            size = static_cast<int>(sizeof(t)/sizeof(*t));
            erfc1 = new double[size];
            erfc2 = new double[size];
            gauss = new double[size];
            edgeFunction = new double[size];
        }
}

edgefunction::~edgefunction()
{

    if(erfc1!=nullptr){
        delete[] erfc1;
        erfc1 = nullptr;
    }

    if(erfc2!=nullptr){
        delete[] erfc2;
        erfc2 = nullptr;
    }

    if(gauss!=nullptr){
        delete[] gauss;
        gauss = nullptr;
    }

    if(edgeFunction!=nullptr){
        delete[] edgeFunction;
        edgeFunction = nullptr;
    }

    if(t!=nullptr){
        delete[] t;
        t = nullptr;
    }

}


void edgefunction::computeErfc1(double *erfc, double *t)
{

    for (int i=0; i<size; ++i)
    {
        erfc[i] = Faddeeva::erfc(-(t[i]-t0)/(sigma*dsqrt2));
    }

}

void edgefunction::computeErfc2(double *erfc, double *t)
{

    for (int i=0; i<size; ++i)
    {
        erfc[i] = Faddeeva::erfc(-(t[i]-t0)/(sigma*dsqrt2)+sigma/alpha);
    }

}

void edgefunction::computeGauss(double *gauss, double *t){

    for (int i=0; i<size; ++i)
    {
        gauss[i] = exp(-(t[i]-t0)/alpha+(sigma*sigma/(alpha*alpha)));
    }

}

void edgefunction::computeEdge(double *t)
{

    computeGauss(gauss,t);
    computeErfc1(erfc1,t);
    computeErfc2(erfc2,t);

    for (int i=0; i<size; ++i)
    {
        edgeFunction[i] = (1-erfc1[i]-erfc2[i]*gauss[i]);
    }


}


}}
