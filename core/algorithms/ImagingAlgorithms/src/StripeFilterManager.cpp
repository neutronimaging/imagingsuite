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
    pool(std::thread::hardware_concurrency()),
    filters(pool.pool_size(), nullptr)
{
    for (size_t i=0; i<pool.pool_size(); ++i)
        filters[i] = new StripeFilter(dims, wname, scale, sigma);
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
{
    for (auto &f : filters)
        delete f;
}

std::vector<int> StripeFilterManager::dims()
{
    if (filters[0] == nullptr)
        throw ImagingException("The filter vector is not initialized", __FILE__, __LINE__);

    return filters[0]->dims();
}

std::string StripeFilterManager::waveletName()
{
    return filters[0]->waveletName();
}

int StripeFilterManager::decompositionLevels()
{
    return filters[0]->decompositionLevels();
}

float StripeFilterManager::sigma()
{
    return filters[0]->sigma();
}

std::vector<float> StripeFilterManager::filterWindow(int level)
{
    return filters[0]->filterWindow(level);
}
    
void StripeFilterManager::configure(const std::vector<int> &dims, const std::string &wname, int scale, float sigma)
{
    for (auto &f : filters) 
    {
        f->configure(dims, wname, scale, sigma);
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
    filters[0]->process(img, op);
}


void StripeFilterManager::process(  kipl::base::TImage<float,3> &img, 
                                    eStripeFilterOperation       op,
                                    bool                         multiThreaded)
{
    if (multiThreaded)
        processMultiThreaded(img, op);
    else
        processSingleThreaded(img, op);
}


void StripeFilterManager::processMultiThreaded(  kipl::base::TImage<float,3> &img, 
                                    eStripeFilterOperation       op)
{
    if (filters.size() < pool.pool_size())
    {
        std::ostringstream msg;
        msg << "There are more threads than filters.";
        throw ImagingException(msg.str(), __FILE__, __LINE__);
    }  

    size_t nSinograms = img.Size(1); 
    size_t nBlockSize = nSinograms / pool.pool_size();
    int nRemainder    = nSinograms % pool.pool_size();

    size_t nStart = 0;
    size_t nEnd   = nBlockSize + (nRemainder-- > 0 ? 1 : 0);

    // std::ostringstream msg;
    // msg << "Processing parameters start:"<<nStart
    // <<", nEnd:"<<nEnd
    // <<", size:"<<img.Size(1)
    // <<", nSinograms:"<<nSinograms
    // <<", nBlockSize:"<<nBlockSize
    // <<", nRemainder:"<<nRemainder;
    // logger.message(msg.str());

    for (size_t i = 0; i<pool.pool_size(); i++)
    {
        pool.enqueue([this, &img, nStart, nEnd, i, op]() 
        {
            std::vector<size_t> dims = { static_cast<size_t>(this->filters[i]->dims()[0]),
                                         static_cast<size_t>(this->filters[i]->dims()[1])};
            kipl::base::TImage<float,2> sino(dims);

            if ((nStart > img.Size(1)) || (nEnd > img.Size(1)))
            {
                std::ostringstream msg;
                msg << "The sinogram index is out of bounds. start:"<<nStart
                    <<", nEnd:"<<nEnd
                    <<", size:"<<img.Size(1);
                throw ImagingException(msg.str(), __FILE__, __LINE__);
            }
            for (size_t j = nStart; j < nEnd; j++)
            {    
                ExtractSinogram(img, sino, j);
                this->filters[i]->checkDims(sino.dims());
                this->filters[i]->process(sino,op);
                InsertSinogram(sino,img,j);
            }
        });

        nStart = nEnd;
        nEnd   = nStart + nBlockSize + (nRemainder-- > 0 ? 1 : 0);
    }
    pool.barrier();
}

void StripeFilterManager::processSingleThreaded(  kipl::base::TImage<float,3> &img, 
                                                  eStripeFilterOperation       op)
{
    auto  &filter = *filters[0];
    for (size_t i = 0; i<img.Size(1); i++)
    {
        std::vector<size_t> dims = { static_cast<size_t>(filter.dims()[0]),
                                         static_cast<size_t>(filter.dims()[1])};
        kipl::base::TImage<float,2> sino(dims);

        ExtractSinogram(img, sino, i);
        filter.checkDims(sino.dims());
        filter.process(sino,op);
        InsertSinogram(sino,img,i);
            
    }

}
} // namespace ImagingAlgorithms