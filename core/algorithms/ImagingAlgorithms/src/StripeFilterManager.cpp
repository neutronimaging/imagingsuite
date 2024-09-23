//<LICENSE>

#include <sstream>
#include <thread>

#include "../include/StripeFilterManager.h"
#include "../include/ImagingException.h"
#include "../include/sinogram.h"

namespace ImagingAlgorithms 
{

StripeFilterManager::StripeFilterManager(   const std::vector<size_t> & dims, 
                                            const std::string &wname, 
                                            int scale, 
                                            float sigma) :
    logger("StripeFilterManager"),
    pool(std::thread::hardware_concurrency())
{
    for (size_t i=0; i<pool.pool_size(); ++i)
        filters.emplace_back(dims, wname, scale, sigma);
}

// StripeFilterManager::StripeFilterManager(   const std::vector<int> &dims, 
//                                             const string &wname, 
//                                             int   scale, 
//                                             float sigma):
//     logger("StripeFilterManager"),
//     pool(std::thread::hardware_concurrency()),
//     filters(pool.pool_size())
// {
//     for (auto & flt: filters)
//         flt=StripeFilter(dims, wname, scale, sigma);
// }

StripeFilterManager::~StripeFilterManager()
{}

std::vector<int> StripeFilterManager::dims()
{
    return filters[0].dims();
}

std::string StripeFilterManager::waveletName()
{
    return filters[0].waveletName();
}

int StripeFilterManager::decompositionLevels()
{
    return filters[0].decompositionLevels();
}

float StripeFilterManager::sigma()
{
    return filters[0].sigma();
}

std::vector<float> StripeFilterManager::filterWindow(int level)
{
    return filters[0].filterWindow(level);
}
    
void StripeFilterManager::configure(const std::vector<int> &dims, const std::string &wname, int scale, float sigma)
{
    for (auto &f : filters) 
    {
        f.configure(dims, wname, scale, sigma);
    }

}

// void StripeFilterManager::setNumberOfThreads(int N)
// {
//     pool.setNumberOfThreads(N);
// }

int  StripeFilterManager::numberOfThreads()
{
    return pool.pool_size();
}

void StripeFilterManager::process(  kipl::base::TImage<float,2> &img, 
                                    eStripeFilterOperation       op)
{
    filters[0].process(img, op);
}

void StripeFilterManager::process(  kipl::base::TImage<float,3> &img, 
                                    eStripeFilterOperation       op)
{
    if (filters.size() < pool.pool_size())
    {
        std::ostringstream msg;
        msg << "There are more threads than filters.";
        throw ImagingException(msg.str(), __FILE__, __LINE__);
    }  

    size_t nSinograms = img.Size(2); 
    size_t nBlockSize = nSinograms / pool.pool_size();
    int nRemainder    = nSinograms % pool.pool_size();

    size_t nStart = 0;
    size_t nEnd   = nBlockSize + (nRemainder-- > 0 ? 1 : 0);

    for (size_t i = 0; i<pool.pool_size(); i++)
    {
        pool.enqueue([this, &img, nStart, nEnd, i, op]() 
        {
            std::vector<size_t> dims(filters[i].dims().begin(),filters[i].dims().end());
            kipl::base::TImage<float,2> sino(dims);

            for (size_t j = nStart; j < nEnd; j++)
            {    
                ExtractSinogram(img, sino, j);
                filters[i].checkDims(sino.dims());
                filters[i].process(sino,op);
                InsertSinogram(sino,img,j);
            }
        });

        nStart = nEnd;
        nEnd   = nStart + nBlockSize + (nRemainder-- > 0 ? 1 : 0);
    }
    pool.barrier();
}


}