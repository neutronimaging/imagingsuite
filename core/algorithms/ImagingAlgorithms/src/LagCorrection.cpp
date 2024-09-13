#include "../include/LagCorrection.h"
#include <cmath>

// Constructor
NLCSC::NLCSC(const std::vector<float>& baseLagRates, const std::vector<float>& initialLagCoeffs,
             std::function<std::vector<float>(float, const std::vector<float>&)> exposureCurve,
             float frameRate)
    : baseLagRates_(baseLagRates), initialLagCoeffs_(initialLagCoeffs), exposureCurve_(exposureCurve), frameRate_(frameRate) {}

// Deconvolve lag function
std::vector<float> NLCSC::deconvolveLag(const std::vector<float>& y, const std::vector<float>& exposure) {
    size_t N = baseLagRates_.size();
    std::vector<float> Sn_k(N, 0.0f);
    std::vector<float> qn_k(N, 0.0f);
    std::vector<float> correctedOutput(y.size(), 0.0f);

    for (size_t k = 0; k < y.size(); ++k) {
        auto [bn_x, an_x] = getExposureDependentParams(exposure[k]);
        auto [x_k, S_star_k] = deconvolutionStep(y[k], bn_x, an_x, Sn_k);
        correctedOutput[k] = x_k;
        std::tie(Sn_k, qn_k) = updateStoredCharge(S_star_k, x_k, bn_x, an_x);
    }

    return correctedOutput;
}

// Get exposure-dependent parameters
std::pair<std::vector<float>, std::vector<float>> NLCSC::getExposureDependentParams(float exposure) {
    std::vector<float> bn_x = exposureCurve_(exposure, initialLagCoeffs_);
    std::vector<float> an_x = getExposureDependentRates(exposure);
    return {bn_x, an_x};
}

// Perform the deconvolution step
std::pair<float, std::vector<float>> NLCSC::deconvolutionStep(float y_k, const std::vector<float>& bn_x,
                                                              const std::vector<float>& an_x, const std::vector<float>& Sn_k) {
    std::vector<float> S_star_k(bn_x.size());

    // std::transform(Sn_k.begin(), Sn_k.end(), bn_x.begin(), an_x.begin(), S_star_k.begin(),
    //                [this](float Sn, float bn, float an) { return getS_star(Sn, bn, an); });

    auto it_Sn = Sn_k.begin();
    auto it_bn = bn_x.begin();
    auto it_an = an_x.begin();
    auto it_S_star = S_star_k.begin();

    while (it_Sn != Sn_k.end() && it_bn != bn_x.end() && it_an != an_x.end() && it_S_star != S_star_k.end()) {
        *it_S_star = getS_star(*it_Sn, *it_bn, *it_an);
        ++it_Sn;
        ++it_bn;
        ++it_an;
        ++it_S_star;
    }

    float x_k = (y_k - dotProduct(bn_x, S_star_k)) / sum(bn_x);
    return {x_k, S_star_k};
}

// Update the stored charge
std::pair<std::vector<float>, std::vector<float>> NLCSC::updateStoredCharge(const std::vector<float>& S_star_k,
                                                                            float x_k, const std::vector<float>& bn_x,
                                                                            const std::vector<float>& an_x) {
    std::vector<float> Sn_k_plus_1(S_star_k.size());
    std::vector<float> qn_k_plus_1(S_star_k.size());

    for (size_t n = 0; n < Sn_k_plus_1.size(); ++n) {
        Sn_k_plus_1[n] = x_k + S_star_k[n] * std::exp(-an_x[n]);
        qn_k_plus_1[n] = getStoredCharge(Sn_k_plus_1[n], bn_x[n], an_x[n]);
    }

    return {Sn_k_plus_1, qn_k_plus_1};
}

// Calculate S*_n,k
float NLCSC::getS_star(float Sn_k, float bn_x, float an_x) {
    return Sn_k * (bn_x * std::exp(-an_x)) / (1 - std::exp(-an_x));
}

// Calculate the stored charge
float NLCSC::getStoredCharge(float Sn_k, float bn_x, float an_x) {
    return Sn_k * (bn_x * std::exp(-an_x)) / (1 - std::exp(-an_x));
}

// Get exposure-dependent lag rates
std::vector<float> NLCSC::getExposureDependentRates(float exposure) {
    std::vector<float> an_x(baseLagRates_.size(), 0.0f);
    float c1 = 0.02f;  // Example coefficient
    float c2 = 0.01f;  // Example coefficient

    std::transform(baseLagRates_.begin(), baseLagRates_.end(), an_x.begin(),
                   [c1, c2, exposure](float base_an) { return base_an + c1 * (1 - std::exp(-c2 * exposure)); });

    return an_x;
}

// Utility function to calculate dot product
float NLCSC::dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
}

// Utility function to sum a vector
float NLCSC::sum(const std::vector<float>& v) {
    return std::accumulate(v.begin(), v.end(), 0.0f);
}

// Example exposure curve function
std::vector<float> exampleExposureCurve(float exposure, const std::vector<float>& initialLagCoeffs) {
    std::vector<float> result(initialLagCoeffs.size());
    std::transform(initialLagCoeffs.begin(), initialLagCoeffs.end(), result.begin(),
                   [exposure](float coeff) { return coeff * (1 + 0.1f * exposure); });
    return result;
}
