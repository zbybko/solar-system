// main.cpp — точка входа приложения.
//
// Фаза 8: весь UI вынесен в UIManager (панель времени/камеры/вида + инфопанель
// выбранного тела). Добавлен второй провайдер эфемерид KeplerEphemeris;
// переключение libnova ↔ Kepler прямо из панели демонстрирует паттерн
// «Стратегия» — SolarSystem::setEphemeris подменяет стратегию, тела остаются.
// main лишь связывает систему, часы, камеру, рендер и UI.

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#ifdef HAVE_LIBNOVA
#include "ephemeris/LibnovaEphemeris.hpp"
#endif
#include "ephemeris/KeplerEphemeris.hpp"
#include "SolarSystem.hpp"
#include "SimulationClock.hpp"
#include "CameraController.hpp"
#include "Renderer.hpp"
#include "RenderContext.hpp"
#include "UIManager.hpp"
#include "AssetPath.hpp"

#include <cstdlib>
#include <memory>
#include <string>

namespace {

// Создать провайдер эфемерид по индексу из UI. Если libnova доступна:
// 0 — libnova, 1 — Kepler. Если нет — всегда Kepler (единственный вариант).
std::unique_ptr<solar::IEphemeris> makeProvider(int index) {
#ifdef HAVE_LIBNOVA
    if (index == 0)
        return std::make_unique<solar::LibnovaEphemeris>();
#else
    (void)index;
#endif
    return std::make_unique<solar::KeplerEphemeris>();
}

} // namespace

int main() {
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Solar System — фаза 8 (UI)");
    SetTargetFPS(60);

    // ImGui с кириллическим шрифтом (см. фазу 5).
    rlImGuiBeginInitImGui();
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        const std::string fontPath = solar::assetPath("fonts/DejaVuSans.ttf");
        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f, nullptr,
                                     io.Fonts->GetGlyphRangesCyrillic());
        ImGui::StyleColorsDark();
    }
    rlImGuiEndInitImGui();

    // Явная область видимости: владельцы GPU-ресурсов (SolarSystem, Renderer)
    // должны освободить их (RAII) ДО CloseWindow, разрушающего GL-контекст.
    {
    solar::SolarSystem system =
        solar::SolarSystem::createRealistic(makeProvider(0));

    solar::SimulationClock clock{5.0};
    solar::RenderContext rctx{};

    solar::Renderer renderer;
    renderer.applyLighting(system);

    solar::CameraController controller;
    solar::UIManager ui;
    solar::CelestialBody* selected = nullptr;
    int activeProvider = 0;

    const char* shotPath = std::getenv("SS_SHOT");
    const char* focusEnv = std::getenv("SS_FOCUS");
    const int focusIdx = focusEnv ? std::atoi(focusEnv) : -1;
    bool focusDone = false;
    int frame = 0;

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        clock.advance(dt);

        // Смена провайдера эфемерид по выбору в UI (паттерн «Стратегия»).
        if (ui.providerIndex() != activeProvider) {
            activeProvider = ui.providerIndex();
            system.setEphemeris(makeProvider(activeProvider));
        }

        system.update(clock.julianDay());
        rctx.scale = ui.scale();

        if (!focusDone && focusIdx >= 0 &&
            focusIdx < static_cast<int>(system.bodies().size())) {
            selected = system.bodies()[focusIdx].get();
            controller.focusOn(selected->renderPosition(rctx), selected->radius());
            focusDone = true;
        }

        // --- Ввод камеры/выбора (с учётом перехвата мыши UI) ---
        const ImGuiIO& io = ImGui::GetIO();
        const bool acceptMouse = !io.WantCaptureMouse;
        const bool acceptKeys = !io.WantCaptureKeyboard;

        if (acceptKeys && IsKeyPressed(KEY_F))
            controller.toggleMode();

        if (acceptMouse && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            solar::CelestialBody* hit = system.pickBody(controller.mouseRay(), rctx);
            if (hit) {
                selected = hit;
                controller.focusOn(hit->renderPosition(rctx), hit->radius());
            }
        }

        if (selected && controller.mode() == solar::CameraController::Mode::Orbital)
            controller.setOrbitTarget(selected->renderPosition(rctx));

        controller.update(dt, acceptMouse, acceptKeys);
        const Camera3D& camera = controller.camera();

        renderer.updateLighting(rctx.toWorld(Vector3{0.f, 0.f, 0.f}),
                                camera.position, ui.ambient());

        BeginDrawing();
        ClearBackground(Color{4, 4, 10, 255});

        BeginMode3D(camera);
        renderer.drawStars(camera.position);
        if (ui.showGrid())
            DrawGrid(40, 1.0f);
        if (ui.showOrbits())
            system.drawOrbits(rctx);
        system.draw(rctx);
        if (selected) {
            const Vector3 c = selected->renderPosition(rctx);
            DrawSphereWires(c, selected->radius() * rctx.radiusScale * 1.25f, 10, 10,
                            Color{255, 255, 255, 120});
        }
        EndMode3D();

        rlImGuiBegin();
        ui.draw(clock, controller, selected);
        rlImGuiEnd();

        EndDrawing();

        if (shotPath && ++frame == 90) {
            TakeScreenshot(shotPath);
            break;
        }
    }
    } // ~SolarSystem/~Renderer: GPU-ресурсы освобождаются при живом GL-контексте

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
