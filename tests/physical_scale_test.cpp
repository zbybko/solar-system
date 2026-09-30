#include "PhysicalScale.hpp"
#include <cmath>
#include <iostream>

int main() {
    using namespace solar;
    const double earth = meanRadiusKm(BodyId::Earth, BodyKind::Planet);
    const double sun = meanRadiusKm(BodyId::Sun, BodyKind::Star);
    const double moon = meanRadiusKm(BodyId::Earth, BodyKind::Moon);
    for (float scale : {1.f, 4.f, 100.f}) {
        const float earthRadius = physicalRenderRadius(earth, scale);
        if (std::abs(earthRadius / scale - earth / astronomicalUnitKm) > 1e-10 ||
            std::abs(physicalRenderRadius(sun, scale) / earthRadius - sun / earth) > 1e-4 ||
            std::abs(physicalRenderRadius(moon, scale) / earthRadius - moon / earth) > 1e-6) {
            std::cerr << "Body sizes and AU distances do not share the same scale\n";
            return 1;
        }
        for (int id = 0; id <= static_cast<int>(BodyId::Neptune); ++id)
            if (physicalRenderRadius(meanRadiusKm(static_cast<BodyId>(id), BodyKind::Planet), scale) <= 0)
                return 1;
    }
    return 0;
}
