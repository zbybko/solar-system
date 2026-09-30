#pragma once

// Moon — спутник: положение задаётся ОТНОСИТЕЛЬНО родительского тела.
//
// ООП-приёмы: НАСЛЕДОВАНИЕ + АГРЕГАЦИЯ (луна ссылается на родителя, но не
// владеет им). Демонстрирует, что разные наследники CelestialBody вычисляют
// позицию принципиально по-разному (планета — гелиоцентрически, луна —
// относительно планеты), оставаясь взаимозаменяемыми для клиента.
//
// Направление и расстояние Луны берутся из провайдера эфемерид
// (moonGeocentricPosition), но расстояние домножается на коэффициент
// видимости: настоящие ~0.0026 а.е. на компактной сцене неразличимы.

#include "CelestialBody.hpp"

namespace solar {

class Moon final : public CelestialBody {
public:
    Moon(std::string name, BodyId parentId, const CelestialBody& parent,
         float radius, float distanceExaggeration, Color color);

    void update(double jd, const IEphemeris& ephemeris) override;
    void draw(const RenderContext& ctx) const override;
    BodyKind kind() const override { return BodyKind::Moon; }

private:
    const CelestialBody& parent_;   // агрегация: не владеем родителем
    float distanceExaggeration_;    // во сколько раз раздуть расстояние до родителя
};

} // namespace solar
