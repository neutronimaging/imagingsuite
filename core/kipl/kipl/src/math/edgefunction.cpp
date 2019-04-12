#include "../include/math/edgefunction.h"
using namespace std;


namespace kipl {namespace math {



edgefunction::edgefunction() :
    term3(nullptr),
    term4(nullptr),
    term5(nullptr),
    edgeFunction(nullptr),
    sigma(-0.1),
    alpha(-0.1),
    t0(0.0),
    t(nullptr),
    size(200)
{
        if(t!=nullptr){
            size = static_cast<int>(sizeof(t)/sizeof(*t));
            term3 = new double[size];
            term4 = new double[size];
            term5 = new double[size];
            edgeFunction = new double[size];
        }
}

edgefunction::~edgefunction()
{

    if(term3!=nullptr){
        delete[] term3;
        term3 = nullptr;
    }

    if(term4!=nullptr){
        delete[] term4;
        term4 = nullptr;
    }

    if(term5!=nullptr){
        delete[] term5;
        term5 = nullptr;
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


void edgefunction::computeTerm3(double *fun, double *t)
{

    for (int i=0; i<size; ++i)
    {
        fun[i] = Faddeeva::erfc(-(t[i]-t0)/(sigma*dsqrt2));
    }

}

void edgefunction::computeTerm5(double *fun, double *t)
{

    for (int i=0; i<size; ++i)
    {
        fun[i] = Faddeeva::erfc(-(t[i]-t0)/(sigma*dsqrt2)+sigma/alpha);
    }

}

void edgefunction::computeTerm4(double *fun, double *t){

    for (int i=0; i<size; ++i)
    {
        fun[i] = exp(-((t[i]-t0)/alpha)+((sigma*sigma)/(2*alpha*alpha)));
    }

}

void edgefunction::computeEdge(double *t)
{

    computeTerm4(term4,t);
    computeTerm3(term3,t);
    computeTerm5(term5,t);

    for (int i=0; i<size; ++i)
    {
        edgeFunction[i] = (1-(term3[i]-term4[i]*term5[i]));
    }


}


}}
