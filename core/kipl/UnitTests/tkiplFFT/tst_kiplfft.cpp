#include <QtTest>

// add necessary includes here
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

    void test_FFTinThreads();

};

kiplFFT::kiplFFT()
{

}

kiplFFT::~kiplFFT()
{

}

void kiplFFT::test_FFTdoubleCTor()
{
  //  kipl::math::fft::FFTBase fft();
}

void kiplFFT::test_FFTfloatCTor()
{

}

void kiplFFT::test_FFTinThreads()
{

}

QTEST_APPLESS_MAIN(kiplFFT)

#include "tst_kiplfft.moc"
