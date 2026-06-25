#pragma once

// Starfield — процедурный звёздный фон (замена полноценного skybox без ассета).
//
// ООП-приём: инкапсуляция. Звёзды генерируются один раз как случайные
// направления с разной яркостью и рисуются точками на большой сфере вокруг
// камеры, создавая иллюзию бесконечно далёкого неба.

#include "raylib.h"

#include <vector>

namespace solar {

class Starfield {
public:
    void generate(int count, unsigned seed = 1337);

    // Нарисовать звёзды вокруг позиции камеры (фон следует за наблюдателем).
    void draw(Vector3 cameraPos) const;

private:
    std::vector<Vector3> directions_; // единичные направления
    std::vector<Color> colors_;
    float radius_ = 480.0f;
};

} // namespace solar
