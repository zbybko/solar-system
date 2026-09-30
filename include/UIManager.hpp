#pragma once

// UIManager — весь интерфейс приложения (rlImGui) в одном классе.
//
// ООП-приём: ЕДИНСТВЕННАЯ ОТВЕТСТВЕННОСТЬ и инкапсуляция. Хранит состояние
// отображения (масштаб, орбиты, фон. свет, выбранный провайдер) и рисует
// панели — управление временем/камерой/видом и инфопанель выбранного тела.
// main лишь читает геттеры и применяет их к системе и рендеру.

#include "RenderContext.hpp"
#include "Localization.hpp"

namespace solar {

class SimulationClock;
class CameraController;
class CelestialBody;
class SolarSystem;

class UIManager {
public:
    explicit UIManager(Language language = Language::English) : language_(language) {}
    // Нарисовать все панели. Может изменить часы, камеру и снять выделение
    // (selected зануляется по кнопке «Сбросить вид»).
    void draw(SimulationClock& clock, CameraController& camera,
              CelestialBody*& selected, const SolarSystem& system);

    // Текущее состояние вида (читает main при рендере).
    RenderContext::Scale scale() const {
        return scaleMode_ == 0 ? RenderContext::Scale::Compact
                               : RenderContext::Scale::Real;
    }
    void setScale(RenderContext::Scale value) {
        scaleMode_ = value == RenderContext::Scale::Compact ? 0 : 1;
    }
    bool showOrbits() const { return showOrbits_; }
    bool showGrid() const { return showGrid_; }
    bool showSelectionWireframe() const { return showSelectionWireframe_; }
    float ambient() const { return ambient_; }
    int providerIndex() const { return provider_; } // 0 — libnova, 1 — Kepler

private:
    const char* tr(Text key) const { return text(key, language_); }
    void drawInfoPanel(const CelestialBody* selected) const;

    Language language_;
    int scaleMode_{0};
    bool showOrbits_{true};
    bool showGrid_{false};
    bool showSelectionWireframe_{false};
    float ambient_{0.12f};
    int provider_{0};
    bool settingsOpen_{false};
};

} // namespace solar
