#include "Star.hpp"

namespace solar {

Star::Star(std::string name, float radius, float axialTiltDeg,
           float rotationPeriodHours, Color color)
    : CelestialBody(std::move(name), BodyId::Sun, radius, axialTiltDeg,
                    rotationPeriodHours, color) {}

void Star::update(double jd, const IEphemeris& /*ephemeris*/) {
    worldPos_ = Vector3{0.f, 0.f, 0.f}; // звезда — центр системы
    updateRotation(jd);
}

void Star::draw(const RenderContext& ctx) const {
    // Пока без эмиссии/освещения (фаза 7): звезда — просто яркая сфера.
    renderSphere(ctx);
}

} // namespace solar
