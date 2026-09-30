#pragma once

// CameraController — управление 3D-камерой raylib.
//
// ООП-приём: ИНКАПСУЛЯЦИЯ поведения камеры за простым интерфейсом. Внутри —
// два режима (стратегии управления): орбитальный облёт цели и свободный полёт.
// Клиент (main) не знает деталей сферических координат и обработки ввода —
// только update(), focusOn() и геттер камеры.

#include "raylib.h"

namespace solar {

class CameraController {
public:
    enum class Mode { Orbital, Free };

    CameraController();

    // Обработать ввод и пересчитать камеру. Флаги позволяют игнорировать ввод,
    // когда мышь/клавиатуру перехватил UI (ImGui).
    void update(float dt, bool acceptMouse, bool acceptKeys);

    // Сфокусироваться на точке (орбитальный режим): цель + удобная дистанция,
    // зависящая от радиуса тела.
    void focusOn(Vector3 worldPos, float bodyRadius);

    // Обновить точку, вокруг которой вращается орбитальная камера (для
    // слежения за движущимся телом).
    void setOrbitTarget(Vector3 worldPos);

    void setMode(Mode mode);
    void toggleMode();
    Mode mode() const { return mode_; }

    const Camera3D& camera() const { return camera_; }
    Ray mouseRay() const;

private:
    void updateOrbital(bool acceptMouse);
    void updateFree(float dt, bool acceptMouse, bool acceptKeys);
    void applyOrbital();
    void syncFreeFromCamera(); // чтобы при переходе в полёт не было рывка

    Camera3D camera_{};
    Mode mode_{Mode::Orbital};

    // Орбитальный режим: сферические координаты вокруг target_.
    Vector3 target_{0.f, 0.f, 0.f};
    float distance_{55.f};
    float minDistance_{2.f};
    float yaw_{0.f};    // рад, поворот вокруг вертикали
    float pitch_{0.6f}; // рад, угол возвышения

    // Свободный полёт: позиция + направление взгляда (yaw/pitch).
    float freeYaw_{0.f};
    float freePitch_{0.f};
    float moveSpeed_{25.f};
};

} // namespace solar
