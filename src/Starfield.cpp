#include "Starfield.hpp"

#include "raymath.h"

#include <random>

namespace solar {

void Starfield::generate(int count, unsigned seed) {
    directions_.clear();
    colors_.clear();
    directions_.reserve(count);
    colors_.reserve(count);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> uni(-1.f, 1.f);
    std::uniform_real_distribution<float> bright(0.3f, 1.f);

    for (int i = 0; i < count; ++i) {
        // Случайная точка на сфере (отбрасываем нулевые/слишком длинные).
        Vector3 v;
        float len;
        do {
            v = Vector3{uni(rng), uni(rng), uni(rng)};
            len = Vector3Length(v);
        } while (len < 0.1f || len > 1.f);

        directions_.push_back(Vector3Scale(v, 1.f / len));
        const unsigned char b = static_cast<unsigned char>(bright(rng) * 255.f);
        colors_.push_back(Color{b, b, static_cast<unsigned char>(std::min(255, b + 15)), 255});
    }
}

void Starfield::draw(Vector3 cameraPos) const {
    for (size_t i = 0; i < directions_.size(); ++i) {
        const Vector3 p = Vector3Add(cameraPos, Vector3Scale(directions_[i], radius_));
        DrawPoint3D(p, colors_[i]);
    }
}

} // namespace solar
