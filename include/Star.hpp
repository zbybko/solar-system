#pragma once

// Star — звезда (Солнце): покоится в центре системы, источник света.
//
// ООП-приём: НАСЛЕДОВАНИЕ + полиморфная реализация update/draw. Позиция всегда
// (0,0,0): гелиоцентрическая система отсчёта привязана к звезде.

#include "CelestialBody.hpp"

namespace solar {

class Star final : public CelestialBody {
public:
    Star(std::string name, float radius, float axialTiltDeg,
         float rotationPeriodHours, Color color);

    void update(double jd, const IEphemeris& ephemeris) override;
    void draw(const RenderContext& ctx) const override;

    bool isLit() const override { return false; } // звезда светит сама
    BodyKind kind() const override { return BodyKind::Star; }
};

} // namespace solar
