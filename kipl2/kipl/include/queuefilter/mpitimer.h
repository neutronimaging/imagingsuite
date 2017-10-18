//<LICENCE>

#ifndef MPITIMER_H
#define MPITIMER_H

/**
@author Anders Kaestner
*/
class mpitimer 
{
public:
    mpitimer();

    double tic();
    double toc();
    double reset() {temp_timer=timer; timer=0; return temp_timer;}
    double operator()() {return timer;}
private:
	double timer;
	double temp_timer;

};

#endif
