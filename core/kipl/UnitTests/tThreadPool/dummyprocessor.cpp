#include <cmath>
#include "dummyprocessor.h"

DummyProcessor::DummyProcessor(size_t nThreads) :
    pool(nThreads)
{

}

std::vector<float> DummyProcessor::process(std::vector<float> &data)
{
    std::vector<float> result(data.size(),0.0f);

    auto N = data.size();
    auto blocksize = N/pool.pool_size();

    auto p = this;
    for (size_t i=0; i<N; i+=blocksize)
    {
        size_t begin = i;
        size_t end   = begin+blocksize;

        pool.enqueue(
            [p,&data,&result,begin,end]{
                    p->subprocess(data,result,begin,end);
                    // for (size_t i=begin; i<end; ++i)
                    //     result[i]=data[i]*data[i];
                }
            );
    }

    pool.barrier();

    return result;
}

void DummyProcessor::subprocess(std::vector<float> &in, std::vector<float> &out,size_t begin, size_t end)
{
    for (size_t i=begin; i<end; ++i)
        out[i]=std::floor(sqrt(in[i])*1000.0f);
}
