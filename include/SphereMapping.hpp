#pragma once

#include <utility>

namespace solar {

// raylib/par_shapes spheres use Z as their pole axis and (latitude, longitude)
// as UV coordinates. Equirectangular maps need Y-up poles and (longitude, latitude).
// Rotate the geometry and its normal by -90 degrees around X; preserve winding.
inline void orientSphereSurface(float* position, float* normal, float* uv) {
    const float y = position[1];
    position[1] = position[2];
    position[2] = -y;
    const float ny = normal[1];
    normal[1] = normal[2];
    normal[2] = -ny;
    std::swap(uv[0], uv[1]);
}

} // namespace solar
