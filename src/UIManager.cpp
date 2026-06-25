#include "UIManager.hpp"

#include "SimulationClock.hpp"
#include "CameraController.hpp"
#include "CelestialBody.hpp"

#include "imgui.h"

#include <cmath>

namespace solar {

void UIManager::draw(SimulationClock& clock, CameraController& camera,
                     CelestialBody*& selected) {
    // Стартовые позиция/размер (пользователь может перетащить/растянуть).
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360, 470), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"Панель управления");

    // --- Провайдер эфемерид (паттерн «Стратегия») ---
    ImGui::TextUnformatted(u8"Провайдер эфемерид:");
#ifdef HAVE_LIBNOVA
    const char* providers[] = {u8"libnova (VSOP87)", u8"Kepler (элементы JPL)"};
#else
    const char* providers[] = {u8"Kepler (элементы JPL)"};
#endif
    ImGui::Combo(u8"##provider", &provider_, providers, IM_ARRAYSIZE(providers));
    ImGui::Separator();

    // --- Камера ---
    const bool orbital = camera.mode() == CameraController::Mode::Orbital;
    ImGui::Text(u8"Камера: %s", orbital ? u8"орбита" : u8"свободный полёт");
    if (ImGui::Button(orbital ? u8"В свободный полёт" : u8"В режим орбиты"))
        camera.toggleMode();
    if (ImGui::Button(u8"Сбросить вид")) {
        selected = nullptr;
        camera.focusOn(Vector3{0.f, 0.f, 0.f}, 7.0f);
    }
    ImGui::TextWrapped(u8"ЛКМ — выбрать тело, ПКМ — поворот, колесо — "
                       u8"зум/скорость, F — режим, WASD+QE — полёт");
    ImGui::Separator();

    // --- Отображение ---
    ImGui::TextUnformatted(u8"Отображение:");
    ImGui::RadioButton(u8"Компактный масштаб", &scaleMode_, 0);
    ImGui::SameLine();
    ImGui::RadioButton(u8"Реальный", &scaleMode_, 1);
    ImGui::Checkbox(u8"Орбиты", &showOrbits_);
    ImGui::SameLine();
    ImGui::Checkbox(u8"Сетка", &showGrid_);
    ImGui::SliderFloat(u8"Фон. свет", &ambient_, 0.0f, 0.5f, "%.2f");
    ImGui::Separator();

    // --- Время ---
    ImGui::TextUnformatted(u8"Время:");
    bool paused = clock.isPaused();
    if (ImGui::Checkbox(u8"Пауза", &paused))
        clock.setPaused(paused);
    float speed = static_cast<float>(clock.speed());
    if (ImGui::SliderFloat(u8"Скорость (сут/с)", &speed, 0.0f, 100.0f, "%.1f"))
        clock.setSpeed(speed);
    ImGui::SameLine();
    if (ImGui::Button(u8"Сброс"))
        clock.reset();
    ImGui::Text(u8"JD = %.4f", clock.julianDay());
    ImGui::Separator();
    ImGui::Text("FPS: %d", GetFPS());

    ImGui::End();

    drawInfoPanel(selected);
}

void UIManager::drawInfoPanel(const CelestialBody* selected) const {
    if (selected == nullptr)
        return;

    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 290.f, 10.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 260), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"Информация о теле");
    ImGui::Text(u8"%s", selected->name().c_str());
    ImGui::Text(u8"Тип: %s", selected->typeName());
    ImGui::Separator();

    const Vector3 p = selected->worldPosition();
    const float dist = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    ImGui::Text(u8"Радиус (модель): %.2f", selected->radius());
    ImGui::Text(u8"Наклон оси: %.2f°", selected->axialTilt());
    ImGui::Text(u8"Период вращения: %.2f ч", selected->rotationPeriodHours());
    ImGui::Separator();
    ImGui::Text(u8"Гелиоцентр. позиция (а.е.):");
    ImGui::Text(u8"  x = %+.4f", p.x);
    ImGui::Text(u8"  y = %+.4f", p.y);
    ImGui::Text(u8"  z = %+.4f", p.z);
    ImGui::Text(u8"Расстояние от Солнца: %.4f а.е.", dist);
    ImGui::End();
}

} // namespace solar
