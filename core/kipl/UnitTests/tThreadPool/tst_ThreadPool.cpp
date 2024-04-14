//<LICENSE>

#include <QtTest>

#include <vector>
#include <string>
#include <thread>
#include <cmath>

#include <utilities/threadpool.h>

#include "dummyprocessor.h"

class TestThreadPool : public QObject
{
    Q_OBJECT

public:
    TestThreadPool();
    ~TestThreadPool();

private slots:
    void test_Constructor();
    void test_FillTaskList();
    void test_Processor();
    void test_ProcessorSingle();


private:
    std::string dataPath;

};

TestThreadPool::TestThreadPool()
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    // kipl::strings::filenames::CheckPathSlashes(dataPath,true);


}

TestThreadPool::~TestThreadPool()
{

}

void TestThreadPool::test_Constructor()
{
    kipl::utilities::ThreadPool pool(std::thread::hardware_concurrency());
    QCOMPARE(pool.pool_size(),       std::thread::hardware_concurrency());
    QCOMPARE(pool.queue_size(),      0UL);
    QCOMPARE(pool.tasks_processed(), 0UL);
    QCOMPARE(pool.tasks_submitted(), 0UL);

    kipl::utilities::ThreadPool pool2(3UL);
    QCOMPARE(pool2.pool_size(),       3UL);
    QCOMPARE(pool2.queue_size(),      0UL);
    QCOMPARE(pool2.tasks_processed(), 0UL);
    QCOMPARE(pool2.tasks_submitted(), 0UL);
}

void TestThreadPool::test_FillTaskList()
{
    kipl::utilities::ThreadPool pool(std::thread::hardware_concurrency());

    // logger.set_loglevel(kipl::logging::Logger::LogVerbose);

    std::ostringstream msg;
    size_t N=200;
    for (int i = 0; i < N; ++i) 
    {
        int b = i*2;
        pool.enqueue([i,b] {
            float sum=0.0f;

            for (size_t cnt=0; cnt<100000; ++cnt)
                sum+=sqrt(static_cast<float>(cnt));
        });
    }

    pool.barrier(); // Wait until all tasks have been processed
    QCOMPARE(pool.tasks_processed(),N);
    QCOMPARE(pool.tasks_submitted(),N);
}

void TestThreadPool::test_Processor()
{
    size_t N=std::thread::hardware_concurrency();
    DummyProcessor processor(N);

    std::vector<float> data(1000000*N);
    std::vector<float> result;
    std::iota(data.begin(),data.end(),0);

    QBENCHMARK{
        result = processor.process(data);
    }

    QCOMPARE(data.size(),result.size());

    for (size_t i=0; i<data.size(); ++i)
        QCOMPARE(result[i],std::floor(sqrt(data[i])*10000));
}

void TestThreadPool::test_ProcessorSingle()
{
    size_t N=std::thread::hardware_concurrency();
    DummyProcessor processor(1);

    std::vector<float> data(1000000*N);
    std::vector<float> result;
    std::iota(data.begin(),data.end(),0);

    QBENCHMARK{
        result = processor.process(data);
    }
    
    QCOMPARE(data.size(),result.size());

    for (size_t i=0; i<data.size(); ++i)
        QCOMPARE(result[i],std::floor(sqrt(data[i])*10000));
}

QTEST_APPLESS_MAIN(TestThreadPool)

#include "tst_ThreadPool.moc"
