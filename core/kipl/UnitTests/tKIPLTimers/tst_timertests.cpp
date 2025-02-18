#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include <typeinfo>

#include <profile/Timer.h>

// the function f() does some time-consuming work
void f()
{
    volatile double d = 0;
    for(int n=0; n<10000; ++n)
       for(int m=0; m<10000; ++m)
           d += d*n*m;
}

class TimerTests : public QObject
{
    Q_OBJECT

public:
    TimerTests();
    ~TimerTests();

private slots:
    void test_case1();
    void test_BasicTiming();

private:
    void test_ThreadedTiming();

};

TimerTests::TimerTests()
{

}

TimerTests::~TimerTests()
{

}

void TimerTests::test_case1()
{
    std::clock_t c_start = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();
    std::thread t1(f);
    std::thread t2(f); // f() is called on two threads
    t1.join();
    t2.join();
    std::clock_t c_end = std::clock();
    std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

    std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
            << " ms\n";
}

void TimerTests::test_BasicTiming()
{
    kipl::profile::Timer timer;
    std::clock_t c_start = std::clock();
    timer.Tic();
    f();
    std::clock_t c_end = std::clock();
    double dt=timer.Toc();

    qDebug() << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << dt;
    QVERIFY(std::fabs(1000.0 * (c_end-c_start) / CLOCKS_PER_SEC - dt)/dt <0.05);
    std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(c_end-c_start).count()
            << " ms\n";
}

void TimerTests::test_ThreadedTiming()
{
    kipl::profile::Timer timer;

    std::clock_t c_start = std::clock();
    timer.Tic();
    std::thread t1(f);
    std::thread t2(f); // f() is called on two threads
    t1.join();
    t2.join();
    std::clock_t c_end = std::clock();
    double t=timer.Toc();

#ifdef _MSC_VER
    double rtime = std::fabs(1000.0 * (c_end-c_start) / CLOCKS_PER_SEC/t-1);
#else
    double rtime = std::fabs(1000.0 * (c_end-c_start) / CLOCKS_PER_SEC/t-2);
#endif
    QVERIFY(rtime <0.05);
    QVERIFY((timer.elapsedTime()-t)/t<0.1);

    QVERIFY(std::fabs(1-timer.elapsedTime(kipl::profile::Timer::nanoSeconds)*1e-6/t)<0.01);
    QVERIFY(std::fabs(1-timer.elapsedTime(kipl::profile::Timer::microSeconds)*1e-3/t)<0.01);
    QVERIFY(std::fabs(1-timer.elapsedTime(kipl::profile::Timer::milliSeconds)/t)<0.01);
    QVERIFY(std::fabs(1-timer.elapsedTime(kipl::profile::Timer::seconds)*1e3/t)<0.01);

//    std::cout<<timer.elapsedTime()<<"ms\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::nanoSeconds)<<"ns\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::microSeconds)<<"ns\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::milliSeconds)<<"ms\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::seconds)<<"s\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::minutes)<<"min\n";
//    std::cout<<timer.elapsedTime(kipl::profile::Timer::hours)<<"h\n";

}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(TimerTests)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(TimerTests)
#endif


#include "tst_timertests.moc"


