#pragma once

// Planet — планета: положение берётся из провайдера эфемерид (IEphemeris).
//
// ООП-приёмы: НАСЛЕДОВАНИЕ + ПОЛИМОРФИЗМ (через update обращается к IEphemeris,
// не зная конкретного провайдера — связка с паттерном «Стратегия»). Опционально
// владеет кольцами (как Сатурн) — пример агрегации видимого атрибута.

#include "CelestialBody.hpp"

#include <vector>

namespace solar {

class Planet final : public CelestialBody {
public:
    Planet(std::string name, BodyId id, float radius, float axialTiltDeg,
           float rotationPeriodHours, double orbitalPeriodDays, Color color);
    ~Planet() override;

    // Включить плоское кольцо в экваториальной плоскости планеты (мировые
    // единицы радиусов). По умолчанию у планеты колец нет.
    void enableRings(float innerRadius, float outerRadius, Color color);

    void update(double jd, const IEphemeris& ephemeris) override;
    void draw(const RenderContext& ctx) const override;
    void drawOrbit(const RenderContext& ctx) const override;
    void rebuildOrbit(const IEphemeris& ephemeris) override;
    BodyKind kind() const override { return BodyKind::Planet; }

private:
    double orbitalPeriodDays_;
    bool hasRings_{false};
    float ringInner_{0.f};
    float ringOuter_{0.f};
    Color ringColor_{};
    Texture2D ringTexture_{};
    std::vector<Vector3> orbitAu_; // точки орбиты в а.е.
};

} // namespace solar
