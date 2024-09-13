//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_csv.h>

#include <LagCorrection.h>

// Test class for the NLCSC algorithm using QTest
class TestLagCorrection : public QObject {
    Q_OBJECT

private slots:
    void testExposureDependentParams();
    void testDeconvolutionStep();
    void testDeconvolveLag();

private:
    bool compareVectors(const std::vector<float>& v1, const std::vector<float>& v2, float tolerance = 1e-6);
};

// Helper function to compare vectors (tolerant to floating-point precision differences)
bool TestLagCorrection::compareVectors(const std::vector<float>& v1, const std::vector<float>& v2, float tolerance) {
    if (v1.size() != v2.size()) return false;
    for (size_t i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] - v2[i]) > tolerance) return false;
    }
    return true;
}

void TestLagCorrection::testExposureDependentParams() {
    // Define base lag rates and coefficients for testing
    std::vector<float> baseLagRates = {2.5e-3f, 2.1e-2f, 1.6e-1f, 7.6e-1f};
    std::vector<float> initialLagCoeffs = {7.1e-6f, 1.1e-4f, 1.7e-3f, 1.8e-2f};

    NLCSC nlscc(baseLagRates, initialLagCoeffs, exampleExposureCurve, 15.0f);

    float exposure = 50.0f;
    auto [bn_x, an_x] = nlscc.getExposureDependentParams(exposure);

    std::vector<float> expected_bn_x = exampleExposureCurve(exposure, initialLagCoeffs);
    std::vector<float> expected_an_x = {0.02875f, 0.021f, 0.16032f, 0.776f};  // Adjust these expected values

    QVERIFY(compareVectors(bn_x, expected_bn_x));
    QVERIFY(compareVectors(an_x, expected_an_x));
}

void TestLagCorrection::testDeconvolutionStep() {
    // Define base lag rates and coefficients
    std::vector<float> baseLagRates = {2.5e-3f, 2.1e-2f, 1.6e-1f, 7.6e-1f};
    std::vector<float> initialLagCoeffs = {7.1e-6f, 1.1e-4f, 1.7e-3f, 1.8e-2f};

    NLCSC nlscc(baseLagRates, initialLagCoeffs, exampleExposureCurve, 15.0f);

    float y_k = 0.5f;
    std::vector<float> bn_x = {1.1e-4f, 1.2e-3f, 1.3e-2f, 1.4e-1f};
    std::vector<float> an_x = {0.005f, 0.010f, 0.015f, 0.020f};
    std::vector<float> Sn_k = {0.1f, 0.2f, 0.3f, 0.4f};

    auto [x_k, S_star_k] = nlscc.deconvolutionStep(y_k, bn_x, an_x, Sn_k);

    float expected_x_k = 0.485f;
    std::vector<float> expected_S_star_k = {0.0991f, 0.198f, 0.2955f, 0.392f};

    QVERIFY(std::abs(x_k - expected_x_k) < 1e-6);
    QVERIFY(compareVectors(S_star_k, expected_S_star_k));
}

void TestLagCorrection::testDeconvolveLag() {
    // Define base lag rates and coefficients
    std::vector<float> baseLagRates = {2.5e-3f, 2.1e-2f, 1.6e-1f, 7.6e-1f};
    std::vector<float> initialLagCoeffs = {7.1e-6f, 1.1e-4f, 1.7e-3f, 1.8e-2f};

    NLCSC nlscc(baseLagRates, initialLagCoeffs, exampleExposureCurve, 15.0f);

    std::vector<float> y = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> exposure = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};

    std::vector<float> correctedOutput = nlscc.deconvolveLag(y, exposure);

    std::vector<float> expected_corrected_output = {0.095f, 0.19f, 0.285f, 0.38f, 0.475f};

    QVERIFY(compareVectors(correctedOutput, expected_corrected_output));
}

// Main function to run the tests
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
QTEST_APPLESS_MAIN(TestLagCorrection)
#pragma clang diagnostic pop

#include "tstLagCorrection.moc"

