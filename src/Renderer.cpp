#include "Renderer.hpp"

#include "SolarSystem.hpp"
#include "AssetPath.hpp"

#include <string>

namespace solar {

Renderer::Renderer() {
#ifdef __EMSCRIPTEN__
    const std::string vs = assetPath("shaders/lighting-web.vs");
    const std::string fs = assetPath("shaders/lighting-web.fs");
#else
    const std::string vs = assetPath("shaders/lighting.vs");
    const std::string fs = assetPath("shaders/lighting.fs");
#endif
    shader_ = LoadShader(vs.c_str(), fs.c_str());

    // Позицию наблюдателя raylib умеет подставлять сам по стандартному имени.
    shader_.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader_, "viewPos");
    locSunPos_ = GetShaderLocation(shader_, "sunPos");
    locAmbient_ = GetShaderLocation(shader_, "ambient");

    stars_.generate(1600);
}

Renderer::~Renderer() {
    UnloadShader(shader_); // RAII
}

void Renderer::applyLighting(SolarSystem& system) {
    for (const auto& body : system.bodies()) {
        if (body->isLit())
            body->setShader(shader_);
    }
}

void Renderer::updateLighting(Vector3 sunWorldPos, Vector3 cameraPos, float ambient) {
    SetShaderValue(shader_, locSunPos_, &sunWorldPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader_, locAmbient_, &ambient, SHADER_UNIFORM_FLOAT);
    if (shader_.locs[SHADER_LOC_VECTOR_VIEW] != -1)
        SetShaderValue(shader_, shader_.locs[SHADER_LOC_VECTOR_VIEW],
                       &cameraPos, SHADER_UNIFORM_VEC3);
}

} // namespace solar
