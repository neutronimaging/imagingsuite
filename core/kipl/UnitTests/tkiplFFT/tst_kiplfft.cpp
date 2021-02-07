#include <QtTest>
#include <QDebug>
// add necessary includes here

#include <vector>
#include <cmath>
#include <thread>
#include <fft/fftbase.h>
#include <fft/zeropadding.h>
#include <profile/Timer.h>

void doubleThread(size_t i, std::vector<double> *fdata);
void floatThread(size_t i, std::vector<float> *fdata);

static inline bool qFuzzyCompare(double p1, double p2, double delta)
{
    return (qAbs(p1 - p2) <= delta );
}

class kiplFFT : public QObject
{
    Q_OBJECT

public:
    kiplFFT();
    ~kiplFFT();

private slots:
    void test_FFTdoubleCTor();
    void test_FFTfloatCTor();

    void test_FFTdouble();
    void test_FFTfloat();

    void test_FFTdoubleInThreads();
    void test_FFTfloatInThreads();

private:

    std::vector<double> ddata;
    std::vector<float>  fdata;

};

kiplFFT::kiplFFT() :
    ddata(8192,0.0),
    fdata(8192,0.0f)
{
    for (size_t i=0; i<ddata.size(); ++i)
    {
        ddata[i]=sin(i*0.01);
        fdata[i]=sin(i*0.01f);
    }
}

kiplFFT::~kiplFFT()
{

}

void kiplFFT::test_FFTdoubleCTor()
{
    kipl::math::fft::FFTBase fft({1024UL});
}

void kiplFFT::test_FFTfloatCTor()
{
    kipl::math::fft::FFTBaseFloat fft({1024UL});
}

void kiplFFT::test_FFTdouble()
{
    kipl::profile::Timer timer;

    kipl::math::fft::FFTBase fft({ddata.size()});

    std::vector<std::complex<double> > resC(ddata.size(),0.0);

    fft(ddata.data(),resC.data());

    std::vector<double> resR(ddata.size(),0.0);

    fft(resC.data(),resR.data());

    timer.Toc();

    qDebug() << timer.elapsedTime();
    std::ostringstream msg;
    for (size_t i=0; i<ddata.size(); ++i)
    {
        msg.str(""); msg<<"i="<<i<<" res="<<resR[i]/(resR.size())<<", ddata="<<ddata[i];
        QVERIFY2(qFuzzyCompare(resR[i]/(resR.size()),ddata[i],1e-9),msg.str().c_str());
    }
}

void kiplFFT::test_FFTfloat()
{
    kipl::profile::Timer timer;

    timer.Tic();
    kipl::math::fft::FFTBaseFloat fft({fdata.size()});

    std::vector<std::complex<float> > resC(fdata.size(),0.0);

    fft(fdata.data(),resC.data());
    std::vector<float> resR(fdata.size(),0.0);

    fft(resC.data(),resR.data());

    timer.Toc();

    qDebug() << timer.elapsedTime();

    std::ostringstream msg;
    for (size_t i=0; i<ddata.size(); ++i)
    {
        msg.str(""); msg<<"i="<<i<<" res="<<resR[i]/(resR.size())<<", ddata="<<ddata[i];
        QVERIFY2(qFuzzyCompare(resR[i]/(resR.size()),fdata[i],1e-5),msg.str().c_str());
    }

}

void kiplFFT::test_FFTdoubleInThreads()
{
    kipl::profile::Timer timer;

    timer.Tic();
    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;

    for(size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        // spawn threads
        threads.push_back(std::thread([=] { doubleThread(i,&ddata); }));
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    timer.Toc();

    qDebug() << timer.elapsedTime();
}

void kiplFFT::test_FFTfloatInThreads()
{
    kipl::profile::Timer timer;

    timer.Tic();
    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;

    for(size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        // spawn threads
        threads.push_back(std::thread([=] { floatThread(i,&fdata); }));
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    timer.Toc();

    qDebug() << timer.elapsedTime();
}

void doubleThread(size_t i, std::vector<double> *ddata)
{
    qDebug() << "Thread"<<i<<"started";

    kipl::math::fft::FFTBase fft({ddata->size()});

    std::vector<std::complex<double> > res(ddata->size(),0.0);

    fft(ddata->data(),res.data());
    qDebug() << "Thread"<<i<<"made it!";
}

void floatThread(size_t i, std::vector<float> *fdata)
{
    qDebug() << "Thread"<<i<<"started";

    kipl::math::fft::FFTBaseFloat fft({fdata->size()});

    std::vector<std::complex<float> > res(fdata->size(),0.0);

    fft(fdata->data(),res.data());
    qDebug() << "Thread"<<i<<"made it!";
}

QTEST_APPLESS_MAIN(kiplFFT)

#include "tst_kiplfft.moc"
