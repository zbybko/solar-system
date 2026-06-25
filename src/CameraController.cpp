#include "CameraController.hpp"

#include "raymath.h"

#include <algorithm>
#include <cmath>

namespace solar {

namespace {
constexpr float kPitchLimit = 1.5f;     // ~86°
constexpr float kMinDistance = 2.f;
constexpr float kMaxDistance = 600.f;
constexpr float kRotateSens = 0.005f;
constexpr float kLookSens = 0.004f;

float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }
} // namespace

CameraController::CameraController() {
    camera_.up = Vector3{0.f, 1.f, 0.f};
    camera_.fovy = 50.f;
    camera_.projection = CAMERA_PERSPECTIVE;
    applyOrbital();
}

Ray CameraController::mouseRay() const {
    return GetScreenToWorldRay(GetMousePosition(), camera_);
}

void CameraController::applyOrbital() {
    const float cp = std::cos(pitch_), sp = std::sin(pitch_);
    const float sy = std::sin(yaw_), cy = std::cos(yaw_);
    const Vector3 offset{distance_ * cp * sy, distance_ * sp, distance_ * cp * cy};
    camera_.position = Vector3Add(target_, offset);
    camera_.target = target_;
    camera_.up = Vector3{0.f, 1.f, 0.f};
}

void CameraController::focusOn(Vector3 worldPos, float bodyRadius) {
    mode_ = Mode::Orbital;
    target_ = worldPos;
    distance_ = clampf(bodyRadius * 8.f, 4.f, 120.f);
    applyOrbital();
}

void CameraController::setOrbitTarget(Vector3 worldPos) {
    target_ = worldPos;
}

void CameraController::setMode(Mode mode) {
    if (mode == mode_)
        return;
    if (mode == Mode::Free) {
        syncFreeFromCamera();
    } else {
        // Восстановить сферические координаты из текущего положения камеры
        // относительно цели, чтобы переход был плавным.
        const Vector3 off = Vector3Subtract(camera_.position, target_);
        distance_ = clampf(Vector3Length(off), kMinDistance, kMaxDistance);
        if (distance_ > 0.001f) {
            pitch_ = std::asin(clampf(off.y / distance_, -1.f, 1.f));
            yaw_ = std::atan2(off.x, off.z);
        }
    }
    mode_ = mode;
}

void CameraController::toggleMode() {
    setMode(mode_ == Mode::Orbital ? Mode::Free : Mode::Orbital);
}

void CameraController::syncFreeFromCamera() {
    const Vector3 dir = Vector3Normalize(Vector3Subtract(camera_.target, camera_.position));
    freePitch_ = std::asin(clampf(dir.y, -1.f, 1.f));
    freeYaw_ = std::atan2(dir.x, dir.z);
}

void CameraController::update(float dt, bool acceptMouse, bool acceptKeys) {
    if (mode_ == Mode::Orbital)
        updateOrbital(acceptMouse);
    else
        updateFree(dt, acceptMouse, acceptKeys);
}

void CameraController::updateOrbital(bool acceptMouse) {
    if (acceptMouse) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            const Vector2 d = GetMouseDelta();
            yaw_ -= d.x * kRotateSens;
            pitch_ -= d.y * kRotateSens; // тянем вверх — поднимаемся
            pitch_ = clampf(pitch_, -kPitchLimit, kPitchLimit);
        }
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.f) {
            distance_ *= (1.f - wheel * 0.1f);
            distance_ = clampf(distance_, kMinDistance, kMaxDistance);
        }
    }
    applyOrbital();
}

void CameraController::updateFree(float dt, bool acceptMouse, bool acceptKeys) {
    if (acceptMouse && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        const Vector2 d = GetMouseDelta();
        freeYaw_ -= d.x * kLookSens;
        freePitch_ -= d.y * kLookSens;
        freePitch_ = clampf(freePitch_, -kPitchLimit, kPitchLimit);
    }

    const float cp = std::cos(freePitch_);
    const Vector3 forward = Vector3Normalize(Vector3{
        cp * std::sin(freeYaw_), std::sin(freePitch_), cp * std::cos(freeYaw_)});
    const Vector3 up{0.f, 1.f, 0.f};
    const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));

    if (acceptKeys) {
        Vector3 move{0.f, 0.f, 0.f};
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_E)) move = Vector3Add(move, up);
        if (IsKeyDown(KEY_Q)) move = Vector3Subtract(move, up);
        if (Vector3Length(move) > 0.f) {
            float v = moveSpeed_ * dt;
            if (IsKeyDown(KEY_LEFT_SHIFT)) v *= 3.f;
            move = Vector3Scale(Vector3Normalize(move), v);
            camera_.position = Vector3Add(camera_.position, move);
        }
    }

    if (acceptMouse) {
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.f)
            moveSpeed_ = clampf(moveSpeed_ * (1.f + wheel * 0.1f), 1.f, 400.f);
    }

    camera_.target = Vector3Add(camera_.position, forward);
    camera_.up = up;
}

} // namespace solar
