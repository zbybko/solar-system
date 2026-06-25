#pragma once

// Vec3 — лёгкий трёхмерный вектор с double-компонентами.
//
// ООП-приём: ПЕРЕГРУЗКА ОПЕРАТОРОВ. Сложение, вычитание, умножение на скаляр,
// скалярное и векторное произведения записываются естественной математической
// нотацией. double выбран намеренно: гелиоцентрические координаты в а.е.
// требуют точности, недоступной float. Header-only: тип маленький, без
// инвариантов, и инлайнинг важнее раздельной компиляции.

#include <cmath>

namespace solar {

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};

    constexpr Vec3() = default;
    constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // --- Унарные ---
    constexpr Vec3 operator-() const { return Vec3{-x, -y, -z}; }

    // --- Составные присваивания ---
    constexpr Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr Vec3& operator*=(double s)      { x *= s;   y *= s;   z *= s;   return *this; }
    constexpr Vec3& operator/=(double s)      { x /= s;   y /= s;   z /= s;   return *this; }

    // --- Длина и нормализация ---
    double length() const { return std::sqrt(x * x + y * y + z * z); }
    constexpr double lengthSquared() const { return x * x + y * y + z * z; }

    // Единичный вектор; для нулевого вектора возвращает (0,0,0).
    Vec3 normalized() const {
        const double len = length();
        return (len > 0.0) ? Vec3{x / len, y / len, z / len} : Vec3{};
    }
};

// --- Бинарные операторы (свободные функции) ---
constexpr Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3{a.x + b.x, a.y + b.y, a.z + b.z}; }
constexpr Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3{a.x - b.x, a.y - b.y, a.z - b.z}; }
constexpr Vec3 operator*(const Vec3& v, double s)      { return Vec3{v.x * s, v.y * s, v.z * s}; }
constexpr Vec3 operator*(double s, const Vec3& v)      { return v * s; }
constexpr Vec3 operator/(const Vec3& v, double s)      { return Vec3{v.x / s, v.y / s, v.z / s}; }

// --- Произведения векторов ---
constexpr double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

inline double distance(const Vec3& a, const Vec3& b) {
    return (a - b).length();
}

} // namespace solar
