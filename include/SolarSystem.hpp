#pragma once

// SolarSystem — владелец всех небесных тел и активного провайдера эфемерид.
//
// ООП-приёмы: КОМПОЗИЦИЯ (владеет телами через unique_ptr и провайдером),
// ФАБРИЧНЫЙ МЕТОД (createRealistic собирает реалистичную систему), а также
// полиморфная диспетчеризация update/draw по базовому указателю. Знает лишь
// интерфейс IEphemeris — конкретная стратегия подменяема (паттерн «Стратегия»).

#include "CelestialBody.hpp"
#include "RenderContext.hpp"
#include "ephemeris/IEphemeris.hpp"

#include <memory>
#include <vector>

namespace solar {

class SolarSystem {
public:
    // Фабрика: Солнце + 8 планет + Луна + кольца Сатурна с заданным провайдером.
    static SolarSystem createRealistic(std::unique_ptr<IEphemeris> ephemeris);

    // Сменить провайдера эфемерид (демонстрация паттерна «Стратегия» из UI).
    // Орбиты пересчитываются под нового провайдера; тела сохраняются.
    void setEphemeris(std::unique_ptr<IEphemeris> ephemeris);

    void update(double julianDay);
    void draw(const RenderContext& ctx) const;
    void drawOrbits(const RenderContext& ctx) const;

    // Подбор тела лучом мыши: ближайшее тело, чью ограничивающую сферу
    // пересекает луч, иначе nullptr. Указатель невладеющий — система остаётся
    // владельцем тела.
    CelestialBody* pickBody(const Ray& ray, const RenderContext& ctx) const;

    const std::vector<std::unique_ptr<CelestialBody>>& bodies() const { return bodies_; }
    const IEphemeris& ephemeris() const { return *ephemeris_; }

private:
    SolarSystem() = default; // создаётся только через фабрику

    std::unique_ptr<IEphemeris> ephemeris_;
    std::vector<std::unique_ptr<CelestialBody>> bodies_;
};

} // namespace solar
