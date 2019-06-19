#include "../include/math/edgefunction.h"
using namespace std;


namespace kipl {namespace math {



edgefunction::edgefunction() :
    term1(nullptr),
    term2(nullptr),
    term3(nullptr),
    term4(nullptr),
    term5(nullptr),
    line_before(nullptr),
    line_after(nullptr),
    exp_before(nullptr),
    exp_after(nullptr),
    edgeFunction(nullptr),
    fullEdge(nullptr),
    sigma(-0.1),
    alpha(-0.1),
    t0(0.0),      
    a1(0),
    a2(0),
    a5(0),
    a6(0),
    t(nullptr),
    size(200)
{
//        if(t!=nullptr){
            size = static_cast<int>(sizeof(t)/sizeof(*t));
            term1 = new double[size];
            term2 = new double[size];
            term3 = new double[size];
            term4 = new double[size];
            term5 = new double[size];
            line_before = new double[size];
            line_after = new double[size];
            exp_after = new double[size];
            exp_before = new double[size];
            edgeFunction = new double[size];
            fullEdge = new double[size];
//        }
}

edgefunction::~edgefunction()
{
    if(term1!=nullptr){
        delete[] term1;
        term1 = nullptr;
    }

    if(term2!=nullptr){
        delete[] term2;
        term2 = nullptr;
    }

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

    if(line_before!=nullptr){
        delete[] line_before;
        line_before = nullptr;
    }

    if(line_after!=nullptr){
        delete[] line_after;
        line_after = nullptr;
    }

    if(exp_before!=nullptr){
        delete[] exp_before;
        exp_before = nullptr;
    }

    if(exp_after!=nullptr){
        delete[] exp_after;
        exp_after = nullptr;
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

void edgefunction::computeTerm1(double *fun, double *t)
{

    for (int i=0; i<size; ++i)
    {
        fun[i] = a1+a2*(t[i]-a6);
    }

}

void edgefunction::computeTerm2(double *fun, double *t)
{
    for (int i=0; i<size;++i) {
        fun[i] = ((a5-a2)/2)*(t[i]-a6);
    }

}

void edgefunction::computeLineBefore(double *fun, double *t)
{
    for (int i=0; i<size; ++i)
    {
        fun[i] = a5+a6*t[i];
    }
}

void edgefunction::computeLineAfter(double *fun, double *t)
{
    for (int i=0; i<size; ++i)
    {
        fun[i] = a1+a2*t[i];
    }

}

void edgefunction::computeExpBefore(double *fun, double *t)
{
    for (int i=0; i<size; ++i)
    {
        fun[i] = exp(-(a1+a2*t[i]));
    }

}

void edgefunction::computeExpAfter(double *fun, double *t)
{
    for (int i=0; i<size; ++i)
    {
        fun[i] = exp(-(a1+a2*t[i]));
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

void edgefunction::computeSimplifiedEdge(double *t)
{

    computeTerm4(term4,t);
    computeTerm3(term3,t);
    computeTerm5(term5,t);

    for (int i=0; i<size; ++i)
    {
        edgeFunction[i] = (0.5*(term3[i]-term4[i]*term5[i]));
    }


}

void edgefunction::computeEdge(double *t)
{
//    computeTerm1(term1,t);
//    computeTerm2(term2,t);
//    computeTerm3(term3,t);
//    computeTerm4(term4,t);
//    computeTerm5(term5,t);

    computeLineBefore(line_before,t);
    computeLineAfter(line_after,t);

    computeSimplifiedEdge(t);

    for (int i=0; i<size; ++i)
    {
//        edgeFunction[i] = term1[i]+term2[i]*(1-(term3[i]-term4[i]*term5[i]));
        fullEdge[i] = line_after[i]*edgeFunction[i]+line_before[i]*(1.0-edgeFunction[i]);
    }



}

}}
