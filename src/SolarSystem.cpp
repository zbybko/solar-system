#include "SolarSystem.hpp"

#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"

#include <utility>

namespace solar {

SolarSystem SolarSystem::createRealistic(std::unique_ptr<IEphemeris> ephemeris) {
    SolarSystem system;
    system.ephemeris_ = std::move(ephemeris);
    auto& b = system.bodies_;
    const IEphemeris& eph = *system.ephemeris_;

    // Параметры компактные/наглядные: радиусы не в реальном масштабе (иначе
    // планеты были бы невидимы рядом с Солнцем). Осевые наклоны и периоды
    // вращения — настоящие. Позиции планет берутся из провайдера в реальных
    // а.е.; масштаб расстояний задаёт RenderContext.

    // Создать планету, посчитать её орбиту и добавить в систему; вернуть
    // сырой указатель (например, чтобы привязать луну или включить кольца).
    auto addPlanet = [&](const char* name, BodyId id, float radius, float tilt,
                         float rotHours, double orbitDays, Color color) -> Planet* {
        auto planet = std::make_unique<Planet>(name, id, radius, tilt, rotHours,
                                               orbitDays, color);
        planet->rebuildOrbit(eph);
        Planet* raw = planet.get();
        b.push_back(std::move(planet));
        return raw;
    };

    b.push_back(std::make_unique<Star>(
        "Солнце", 1.1f, 7.25f, 609.12f, Color{255, 220, 90, 255}));

    addPlanet("Меркурий", BodyId::Mercury, 0.25f, 0.03f, 1407.6f, 87.97, Color{150, 140, 130, 255});
    addPlanet("Венера", BodyId::Venus, 0.38f, 177.4f, 5832.5f, 224.7, Color{210, 180, 120, 255});
    Planet* earth = addPlanet("Земля", BodyId::Earth, 0.40f, 23.44f, 23.934f, 365.25, Color{70, 130, 220, 255});
    addPlanet("Марс", BodyId::Mars, 0.30f, 25.19f, 24.62f, 686.98, Color{200, 90, 60, 255});
    addPlanet("Юпитер", BodyId::Jupiter, 0.85f, 3.13f, 9.925f, 4332.6, Color{200, 160, 120, 255});

    Planet* saturn = addPlanet("Сатурн", BodyId::Saturn, 0.72f, 26.73f, 10.66f, 10759.2, Color{220, 200, 150, 255});
    saturn->enableRings(1.0f, 1.7f, Color{210, 200, 170, 150});

    addPlanet("Уран", BodyId::Uranus, 0.55f, 97.77f, 17.24f, 30688.5, Color{150, 220, 220, 255});
    addPlanet("Нептун", BodyId::Neptune, 0.52f, 28.32f, 16.11f, 60182.0, Color{70, 100, 220, 255});

    // Луна — спутник Земли. Расстояние раздуто (×50) для видимости.
    b.push_back(std::make_unique<Moon>(
        "Луна", BodyId::Earth, *earth, 0.12f, 50.0f, Color{185, 185, 185, 255}));

    return system;
}

void SolarSystem::setEphemeris(std::unique_ptr<IEphemeris> ephemeris) {
    ephemeris_ = std::move(ephemeris);
    for (auto& body : bodies_)
        body->rebuildOrbit(*ephemeris_); // орбиты пересчитываются под провайдера
}

void SolarSystem::update(double julianDay) {
    for (auto& body : bodies_)
        body->update(julianDay, *ephemeris_); // полиморфизм
}

void SolarSystem::draw(const RenderContext& ctx) const {
    for (const auto& body : bodies_)
        body->draw(ctx); // полиморфизм
}

void SolarSystem::drawOrbits(const RenderContext& ctx) const {
    for (const auto& body : bodies_)
        body->drawOrbit(ctx);
}

CelestialBody* SolarSystem::pickBody(const Ray& ray, const RenderContext& ctx) const {
    CelestialBody* nearest = nullptr;
    float nearestDist = 0.f;
    for (const auto& body : bodies_) {
        const Vector3 center = body->renderPosition(ctx);
        const float radius = body->radius() * ctx.radiusScale;
        const RayCollision hit = GetRayCollisionSphere(ray, center, radius);
        if (hit.hit && (nearest == nullptr || hit.distance < nearestDist)) {
            nearest = body.get();
            nearestDist = hit.distance;
        }
    }
    return nearest;
}

} // namespace solar
