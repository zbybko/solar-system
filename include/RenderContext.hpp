#pragma once

// RenderContext — параметры отрисовки, общие для всех тел в кадре.
//
// ООП-приём: разделение модели и вида. Тело хранит позицию в «настоящих»
// астрономических единицах (а.е.), а перевод в экранные мировые координаты —
// ответственность RenderContext. Поддерживаются два масштаба расстояний:
// реальный (линейный) и компактный (sqrt-сжатие, чтобы внешние планеты не
// уходили далеко за кадр). Переключение не требует изменений в самих телах.

#include "raylib.h"

#include <cmath>

namespace solar {

struct RenderContext {
    enum class Scale { Compact, Real };

    Scale scale = Scale::Compact;
    float realScale = 4.0f;    // мировых единиц на а.е. (линейно)
    float compactScale = 7.0f; // множитель sqrt-отображения расстояний
    float radiusScale = 1.0f;  // множитель радиусов тел

    // Эклиптические а.е. (x,y,z) → мировые координаты raylib (y — вверх):
    // орбитальная плоскость ложится на плоскость x-z, широта даёт высоту.
    Vector3 toWorld(Vector3 au) const {
        const float r = std::sqrt(au.x * au.x + au.y * au.y + au.z * au.z);
        float k;
        if (scale == Scale::Real) {
            k = realScale;
        } else {
            // sqrt-сжатие сохраняет направление, но подтягивает дальние тела.
            k = (r > 1e-6f) ? compactScale * std::sqrt(r) / r : 0.f;
        }
        return Vector3{au.x * k, au.z * k, au.y * k}; // масштаб + своп осей
    }
};

} // namespace solar
