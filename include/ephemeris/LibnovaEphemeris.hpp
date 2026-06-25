#pragma once

// LibnovaEphemeris — провайдер эфемерид поверх библиотеки libnova.
//
// ООП-приёмы: КОНКРЕТНАЯ СТРАТЕГИЯ + инкапсуляция (обёртка прячет C-API libnova
// за чистым C++-интерфейсом IEphemeris). Это провайдер по умолчанию: тяжёлая
// математика VSOP87 живёт в библиотеке, а наш код лишь переводит сферические
// гелиоэклиптические координаты libnova в декартов Vec3.

#include "ephemeris/IEphemeris.hpp"

namespace solar {

class LibnovaEphemeris final : public IEphemeris {
public:
    Vec3 heliocentricPosition(BodyId body, double julianDay) const override;
    Vec3 moonGeocentricPosition(double julianDay) const override;
    const char* providerName() const override { return "libnova (VSOP87)"; }
};

} // namespace solar
