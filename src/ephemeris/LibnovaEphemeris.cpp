#include "ephemeris/LibnovaEphemeris.hpp"

#include <cmath>

// Заголовки libnova. Каждая планета — отдельный заголовок с функцией
// ln_get_<planet>_helio_coords(double JD, struct ln_helio_posn*).
#include <libnova/ln_types.h>
#include <libnova/earth.h>
#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/lunar.h>

namespace solar {

namespace {

constexpr double kDegToRad = 3.14159265358979323846 / 180.0;

// Перевод сферических гелиоэклиптических координат libnova (долгота L и широта B
// в градусах, радиус-вектор R в а.е.) в декартов вектор в той же системе (а.е.).
Vec3 sphericalToCartesian(const ln_helio_posn& p) {
    const double lon = p.L * kDegToRad;
    const double lat = p.B * kDegToRad;
    const double r = p.R;
    const double cosLat = std::cos(lat);
    return Vec3{
        r * cosLat * std::cos(lon),
        r * cosLat * std::sin(lon),
        r * std::sin(lat)};
}

} // namespace

Vec3 LibnovaEphemeris::heliocentricPosition(BodyId body, double julianDay) const {
    if (body == BodyId::Sun)
        return Vec3{}; // Солнце — центр гелиоцентрической системы.

    ln_helio_posn p{};
    switch (body) {
        case BodyId::Mercury: ln_get_mercury_helio_coords(julianDay, &p); break;
        case BodyId::Venus:   ln_get_venus_helio_coords(julianDay, &p);   break;
        case BodyId::Earth:   ln_get_earth_helio_coords(julianDay, &p);   break;
        case BodyId::Mars:    ln_get_mars_helio_coords(julianDay, &p);    break;
        case BodyId::Jupiter: ln_get_jupiter_helio_coords(julianDay, &p); break;
        case BodyId::Saturn:  ln_get_saturn_helio_coords(julianDay, &p);  break;
        case BodyId::Uranus:  ln_get_uranus_helio_coords(julianDay, &p);  break;
        case BodyId::Neptune: ln_get_neptune_helio_coords(julianDay, &p); break;
        case BodyId::Sun:     break; // обработано выше
    }
    return sphericalToCartesian(p);
}

Vec3 LibnovaEphemeris::moonGeocentricPosition(double julianDay) const {
    constexpr double kKmPerAu = 149597870.7;
    ln_lnlat_posn ecl{};
    ln_get_lunar_ecl_coords(julianDay, &ecl, /*precision*/ 0.0);
    const double distAu = ln_get_lunar_earth_dist(julianDay) / kKmPerAu;
    const double lon = ecl.lng * kDegToRad;
    const double lat = ecl.lat * kDegToRad;
    const double cosLat = std::cos(lat);
    return Vec3{
        distAu * cosLat * std::cos(lon),
        distAu * cosLat * std::sin(lon),
        distAu * std::sin(lat)};
}

} // namespace solar
