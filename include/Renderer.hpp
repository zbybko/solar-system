#pragma once

// Renderer — рендер-уровень: освещение от Солнца и звёздный фон.
//
// ООП-приёмы: RAII (владеет Shader, выгружает в деструкторе; копирование
// запрещено) и инкапсуляция деталей шейдера. Назначает планетам шейдер
// точечного освещения, обновляет его униформы и рисует звёздное небо.

#include "raylib.h"
#include "Starfield.hpp"

namespace solar {

class SolarSystem;

class Renderer {
public:
    Renderer(); // загружает шейдер и генерирует звёзды (нужен GL-контекст)
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Назначить шейдер освещения всем телам, которые освещаются (не звезде).
    void applyLighting(SolarSystem& system);

    // Обновить униформы освещения на текущий кадр.
    void updateLighting(Vector3 sunWorldPos, Vector3 cameraPos, float ambient);

    void drawStars(Vector3 cameraPos) const { stars_.draw(cameraPos); }

private:
    Shader shader_{};
    int locSunPos_{-1};
    int locAmbient_{-1};
    Starfield stars_;
};

} // namespace solar
