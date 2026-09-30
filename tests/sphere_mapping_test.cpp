#include "SphereMapping.hpp"

#include <cmath>
#include <iostream>

int main() {
    constexpr float pi = 3.14159265358979323846f;
    int failures = 0;
    auto check = [&](bool condition, const char* message) {
        if (!condition) {
            std::cerr << message << '\n';
            ++failures;
        }
    };
    // Sample raylib/par_shapes' sphere parameterization across both hemispheres
    // and the longitude seam, then verify its relationship to the image map.
    for (int latitude = 0; latitude <= 16; ++latitude) {
        for (int longitude = 0; longitude <= 32; ++longitude) {
            const float v = latitude / 16.0f;
            const float u = longitude / 32.0f;
            const float phi = v * pi;
            const float theta = u * 2.0f * pi;
            float normal[] = {std::cos(theta) * std::sin(phi),
                              std::sin(theta) * std::sin(phi), std::cos(phi)};
            float position[] = {2.0f * normal[0], 2.0f * normal[1], 2.0f * normal[2]};
            float uv[] = {v, u};
            solar::orientSphereSurface(position, normal, uv);
            check(std::abs(uv[0] - u) < 1e-6f && std::abs(uv[1] - v) < 1e-6f,
                  "Map longitude and latitude are swapped");
            check(std::abs(position[1] - 2.0f * std::cos(pi * uv[1])) < 1e-5f,
                  "Image latitude does not match the Y-up pole axis");
            check(std::abs(position[0] - 2.0f * std::cos(2.0f * pi * uv[0]) * std::sin(phi)) < 1e-5f &&
                  std::abs(position[2] + 2.0f * std::sin(2.0f * pi * uv[0]) * std::sin(phi)) < 1e-5f,
                  "Longitude is mirrored or rotated relative to the geometry");
            for (int axis = 0; axis < 3; ++axis)
                check(std::abs(position[axis] - 2.0f * normal[axis]) < 1e-5f,
                      "Lighting normal is not aligned with the surface");
        }
    }
    return failures == 0 ? 0 : 1;
}
