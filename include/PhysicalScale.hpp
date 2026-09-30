#pragma once
#include "Localization.hpp"
#include "ephemeris/IEphemeris.hpp"

namespace solar {
inline constexpr double astronomicalUnitKm = 149597870.7;
// Spherical mean radii (NASA/JPL); nominal solar radius.
inline constexpr double meanRadiusKm(BodyId id, BodyKind kind) {
    if (kind == BodyKind::Moon) return 1737.4;
    switch (id) {
        case BodyId::Sun: return 695700.0;
        case BodyId::Mercury: return 2439.4;
        case BodyId::Venus: return 6051.8;
        case BodyId::Earth: return 6371.0084;
        case BodyId::Mars: return 3389.5;
        case BodyId::Jupiter: return 69911.0;
        case BodyId::Saturn: return 58232.0;
        case BodyId::Uranus: return 25362.0;
        case BodyId::Neptune: return 24622.0;
    }
    return 0.0;
}
inline constexpr float physicalRenderRadius(double km, float unitsPerAU) {
    return static_cast<float>(km / astronomicalUnitKm * unitsPerAU);
}
}
