#include "CelestialBody.hpp"
#include "SphereMapping.hpp"

#include "rlgl.h"

#include <cmath>
#include <utility>

namespace solar {

CelestialBody::CelestialBody(std::string name, BodyId id, float radius,
                             float axialTiltDeg, float rotationPeriodHours,
                             Color color)
    : name_(std::move(name)),
      id_(id),
      radius_(radius),
      axialTilt_(axialTiltDeg),
      rotationPeriod_(rotationPeriodHours),
      color_(color) {
    // Сфера-заглушка с цветом тела. Если позже загрузить текстуру, она ляжет
    // поверх (fallback на цветную сферу — требование проекта).
    Mesh mesh = GenMeshSphere(radius_, 32, 64);
    for (int i = 0; i < mesh.vertexCount; ++i)
        orientSphereSurface(mesh.vertices + i * 3, mesh.normals + i * 3,
                            mesh.texcoords + i * 2);
    UpdateMeshBuffer(mesh, 0, mesh.vertices, mesh.vertexCount * 3 * sizeof(float), 0);
    UpdateMeshBuffer(mesh, 1, mesh.texcoords, mesh.vertexCount * 2 * sizeof(float), 0);
    UpdateMeshBuffer(mesh, 2, mesh.normals, mesh.vertexCount * 3 * sizeof(float), 0);
    model_ = LoadModelFromMesh(mesh);
    model_.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color_;
}

CelestialBody::~CelestialBody() {
    // RAII: освобождаем GPU-ресурсы. Вызывается, пока ещё жив контекст raylib.
    if (hasTexture_)
        UnloadTexture(texture_);
    UnloadModel(model_);
}

void CelestialBody::loadTexture(const char* path) {
    if (path == nullptr || !FileExists(path))
        return; // остаётся цветная сфера
    Texture2D loaded = LoadTexture(path);
    if (!IsTextureValid(loaded))
        return;
    if (hasTexture_)
        UnloadTexture(texture_);
    texture_ = loaded;
    GenTextureMipmaps(&texture_);
    SetTextureFilter(texture_, TEXTURE_FILTER_TRILINEAR);
    // Clamp at the latitude edges so the south pole never samples the north pole.
    SetTextureWrap(texture_, TEXTURE_WRAP_CLAMP);
    hasTexture_ = true;
    model_.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture_;
    // С текстурой цвет-модулятор делаем белым, чтобы не искажать цвета.
    model_.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
}

void CelestialBody::updateRotation(double jd) {
    if (rotationPeriod_ <= 0.f)
        return;
    const double periodDays = rotationPeriod_ / 24.0;
    const double turns = jd / periodDays;
    rotationAngle_ = static_cast<float>((turns - std::floor(turns)) * 360.0);
}

Vector3 CelestialBody::scaledPosition(const RenderContext& ctx) const {
    return ctx.toWorld(worldPos_);
}

void CelestialBody::renderSphere(const RenderContext& ctx) const {
    const Vector3 p = scaledPosition(ctx);

    rlPushMatrix();
    rlTranslatef(p.x, p.y, p.z);
    rlRotatef(axialTilt_, 0.f, 0.f, 1.f);     // наклон оси вращения
    rlRotatef(rotationAngle_, 0.f, 1.f, 0.f); // суточное вращение
    DrawModel(model_, Vector3{0.f, 0.f, 0.f}, ctx.radiusScale, WHITE);
    rlPopMatrix();
}

} // namespace solar
