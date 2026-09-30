#include "UIManager.hpp"
#include "SimulationClock.hpp"
#include "CameraController.hpp"
#include "CelestialBody.hpp"
#include "imgui.h"
#include <cmath>

namespace solar {
namespace {
Text nameKey(const CelestialBody& body) {
    if (body.kind() == BodyKind::Moon) return Text::Moon;
    switch (body.id()) {
        case BodyId::Sun: return Text::Sun;
        case BodyId::Mercury: return Text::Mercury;
        case BodyId::Venus: return Text::Venus;
        case BodyId::Earth: return Text::Earth;
        case BodyId::Mars: return Text::Mars;
        case BodyId::Jupiter: return Text::Jupiter;
        case BodyId::Saturn: return Text::Saturn;
        case BodyId::Uranus: return Text::Uranus;
        case BodyId::Neptune: return Text::Neptune;
    }
    return Text::Sun;
}
Text kindKey(BodyKind kind) {
    switch (kind) {
        case BodyKind::Star: return Text::StarType;
        case BodyKind::Planet: return Text::PlanetType;
        case BodyKind::Moon: return Text::MoonType;
    }
    return Text::PlanetType;
}
} // namespace

void UIManager::draw(SimulationClock& clock, CameraController& camera,
                     CelestialBody*& selected) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(390, 590), ImGuiCond_FirstUseEver);
    ImGui::Begin(tr(Text::Controls));
    ImGui::TextUnformatted(tr(Text::LanguageLabel));
    int language = static_cast<int>(language_);
    const char* languages[] = {"English", "Deutsch", "Русский"};
    if (ImGui::Combo("##language", &language, languages, IM_ARRAYSIZE(languages))) {
        language_ = static_cast<Language>(language);
        SetWindowTitle(tr(Text::AppTitle));
    }
    ImGui::Separator();
    ImGui::TextUnformatted(tr(Text::Provider));
#ifdef HAVE_LIBNOVA
    const char* providers[] = {"libnova (VSOP87)", tr(Text::Kepler)};
#else
    const char* providers[] = {tr(Text::Kepler)};
#endif
    ImGui::Combo("##provider", &provider_, providers, IM_ARRAYSIZE(providers));
    ImGui::Separator();
    const bool orbital = camera.mode() == CameraController::Mode::Orbital;
    ImGui::Text("%s: %s", tr(Text::Camera), tr(orbital ? Text::Orbital : Text::Free));
    if (ImGui::Button(tr(orbital ? Text::SwitchFree : Text::SwitchOrbit))) camera.toggleMode();
    if (ImGui::Button(tr(Text::ResetView))) {
        selected = nullptr;
        camera.focusOn(Vector3{0.f, 0.f, 0.f}, 7.0f);
    }
    ImGui::TextWrapped("%s", tr(Text::Instructions));
    ImGui::Separator();
    ImGui::TextUnformatted(tr(Text::Display));
    ImGui::RadioButton(tr(Text::Compact), &scaleMode_, 0);
    ImGui::SameLine();
    ImGui::RadioButton(tr(Text::Real), &scaleMode_, 1);
    ImGui::Checkbox(tr(Text::Orbits), &showOrbits_);
    ImGui::SameLine();
    ImGui::Checkbox(tr(Text::Grid), &showGrid_);
    ImGui::SetNextItemWidth(150);
    ImGui::SliderFloat(tr(Text::Ambient), &ambient_, 0.0f, 0.5f, "%.2f");
    ImGui::Separator();
    ImGui::TextUnformatted(tr(Text::Time));
    bool paused = clock.isPaused();
    if (ImGui::Checkbox(tr(Text::Pause), &paused)) clock.setPaused(paused);
    float speed = static_cast<float>(clock.speed());
    ImGui::SetNextItemWidth(150);
    if (ImGui::SliderFloat(tr(Text::Speed), &speed, 0.0f, 100.0f, "%.1f")) clock.setSpeed(speed);
    if (ImGui::Button(tr(Text::ResetTime))) clock.reset();
    ImGui::Text("JD = %.4f", clock.julianDay());
    ImGui::Separator();
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::End();
    drawInfoPanel(selected);
}

void UIManager::drawInfoPanel(const CelestialBody* selected) const {
    if (!selected) return;
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 350.f, 10.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin(tr(Text::BodyInfo));
    ImGui::TextUnformatted(tr(nameKey(*selected)));
    ImGui::TextWrapped("%s: %s", tr(Text::Type), tr(kindKey(selected->kind())));
    ImGui::Separator();
    const Vector3 p = selected->worldPosition();
    const float dist = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    ImGui::Text("%s: %.2f", tr(Text::Radius), selected->radius());
    ImGui::Text("%s: %.2f°", tr(Text::Tilt), selected->axialTilt());
    ImGui::Text("%s: %.2f", tr(Text::Rotation), selected->rotationPeriodHours());
    ImGui::Separator();
    ImGui::TextWrapped("%s:", tr(Text::Position));
    ImGui::Text("  x = %+.4f", p.x);
    ImGui::Text("  y = %+.4f", p.y);
    ImGui::Text("  z = %+.4f", p.z);
    ImGui::TextWrapped("%s: %.4f", tr(Text::Distance), dist);
    ImGui::End();
}
} // namespace solar
