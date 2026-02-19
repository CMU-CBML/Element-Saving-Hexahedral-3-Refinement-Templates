#include "geometry.h"

void Triangle::ComputeShapeDiameterFunction() {
    std::cout << "compute shape diameter function" << std::endl;
    sDF.resize(f.size());

    const size_t numRays = 25;
    std::vector<std::array<double, 3>> vogelXYWSamples(numRays);
    const double PI = 3.141592653589793238;
    const double goldenAngle = PI * (3.0 - std::sqrt(5.0));
    for (size_t i = 0; i < numRays; ++i) {
        const double r = i * 1.0 / numRays;
        const double theta = i * goldenAngle;

        vogelXYWSamples[i] = { r * std::cos(theta), r * std::sin(theta), 1.0 };
    }
    const double coneAngle = PI * 2.0 / 3.0;
    const double MADFactor = 1.5;
    const double coneSin = std::sin(coneAngle / 2.0);
    const double coneCos = std::cos(coneAngle / 2.0);

#pragma warning(disable: 6993)
#pragma omp parallel for
    for (long long fIdx = 0; fIdx < static_cast<long long>(f.size()); ++fIdx) {
        const auto& face = f[fIdx];
        const auto& v0 = x[face[0]];
        const auto& v1 = x[face[1]];
        const auto& v2 = x[face[2]];
        const std::array<double, 3> faceCenter = { v0[0] / 3 + v1[0] / 3 + v2[0] / 3, v0[1] / 3 + v1[1] / 3 + v2[1] / 3, v0[2] / 3 + v1[2] / 3 + v2[2] / 3 };
        const std::array<double, 3> crossVal = {
            (v1[1] - v0[1]) * (v2[2] - v0[2]) - (v1[2] - v0[2]) * (v2[1] - v0[1]),
            (v1[2] - v0[2]) * (v2[0] - v0[0]) - (v1[0] - v0[0]) * (v2[2] - v0[2]),
            (v1[0] - v0[0]) * (v2[1] - v0[1]) - (v1[1] - v0[1]) * (v2[0] - v0[0]) };
        const double crossValLen = std::sqrt(crossVal[0] * crossVal[0] + crossVal[1] * crossVal[1] + crossVal[2] * crossVal[2]);
        if (crossValLen == 0) {
            std::cerr << "one or more triangle is degenerated";
            std::exit(1);
        }
        const std::array<double, 3> normal = { crossVal[0] / crossValLen, crossVal[1] / crossValLen, crossVal[2] / crossValLen };

        // find two vectors u, v that are perpendicular to normal
        std::array<double, 3> arbitraryVec;
        if (std::abs(normal[0]) > 0.9) {
            arbitraryVec = { 0.0, 1.0, 0.0 };
        }
        else {
            arbitraryVec = { 1.0, 0.0, 0.0 };
        }
        const std::array<double, 3> uRaw = {
            arbitraryVec[1] * normal[2] - arbitraryVec[2] * normal[1],
            arbitraryVec[2] * normal[0] - arbitraryVec[0] * normal[2],
            arbitraryVec[0] * normal[1] - arbitraryVec[1] * normal[0] };
        const double uLen = std::sqrt(uRaw[0] * uRaw[0] + uRaw[1] * uRaw[1] + uRaw[2] * uRaw[2]);
        const std::array<double, 3> u = { uRaw[0] / uLen, uRaw[1] / uLen, uRaw[2] / uLen };
        const std::array<double, 3> v = {
            normal[1] * u[2] - normal[2] * u[1],
            normal[2] * u[0] - normal[0] * u[2],
            normal[0] * u[1] - normal[1] * u[0] };

        // generate ray
        std::vector <std::pair<double, double>> rayResults(numRays);
        for (size_t i = 0; i < 25; ++i) {
            const auto& sample = vogelXYWSamples[i];

            const double diskScale = coneSin;
            const double normScale = coneCos;
            const double dx = sample[0] * diskScale;
            const double dy = sample[1] * diskScale;
            std::array<double, 3> ray = {
                normal[0] * normScale + u[0] * dx + v[0] * dy,
                normal[1] * normScale + u[1] * dx + v[1] * dy,
                normal[2] * normScale + u[2] * dx + v[2] * dy };
            const double rayLen = std::sqrt(ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]);
            ray[0] /= rayLen; ray[1] /= rayLen; ray[2] /= rayLen;

            // get the intersection distance t of ray with tri
            rayResults[i] = std::make_pair(IntersectRayWithTriangles(faceCenter, ray, fIdx), sample[2]);
        }

        // statistic method to get final shape diameter value
        const size_t midIdx = numRays / 2;
        std::nth_element(rayResults.begin(), rayResults.begin() + midIdx, rayResults.end(),
            [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
                return a.first < b.first;
            });
        const double medianSDF = rayResults[midIdx].first;
        std::vector<double> deviations(numRays);
        for (size_t i = 0; i < numRays; ++i) {
            deviations[i] = std::abs(rayResults[i].first - medianSDF);
        }
        std::nth_element(deviations.begin(), deviations.begin() + midIdx, deviations.end());
        const double medianDeviation = deviations[midIdx];
        double threshold = medianDeviation * MADFactor;
        double sumWeightedT = 0;
        double sumWeights = 0;
        for (const auto& p : rayResults) {
            if (std::abs(p.first - medianSDF) <= threshold) {
                sumWeightedT += p.first * p.second;
                sumWeights += p.second;
            }
        }
        
        sDF[fIdx] = sumWeightedT / sumWeights;
    }
    double minSDF = sDF[0];
    size_t minSDFIdx = 0;
    for (size_t i = 1; i < sDF.size(); ++i) {
        if (sDF[i] < minSDF) {
            minSDF = sDF[i];
            minSDFIdx = i;
        }
    }
    std::cout << "minimum shape diameter function value is: " << minSDF << ", at face: " << minSDFIdx << std::endl;
}