//<LICENSE>
#ifndef KMEANS_H
#define KMEANS_H
#include <vector>
#include <logging/logger.h>

class KMeans
{
protected:
    kipl::logging::Logger logger;

public:
    KMeans();

    const std::vector<std::vector<float>> & centroids();
    
    const size_t exec(const std::vector<std::vector<float> &> &data, 
                            std::vector<int>                  &result,
                      const std::vector<std::vector<float>>   &init_centroids = {});

    const size_t exec(const std::vector<float * >             &data, 
                            size_t                             N,
                            float                             *result, 
                      const std::vector<std::vector<float>> &init_centroids = {});

protected:
    template<typename T>
    void  initialize_result(T begin, T end);
    template<typename T>
    void  updateClasses();
    template<typename T>
    void  updateCentroids();

    float euclidean_distance(const std::vector<float> &data, float c);

    std::vector<std::vector<float> > m_centroids;
};
#endif

#include "kmeans.hpp"