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
    // Реальное геоцентрическое направление от провайдера (а.е.), раздутое
    // для видимости, прибавляется к позиции родителя.
    const Vec3 geo = ephemeris.moonGeocentricPosition(jd);
    const Vector3 c = parent_.worldPosition();
    worldPos_ = Vector3{
        c.x + static_cast<float>(geo.x) * distanceExaggeration_,
        c.y + static_cast<float>(geo.y) * distanceExaggeration_,
        c.z + static_cast<float>(geo.z) * distanceExaggeration_};
    updateRotation(jd);
}

void Moon::draw(const RenderContext& ctx) const {
    renderSphere(ctx);
}

} // namespace solar
