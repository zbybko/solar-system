#include "Moon.hpp"
#include "AssetPath.hpp"

namespace solar {

// Сидерический месяц ≈ 27.32 сут; Луна синхронна, поэтому период вращения
// равен орбитальному.
static constexpr float kSiderealMonthHours = 27.32f * 24.f;

Moon::Moon(std::string name, BodyId parentId, const CelestialBody& parent,
           float radius, float distanceExaggeration, Color color)
    : CelestialBody(std::move(name), parentId, radius, /*axialTilt*/ 6.68f,
                    kSiderealMonthHours, color),
      parent_(parent),
      distanceExaggeration_(distanceExaggeration) {
    loadTexture(assetPath("textures/moon.jpg").c_str());
}

void Moon::update(double jd, const IEphemeris& ephemeris) {
    // Keep physical coordinates; exaggerate separation only in compact mode.
    const Vec3 geo = ephemeris.moonGeocentricPosition(jd);
    const Vector3 c = parent_.worldPosition();
    geocentricPosition_ = {static_cast<float>(geo.x), static_cast<float>(geo.y),
                          static_cast<float>(geo.z)};
    worldPos_ = Vector3{
        c.x + geocentricPosition_.x,
        c.y + geocentricPosition_.y,
        c.z + geocentricPosition_.z};
    updateRotation(jd);
}

Vector3 Moon::scaledPosition(const RenderContext& ctx) const {
    if (ctx.scale == RenderContext::Scale::Real) return ctx.toWorld(worldPos_);
    const Vector3 c = parent_.worldPosition();
    return ctx.toWorld({c.x + geocentricPosition_.x * distanceExaggeration_,
                        c.y + geocentricPosition_.y * distanceExaggeration_,
                        c.z + geocentricPosition_.z * distanceExaggeration_});
}

void Moon::draw(const RenderContext& ctx) const {
    renderSphere(ctx);
}

} // namespace solar
