
#include <utilities/threadpool.h>
#include <vector>

class DummyProcessor 
{
    public:
        DummyProcessor(size_t nThreads);
        std::vector<float> process(std::vector<float> &data);

    protected:
        kipl::utilities::ThreadPool pool;

        void subprocess(std::vector<float> &in, std::vector<float> &out,size_t begin, size_t end);
};