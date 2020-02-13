#include <QtTest>

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
    std::chrono::steady_clock::time_point t_end = std::chrono::high_resolution_clock::now();

    std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
            << " ms\n";
}

void TimerTests::test_BasicTiming()
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    std::clock_t c_start = std::clock();
    TimePoint t_start = std::chrono::high_resolution_clock::now();
    std::thread t1(f);
    std::thread t2(f); // f() is called on two threads
    t1.join();
    t2.join();
    std::clock_t c_end = std::clock();
    TimePoint t_end = std::chrono::high_resolution_clock::now();

    std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
            << " ms\n";
}

void TimerTests::test_ThreadedTiming()
{
    kipl::profile::Timer timer;
    timer.Tic();
    std::thread t1(f);
    std::thread t2(f); // f() is called on two threads
    t1.join();
    t2.join();
    double t=timer.Toc();
    std::cout<<t<<"ms\n";
}

QTEST_APPLESS_MAIN(TimerTests)

#include "tst_timertests.moc"


