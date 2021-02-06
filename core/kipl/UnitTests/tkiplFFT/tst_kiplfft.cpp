#include <QtTest>

// add necessary includes here

#include <vector>
#include <fft/fftbase.h>
#include <fft/zeropadding.h>

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

};

kiplFFT::kiplFFT()
{

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

}

void kiplFFT::test_FFTfloat()
{

}

void kiplFFT::test_FFTdoubleInThreads()
{

}

void kiplFFT::test_FFTfloatInThreads()
{

}

QTEST_APPLESS_MAIN(kiplFFT)

#include "tst_kiplfft.moc"
