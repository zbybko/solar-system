#include "Planet.hpp"

#include "rlgl.h"

#include <cmath>

namespace solar {

namespace {

// Плоское кольцо-аннулюс в локальной плоскости x-z (y=0). Рисуется в
// immediate-режиме rlgl — никаких отдельных GPU-ресурсов держать не нужно.
// Треугольники выводятся в обе стороны, чтобы кольцо было видно сверху и снизу.
void drawRing(float inner, float outer, Color c) {
    constexpr int segments = 72;
    constexpr double kTwoPi = 2.0 * 3.14159265358979323846;

    rlBegin(RL_TRIANGLES);
    rlColor4ub(c.r, c.g, c.b, c.a);
    for (int i = 0; i < segments; ++i) {
        const double a0 = kTwoPi * i / segments;
        const double a1 = kTwoPi * (i + 1) / segments;
        const float c0 = static_cast<float>(std::cos(a0)), s0 = static_cast<float>(std::sin(a0));
        const float c1 = static_cast<float>(std::cos(a1)), s1 = static_cast<float>(std::sin(a1));

        const Vector3 iv0{c0 * inner, 0.f, s0 * inner};
        const Vector3 ov0{c0 * outer, 0.f, s0 * outer};
        const Vector3 iv1{c1 * inner, 0.f, s1 * inner};
        const Vector3 ov1{c1 * outer, 0.f, s1 * outer};

        // Лицевая сторона.
        rlVertex3f(iv0.x, iv0.y, iv0.z); rlVertex3f(ov0.x, ov0.y, ov0.z); rlVertex3f(ov1.x, ov1.y, ov1.z);
        rlVertex3f(iv0.x, iv0.y, iv0.z); rlVertex3f(ov1.x, ov1.y, ov1.z); rlVertex3f(iv1.x, iv1.y, iv1.z);
        // Обратная сторона (обратный обход).
        rlVertex3f(iv0.x, iv0.y, iv0.z); rlVertex3f(ov1.x, ov1.y, ov1.z); rlVertex3f(ov0.x, ov0.y, ov0.z);
        rlVertex3f(iv0.x, iv0.y, iv0.z); rlVertex3f(iv1.x, iv1.y, iv1.z); rlVertex3f(ov1.x, ov1.y, ov1.z);
    }
    rlEnd();
}

} // namespace

Planet::Planet(std::string name, BodyId id, float radius, float axialTiltDeg,
               float rotationPeriodHours, double orbitalPeriodDays, Color color)
    : CelestialBody(std::move(name), id, radius, axialTiltDeg,
                    rotationPeriodHours, color),
      orbitalPeriodDays_(orbitalPeriodDays) {}

void Planet::enableRings(float innerRadius, float outerRadius, Color color) {
    hasRings_ = true;
    ringInner_ = innerRadius;
    ringOuter_ = outerRadius;
    ringColor_ = color;
}

void Planet::rebuildOrbit(const IEphemeris& ephemeris) {
    // Точки орбиты: позиции из провайдера за один период от эпохи J2000.
    // Орбита не зависит от времени и масштаба — пересчитываем при смене
    // провайдера.
    constexpr double kJ2000 = 2451545.0;
    constexpr int kSamples = 180;
    orbitAu_.clear();
    orbitAu_.reserve(kSamples);
    for (int i = 0; i < kSamples; ++i) {
        const double jd = kJ2000 + orbitalPeriodDays_ * static_cast<double>(i) / kSamples;
        const Vec3 p = ephemeris.heliocentricPosition(id_, jd);
        orbitAu_.push_back(Vector3{static_cast<float>(p.x),
                                   static_cast<float>(p.y),
                                   static_cast<float>(p.z)});
    }
}

void Planet::drawOrbit(const RenderContext& ctx) const {
    if (orbitAu_.size() < 2)
        return;
    const Color c{color_.r, color_.g, color_.b, 90}; // полупрозрачная линия цвета тела
    const size_t n = orbitAu_.size();
    for (size_t i = 0; i < n; ++i) {
        const Vector3 a = ctx.toWorld(orbitAu_[i]);
        const Vector3 b = ctx.toWorld(orbitAu_[(i + 1) % n]); // замыкаем петлю
        DrawLine3D(a, b, c);
    }
}

void Planet::update(double jd, const IEphemeris& ephemeris) {
    // Полиморфный вызов: планета не знает, какой провайдер активен.
    const Vec3 pos = ephemeris.heliocentricPosition(id_, jd);
    worldPos_ = Vector3{static_cast<float>(pos.x),
                        static_cast<float>(pos.y),
                        static_cast<float>(pos.z)};
    updateRotation(jd);
}

void Planet::draw(const RenderContext& ctx) const {
    renderSphere(ctx);

    if (hasRings_) {
        const Vector3 p = scaledPosition(ctx);
        rlPushMatrix();
        rlTranslatef(p.x, p.y, p.z);
        rlRotatef(axialTilt_, 0.f, 0.f, 1.f); // кольцо в экваториальной плоскости
        drawRing(ringInner_ * ctx.radiusScale, ringOuter_ * ctx.radiusScale, ringColor_);
        rlPopMatrix();
    }
}

} // namespace solar
