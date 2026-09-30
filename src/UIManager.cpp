#include "UIManager.hpp"
#include "SimulationClock.hpp"
#include "CameraController.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "imgui.h"
#include <cmath>
#include <cstring>
#include <algorithm>

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
                     CelestialBody*& selected, const SolarSystem& system) {
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    const float density = std::clamp(width / 1280.f, 1.f, 2.f);
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 14;
    style.FrameRounding = 7;
    style.PopupRounding = 10;
    style.GrabRounding = 7;
    style.WindowPadding = ImVec2(18 * density, 14 * density);
    style.FramePadding = ImVec2(10 * density, 7 * density);
    style.ItemSpacing = ImVec2(10 * density, 10 * density);
    style.WindowBorderSize = 1;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(.035f, .055f, .085f, .93f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(.045f, .065f, .095f, .98f);
    style.Colors[ImGuiCol_Border] = ImVec4(.35f, .48f, .58f, .22f);
    style.Colors[ImGuiCol_Text] = ImVec4(.90f, .94f, .97f, 1);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(.52f, .62f, .70f, 1);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(.10f, .15f, .20f, 1);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(.15f, .23f, .29f, 1);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(.19f, .30f, .34f, 1);
    style.Colors[ImGuiCol_Button] = ImVec4(.10f, .17f, .22f, 1);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(.18f, .31f, .35f, 1);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(.23f, .40f, .41f, 1);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(.58f, .87f, .78f, 1);
    style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_CheckMark];
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(.72f, 1, .89f, 1);
    style.Colors[ImGuiCol_Header] = ImVec4(.12f, .22f, .27f, 1);
    style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_ButtonHovered];
    const bool narrow = width < 1100 * density;
    const float topHeight = (narrow ? 112.f : 76.f) * density;
    const auto panelFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(16 * density, 16 * density));
    ImGui::SetNextWindowSize(ImVec2(width - 32 * density, topHeight));
    ImGui::Begin("##atlasBar", nullptr, panelFlags);
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(.58f, .87f, .78f, 1), "SOLAR ATLAS");
    ImGui::SameLine(165 * density);
    ImGui::SetNextItemWidth(130 * density);
    int language = static_cast<int>(language_);
    const char* languages[] = {"English", "Deutsch", "Русский"};
    if (ImGui::Combo("##language", &language, languages, IM_ARRAYSIZE(languages))) {
        language_ = static_cast<Language>(language);
        SetWindowTitle(tr(Text::AppTitle));
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(180 * density);
    if (ImGui::BeginCombo("##selectBody", selected ? tr(nameKey(*selected)) : tr(Text::Explore))) {
        for (const auto& body : system.bodies()) {
            if (ImGui::Selectable(tr(nameKey(*body)), selected == body.get())) {
                selected = body.get();
                RenderContext ctx;
                ctx.scale = scale();
                camera.focusOn(selected->renderPosition(ctx), selected->renderRadius(ctx));
            }
        }
        ImGui::EndCombo();
    }
    if (!narrow) ImGui::SameLine();
    ImGui::RadioButton(tr(Text::Compact), &scaleMode_, 0);
    ImGui::SameLine();
    ImGui::RadioButton(tr(Text::Real), &scaleMode_, 1);
    ImGui::SameLine();
    if (ImGui::Button(tr(Text::Overview))) {
        selected = nullptr;
        camera.focusOn(Vector3{0, 0, 0}, 7);
    }
    ImGui::SameLine();
    if (ImGui::Button(tr(Text::Settings))) settingsOpen_ = !settingsOpen_;
    ImGui::End();

    if (settingsOpen_) {
        ImGui::SetNextWindowPos(ImVec2(width - 356 * density, topHeight + 30 * density));
        ImGui::SetNextWindowSize(ImVec2(340 * density, height - topHeight - 160 * density));
        ImGui::Begin("##settingsPanel", nullptr, panelFlags);
        ImGui::TextUnformatted(tr(Text::Display));
        ImGui::Separator();
        ImGui::Checkbox(tr(Text::Orbits), &showOrbits_);
        ImGui::Checkbox(tr(Text::Grid), &showGrid_);
        ImGui::Checkbox(tr(Text::SelectionWireframe), &showSelectionWireframe_);
        const char* ambientLabel = tr(Text::Ambient);
        ImGui::TextUnformatted(ambientLabel, std::strstr(ambientLabel, "###"));
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("##ambient", &ambient_, 0, .5f, "%.2f");
        if (scaleMode_ == 1) ImGui::TextWrapped("%s", tr(Text::RealScaleHint));
        ImGui::Spacing();
        const bool orbital = camera.mode() == CameraController::Mode::Orbital;
        if (ImGui::Button(tr(orbital ? Text::SwitchFree : Text::SwitchOrbit))) camera.toggleMode();
        if (ImGui::CollapsingHeader(tr(Text::Help))) ImGui::TextWrapped("%s", tr(Text::Instructions));
        if (ImGui::CollapsingHeader(tr(Text::Diagnostics))) {
            ImGui::Text("FPS: %d", GetFPS());
            ImGui::Text("JD: %.4f", clock.julianDay());
            ImGui::TextWrapped("%s", tr(Text::Provider));
#ifdef HAVE_LIBNOVA
            const char* providers[] = {"libnova (VSOP87)", tr(Text::Kepler)};
#else
            const char* providers[] = {tr(Text::Kepler)};
#endif
            ImGui::SetNextItemWidth(-1);
            ImGui::Combo("##provider", &provider_, providers, IM_ARRAYSIZE(providers));
        }
        ImGui::End();
    }

    ImGui::SetNextWindowPos(ImVec2(16 * density, height - 100 * density));
    ImGui::SetNextWindowSize(ImVec2(width - 32 * density, 84 * density));
    ImGui::Begin("##playback", nullptr, panelFlags);
    if (ImGui::Button(tr(clock.isPaused() ? Text::Play : Text::Pause))) clock.setPaused(!clock.isPaused());
    ImGui::SameLine();
    float speed = static_cast<float>(clock.speed());
    ImGui::SetNextItemWidth((width > 900 * density ? 340 : 180) * density);
    if (ImGui::SliderFloat(tr(Text::Speed), &speed, .001f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic)) clock.setSpeed(speed);
    ImGui::SameLine();
    if (ImGui::Button(tr(Text::ResetTime))) clock.reset();
    ImGui::End();
    drawInfoPanel(selected);
}

void UIManager::drawInfoPanel(const CelestialBody* selected) const {
    if (!selected) return;
    const float width = ImGui::GetIO().DisplaySize.x;
    const float density = std::clamp(width / 1280.f, 1.f, 2.f);
    const float top = (width < 1100 * density ? 142.f : 106.f) * density;
    ImGui::SetNextWindowPos(ImVec2(16 * density, top));
    ImGui::SetNextWindowSize(ImVec2(300 * density, 0));
    ImGui::Begin("##bodyInfo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted(tr(nameKey(*selected)));
    ImGui::TextWrapped("%s: %s", tr(Text::Type), tr(kindKey(selected->kind())));
    ImGui::Separator();
    const Vector3 p = selected->worldPosition();
    const float dist = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    ImGui::Text("%s: %.1f", tr(Text::Radius), selected->physicalRadiusKm());
    ImGui::TextWrapped("%s: %.4f", tr(Text::Distance), dist);
    if (ImGui::CollapsingHeader(tr(Text::Details))) {
        ImGui::Text("%s: %.2f°", tr(Text::Tilt), selected->axialTilt());
        ImGui::TextWrapped("%s: %.2f", tr(Text::Rotation), selected->rotationPeriodHours());
        ImGui::TextWrapped("%s:", tr(Text::Position));
        ImGui::Text("x %+.4f / y %+.4f / z %+.4f", p.x, p.y, p.z);
    }
    ImGui::End();
}
} // namespace solar
